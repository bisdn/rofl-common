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

class cthread; // forward declaration

class cthread_env {
  friend class cthread;

public:
  cthread_env() {}
  virtual ~cthread_env() {}

protected:
  virtual void handle_wakeup(cthread &thread) = 0;
  virtual void handle_timeout(cthread &thread, uint32_t timer_id) = 0;
  virtual void handle_read_event(cthread &thread, int fd) = 0;
  virtual void handle_write_event(cthread &thread, int fd) = 0;
};

class cthread {
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
  cthread(cthread_env *env) : env(env), state(STATE_IDLE) { initialize(); };

public:
  /**
   *
   */
  pthread_t get_thread_id() const { return tid; };

  /**
   * @brief	Wake up RX thread via rx pipe
   */
  void wakeup();

  /**
   * @brief	Register file descriptor
   */
  void add_fd(int fd, bool exception = false, bool edge_triggered = true);

  /**
   * @brief	Deregister file descriptor
   */
  void drop_fd(int fd, bool exception = false);

  /**
   * @brief	Add file descriptor to set of observed fds
   */
  void add_read_fd(int fd, bool exception = true, bool edge_triggered = true);

  /**
   * @brief	Drop file descriptor from set of observed fds
   */
  void drop_read_fd(int fd, bool exception = true);

  /**
   * @brief	Add file descriptor to set of observed fds
   */
  void add_write_fd(int fd, bool exception = true, bool edge_triggered = true);

  /**
   * @brief	Drop file descriptor from set of observed fds
   */
  void drop_write_fd(int fd, bool exception = true);

public:
  /**
   *
   */
  void clear_timers();

  /**
   *
   */
  bool add_timer(uint32_t timer_id, const ctimespec &tspec);

  /**
   *
   */
  const ctimer &get_timer(uint32_t timer_id) const;

  /**
   *
   */
  bool drop_timer(uint32_t timer_id);

  /**
   *
   */
  bool has_timer(uint32_t timer_id) const;

public:
  /**
   * @brief	Starts worker thread
   *
   * Sets run flag run_rx_thread to true and calls
   * syscall pthread_create() for starting worker thread.
   */
  void start();

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
    os << "cthread, tid: " << &thread << std::endl;
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
   * @brief	Static function for starting RX thread for this cradsock
   * instance
   */
  static void *start_loop(void *arg) {
    return static_cast<cthread *>(arg)->run_loop();
  };

  /**
   * @brief	Main loop for RX thread
   */
  void *run_loop();

private:
  // true: continue to run worker thread
  std::atomic_bool running;

  // thread environment
  cthread_env *env;

  // thread related variables
  static const int PIPE_READ_FD;
  static const int PIPE_WRITE_FD;
  int event_fd;  // event fd for worker thread
  pthread_t tid; // pthread_t for worker thread
  // bool				run_thread;	// true: continue to run
  // worker
  // thread
  int retval; // worker thread return value
  int epfd;   // worker thread epoll fd

  crwlock tlock; // thread lock

  std::map<int, uint32_t> fds;     // set of registered file descriptors
  std::set<ctimer> ordered_timers; // ordered set of timers

  enum thread_state_t {
    STATE_IDLE = 0,
    STATE_RUNNING = 1,
  };

  enum thread_state_t state;
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_CTHREAD_HPP_ */
