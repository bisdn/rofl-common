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

namespace rofl {

class event : public cthread_read_event {
public:
  event(cthread_wakeup_event *, void *userdata);
  ~event();
  int get_fd() const { return event_fd; }
  cthread_wakeup_event *get_wev() const { return wake; }
  void notifiy_one();

  void handle_read(int fd, void *userdata) override;

private:
  cthread_wakeup_event *wake;
  void *userdata;
  int event_fd;
};

event::event(cthread_wakeup_event *wake, void *userdata)
    : wake(wake), userdata(userdata) {
  event_fd = eventfd(0, 0);
  if (event_fd == -1) {
    switch (errno) {
    case EMFILE:
    case ENFILE:
    case ENODEV:
    case ENOMEM:
    default:
      LOG(ERROR) << __FUNCTION__
                 << ": failed to create eventfd: " << strerror(errno);
    }
  }
  VLOG(3) << __FUNCTION__ << ": event_fd=" << event_fd;
}

event::~event() {
  if (event_fd != -1)
    close(event_fd);
}

void event::notifiy_one() {
  // XXX(toanju): notify invalid initilization by returning the rv
  uint64_t c = 1;
  ssize_t rv = write(event_fd, &c, sizeof(c));
  (void)rv;
}

void event::handle_read(int fd, void *userdata) {
  assert(fd == event_fd);
  assert(userdata == this->userdata);
  uint64_t c;
  ssize_t rv = read(event_fd, &c, sizeof(c));
  (void)rv;

  if (wake) {
    wake->handle_wakeup(this->userdata);
  }
}

class event_base {
public:
  int fd;

  event_base(const event_base &e) { fd = e.fd; }

  event_base() {}
  virtual ~event_base() {}
};

class io_event : public event_base {
public:
  cthread_read_event *re;
  cthread_write_event *we;
  void *userdata_re;
  void *userdata_we;
  event *ev;
  io_event()
      : re(nullptr), we(nullptr), userdata_re(nullptr), userdata_we(nullptr),
        ev(nullptr) {}
  io_event(const io_event &e) : event_base(e) {
    this->re = e.re;
    this->we = e.we;
    this->userdata_re = e.userdata_re;
    this->userdata_we = e.userdata_we;
    this->ev = nullptr; // never copy this
  }
};

void cthread::initialize() {
  running = false;
  tid = 0;
  events_reg.resize(max_fds);

  // event loop
  if ((epfd = epoll_create(1)) < 0) {
    throw eSysCall("eSysCall", "epoll_create", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  add_wakeup_observer(nullptr, &wake_handle);
}

void cthread::release() {
  clear_timers();

  stop();

  remove_wakeup_observer(nullptr, wake_handle);

  {
    AcquireReadWriteLock lock(tlock);
    for (auto it : fds) {
      struct epoll_event epev;
      memset(&epev, 0, sizeof(struct epoll_event));
      epev.events = 0;
      epev.data.fd = it.first;
      epoll_ctl(epfd, EPOLL_CTL_DEL, it.first, &epev);

      if (events_reg[it.first]) {
        delete events_reg[it.first];
        events_reg[it.first] = nullptr;
      }
    }
    fds.clear();
  }

  ::close(epfd);
}

int cthread::add_wakeup_observer(cthread_wakeup_event *cb, int *handle,
                                 void *userdata) {
  if (handle == nullptr)
    LOG(FATAL) << __FUNCTION__ << ": handle cannot be null";

  event *ev = new event(cb, userdata);
  if (ev->get_fd() == -1) {
    VLOG(1) << __FUNCTION__ << ": invalid fd";
    delete ev;
    return -EIO;
  }

  if (events_reg[ev->get_fd()] == nullptr) {
    io_event *wue = new io_event();
    wue->fd = ev->get_fd();
    wue->ev = ev;
    wue->re = ev;
    wue->userdata_re = userdata;
    events_reg[ev->get_fd()] = wue;
    VLOG(2) << __FUNCTION__ << ": fd=" << ev->get_fd();
  } else {
    delete ev;
    LOG(FATAL) << __FUNCTION__ << ": already registered";
    return -EEXIST;
  }

  // XXX(toanju): use add_fd
  // register fd
  struct epoll_event epev;
  memset(&epev, 0, sizeof(struct epoll_event));
  epev.events = EPOLLIN;
  epev.data.fd = ev->get_fd();

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, ev->get_fd(), &epev) < 0) {
    switch (errno) {
    case ENOMEM:
    case ENOSPC:
      delete events_reg[ev->get_fd()];
      events_reg[ev->get_fd()] = nullptr;
      delete ev;
      return -errno;
      break;
    default:
      assert(0 && "not reached");
      break;
    }
  }

  *handle = ev->get_fd();
  VLOG(2) << __FUNCTION__ << ": added cb=" << cb << " fd=" << *handle
          << " (handle)";

  return 0;
}

int cthread::remove_wakeup_observer(cthread_wakeup_event *cb, int handle) {
  int rv = -EINVAL;

  VLOG(2) << __FUNCTION__ << ": cb=" << cb << " handle=" << handle;

  if (handle <= 0 || handle > max_fds)
    return rv;

  if (events_reg[handle]) {
    AcquireReadWriteLock lock(events_lock);
    io_event *ev = static_cast<io_event *>(events_reg[handle]);

    if (ev && ev->ev && ev->ev->get_wev() == cb) {
      // deregister fd from kernel
      struct epoll_event epev;
      memset(&epev, 0, sizeof(struct epoll_event));
      epev.events = 0;
      epev.data.fd = handle;

      epoll_ctl(epfd, EPOLL_CTL_DEL, handle, &epev);

      delete ev->ev;
      delete ev;
      events_reg[handle] = nullptr;
      rv = 0;
    } else {
      VLOG(2) << __FUNCTION__ << ": invalid cb=" << cb;
    }
  }

  return rv;
}

int cthread::notify_wake(int handle) {
  int rv = -EINVAL;

  VLOG(3) << __FUNCTION__ << ": trying to wake fd=" << handle;

  if (handle < 0 || handle > max_fds)
    return rv;

  if (events_reg[handle]) {
    io_event *ev = static_cast<io_event *>(events_reg[handle]);
    if (ev->ev && ev->ev->get_fd() == handle) {
      ev->ev->notifiy_one();
      rv = 0;
    }
  }

  return rv;
}

void cthread::add_fd(int fd, bool exception, bool edge_triggered) {
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) != fds.end()) {
    VLOG(3) << __FUNCTION__ << ": fd=" << fd << " already added";
    return;
  }

  uint32_t events = edge_triggered ? EPOLLET : 0;
  struct epoll_event epev;
  memset(&epev, 0, sizeof(struct epoll_event));
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
                       __FUNCTION__, __LINE__);
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
  memset(&epev, 0, sizeof(struct epoll_event));
  epev.data.fd = fd;

  VLOG(3) << __FUNCTION__ << ": fd=" << fd << " thread=" << this;

  if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &epev) < 0) {
    switch (errno) {
    case ENOENT: {
      /* do nothing */
    } break;
    default: {
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_DEL)", __FILE__,
                       __FUNCTION__, __LINE__);
    };
    }
  }

  fds.erase(fd);
}

void cthread::add_read_fd(int fd, cthread_read_event *cb, void *userdata,
                          bool exception, bool edge_triggered) {

  if ((unsigned)fd >= events_reg.capacity()) {
    VLOG(1) << __FUNCTION__ << ": maximum capacity of fd events is reached";
    return; // XXX(toanju): return values should be added
  }

  {
    AcquireReadWriteLock lock(events_lock);
    io_event *ioev = static_cast<io_event *>(events_reg[fd]);

    if (ioev == nullptr) {
      io_event *e = new io_event();
      e->fd = fd;
      e->re = cb;
      e->userdata_re = userdata;
      events_reg[fd] = e;
      VLOG(2) << __FUNCTION__ << ": fd=" << fd << " cb=" << cb
              << " userdata=" << userdata;
    } else if (ioev->re == nullptr) {
      ioev->re = cb;
      ioev->userdata_re = userdata;
      VLOG(2) << __FUNCTION__ << ": fd=" << fd << " cb=" << cb
              << " userdata=" << userdata;
    } else {
      // XXX(toanju): log error?
      VLOG(2) << __FUNCTION__ << ": already registered fd=" << fd
              << " cb=" << cb;
      assert(cb == ioev->re);
    }
  }

  add_fd(fd, exception, edge_triggered);

  AcquireReadWriteLock lock(tlock);

  fds[fd] |= EPOLLIN;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd];
  epev.data.fd = fd;

  if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epev) < 0) {
    switch (errno) {
    case ENOENT:
      epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev);
      break;
    default:
      if (exception)
        throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_MOD)", __FILE__,
                       __FUNCTION__, __LINE__);
    }
  }
}

void cthread::drop_read_fd(int fd, bool exception) {
  {
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
      case ENOENT:
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev);
        break;
      default:
        if (exception)
          throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_MOD)", __FILE__,
                         __FUNCTION__, __LINE__);
      }
    }
  }

  AcquireReadWriteLock lock(events_lock);
  if (events_reg[fd]) {
    io_event *ioev = static_cast<io_event *>(events_reg[fd]);

    VLOG(2) << __FUNCTION__ << ": fd=" << fd << " cb_read=" << ioev->re
            << " cb_write=" << ioev->we;

    if (ioev->we) {
      ioev->re = nullptr;
    } else {
      delete events_reg[fd];
      events_reg[fd] = nullptr;
      drop_fd(fd);
    }
  }
}

void cthread::add_write_fd(int fd, cthread_write_event *cb, void *userdata,
                           bool exception, bool edge_triggered) {

  if ((unsigned)fd >= events_reg.capacity()) {
    VLOG(1) << __FUNCTION__ << ": maximum capacity of fd events is reached";
    return; // XXX(toanju): return values should be added
  }

  {
    AcquireReadWriteLock lock(events_lock);
    io_event *ioev = static_cast<io_event *>(events_reg[fd]);

    if (ioev == nullptr) {
      ioev = new io_event();
      ioev->fd = fd;
      ioev->we = cb;
      ioev->userdata_we = userdata;
      events_reg[fd] = ioev;
      VLOG(2) << __FUNCTION__ << ": fd=" << fd << " cb=" << cb
              << " userdata=" << userdata;
    } else if (ioev->we == nullptr) {
      ioev->we = cb;
      ioev->userdata_we = userdata;
      VLOG(2) << __FUNCTION__ << ": fd=" << fd << " cb=" << cb
              << " userdata=" << userdata;
    } else {
      // XXX(toanju): log error?
      VLOG(2) << __FUNCTION__ << ": already registered fd=" << fd
              << " cb=" << cb;
    }
  }

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
                       __FUNCTION__, __LINE__);
    };
    }
  }
}

void cthread::drop_write_fd(int fd, bool exception) {
  {
    AcquireReadWriteLock lock(tlock);
    if (fds.find(fd) == fds.end())
      return;

    fds[fd] &= ~EPOLLOUT;

    struct epoll_event epev;
    memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
    epev.events = fds[fd];
    epev.data.fd = fd;

    VLOG(2) << __FUNCTION__ << " fd=" << fd << " thread=" << this;

    if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epev) < 0) {
      switch (errno) {
      case ENOENT:
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epev);
        break;
      default:
        if (exception)
          throw eSysCall("eSysCall", "epoll_ctl (EPOLL_CTL_MOD)", __FILE__,
                         __FUNCTION__, __LINE__);
      }
    }
  }

  AcquireReadWriteLock lock(events_lock);
  if (events_reg[fd]) {
    io_event *ioev = static_cast<io_event *>(events_reg[fd]);

    VLOG(2) << __FUNCTION__ << ": fd=" << fd << " cb_read=" << ioev->re
            << " cb_write=" << ioev->we;

    if (ioev->re) {
      ioev->we = nullptr;
    } else {
      delete events_reg[fd];
      events_reg[fd] = nullptr;
      drop_fd(fd);
    }
  }
}

void cthread::clear_timers() {
  AcquireReadWriteLock lock(tlock);
  ordered_timers.clear();
};

bool cthread::add_timer(cthread_timeout_event *e, uint32_t timer_id,
                        const ctimespec &tspec) {
  std::pair<std::set<ctimer>::iterator, bool> rv;
  bool do_wakeup = false;

  VLOG(2) << __FUNCTION__ << ": e=" << e << " timer_id=" << timer_id << " "
          << tspec;

  {
    AcquireReadWriteLock lock(tlock);

    if (ordered_timers.empty() || tspec < ordered_timers.begin()->get_tspec())
      do_wakeup = true;

    auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                            ctimer_find_by_timer_and_event(e, timer_id));
    if (timer_it != ordered_timers.end()) {
      ordered_timers.erase(timer_it);
    }

    rv = ordered_timers.emplace(e, timer_id, tspec);
  }

  // rearm timer
  if ((do_wakeup) && (tid != pthread_self())) {
    notify_wake(wake_handle);
  }

  return rv.second;
}

const ctimer &cthread::get_timer(cthread_timeout_event *e,
                                 uint32_t timer_id) const {
  AcquireReadLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_and_event(e, timer_id));
  if (timer_it == ordered_timers.end()) {
    throw eThreadNotFound("cthread::get_timer() timer_id not found");
  }
  return *timer_it;
}

bool cthread::drop_timer(cthread_timeout_event *e, uint32_t timer_id) {
  AcquireReadWriteLock lock(tlock);

  if (timer_id != ALL_TIMERS) {
    // erase timer event with timer_id
    auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                            ctimer_find_by_timer_and_event(e, timer_id));

    if (timer_it == ordered_timers.end()) {
      return false;
    }

    ordered_timers.erase(timer_it);
  } else {
    // erase all timer events
    auto ot_iter = ordered_timers.begin();
    while (ot_iter != ordered_timers.end()) {
      if (ot_iter->get_callback() == e) {
        ordered_timers.erase(ot_iter++);
      } else {
        ++ot_iter;
      }
    }
  }

  if (tid != pthread_self()) {
    notify_wake(wake_handle);
  }

  return true;
}

bool cthread::has_timer(cthread_timeout_event *e, uint32_t timer_id) const {
  AcquireReadLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_and_event(e, timer_id));
  return (not(timer_it == ordered_timers.end()));
}

void cthread::start(const std::string &thread_name) {
  switch (state) {
  case STATE_IDLE: {

    running = true;
    if (pthread_create(&tid, nullptr, &(cthread::start_loop), this) < 0) {
      throw eSysCall("eSysCall", "pthread_create", __FILE__, __FUNCTION__,
                     __LINE__);
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

    notify_wake(wake_handle);

    /* deletion of thread not initiated within this thread */
    if (pthread_self() == tid) {
      assert(0 && "stoping thread within thread");
      return;
    }

    int rv = pthread_join(tid, nullptr);
    if (rv != 0) {
      pthread_cancel(tid);
    }

    state = STATE_IDLE;

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

      VLOG(5) << __FUNCTION__ << " timeout=" << timeout;
      rc = epoll_pwait(epfd, events, 64, timeout, &signal_set);

      if (not running)
        goto out;

      ctimer timer;
      ctimespec now = timer.get_created();
      while (true) {
        /* handle expired timers */
        {
          AcquireReadWriteLock lock(tlock);
          if (ordered_timers.empty()) {
            break;
          }
          timer = *(ordered_timers.begin());
          if (now < timer.get_tspec() /* created after the loop started*/ ||
              not timer.get_tspec().is_expired()) {
            break;
          }
          ordered_timers.erase(ordered_timers.begin());
        } // release lock here
        if (not running)
          goto out;

        VLOG(3) << __FUNCTION__
                << ": calling handle_timeout for id=" << timer.get_timer_id()
                << " on cb=" << timer.get_callback();
        timer.get_callback()->handle_timeout(
            (void *)((intptr_t)timer.get_timer_id()));
      }

      if (not running)
        goto out;

      /* handle file descriptors */
      if (rc > 0) {
        for (int i = 0; i < rc; i++) {

          if (not running)
            goto out;

          VLOG(3) << __FUNCTION__ << ": fd=" << events[i].data.fd
                  << " events=0x" << std::hex << events[i].events << std::dec;

          io_event *ioev = nullptr;
          {
            // XXX(toanju): check if can we get rid of the copy
            AcquireReadLock lock(events_lock);
            ioev = static_cast<io_event *>(events_reg[events[i].data.fd]);
            if (ioev)
              ioev = new io_event(*ioev);
            else
              continue;
          }

          if (events[i].events & EPOLLIN && ioev->re) {
            VLOG(1) << "call handle_read on fd=" << events[i].data.fd;
            ioev->re->handle_read(events[i].data.fd, ioev->userdata_re);
          }

          if (events[i].events & EPOLLOUT && ioev->we) {
            VLOG(1) << "call handle_write on fd=" << events[i].data.fd;
            ioev->we->handle_write(events[i].data.fd, ioev->userdata_we);
          }

          delete ioev;
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

} // namespace rofl
