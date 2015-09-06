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

	wakeup_pending = false;

	// worker thread
	if ((epfd = epoll_create(1)) < 0) {
		log << logging::error << "cthread::initialize() failed to create epoll set: "
				<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
		throw eSysCall("cthread::initialize() syscall epoll_create() failed");
	}

	// pipe
	if (pipe2(pipefd, pipe_flags) < 0) {
		log << logging::error << "cthread::initialize() syscall pipe2(): "
				<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
		throw eSysCall("cthread::initialize() unable to create rxpipe");
	}

	add_read_fd(pipefd[PIPE_READ_FD]);

	start_thread();
}



void
cthread::release()
{
	clear_timers();

	stop_thread();

	drop_read_fd(pipefd[PIPE_READ_FD]);

	::close(epfd);

	::close(pipefd[PIPE_READ_FD]);
	::close(pipefd[PIPE_WRITE_FD]);
}



void
cthread::add_read_fd(
		int fd)
{
	AcquireReadWriteLock lock(tlock);
	rfds.insert(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = EPOLLIN;
	epev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev) < 0) {
		log << logging::error << "cthread::add_read_fd() failed to add fd to epoll set: "
				<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
		throw eSysCall("cthread::add_read_fd() syscall epoll_ctl() failed");
	}

	wakeup();
}



void
cthread::drop_read_fd(
		int fd)
{
	AcquireReadWriteLock lock(tlock);
	rfds.erase(fd);

	struct epoll_event epev;
	memset((uint8_t*)&epev, 0, sizeof(struct epoll_event));
	epev.events = EPOLLIN;
	epev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &epev) < 0) {
		log << logging::error << "cthread::drop_read_fd() failed to add fd to epoll set: "
				<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
		throw eSysCall("cthread::drop_read_fd() syscall epoll_ctl() failed");
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
		log << logging::error << "cthread::add_write_fd() failed to add fd to epoll set: "
				<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
		throw eSysCall("cthread::add_write_fd() syscall epoll_ctl() failed");
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
		log << logging::error << "cthread::drop_write_fd() failed to add fd to epoll set: "
				<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
		throw eSysCall("cthread::drop_write_fd() syscall epoll_ctl() failed");
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

		if (do_wakeup)
			wakeup();
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

	wakeup();

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
cthread::start_thread()
{
	run_thread = true;
	if (pthread_create(&tid, NULL, &(cthread::start_loop), this) < 0) {
		log << logging::error << "cthread::start_thread() failed to start worker thread: "
						<< "errno=" << errno << " (" << strerror(errno) << ")" << std::flush;
		throw eSysCall("cthread::start_thread() failed to start RX thread");
	}
}



void
cthread::stop_thread()
{
	run_thread = false;

	/* deletion of thread initiated within this thread */
	if (pthread_self() == tid) {
		pthread_cancel(tid);

	/* deletion of thread initiated from external thread */
	} else {
		wakeup();
		if (pthread_join(tid, NULL) < 0) {
			log << logging::error << "cthread::stop_thread() failed to stop worker thread: "
							<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
			pthread_cancel(tid);
		}
	}
}



void
cthread::wakeup()
{
	if (wakeup_pending)
		return;
	char c = 1;
	if (write(pipefd[PIPE_WRITE_FD], &c, sizeof(c)) < 0) {
		log << logging::error << "cthread::thread_wakeup() syscall send() failed, "
				<< "errno: " << errno << " (" << strerror(errno) << ")" << std::ends;
		switch (errno) {
		case EAGAIN: {
			// do nothing
		} break;
		case EINTR: {
			// signal received
		} break;
		default: {
			throw eSysCall("cthread::thread_wakeup() unable to wakeup RX thread");
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
#if 0
					std::cerr << "spring::netlink::cthread::run_loop() ordered_timers list: " << std::endl;
					for (auto tspec : ordered_timers) {
						std::cerr << "spring::netlink::cthread::run_loop() element: " << tspec << std::endl;
					}
					std::cerr << "spring::netlink::cthread::run_loop() now: " << ctimespec::now() << std::endl;

					std::cerr << ">>> P0.0 (AAA) timeout: " << timeout << std::endl;
#endif
				}
			}
#if 0
			std::cerr << ">>> P0.0 (final) timeout: " << timeout << std::endl;
#endif

			rc = epoll_pwait(epfd, events, 64, timeout, &sigmask);

			if (rc == 0) {

				std::list<unsigned int> ttypes;
				uint32_t timer_id = 0;
				ctimespec ts;
				while (true) {
					{
						AcquireReadLock lock(tlock);
						if (ordered_timers.empty()) {
							break;
						}
#if 0
						std::cerr << ">>> P1.0" << std::endl;
#endif
						ts = *(ordered_timers.begin());
						if (not ts.is_expired()) {
							break;
						}
#if 0
						std::cerr << ">>> P2.0" << std::endl;
#endif
						ordered_timers.erase(ordered_timers.begin());
					} // release lock here
#if 0
					std::cerr << ">>> P3.0" << std::endl;
#endif
					timer_id = ts.get_timer_id();
					if (has_timer(timer_id)) {
						ttypes = get_timer(timer_id).get_timer_types();
					}
					drop_timer(timer_id);
#if 0
					std::cerr << "spring::netlink::cthread::run_loop() ordered_timers list: " << std::endl;
					for (auto tspec : ordered_timers) {
						std::cerr << "spring::netlink::cthread::run_loop() element: " << tspec << std::endl;
					}
					std::cerr << "spring::netlink::cthread::run_loop() now: " << ctimespec::now() << std::endl;
					std::cerr << ">>> P4.0" << std::endl;
#endif
					cthread_env::call_env(env).handle_timeout(*this, timer_id, ttypes);
				}

			} else
			if (rc > 0) {
				for (int i = 0; i < rc; i++) {
					if (events[i].data.fd == pipefd[PIPE_READ_FD]) {

						//std::cerr << "WAKEUP" << std::endl;

						wakeup_pending = false;

						if (not run_thread) {
							return &retval;
						}

						if (events[i].events & EPOLLIN) {
							char c;
							int rcode = read(pipefd[PIPE_READ_FD], &c, sizeof(c));
							(void)rcode;
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
					log << logging::error << "cthread::run_loop() syscall epoll_wait() failed "
								<< "errno: " << errno << " (" << strerror(errno) << ")" << std::flush;
					retval = -1;
					goto out;
				};
				}

			}

		} catch (eThreadNotFound& e) {
			log << logging::error << "cthread::run_loop() env not found" << std::flush;
		} catch (std::runtime_error& e) {
			log << logging::error << "cthread::run_loop() exception, what: " << e.what() << std::flush;
		} catch (...) {
			log << logging::error << "cthread::run_loop() exception" << std::flush;
		}
	}

out:

	return &retval;
}

