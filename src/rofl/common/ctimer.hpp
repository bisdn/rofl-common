/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimer.hpp
 *
 *  Created on: 25.05.2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_CTIMER_HPP_
#define SRC_ROFL_COMMON_CTIMER_HPP_

#include <algorithm>
#include <errno.h>
#include <list>
#include <ostream>
#include <time.h>

#include "rofl/common/callbacks.hpp"
#include "rofl/common/ctimespec.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"

namespace rofl {

class eTimerBase : public exception {
public:
  eTimerBase(const std::string &__arg) : exception(__arg){};
};

class eTimerSysCall : public eTimerBase {
public:
  eTimerSysCall(const std::string &__arg) : eTimerBase(__arg){};
};

class ctimer {
public:
  /**
   *
   */
  ~ctimer(){};

  /**
   *
   */
  ctimer(cthread_timeout_event *e = nullptr, uint32_t timer_id = 0,
         const ctimespec &tspec = ctimespec(::timespec{0, 0}))
      : timer_id(timer_id), tspec(tspec), e(e){};

  /**
   *
   */
  ctimer(const ctimer &ts) { *this = ts; };

  /**
   *
   */
  ctimer &operator=(const ctimer &ts) {
    if (this == &ts)
      return *this;

    timer_id = ts.timer_id;
    tspec = ts.tspec;
    e = ts.e;

    return *this;
  };

  bool operator<(const ctimer &t) const {
    if (tspec != t.tspec)
      return tspec < t.tspec;
    // tspec == t.tspec
    if (timer_id != t.timer_id)
      return timer_id < t.timer_id;
    // timer_id == t.timer_id
    return e < t.e;
  }

  uint32_t get_timer_id() const { return timer_id; }

  const ctimespec &get_tspec() const { return tspec; }

  const ctimespec &get_created() const { return created; }

  cthread_timeout_event *get_callback() { return e; }

  const cthread_timeout_event *get_callback() const { return e; }

  /**
   *
   */
  void expire_in(time_t tv_sec = 0, long tv_nsec = 0) {
    tspec.expire_in(tv_sec, tv_nsec);
  }

  /**
   *
   */
  int get_relative_timeout() const {
    return get_tspec().get_relative_timeout();
  }

  /**
   *
   */
  friend std::ostream &operator<<(std::ostream &os, const ctimer &ts) {
    os << "<ctimer: >";
    return os;
  }

private:
  uint32_t timer_id;
  ctimespec tspec;
  ctimespec created;
  cthread_timeout_event *e;
};

/**
 *
 */
class ctimer_find_by_timer_and_event {
  cthread_timeout_event *e;
  uint32_t timer_id;

public:
  ctimer_find_by_timer_and_event(cthread_timeout_event *e, uint32_t timer_id)
      : e(e), timer_id(timer_id) {}
  bool operator()(const ctimer &t) {
    return (t.get_callback() == e && t.get_timer_id() == timer_id);
  }
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_CTIMER_HPP_ */
