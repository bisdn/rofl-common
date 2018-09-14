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

/*static*/ std::atomic_bool cthread::pool_initialized(false);
/*static*/ std::map<uint32_t, cthread *> cthread::pool;
/*static*/ crwlock cthread::pool_lock;
/*static*/ uint32_t cthread::pool_num_mgt_threads;
/*static*/ uint32_t cthread::pool_mgt_loop_index;
/*static*/ uint32_t cthread::pool_num_io_threads;
/*static*/ uint32_t cthread::pool_io_loop_index;
/*static*/ uint32_t cthread::pool_num_hnd_threads;
/*static*/ uint32_t cthread::pool_hnd_loop_index;

/*static*/ std::set<cthread_env *> cthread_env::envs;
/*static*/ crwlock cthread_env::envs_lock;

/*static*/ BIO *cthread::bio_stderr;

/*static*/ void cthread::openssl_initialize() {
  SSL_library_init();
  SSL_load_error_strings();
  ERR_load_ERR_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();
  OpenSSL_add_all_ciphers();
  OpenSSL_add_all_digests();
  bio_stderr = BIO_new_fp(stderr, BIO_NOCLOSE);
}

/*static*/ void cthread::openssl_terminate() {
  BIO_free_all(bio_stderr);
#if 0
  FIPS_mode_set(0);
  ENGINE_cleanup();
  CONF_modules_unload(1);
  CONF_modules_free();
  ERR_clear_error();
  RAND_cleanup();
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
  ERR_free_strings();
  OPENSSL_cleanup();
  ASN1_STRING_TABLE_cleanup();
#endif
  FIPS_mode_set(0);
  ENGINE_cleanup();
  CONF_modules_unload(1);
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
  ERR_free_strings();
}

/*static*/ void cthread::pool_initialize(uint32_t pool_num_hnd_threads,
                                         uint32_t pool_num_io_threads,
                                         uint32_t pool_num_mgt_threads) {
  AcquireReadWriteLock lock(cthread::pool_lock);
  if (cthread::pool_initialized)
    return;

  openssl_initialize();

  /* sanity check for thread numbers */
  cthread::pool_num_mgt_threads =
      (pool_num_mgt_threads == 0) ? 1 : pool_num_mgt_threads;
  cthread::pool_num_io_threads =
      (pool_num_io_threads == 0) ? 2 : pool_num_io_threads;
  cthread::pool_num_hnd_threads =
      (pool_num_hnd_threads == 0) ? 1 : pool_num_hnd_threads;

  /* number of IO threads should be an even number */
  if (cthread::pool_num_io_threads % 2) {
    cthread::pool_num_io_threads += 1;
  }

  /* start offsets for loop indices */
  cthread::pool_mgt_loop_index = 0;
  cthread::pool_io_loop_index = cthread::pool_num_mgt_threads;
  cthread::pool_hnd_loop_index =
      cthread::pool_num_mgt_threads + cthread::pool_num_io_threads;

  for (uint32_t i = cthread::pool_mgt_loop_index;
       i < cthread::pool_io_loop_index; ++i) {
    std::stringstream thread_name;
    thread_name << "rofl-mgt-(" << (unsigned int)i << ")";
    (cthread::pool[i] = new cthread(i))->start(thread_name.str());
  }
  for (uint32_t i = cthread::pool_io_loop_index;
       i < (cthread::pool_hnd_loop_index); ++i) {
    std::stringstream thread_name;
    thread_name << "rofl-io-(" << (unsigned int)i << ")";
    (cthread::pool[i] = new cthread(i))->start(thread_name.str());
  }
  for (uint32_t i = cthread::pool_hnd_loop_index;
       i < (cthread::pool_num_mgt_threads + cthread::pool_num_io_threads +
            cthread::pool_num_hnd_threads);
       ++i) {
    std::stringstream thread_name;
    thread_name << "rofl-app-(" << (unsigned int)i << ")";
    (cthread::pool[i] = new cthread(i))->start(thread_name.str());
  }
  cthread::pool_initialized = true;
}

/*static*/ void cthread::pool_terminate() {
  if (!cthread::pool_initialized)
    return;
  pool_stop_all_threads();
  sleep(2);
  {
    AcquireReadWriteLock lock(cthread::pool_lock);
    for (uint32_t i = 0; i < cthread::pool.size(); ++i) {
      delete cthread::pool[i];
    }
    cthread::pool.clear();
    cthread::pool_initialized = false;
  }
  openssl_terminate();
}

/*static*/ void cthread::pool_stop_all_threads() {
  AcquireReadLock lock(cthread::pool_lock);
  for (uint32_t i = 0; i < cthread::pool.size(); ++i) {
    cthread::pool[i]->stop();
  }
}

/*static*/ cthread &cthread::thread(uint32_t thread_num) {
  if (not cthread::pool_initialized) {
    cthread::pool_initialize();
  }
  AcquireReadLock lock(cthread::pool_lock);
  if (cthread::pool.find(thread_num) == cthread::pool.end()) {
    throw eThreadNotFound("thread number not found");
  }
  return *(cthread::pool[thread_num]);
}

/*static*/ uint32_t cthread::get_mgt_thread_num_from_pool() {
  if (not cthread::pool_initialized) {
    cthread::pool_initialize();
  }
  AcquireReadWriteLock lock(cthread::pool_lock);
  if (pool_mgt_loop_index < cthread::pool_num_mgt_threads - 1) {
    pool_mgt_loop_index++;
  } else {
    pool_mgt_loop_index = 0; /* wrap around: start offset */
  }
  return pool_mgt_loop_index;
}

/*static*/ uint32_t cthread::get_io_thread_num_from_pool() {
  if (not cthread::pool_initialized) {
    cthread::pool_initialize();
  }
  AcquireReadWriteLock lock(cthread::pool_lock);
  if (pool_io_loop_index <
      (cthread::pool_num_mgt_threads + cthread::pool_num_io_threads - 1)) {
    pool_io_loop_index++;
  } else {
    pool_io_loop_index =
        cthread::pool_num_mgt_threads; /* wrap around: start offset */
  }
  return pool_io_loop_index;
}

/*static*/ uint32_t cthread::get_hnd_thread_num_from_pool() {
  if (not cthread::pool_initialized) {
    cthread::pool_initialize();
  }
  AcquireReadWriteLock lock(cthread::pool_lock);
  if (pool_hnd_loop_index <
      (cthread::pool_num_mgt_threads + cthread::pool_num_io_threads +
       cthread::pool_num_hnd_threads - 1)) {
    pool_hnd_loop_index++;
  } else {
    pool_hnd_loop_index =
        cthread::pool_num_mgt_threads +
        cthread::pool_num_io_threads; /* wrap around: start offset */
  }
  return pool_hnd_loop_index;
}

void cthread::initialize() {

  running = false;
  thread_tid = 0;

  // worker thread
  if ((epfd = epoll_create(1)) < 0) {
    throw eSysCall("eSysCall", "epoll_create", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  // eventfd
  event_fd = eventfd(0, EFD_NONBLOCK);
  if (event_fd < 0) {
    throw eSysCall("eSysCall", "eventfd", __FILE__, __FUNCTION__, __LINE__);
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
                     __FUNCTION__, __LINE__);
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
                     __FUNCTION__, __LINE__);
    };
    }
  }

  ::close(epfd);
  ::close(event_fd);
}

void cthread::add_fd(cthread_env *env, int fd, bool exception,
                     bool edge_triggered) {
  if (env == nullptr)
    throw eThreadInvalid("thread environment must not be null");

  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) != fds.end())
    return;

  uint32_t events = edge_triggered ? (uint32_t)EPOLLET : 0;
  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = events;
  epev.data.fd = fd;

  VLOG(7) << __FUNCTION__ << " fd=" << fd << " env=" << env
          << " edge-triggered=" << edge_triggered << " thread: "
          << " tid=0x" << std::hex << thread_tid << std::dec
          << " num=" << thread_num << " name=" << thread_name
          << " ptr=" << this;

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

  fds[fd].events = events;
  fds[fd].env = env;
}

void cthread::drop_fd(int fd, bool exception) {
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) == fds.end())
    return;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.data.fd = fd;

  VLOG(7) << __FUNCTION__ << " fd=" << fd << " thread: "
          << " tid=0x" << std::hex << thread_tid << std::dec
          << " num=" << thread_num << " name=" << thread_name
          << " ptr=" << this;

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

void cthread::add_read_fd(cthread_env *env, int fd, bool exception,
                          bool edge_triggered) {
  add_fd(env, fd, exception, edge_triggered);

  AcquireReadWriteLock lock(tlock);

  fds[fd].events |= EPOLLIN;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd].events;
  epev.data.fd = fd;

  VLOG(7) << __FUNCTION__ << " fd=" << fd << " env=" << env
          << " edge-triggered=" << edge_triggered << " thread: "
          << " tid=0x" << std::hex << thread_tid << std::dec
          << " num=" << thread_num << " name=" << thread_name
          << " ptr=" << this;

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

void cthread::drop_read_fd(int fd, bool exception) {
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) == fds.end())
    return;

  fds[fd].events &= ~EPOLLIN;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd].events;
  epev.data.fd = fd;

  VLOG(7) << __FUNCTION__ << " fd=" << fd << " thread: "
          << " tid=0x" << std::hex << thread_tid << std::dec
          << " num=" << thread_num << " name=" << thread_name
          << " ptr=" << this;

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

void cthread::add_write_fd(cthread_env *env, int fd, bool exception,
                           bool edge_triggered) {
  add_fd(env, fd, exception, edge_triggered);

  AcquireReadWriteLock lock(tlock);

  fds[fd].events |= EPOLLOUT;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd].events;
  epev.data.fd = fd;

  VLOG(7) << __FUNCTION__ << " fd=" << fd << " env=" << env
          << " edge-triggered=" << edge_triggered << " thread: "
          << " tid=0x" << std::hex << thread_tid << std::dec
          << " num=" << thread_num << " name=" << thread_name
          << " ptr=" << this;

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
  AcquireReadWriteLock lock(tlock);
  if (fds.find(fd) == fds.end())
    return;

  fds[fd].events &= ~EPOLLOUT;

  struct epoll_event epev;
  memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
  epev.events = fds[fd].events;
  epev.data.fd = fd;

  VLOG(7) << __FUNCTION__ << " fd=" << fd << " thread: "
          << " tid=0x" << std::hex << thread_tid << std::dec
          << " num=" << thread_num << " name=" << thread_name
          << " ptr=" << this;

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

void cthread::drop_fds(cthread_env *env) {
  AcquireReadWriteLock lock(tlock);
restart:
  for (auto it = fds.begin(); it != fds.end(); ++it) {
    if (it->second.env == env) {
      struct epoll_event epev;
      memset((uint8_t *)&epev, 0, sizeof(struct epoll_event));
      epev.data.fd = it->first;
      epoll_ctl(epfd, EPOLL_CTL_DEL, it->first, &epev);
      fds.erase(it);
      goto restart;
    }
  }
}

void cthread::clear_timers() {
  AcquireReadWriteLock lock(tlock);
  ordered_timers.clear();
};

bool cthread::add_timer(cthread_env *env, uint32_t timer_id,
                        const ctimespec &tspec) {
  std::pair<std::set<ctimer>::iterator, bool> rv;
  bool do_wakeup = false;
  {
    AcquireReadWriteLock lock(tlock);

    if (ordered_timers.empty() || tspec < ordered_timers.begin()->get_tspec())
      do_wakeup = true;

    auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                            ctimer_find_by_timer_env_and_id(env, timer_id));
    if (timer_it != ordered_timers.end()) {
      ordered_timers.erase(timer_it);
      rv = ordered_timers.emplace(env, timer_id, tspec);
    } else {
      rv = ordered_timers.emplace(env, timer_id, tspec);
    }
  }

  if ((do_wakeup) && (thread_tid != pthread_self())) {
    wakeup();
  }

  return rv.second;
}

const ctimer &cthread::get_timer(cthread_env *env, uint32_t timer_id) const {
  AcquireReadLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_env_and_id(env, timer_id));
  if (timer_it == ordered_timers.end()) {
    throw eThreadNotFound("cthread::get_timer() timer_id not found");
  }
  return *timer_it;
}

bool cthread::drop_timer(cthread_env *env, uint32_t timer_id) {
  AcquireReadWriteLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_env_and_id(env, timer_id));
  if (timer_it == ordered_timers.end()) {
    return false;
  }
  ordered_timers.erase(timer_it);

  if (thread_tid != pthread_self()) {
    wakeup();
  }

  return true;
}

void cthread::drop_timers(cthread_env *env) {
  AcquireReadWriteLock lock(tlock);
  bool do_wakeup = false;
  std::set<ctimer>::iterator pos;
  while ((pos = find_if(ordered_timers.begin(), ordered_timers.end(),
                        ctimer_find_by_timer_env(env))) !=
         ordered_timers.end()) {
    ordered_timers.erase(pos);
    do_wakeup = true;
  }
  if (do_wakeup && (thread_tid != pthread_self())) {
    wakeup();
  }
}

bool cthread::has_timer(cthread_env *env, uint32_t timer_id) const {
  AcquireReadLock lock(tlock);
  auto timer_it = find_if(ordered_timers.begin(), ordered_timers.end(),
                          ctimer_find_by_timer_env_and_id(env, timer_id));
  return (not(timer_it == ordered_timers.end()));
}

void cthread::start(const std::string &thread_name) {
  switch (state) {
  case STATE_IDLE: {

    running = true;
    if (pthread_create(&thread_tid, NULL, &(cthread::start_loop), this) < 0) {
      throw eSysCall("eSysCall", "pthread_create", __FILE__, __FUNCTION__,
                     __LINE__);
    }

    this->thread_name = thread_name;

    if (thread_name.length() && thread_name.length() < 16)
      pthread_setname_np(thread_tid, thread_name.c_str());

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
    if (pthread_self() == thread_tid) {
      return;
    }

    int rv = pthread_join(thread_tid, NULL);
    if (rv != 0) {
      pthread_cancel(thread_tid);
    }

    state = STATE_IDLE;

  } break;
  default: {};
  }
}

void cthread::wakeup(cthread_env *env) {
  switch (state) {
  case STATE_RUNNING: {
    uint64_t c = 1;
    if (env) {
      AcquireReadWriteLock lock(tlock);
      wakeups.insert(env);
    }
    if (write(event_fd, &c, sizeof(c)) < 0) {
      switch (errno) {
      case EAGAIN: {
        // do nothing
      } break;
      case EINTR: {
        // signal received
      } break;
      default: {
        throw eSysCall("eSysCall", "write to event_fd", __FILE__, __FUNCTION__,
                       __LINE__);
      };
      }
    }
  } break;
  default: {};
  }
}

void cthread::handle_wakeup() {
  std::set<cthread_env *> envs;
  {
    AcquireReadWriteLock lock(tlock);
    for (auto it = wakeups.begin(); it != wakeups.end(); ++it) {
      envs.insert(*it);
    }
    wakeups.clear();
  }
  for (auto it = envs.begin(); it != envs.end(); ++it) {
    try {
      env((*it)).handle_wakeup(*this);
    } catch (eThreadNotFound &e) {
    }
  }
}

void cthread::drop_wakeup(cthread_env *env) {
  AcquireReadWriteLock lock(tlock);
  std::set<cthread_env *>::iterator it;
  if (wakeups.find(env) != wakeups.end()) {
    wakeups.erase(env);
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

        env(timer.env()).handle_timeout(*this, timer.get_timer_id());
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
              handle_wakeup();
            }

          } else {
            try {
              if (events[i].events & EPOLLIN)
                env(events[i].data.fd)
                    .handle_read_event(*this, events[i].data.fd);
              if (events[i].events & EPOLLOUT)
                env(events[i].data.fd)
                    .handle_write_event(*this, events[i].data.fd);
            } catch (eThreadNotFound &e) {
            }
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

    } catch (eThreadInvalid &e) {
      std::cerr << __FUNCTION__
                << ": ERROR, caught eThreadInvalid: " << e.what() << std::endl;
    } catch (eThreadNotFound &e) {
      std::cerr << __FUNCTION__
                << ": ERROR, caught eThreadNotFound: " << e.what() << std::endl;
    } catch (eThreadBase &e) {
      std::cerr << __FUNCTION__ << ": ERROR, caught eThreadBase: " << e.what()
                << std::endl;
    } catch (std::runtime_error &e) {
      std::cerr << __FUNCTION__
                << ": ERROR, caught runtimer_error: " << e.what() << std::endl;
    } catch (std::exception &e) {
      std::cerr << __FUNCTION__ << ": ERROR, caught exception: " << e.what()
                << std::endl;
    } catch (...) {
      std::cerr << __FUNCTION__ << ": ERROR, caught unknown error" << std::endl;
    }
  } /* while (running) */

out:

  return &retval;
}
