/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cthread.hpp
 *
 *  Created on: May 21, 2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_CTHREAD_HPP_
#define SRC_ROFL_COMMON_CTHREAD_HPP_

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <cassert>
#include <inttypes.h>
#include <iostream>
#include <list>
#include <map>
#include <set>

#include "rofl/common/ctimer.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"

namespace rofl {

class eThreadBase : public exception {
public:
  eThreadBase(const std::string &__arg) : exception(__arg){};
};

class eThreadNotFound : public eThreadBase {
public:
  eThreadNotFound(const std::string &__arg) : eThreadBase(__arg){};
};

class eThreadInvalid : public eThreadBase {
public:
  eThreadInvalid(const std::string &__arg) : eThreadBase(__arg){};
};

class cthread; // forward declaration

class cthread_env : public ctimer_env {
  friend class cthread;
  static std::set<cthread_env *> envs;
  static crwlock envs_lock;

public:
  cthread_env() {
    AcquireReadWriteLock lock(envs_lock);
    envs.insert(this);
  }
  virtual ~cthread_env() {
    AcquireReadWriteLock lock(envs_lock);
    envs.erase(this);
  }
  static cthread_env &env(cthread_env *thread_env) {
    AcquireReadLock lock(envs_lock);
    if (envs.find(thread_env) == envs.end()) {
      throw eThreadNotFound("thread environment not found");
    }
    return *(thread_env);
  }

protected:
  virtual void handle_wakeup(cthread &thread) = 0;
  virtual void handle_timeout(cthread &thread, uint32_t timer_id) = 0;
  virtual void handle_read_event(cthread &thread, int fd) = 0;
  virtual void handle_write_event(cthread &thread, int fd) = 0;
};

class cthread {
public:
  /**
   * @brief Initialize thread pool
   */
  static void
  pool_initialize(uint32_t num_of_hnd_threads = DEFAULT_POOL_NUM_HND_THREADS,
                  uint32_t num_of_io_threads = DEFAULT_POOL_NUM_IO_THREADS,
                  uint32_t num_of_mgt_threads = DEFAULT_POOL_NUM_MGT_THREADS);

  /**
   * @brief Terminate thread pool
   */
  static void pool_terminate();

  /**
   * @brief Stop all threads in pool
   */
  static void pool_stop_all_threads();

  /**
   * @brief Get reference to cthread instance
   */
  static cthread &thread(uint32_t thread_num);

  /**
   * @brief Assign next cthread instance
   */
  static uint32_t get_mgt_thread_num_from_pool();

  /**
   * @brief Assign next cthread instance
   */
  static uint32_t get_io_thread_num_from_pool();

  /**
   * @brief Assign next cthread instance
   */
  static uint32_t get_hnd_thread_num_from_pool();

public:
  /**
   *
   */
  virtual ~cthread() {
    try { // don't throw in destructor
      release();
    } catch (std::exception &e) {
      std::cerr << __FUNCTION__ << "(): failed with " << e.what() << std::endl;
    }
  };

  /**
   *
   */
  cthread(uint32_t thread_num) : thread_num(thread_num), state(STATE_IDLE) {
    initialize();
  };

public:
  /**
   *
   */
  std::string get_thread_name() const { return thread_name; };
  /**
   *
   */
  uint32_t get_thread_num() const { return thread_num; };
  /**
   *
   */
  pthread_t get_thread_id() const { return thread_tid; };

  /**
   *
   */
  void drop(cthread_env *env) {
    try {
      drop_wakeup(env);
      drop_fds(env);
      drop_timers(env);
    } catch (...) {
    };
  };

public:
  /**
   * @brief	Wake up RX thread via rx pipe
   */
  void wakeup(cthread_env *env = nullptr);

  /**
   * @brief Removes pointer cthread_env instance from std::set<cthread_env*>
   * wakeups
   */
  void drop_wakeup(cthread_env *env);

public:
  /**
   * @brief	Register file descriptor
   */
  void add_fd(cthread_env *env, int fd, bool exception = false,
              bool edge_triggered = true);

  /**
   * @brief	Deregister file descriptor
   */
  void drop_fd(int fd, bool exception = false);

  /**
   * @brief	Add file descriptor to set of observed fds
   */
  void add_read_fd(cthread_env *env, int fd, bool exception = true,
                   bool edge_triggered = true);

  /**
   * @brief	Drop file descriptor from set of observed fds
   */
  void drop_read_fd(int fd, bool exception = true);

  /**
   * @brief	Add file descriptor to set of observed fds
   */
  void add_write_fd(cthread_env *env, int fd, bool exception = true,
                    bool edge_triggered = true);

  /**
   * @brief	Drop file descriptor from set of observed fds
   */
  void drop_write_fd(int fd, bool exception = true);

  /**
   * @brief	Removes all file descriptors managed by a specific cthread_env
   * instance
   */
  void drop_fds(cthread_env *env);

public:
  /**
   *
   */
  void clear_timers();

  /**
   *
   */
  bool add_timer(cthread_env *env, uint32_t timer_id, const ctimespec &tspec);

  /**
   *
   */
  const ctimer &get_timer(cthread_env *env, uint32_t timer_id) const;

  /**
   *
   */
  bool drop_timer(cthread_env *env, uint32_t timer_id);

  /**
   *
   */
  bool has_timer(cthread_env *env, uint32_t timer_id) const;

  /**
   *
   */
  void drop_timers(cthread_env *env);

public:
  /**
   * @brief	Starts worker thread
   *
   * Sets run flag run_rx_thread to true and calls
   * syscall pthread_create() for starting worker thread.
   */
  void start(const std::string &thread_name = std::string(""));

  /**
   * @brief	Stops worker thread
   *
   * Stops worker thread by setting variable run_rx_thread
   * to false and waking up the thread. Waits for thread termination via
   * pthread_join().
   * If joining the thread fails, syscall pthread_cancel() destroys ultimately
   * the thread.
   */
  void stop();

public:
  friend std::ostream &operator<<(std::ostream &os, const cthread &thread) {
    os << "cthread, "
       << "tid: " << thread.get_thread_id() << ", "
       << "num: " << (unsigned int)thread.get_thread_num() << ", "
       << "name: " << thread.get_thread_name() << " " << std::endl;
    AcquireReadLock lock(thread.tlock);
    if (not thread.ordered_timers.empty()) {
      os << "next timeout: "
         << thread.ordered_timers.begin()->get_relative_timeout() << std::endl;
      for (auto tspec : thread.ordered_timers) {
        os << "timer: " << tspec << std::endl;
      }
    } else {
      os << "timers list is EMPTY" << std::endl;
    }
    return os;
  };

private:
  /**
   * @brief	Initialize data structures for running TX and RX threads
   *
   * Opens an epoll socket and registers file read descriptor for wake up
   * pipes for TX and RX thread. Does not prepare the Radius socket itself.
   * See radius_open() for adding the Radius socket to the running event
   * loop. Starts TX and RX loop.
   */
  void initialize();

  /**
   * @brief	Release data structures allocated for worker thread
   *
   * Closes Radius socket and TX and RX pipe file descriptors. Stops TX and RX
   * threads.
   */
  void release();

  /**
   * @brief	Static function for starting thread for this
   * instance
   */
  static void *start_loop(void *arg) {
    return static_cast<cthread *>(arg)->run_loop();
  };

  /**
   * @brief	Main loop for RX thread
   */
  void *run_loop();

  /**
   * @brief Get environment for file descriptor
   */
  cthread_env &env(int fd) {
    AcquireReadLock lock(tlock);
    std::map<int, fd_priv_data_t>::const_iterator pos;
    if ((pos = fds.find(fd)) == fds.end()) {
      throw eThreadNotFound(
          "file descriptor to thread environment mapping not found");
    }
    return cthread_env::env(pos->second.env);
  };

  /**
   * @brief Get environment for ctimer_env*
   */
  cthread_env &env(ctimer_env *env) {
    return cthread_env::env(dynamic_cast<cthread_env *>(env));
  };

  /**
   * @brief Handle wakeup events
   */
  void handle_wakeup();

private:
  /* thread pool strategy:
   * we have a pipeline of classes run by multiple threads
   *
   * crofbase (1) => crofctl/crofdpt => crofchan (1) => crofconn (1) => crofsock
   * (2)
   *
   * crofbase, crofchan: a single management thread for running garbage
   * collection, etc.
   * crofconn: a single thread for running the application handlers (e.g.,
   * handle_packet_out)
   * crofsock: a pair of threads dedicated to RX/TX operations on the underlying
   * socket
   *
   * we make the number of overall IO and handler threads configurable and add a
   * single thread ("thread(0)") for management purposes.
   *
   * method get_io_thread_num_from_pool: returns in a round robin policy thread
   * numbers
   * for I/O operations
   *
   * method get_hnd_thread_num_from_pool: returns in a round robin policy thread
   * numbers
   * for application handler operations
   */

  // thread pool
  static std::atomic_bool pool_initialized;
  static std::map<uint32_t, cthread *> pool;
  static crwlock pool_lock;
  // default management thread
  // number of threads dedicated to crofsock's IO operations
  static uint32_t pool_num_mgt_threads;
  static const uint32_t DEFAULT_POOL_NUM_MGT_THREADS = 1;
  static uint32_t pool_mgt_loop_index;
  // number of threads dedicated to crofsock's IO operations
  static uint32_t pool_num_io_threads;
  static const uint32_t DEFAULT_POOL_NUM_IO_THREADS = 8;
  static uint32_t pool_io_loop_index;
  // number of threads dedicated to crofconn's handler threads
  static uint32_t pool_num_hnd_threads;
  static const uint32_t DEFAULT_POOL_NUM_HND_THREADS = 4;
  static uint32_t pool_hnd_loop_index;

  // thread name
  std::string thread_name;

  // thread number
  uint32_t thread_num;

  // true: continue to run worker thread
  std::atomic_bool running;

  // thread related variables
  int event_fd;         // event fd for worker thread
  pthread_t thread_tid; // pthread_t for worker thread
  // bool				run_thread;	// true: continue to run
  // worker
  // thread
  int retval; // worker thread return value
  int epfd;   // worker thread epoll fd

  crwlock tlock; // thread lock

  class fd_priv_data_t {
  public:
    uint32_t events;
    cthread_env *env;
    fd_priv_data_t(uint32_t events = 0, cthread_env *env = nullptr)
        : events(events), env(env){};
    fd_priv_data_t(const fd_priv_data_t &priv_data) { *this = priv_data; };
    fd_priv_data_t &operator=(const fd_priv_data_t &priv_data) {
      if (this == &priv_data)
        return *this;
      events = priv_data.events;
      env = priv_data.env;
      return *this;
    };
  };

  std::map<int, fd_priv_data_t> fds; // set of registered file descriptors
  std::set<ctimer> ordered_timers;   // ordered set of timers
  std::set<cthread_env *> wakeups; // set of cthread_env instances to be called

  enum thread_state_t {
    STATE_IDLE = 0,
    STATE_RUNNING = 1,
  };

  enum thread_state_t state;
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_CTHREAD_HPP_ */
