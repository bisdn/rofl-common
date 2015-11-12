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

using namespace rofl;

/*static*/std::set<cthread_env*> cthread_env::thread_envs;
/*static*/crwlock cthread_env::thread_envs_lock;
/*static*/const int	cthread::PIPE_READ_FD  = 0;
/*static*/const int	cthread::PIPE_WRITE_FD = 1;

void
cthread::initialize()
{
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

	add_read_fd(pipefd[PIPE_READ_FD]);

	//start_thread();
}



void
cthread::release()
{
	clear_timers();

	stop();

	drop_read_fd(pipefd[PIPE_READ_FD]);

	::close(epfd);

	::close(pipefd[PIPE_READ_FD]);
	::close(pipefd[PIPE_WRITE_FD]);
}



void
cthread::add_read_fd(
		int fd, bool exception)
{
	AcquireReadWriteLock lock(tlock);
	rfds.insert(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = EPOLLIN;
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
		int fd, bool exception)
{
	AcquireReadWriteLock lock(tlock);
	rfds.erase(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = EPOLLIN;
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
		int fd)
{
	AcquireReadWriteLock lock(tlock);
	wfds.insert(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = EPOLLOUT;
	epev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev) < 0) {
		throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_ADD)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	wakeup();
}



void
cthread::drop_write_fd(
		int fd)
{
	AcquireReadWriteLock lock(tlock);
	wfds.erase(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = EPOLLOUT;
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

		run_thread = true;
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

		state = STATE_IDLE;

		run_thread = false;

		/* deletion of thread not initiated within this thread */
		if (not (pthread_self() == tid)) {

			wakeup();

			if (pthread_join(tid, NULL) < 0) {
				pthread_cancel(tid);
			}
		}

	} break;
	default: {

	};
	}
}



void
cthread::wakeup()
{
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
}



void*
cthread::run_loop()
{
	retval = 0;

	// make valgrind happy, as using NULL indicates a problem
	sigset_t sigmask;
	pthread_sigmask(0, NULL, &sigmask);

	while (run_thread) {
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

			rc = epoll_pwait(epfd, events, 64, timeout, &sigmask);

			/* handle expired timers */
			std::list<unsigned int> ttypes;
			uint32_t timer_id = 0;
			ctimespec ts;
			while (true) {
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
				timer_id = ts.get_timer_id();
				if (has_timer(timer_id)) {
					ttypes = get_timer(timer_id).get_timer_types();
				}
				drop_timer(timer_id);
				cthread_env::call_env(env).handle_timeout(*this, timer_id, ttypes);
			}

			/* handle file descriptors */
			if (rc > 0) {
				for (int i = 0; i < rc; i++) {
					if (events[i].data.fd == pipefd[PIPE_READ_FD]) {

						if (not run_thread) {
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

