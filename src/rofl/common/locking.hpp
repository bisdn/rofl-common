/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * locking.hpp
 *
 *  Created on: Apr 25, 2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_LOCKING_HPP_
#define SRC_ROFL_COMMON_LOCKING_HPP_

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "rofl/common/exception.hpp"

namespace rofl {

class crwlock {
public:
  mutable pthread_rwlock_t rwlock;

public:
  ~crwlock() {
    if (pthread_rwlock_destroy(&rwlock) < 0) {
      kill(getpid(), SIGINT);
    }
  };
  crwlock() {
    if (pthread_rwlock_init(&rwlock, NULL) < 0) {
      throw eSysCall("pthread_rwlock_init syscall failed")
          .set_func(__PRETTY_FUNCTION__)
          .set_line(__LINE__);
    }
  };
};

class AcquireReadLock {
  pthread_rwlock_t *rwlock;

public:
  ~AcquireReadLock() {
    if (pthread_rwlock_unlock(rwlock) < 0) {
      kill(getpid(), SIGINT);
    }
  };
  AcquireReadLock(const crwlock &lock) : rwlock(&(lock.rwlock)) {
    if (pthread_rwlock_rdlock(rwlock) < 0) {
      throw eSysCall("pthread_rwlock_rdlock syscall failed")
          .set_func(__PRETTY_FUNCTION__)
          .set_line(__LINE__);
    }
  };
};

class AcquireReadWriteLock {
  pthread_rwlock_t *rwlock;

public:
  ~AcquireReadWriteLock() {
    if (pthread_rwlock_unlock(rwlock) < 0) {
      kill(getpid(), SIGINT);
    }
  };
  AcquireReadWriteLock(const crwlock &lock) : rwlock(&(lock.rwlock)) {
    if (pthread_rwlock_wrlock(rwlock) < 0) {
      throw eSysCall("pthread_rwlock_wrlock syscall failed")
          .set_func(__PRETTY_FUNCTION__)
          .set_line(__LINE__);
    }
  };
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_LOCKING_HPP_ */
