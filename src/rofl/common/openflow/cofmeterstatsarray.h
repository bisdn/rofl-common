/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterstatsarray.h
 *
 *  Created on: 27.05.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFMETERSTATSARRAY_H
#define ROFL_COMMON_OPENFLOW_COFMETERSTATSARRAY_H 1

#include <iostream>
#include <list>
#include <map>

#include "rofl/common/locking.hpp"
#include "rofl/common/openflow/cofmeterstats.h"

namespace rofl {
namespace openflow {

class cofmeterstatsarray {
public:
  /**
   *
   */
  ~cofmeterstatsarray(){};

  /**
   *
   */
  cofmeterstatsarray(uint8_t ofp_version = OFP_VERSION_UNKNOWN)
      : ofp_version(ofp_version){};

  /**
   *
   */
  cofmeterstatsarray(const cofmeterstatsarray &array) { *this = array; };

  /**
   *
   */
  cofmeterstatsarray &operator=(const cofmeterstatsarray &array) {
    if (this == &array)
      return *this;

    this->array.clear();

    ofp_version = array.ofp_version;
    for (auto it : array.array) {
      add_meter_stats(it.first) = array.get_meter_stats(it.first);
    }

    return *this;
  };

  /**
   *
   */
  bool operator==(const cofmeterstatsarray &array) {
    if (ofp_version != array.ofp_version)
      return false;

    if (this->array.size() != array.array.size())
      return false;

    for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator it =
             array.array.begin();
         it != array.array.end(); ++it) {
      if (not(this->array[it->first] == it->second))
        return false;
    }

    return true;
  };

  /**
   *
   */
  cofmeterstatsarray &operator+=(const cofmeterstatsarray &array) {
    unsigned int index = 0;

    for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator it =
             array.array.begin();
         it != array.array.end(); ++it) {
      while (this->array.find(index) != this->array.end()) {
        index++;
      }
      this->array[index] = it->second;
    }

    return *this;
  };

public:
  /**
   *
   */
  uint8_t get_version() const { return ofp_version; };

  /**
   *
   */
  cofmeterstatsarray &set_version(uint8_t ofp_version) {
    this->ofp_version = ofp_version;
    for (auto it : array) {
      it.second.set_version(ofp_version);
    }
    return *this;
  };

public:
  /**
   *
   */
  std::list<uint32_t> keys() const {
    AcquireReadLock rwlock(array_lock);
    std::list<uint32_t> ids;
    for (auto it : array) {
      ids.push_back(it.first);
    }
    return ids;
  };

  /**
   *
   */
  size_t size() const {
    AcquireReadLock lock(array_lock);
    return array.size();
  };

  /**
   *
   */
  cofmeterstatsarray &clear() {
    AcquireReadWriteLock lock(array_lock);
    array.clear();
    return *this;
  };

public:
  /**
   *
   */
  cofmeter_stats_reply &add_meter_stats(unsigned int index) {
    AcquireReadWriteLock lock(array_lock);
    if (array.find(index) != array.end()) {
      array.erase(index);
    }
    return (array[index] = cofmeter_stats_reply(ofp_version));
  };

  /**
   *
   */
  cofmeter_stats_reply &set_meter_stats(unsigned int index) {
    AcquireReadWriteLock lock(array_lock);
    if (array.find(index) == array.end()) {
      array[index] = cofmeter_stats_reply(ofp_version);
    }
    return array[index];
  };

  /**
   *
   */
  const cofmeter_stats_reply &get_meter_stats(unsigned int index) const {
    AcquireReadLock lock(array_lock);
    if (array.find(index) == array.end()) {
      throw eRofMeterStatsNotFound();
    }
    return array.at(index);
  };

  /**
   *
   */
  bool drop_meter_stats(unsigned int index) {
    AcquireReadWriteLock lock(array_lock);
    if (array.find(index) == array.end()) {
      return false;
    }
    array.erase(index);
    return true;
  };

  /**
   *
   */
  bool has_meter_stats(unsigned int index) const {
    AcquireReadLock lock(array_lock);
    return (not(array.find(index) == array.end()));
  };

public:
  /**
   *
   */
  size_t length() const;

  /**
   *
   */
  void pack(uint8_t *buf = (uint8_t *)0, size_t buflen = 0);

  /**
   *
   */
  void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmeterstatsarray const &mstats) {
    os << "<cofmeterstatsarray #meter-stats:" << (int)mstats.array.size()
       << " >" << std::endl;

    for (std::map<unsigned int, cofmeter_stats_reply>::const_iterator it =
             mstats.array.begin();
         it != mstats.array.end(); ++it) {
      os << it->second;
    }
    return os;
  }

private:
  uint8_t ofp_version;
  std::map<unsigned int, cofmeter_stats_reply> array;
  mutable rofl::crwlock array_lock;
};

}; // namespace openflow
}; // namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFMETERSTATSARRAY_H */
