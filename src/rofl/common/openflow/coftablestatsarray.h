/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coftables.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFTABLESTATSARRAY_H
#define ROFL_COMMON_OPENFLOW_COFTABLESTATSARRAY_H 1

#include <iostream>
#include <list>
#include <map>

#include "rofl/common/locking.hpp"
#include "rofl/common/openflow/coftablestats.h"

namespace rofl {
namespace openflow {

class coftablestatsarray {
public:
  /**
   *
   */
  ~coftablestatsarray(){};

  /**
   *
   */
  coftablestatsarray(uint8_t ofp_version = OFP_VERSION_UNKNOWN)
      : ofp_version(ofp_version){};

  /**
   *
   */
  coftablestatsarray(const coftablestatsarray &tables) { *this = tables; };

  /**
   *
   */
  coftablestatsarray &operator=(const coftablestatsarray &tables) {
    if (this == &tables)
      return *this;

    AcquireReadWriteLock lock(array_lock);

    this->array.clear();

    ofp_version = tables.ofp_version;
    for (std::map<uint8_t, coftable_stats_reply>::const_iterator it =
             tables.array.begin();
         it != tables.array.end(); ++it) {
      this->array[it->first] = it->second;
    }

    return *this;
  };

  /**
   *
   */
  bool operator==(const coftablestatsarray &tables) const {
    AcquireReadLock lock(array_lock);

    if (ofp_version != tables.ofp_version)
      return false;

    if (array.size() != tables.array.size())
      return false;

    for (std::map<uint8_t, coftable_stats_reply>::const_iterator it =
             tables.array.begin();
         it != tables.array.end(); ++it) {
      if (not(array.at(it->first) == it->second))
        return false;
    }

    return true;
  };

  /**
   *
   */
  coftablestatsarray &operator+=(const coftablestatsarray &tables) {
    /*
     * this may replace existing table descriptions
     */
    AcquireReadWriteLock lock(array_lock);
    for (std::map<uint8_t, coftable_stats_reply>::const_iterator it =
             tables.array.begin();
         it != tables.array.end(); ++it) {
      this->array[it->first] = it->second;
    }

    return *this;
  };

public:
  /**
   *
   */
  coftablestatsarray &set_version(uint8_t ofp_version) {
    this->ofp_version = ofp_version;
    AcquireReadLock lock(array_lock);
    for (auto it : array) {
      it.second.set_version(ofp_version);
    }
    return *this;
  };

  /**
   *
   */
  uint8_t get_version() const { return ofp_version; };

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
  coftablestatsarray &clear() {
    AcquireReadWriteLock lock(array_lock);
    array.clear();
    return *this;
  };

public:
  /**
   *
   */
  coftable_stats_reply &add_table_stats(uint8_t table_id) {
    AcquireReadWriteLock lock(array_lock);
    if (array.find(table_id) != array.end()) {
      array.erase(table_id);
    }
    return (array[table_id] = coftable_stats_reply(ofp_version));
  };

  /**
   *
   */
  coftable_stats_reply &set_table_stats(uint8_t table_id) {
    AcquireReadWriteLock lock(array_lock);
    if (array.find(table_id) == array.end()) {
      array[table_id] = coftable_stats_reply(ofp_version);
    }
    return array[table_id];
  };

  /**
   *
   */
  const coftable_stats_reply &get_table_stats(uint8_t table_id) const {
    AcquireReadLock lock(array_lock);
    if (array.find(table_id) == array.end()) {
      throw eTableStatsNotFound();
    }
    return array.at(table_id);
  };

  /**
   *
   */
  bool drop_table_stats(uint8_t table_id) {
    AcquireReadWriteLock lock(array_lock);
    if (array.find(table_id) == array.end()) {
      return false;
    }
    array.erase(table_id);
    return true;
  };

  /**
   *
   */
  bool has_table_stats(uint8_t table_id) const {
    AcquireReadLock lock(array_lock);
    return (not(array.find(table_id) == array.end()));
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
                                  coftablestatsarray const &tablestatsarray) {
    os << "<coftablestatsarray #tables:" << (int)tablestatsarray.array.size()
       << " >" << std::endl;

    for (std::map<uint8_t, coftable_stats_reply>::const_iterator it =
             tablestatsarray.array.begin();
         it != tablestatsarray.array.end(); ++it) {
      os << it->second;
    }
    return os;
  }

private:
  uint8_t ofp_version;
  std::map<uint8_t, coftable_stats_reply> array;
  rofl::crwlock array_lock;
};

}; // namespace openflow
}; // namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFTABLESTATSARRAY_H */
