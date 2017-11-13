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

class ctimer;

class ctimer_env {
  friend class ctimer;

public:
  ctimer_env() {}
  virtual ~ctimer_env() {}
};

class ctimer {
public:
  /**
   *
   */
  static const ctimespec now(cclockid clk_id = cclockid(CLOCK_MONOTONIC)) {
    return ctimespec::now(clk_id);
  };

public:
  /**
   *
   */
  ~ctimer(){};

  /**
   *
   */
  ctimer(ctimer_env *timer_env = nullptr, uint32_t timer_id = 0,
         const ctimespec &tspec = ctimespec(::timespec{0, 0}))
      : timer_env(timer_env), timer_id(timer_id), tspec(tspec){};

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

    timer_env = ts.timer_env;
    timer_id = ts.timer_id;
    tspec = ts.tspec;

    return *this;
  };

  bool operator<(const ctimer &t) const {
    if (tspec == t.tspec) {
      return timer_id < t.timer_id;
    } else {
      return tspec < t.tspec;
    }
  }

  /**
   *
   */
  ctimer_env *env() const { return timer_env; };

  /**
   *
   */
  const ctimer_env *get_timer_env() const { return timer_env; };

  /**
   *
   */
  uint32_t get_timer_id() const { return timer_id; };

  /**
   *
   */
  const ctimespec &get_tspec() const { return tspec; };

  /**
   *
   */
  void expire_in(time_t tv_sec = 0, long tv_nsec = 0) {
    tspec.expire_in(tv_sec, tv_nsec);
  };

  /**
   *
   */
  int get_relative_timeout() const {
    return get_tspec().get_relative_timeout();
  };

public:
  /**
   *
   */
  friend std::ostream &operator<<(std::ostream &os, const ctimer &ts) {
    os << "<ctimer: >";
    return os;
  };

private:
  ctimer_env *timer_env;
  uint32_t timer_id;
  ctimespec tspec;
};

/**
 *
 */
class ctimer_find_by_timer_env {
  ctimer_env *timer_env;

public:
  ctimer_find_by_timer_env(ctimer_env *timer_env) : timer_env(timer_env) {}
  bool operator()(const ctimer &t) {
    return ((t.get_timer_env() == timer_env));
  };
};

/**
 *
 */
class ctimer_find_by_timer_env_and_id {
  ctimer_env *timer_env;
  uint32_t timer_id;

public:
  ctimer_find_by_timer_env_and_id(ctimer_env *timer_env, uint32_t timer_id)
      : timer_env(timer_env), timer_id(timer_id) {}
  bool operator()(const ctimer &t) {
    return ((t.get_timer_env() == timer_env) && (t.get_timer_id() == timer_id));
  };
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_CTIMER_HPP_ */
