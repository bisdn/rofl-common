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
#include <glog/logging.h>
#include <iostream>
#include <sys/eventfd.h>

using namespace rofl;

/*static*/ const int cthread::PIPE_READ_FD = 0;
/*static*/ const int cthread::PIPE_WRITE_FD = 1;

void cthread::initialize() {
  running = false;
  tid = 0;

  // worker thread
  if ((epfd = epoll_create(1)) < 0) {
    throw eSysCall("eSysCall", "epoll_create", __FILE__, __PRETTY_FUNCTION__,
                   __LINE__);
  }

  // eventfd
  event_fd = eventfd(0, EFD_NONBLOCK);
  if (event_fd < 0) {
    throw eSysCall("eSysCall", "eventfd", __FILE__, __PRETTY_FUNCTION__,
                   __LINE__);
  }

  // register event_fd to kernel
  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = EPOLLIN; // level-triggered
  epev.data.fd = event_fd;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, event_fd, &epev) < 0) {
    switch (errno) {
    case EEXIST: {
      /* do nothing */
    } break;
    default: {
      throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_ADD)", __FILE__,
                     __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }
}

void cthread::release() {
  clear_timers();

  stop();

  {
    AcquireReadWriteLock lock(tlock);
    for (auto it : fds) {
      struct epoll_event epev;
      memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
      epev.events = 0;
      epev.data.fd = it.first;
      epoll_ctl(epfd, EPOLL_CTL_DEL, it.first, &epev);
    }
    fds.clear();
  }

  // deregister event_fd from kernel
  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = 0;
  epev.data.fd = event_fd;

  if (epoll_ctl(epfd, EPOLL_CTL_DEL, event_fd, &epev) < 0) {
    switch (errno) {
    case ENOENT: {
      /* do nothing */
    } break;
    default: {
      throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_DEL)", __FILE__,
                     __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }

  ::close(epfd);
  ::close(event_fd);
}

void cthread::add_fd(int fd, bool exception, bool edge_triggered) {
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) != fds.end())
    return;

  uint32_t events = edge_triggered ? EPOLLET : 0;
  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = events;
  epev.data.fd = fd;

  VLOG(3) << __FUNCTION__ << " fd=" << fd
          << " edge_triggered=" << edge_triggered << " thread=" << this;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev) < 0) {
    switch (errno) {
    case EEXIST: {
      /* do nothing */
    } break;
    default: {
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_ADD)", __FILE__,
                       __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }

  fds[fd] = events;
}

void cthread::drop_fd(int fd, bool exception) {
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) == fds.end())
    return;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.data.fd = fd;

  VLOG(3) << __FUNCTION__ << " fd=" << fd << " thread=" << this;

  if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &epev) < 0) {
    switch (errno) {
    case ENOENT: {
      /* do nothing */
    } break;
    default: {
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_DEL)", __FILE__,
                       __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }

  fds.erase(fd);
}

void cthread::add_read_fd(int fd, bool exception, bool edge_triggered) {
  add_fd(fd, exception, edge_triggered);

  AcquireReadWriteLock lock(tlock);

  fds[fd] |= EPOLLIN;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd];
  epev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epev) < 0) {
    switch (errno) {
    case ENOENT: {
      epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev);
    } break;
    default: {
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_MOD)", __FILE__,
                       __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }
}

void cthread::drop_read_fd(int fd, bool exception) {
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) == fds.end())
    return;

  fds[fd] &= ~EPOLLIN;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd];
  epev.data.fd = fd;

  VLOG(3) << __FUNCTION__ << " fd=" << fd << " thread=" << this;

  if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epev) < 0) {
    switch (errno) {
    case ENOENT: {
      epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev);
    } break;
    default: {
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_MOD)", __FILE__,
                       __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }
}

void cthread::add_write_fd(int fd, bool exception, bool edge_triggered) {
  add_fd(fd, exception, edge_triggered);

  AcquireReadWriteLock lock(tlock);

  fds[fd] |= EPOLLOUT;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd];
  epev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epev) < 0) {
    switch (errno) {
    case ENOENT: {
      epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev);
    } break;
    default: {
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_MOD)", __FILE__,
                       __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }
}

void cthread::drop_write_fd(int fd, bool exception) {
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) == fds.end())
    return;

  fds[fd] &= ~EPOLLOUT;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd];
  epev.data.fd = fd;

  VLOG(3) << __FUNCTION__ << " fd=" << fd << " thread=" << this;

  if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epev) < 0) {
    switch (errno) {
    case ENOENT: {
      epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev);
    } break;
    default: {
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_MOD)", __FILE__,
                       __PRETTY_FUNCTION__, __LINE__);
    };
    }
  }
}

void cthread::clear_timers() {
  AcquireReadWriteLock lock(tlock);
  ordered_timers.clear();
};

bool cthread::add_timer(uint32_t timer_id, const ctimespec &tspec) {
  std::pair<std::set<ctimer>::iterator, bool> rv;
  bool do_wakeup = false;
  {
    AcquireReadWriteLock lock(tlock);

    if (ordered_timers.empty() || tspec < ordered_timers.begin()->get_tspec())
      do_wakeup = true;

    auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                            ctimer_find_by_timer_id(timer_id));
    if (timer_it != ordered_timers.end()) {
      ordered_timers.erase(timer_it);
      rv = ordered_timers.emplace(timer_id, tspec);
    } else {
      rv = ordered_timers.emplace(timer_id, tspec);
    }
  }

  if ((do_wakeup) && (tid != pthread_self())) {
    wakeup();
  }

  return rv.second;
}

const ctimer &cthread::get_timer(uint32_t timer_id) const {
  AcquireReadLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_id(timer_id));
  if (timer_it == ordered_timers.end()) {
    throw eThreadNotFound("cthread::get_timer() timer_id not found");
  }
  return *timer_it;
}

bool cthread::drop_timer(uint32_t timer_id) {
  AcquireReadWriteLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_id(timer_id));
  if (timer_it == ordered_timers.end()) {
    return false;
  }
  ordered_timers.erase(timer_it);

  if (tid != pthread_self()) {
    wakeup();
  }

  return true;
}

bool cthread::has_timer(uint32_t timer_id) const {
  AcquireReadLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_id(timer_id));
  return (not(timer_it == ordered_timers.end()));
}

void cthread::start(const std::string &thread_name) {
  switch (state) {
  case STATE_IDLE: {

    running = true;
    if (pthread_create(&tid, NULL, &(cthread::start_loop), this) < 0) {
      throw eSysCall("eSysCall", "pthread_create", __FILE__,
                     __PRETTY_FUNCTION__, __LINE__);
    }

    if (thread_name.length() && thread_name.length() < 16)
      pthread_setname_np(tid, thread_name.c_str());

    state = STATE_RUNNING;

  } break;
  default: {};
  }
}

void cthread::stop() {
  switch (state) {
  case STATE_RUNNING: {

    running = false;

    wakeup();

    /* deletion of thread not initiated within this thread */
    if (pthread_self() == tid) {
      return;
    }

    int rv = pthread_join(tid, NULL);
    if (rv != 0) {
      pthread_cancel(tid);
    }

    state = STATE_IDLE;

  } break;
  default: {};
  }
}

void cthread::wakeup() {
  switch (state) {
  case STATE_RUNNING: {
    uint64_t c = 1;
    if (write(event_fd, &c, sizeof(c)) < 0) {
      switch (errno) {
      case EAGAIN: {
        // do nothing
      } break;
      case EINTR: {
        // signal received
      } break;
      default: {
        throw eSysCall("eSysCall", "write to event_fd", __FILE__,
                       __PRETTY_FUNCTION__, __LINE__);
      };
      }
    }
  } break;
  default: {};
  }
}

void *cthread::run_loop() {
  retval = 0;
  sigset_t signal_set;
  sigfillset(&signal_set); // ignore all signals

  while (running) {
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

      if (not running)
        goto out;

      while (true) {
        /* handle expired timers */
        ctimer timer;
        {
          AcquireReadWriteLock lock(tlock);
          if (ordered_timers.empty()) {
            break;
          }
          timer = *(ordered_timers.begin());
          if (not timer.get_tspec().is_expired()) {
            break;
          }
          ordered_timers.erase(ordered_timers.begin());
        } // release lock here
        if (not running)
          goto out;

        env->handle_timeout(*this, timer.get_timer_id());
      }

      if (not running)
        goto out;

      /* handle file descriptors */
      if (rc > 0) {
        for (int i = 0; i < rc; i++) {

          if (not running)
            goto out;

          if (events[i].data.fd == event_fd) {

            if (not running) {
              return &retval;
            }

            if (events[i].events & EPOLLIN) {
              uint64_t c;
              int rcode = read(event_fd, &c, sizeof(c));
              (void)rcode;
              env->handle_wakeup(*this);
            }

          } else {
            if (events[i].events & EPOLLIN)
              env->handle_read_event(*this, events[i].data.fd);
            if (events[i].events & EPOLLOUT)
              env->handle_write_event(*this, events[i].data.fd);
          }
        }
      } else if (rc < 0) {

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

    } catch (eThreadNotFound &e) {
      std::cerr << __FUNCTION__
                << ": ERROR, caught eThreadNotFound: " << e.what() << std::endl;
    } catch (std::runtime_error &e) {
      std::cerr << __FUNCTION__
                << ": ERROR, caught runtimer_error: " << e.what() << std::endl;
    } catch (std::exception &e) {
      std::cerr << __FUNCTION__ << ": ERROR, caught exception: " << e.what()
                << std::endl;
    } catch (...) {
      std::cerr << __FUNCTION__ << ": ERROR, caught unknown error" << std::endl;
    }
  }

out:

  return &retval;
}
