/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimespec.hpp
 *
 *  Created on: 25.05.2015
 *      Author: andi
 */

#ifndef SRC_ROFL_COMMON_CTIMESPEC_HPP_
#define SRC_ROFL_COMMON_CTIMESPEC_HPP_

#include <algorithm>
#include <errno.h>
#include <inttypes.h>
#include <list>
#include <ostream>
#include <time.h>

#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"

namespace rofl {

class eTimeSpecBase : public exception {
public:
  eTimeSpecBase(const std::string &__arg) : exception(__arg){};
};

class eTimeSpecSysCall : public eTimeSpecBase {
public:
  eTimeSpecSysCall(const std::string &__arg) : eTimeSpecBase(__arg){};
};

class cclockid {
public:
  /**
   *
   */
  ~cclockid(){};

  /**
   *
   */
  explicit cclockid(clockid_t clkid = CLOCK_MONOTONIC) : clkid(clkid){};

  /**
   *
   */
  cclockid(const cclockid &clockid) { *this = clockid; };

  /**
   *
   */
  cclockid &operator=(const cclockid &clockid) {
    if (this == &clockid)
      return *this;
    clkid = clockid.clkid;
    return *this;
  };

public:
  /**
   *
   */
  clockid_t get_clkid() const { return clkid; };

  /**
   *
   */
  cclockid &set_clkid(clockid_t clkid) {
    this->clkid = clkid;
    return *this;
  };

private:
  clockid_t clkid;
};

class ctimespec {
public:
  /**
   *
   */
  static const ctimespec
  now(const cclockid &clk_id = cclockid(CLOCK_MONOTONIC)) {
    return ctimespec(clk_id);
  };

public:
  /**
   *
   */
  ~ctimespec(){};

  /**
   *
   */
  ctimespec(const cclockid &clk_id = cclockid(CLOCK_MONOTONIC))
      : clk_id(clk_id) {
    get_time(clk_id);
  };

  /**
   *
   */
  ctimespec(time_t tv_sec, long tv_nsec,
            const cclockid &clk_id = cclockid(CLOCK_MONOTONIC))
      : clk_id(clk_id) {
    get_time(clk_id);
    expire_in(tv_sec, tv_nsec);
  };

  /**
   *
   */
  ctimespec(struct timespec tspec,
            const cclockid &clk_id = cclockid(CLOCK_MONOTONIC))
      : clk_id(clk_id) {
    this->tspec.tv_nsec = tspec.tv_nsec;
    this->tspec.tv_sec = tspec.tv_sec;
  };
  /**
   *
  */
  ctimespec(const ctimespec &ts) { *this = ts; };

  /**
   *
   */
  ctimespec &operator=(const ctimespec &ts) {
    if (this == &ts)
      return *this;

    clk_id = ts.clk_id;
    memcpy(&tspec, &(ts.tspec), sizeof(struct timespec));

    return *this;
  };

  /**
   *
   */
  ctimespec operator+(const ctimespec &t) const;

  /**
   *
   */
  ctimespec operator-(const ctimespec &t) const;

  /**
   *
   */
  ctimespec &operator+=(const ctimespec &t);

  /**
   *
   */
  ctimespec &operator-=(const ctimespec &t);

  /**
   *
   */
  bool operator==(const ctimespec &t) const;

  /**
   *
   */
  bool operator!=(const ctimespec &t) const;

  /**
   *
   */
  bool operator<(const ctimespec &t) const;

  /**
   *
   */
  bool operator<=(const ctimespec &t) const;

  /**
   *
   */
  bool operator>(const ctimespec &t) const;

  /**
   *
   */
  bool operator>=(const ctimespec &t) const;

public:
  /**
   *
   */
  struct timespec &set_tspec() {
    return tspec;
  };

  /**
   *
   */
  const struct timespec &get_tspec() const { return tspec; };

  /**
   *
   */
  ctimespec &get_time() {
    // get current time
    if (clock_gettime(clk_id.get_clkid(), &tspec) < 0) {
      switch (errno) {
      case EFAULT: {
        throw eTimeSpecSysCall("ctimespec::now() invalid struct timespec");
      } break;
      case EINVAL: {
        throw eTimeSpecSysCall("ctimespec::now() clockid_t is invalid");
      } break;
      default: {
        throw eTimeSpecSysCall("ctimespec::now() unknown internal error on "
                               "syscall clock_gettime()");
      };
      }
    }
    return *this;
  };

  /**
   *
   */
  ctimespec &get_time(const cclockid &clk_id = cclockid(CLOCK_MONOTONIC)) {
    // get current time
    if (clock_gettime(clk_id.get_clkid(), &tspec) < 0) {
      switch (errno) {
      case EFAULT: {
        throw eTimeSpecSysCall("ctimespec::now() invalid struct timespec");
      } break;
      case EINVAL: {
        throw eTimeSpecSysCall("ctimespec::now() clockid_t is invalid");
      } break;
      default: {
        throw eTimeSpecSysCall("ctimespec::now() unknown internal error on "
                               "syscall clock_gettime()");
      };
      }
    }
    return *this;
  };

  /**
   *
   */
  ctimespec &expire_in(time_t tv_sec = 0, long tv_nsec = 0) {
    get_time(clk_id);
    tv_nsec += tspec.tv_nsec;
    tspec.tv_sec += (tv_nsec / (long)1e9) + tv_sec;
    tspec.tv_nsec = (tv_nsec % (long)1e9);
    return *this;
  };

  /**
   *
   */
  int get_relative_timeout(
      const ctimespec &reference_time = ctimespec::now()) const {
    ctimespec delta = *this - reference_time;
    return (((int)1e3 * delta.tspec.tv_sec) + (delta.tspec.tv_nsec / (int)1e6));
  };

  /**
   *
   */
  bool is_expired(const ctimespec &reference_time = ctimespec::now()) const {
    return (*this <= reference_time);
  };

public:
  /**
   *
   */
  friend std::ostream &operator<<(std::ostream &os, const ctimespec &ts) {
    os << "<ctimespec timeout: " << (unsigned int)ts.tspec.tv_sec << "."
       << ts.tspec.tv_nsec << "s>";
    return os;
  };

  /**
   *
   */
  std::string str() const {
    std::stringstream ss;
    ss << tspec.tv_sec << ":" << tspec.tv_nsec;
    return ss.str();
  };

  /**
   *
   */
  std::string date() const {
    time_t t(get_tspec().tv_sec);
    char s[128];
    memset(s, 0, sizeof(s));
    ctime_r(&t, s);
    s[strnlen(s, sizeof(s)) - 1] = '\0';
    return s;
  };

private:
  static const long CC_TIMER_ONE_SECOND_S = 1;
  static const long CC_TIMER_ONE_SECOND_NS = 1000000000;

  crwlock tlock;
  cclockid clk_id;
  struct timespec tspec;
};

}; // end of namespace rofl

#endif /* SRC_ROFL_COMMON_CTIMESPEC_HPP_ */
