/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cthread.cpp
 *
 *  Created on: May 21, 2015
 *      Author: andi
 */

#include "cthread.hpp"
#include <iostream>

using namespace rofl;

/*static*/std::set<cthread_env*> cthread_env::thread_envs;
/*static*/rofl::crwlock cthread_env::thread_envs_lock;
/*static*/std::map<const cthread*, bool> cthread::run_thread;
/*static*/rofl::crwlock cthread::run_thread_lock;
/*static*/const int	cthread::PIPE_READ_FD  = 0;
/*static*/const int	cthread::PIPE_WRITE_FD = 1;

void
cthread::initialize()
{
	{
		/* add boolean parameter run_thread to static map and set it to false */
		AcquireReadWriteLock lock(cthread::run_thread_lock);
		cthread::run_thread[this] = false;
	}

	int pipe_flags = O_NONBLOCK;

	tid = 0;

	wakeup_pending = false;

	// worker thread
	if ((epfd = epoll_create(1)) < 0) {
		throw eSysCall("eSysCall", "epoll_create", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	// pipe
	if (pipe2(pipefd, pipe_flags) < 0) {
		throw eSysCall("eSysCall", "pipe2", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	add_read_fd(pipefd[PIPE_READ_FD], true, EPOLLIN);
}

void
cthread::release()
{
	clear_timers();

	stop();

	drop_read_fd(pipefd[PIPE_READ_FD], true, EPOLLIN);

	::close(epfd);

	::close(pipefd[PIPE_READ_FD]);
	::close(pipefd[PIPE_WRITE_FD]);

	{
		/* remove boolean parameter run_thread from static map */
		AcquireReadWriteLock lock(cthread::run_thread_lock);
		cthread::run_thread.erase(this);
	}
}



void
cthread::add_read_fd(
		int fd, bool exception, uint32_t events)
{
	// check events
	assert(events & (EPOLLIN|EPOLLPRI|EPOLLOUT|EPOLLRDNORM|EPOLLRDBAND|EPOLLWRNORM|EPOLLWRBAND|EPOLLMSG|EPOLLERR|EPOLLHUP|EPOLLRDHUP|EPOLLWAKEUP|EPOLLONESHOT|EPOLLET));

	AcquireReadWriteLock lock(tlock);
	rfds.insert(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = events;
	epev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev) < 0) {
		switch (errno) {
		case EEXIST: {
			/* fd already registered */
		} break;
		default: {
			if (exception)
				throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_ADD)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
	}

	wakeup();
}



void
cthread::drop_read_fd(
		int fd, bool exception, uint32_t events)
{
	// check events
	assert(events & (EPOLLIN|EPOLLPRI|EPOLLOUT|EPOLLRDNORM|EPOLLRDBAND|EPOLLWRNORM|EPOLLWRBAND|EPOLLMSG|EPOLLERR|EPOLLHUP|EPOLLRDHUP|EPOLLWAKEUP|EPOLLONESHOT|EPOLLET));

	AcquireReadWriteLock lock(tlock);
	rfds.erase(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = events;
	epev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &epev) < 0) {
		switch (errno) {
		case ENOENT: {
			/* fd not registered */
		} break;
		default: {
			if (exception)
				throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_DEL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
	}

	wakeup();
}



void
cthread::add_write_fd(
		int fd, uint32_t events)
{
	// check events
	assert(events & (EPOLLIN|EPOLLPRI|EPOLLOUT|EPOLLRDNORM|EPOLLRDBAND|EPOLLWRNORM|EPOLLWRBAND|EPOLLMSG|EPOLLERR|EPOLLHUP|EPOLLRDHUP|EPOLLWAKEUP|EPOLLONESHOT|EPOLLET));

	AcquireReadWriteLock lock(tlock);
	wfds.insert(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = events;
	epev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev) < 0) {
		throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_ADD)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	wakeup();
}



void
cthread::drop_write_fd(
		int fd, uint32_t events)
{
	// check events
	assert(events & (EPOLLIN|EPOLLPRI|EPOLLOUT|EPOLLRDNORM|EPOLLRDBAND|EPOLLWRNORM|EPOLLWRBAND|EPOLLMSG|EPOLLERR|EPOLLHUP|EPOLLRDHUP|EPOLLWAKEUP|EPOLLONESHOT|EPOLLET));

	AcquireReadWriteLock lock(tlock);
	wfds.erase(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = events;
	epev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &epev) < 0) {
		throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_DEL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	wakeup();
}



void
cthread::clear_timers()
{
	AcquireReadWriteLock lock(tlock);
	ordered_timers.clear();
	for (auto it : timers)
		delete it.second;
	timers.clear();
};



ctimer&
cthread::add_timer(
		uint32_t timer_id, const ctimespec& tspec)
{
	AcquireReadWriteLock lock(tlock);
	if (timers.find(timer_id) != timers.end()) {
		std::set<ctimespec>::iterator it;
		while ((it = find_if(ordered_timers.begin(), ordered_timers.end(),
				ctimespec::ctimespec_find_by_timer_id(timer_id))) != ordered_timers.end()) {
			ordered_timers.erase(it);
		}
		delete timers[timer_id];
		timers.erase(timer_id);
	}
	timers[timer_id] = new ctimer(timer_id, tspec);

	{
		bool do_wakeup = false;
		if (ordered_timers.empty() || (tspec < (*ordered_timers.begin()))) {
			do_wakeup = true;
		}

		ctimespec ts(tspec);
		ts.set_timer_id(timer_id);
		ordered_timers.insert(ts);

		if ((do_wakeup) && (tid != pthread_self())) {
			wakeup();
		}
	}

	return *(timers[timer_id]);
};



ctimer&
cthread::set_timer(
		uint32_t timer_id)
{
	AcquireReadLock lock(tlock);
	if (timers.find(timer_id) == timers.end()) {
		throw eThreadNotFound("cthread::set_timer() timer_id not found");
	}
	return *(timers[timer_id]);
};



const ctimer&
cthread::get_timer(
		uint32_t timer_id) const
{
	AcquireReadLock lock(tlock);
	if (timers.find(timer_id) == timers.end()) {
		throw eThreadNotFound("cthread::get_timer() timer_id not found");
	}
	return *(timers.at(timer_id));
};



bool
cthread::drop_timer(
		uint32_t timer_id)
{
	AcquireReadWriteLock lock(tlock);
	if (timers.find(timer_id) == timers.end()) {
		return false;
	}
	std::set<ctimespec>::iterator it;
	while ((it = find_if(ordered_timers.begin(), ordered_timers.end(),
			ctimespec::ctimespec_find_by_timer_id(timer_id))) != ordered_timers.end()) {
		ordered_timers.erase(it);
	}
	delete timers[timer_id];
	timers.erase(timer_id);

	if (tid != pthread_self()) {
		wakeup();
	}

	return true;
};



bool
cthread::has_timer(
		uint32_t timer_id) const
{
	AcquireReadLock lock(tlock);
	return (not (timers.find(timer_id) == timers.end()));
};



void
cthread::start()
{
	switch (state) {
	case STATE_IDLE: {

		set_run_thread(true);
		if (pthread_create(&tid, NULL, &(cthread::start_loop), this) < 0) {
			throw eSysCall("eSysCall", "pthread_create", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		state = STATE_RUNNING;

	} break;
	default: {

	};
	}
}



void
cthread::stop()
{
	switch (state) {
	case STATE_RUNNING: {

		set_run_thread(false);

		wakeup();

		/* deletion of thread not initiated within this thread */
		if (pthread_self() == tid) {
			//std::cerr << "pthread_exit" << std::endl;
			return;
		}

		struct timespec ts;
		ts.tv_nsec = 0;
		ts.tv_sec = 1;
		//std::cerr << "pthread_join" << std::endl;
		if (pthread_timedjoin_np(tid, NULL, &ts) < 0) {
			//std::cerr << "pthread_cancel" << std::endl;
			pthread_cancel(tid);
		}

		state = STATE_IDLE;

	} break;
	default: {

	};
	}
}



void
cthread::wakeup()
{
	switch (state) {
	case STATE_RUNNING: {
		if (wakeup_pending)
				return;
		char c = 1;
		if (write(pipefd[PIPE_WRITE_FD], &c, sizeof(c)) < 0) {
			switch (errno) {
			case EAGAIN: {
				// do nothing
			} break;
			case EINTR: {
				// signal received
			} break;
			default: {
				throw eSysCall("eSysCall", "write", __FILE__, __PRETTY_FUNCTION__, __LINE__);
			};
			}
		}
	} break;
	default: {

	};
	}
}



void*
cthread::run_loop()
{
	retval = 0;
	sigset_t signal_set;
	sigfillset(&signal_set); // ignore all signals

	while (get_run_thread()) {
		try {

			struct epoll_event events[64];

			int rc = 0;

			int timeout = 3600000; // in milliseconds: 1hour

			{
				AcquireReadLock lock(tlock);
				if (not ordered_timers.empty()) {
					timeout = ordered_timers.begin()->get_relative_timeout();
				}
			}

			rc = epoll_pwait(epfd, events, 64, timeout, &signal_set);

			if (not get_run_thread())
				goto out;

			while (true) {
				/* handle expired timers */
				std::list<unsigned int> ttypes;
				uint32_t timer_id = 0;
				ctimespec ts;
				{
					AcquireReadLock lock(tlock);
					if (ordered_timers.empty()) {
						break;
					}
					ts = *(ordered_timers.begin());
					if (not ts.is_expired()) {
						break;
					}
					ordered_timers.erase(ordered_timers.begin());
				} // release lock here
				if (not get_run_thread())
					goto out;

				if (ts.is_expired()) // optimization problem?
				{
					timer_id = ts.get_timer_id();
					if (has_timer(timer_id)) {
						ttypes = get_timer(timer_id).get_timer_types();
					}
					drop_timer(timer_id);
					cthread_env::call_env(env).handle_timeout(*this, timer_id, ttypes);
				}
			}

			if (not get_run_thread())
				goto out;

			/* handle file descriptors */
			if (rc > 0) {
				for (int i = 0; i < rc; i++) {

					if (not get_run_thread())
						goto out;

					if (events[i].data.fd == pipefd[PIPE_READ_FD]) {

						if (not get_run_thread()) {
							return &retval;
						}

						if (events[i].events & EPOLLIN) {
							char c;
							int rcode = read(pipefd[PIPE_READ_FD], &c, sizeof(c));
							(void)rcode;
							wakeup_pending = false;
							cthread_env::call_env(env).handle_wakeup(*this);
						}

					} else {
						if (events[i].events & EPOLLIN)
							cthread_env::call_env(env).handle_read_event(*this, events[i].data.fd);
						if (events[i].events & EPOLLOUT)
							cthread_env::call_env(env).handle_write_event(*this, events[i].data.fd);
					}
				}
			} else
			if (rc < 0) {

				switch (errno) {
				case EINTR: {
					// signal received
				} break;
				default: {
					retval = -1;
					goto out;
				};
				}

			}

		} catch (eThreadNotFound& e) {

		} catch (std::runtime_error& e) {

		} catch (...) {

		}
	}

out:

	return &retval;
}

