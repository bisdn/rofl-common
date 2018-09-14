/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofqueues.h
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFQUEUESTATSARRAY_H
#define ROFL_COMMON_OPENFLOW_COFQUEUESTATSARRAY_H 1

#include <iostream>
#include <list>
#include <map>

#include "rofl/common/locking.hpp"
#include "rofl/common/openflow/cofqueuestats.h"

namespace rofl {
namespace openflow {

class cofqueuestatsarray {
public:
  /**
   *
   */
  ~cofqueuestatsarray(){};

  /**
   *
   */
  cofqueuestatsarray(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : ofp_version(ofp_version){};

  /**
   *
   */
  cofqueuestatsarray(const cofqueuestatsarray &queues) { *this = queues; };

  /**
   *
   */
  cofqueuestatsarray &operator=(const cofqueuestatsarray &queues) {
    if (this == &queues)
      return *this;

    AcquireReadWriteLock lock(array_lock);

    this->array.clear();

    ofp_version = queues.ofp_version;
    for (std::map<uint32_t,
                  std::map<uint32_t, cofqueue_stats_reply>>::const_iterator it =
             queues.array.begin();
         it != queues.array.end(); ++it) {
      for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator jt =
               it->second.begin();
           jt != it->second.end(); ++jt) {
        this->array[it->first][jt->first] = jt->second;
      }
    }

    return *this;
  };

  /**
   *
   */
  bool operator==(const cofqueuestatsarray &queues) {
    AcquireReadLock lock(array_lock);

    if (ofp_version != queues.ofp_version)
      return false;

    if (array.size() != queues.array.size())
      return false;

    for (std::map<uint32_t,
                  std::map<uint32_t, cofqueue_stats_reply>>::const_iterator it =
             queues.array.begin();
         it != queues.array.end(); ++it) {

      if (array[it->first].size() != it->second.size())
        return false;

      for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator jt =
               it->second.begin();
           jt != it->second.end(); ++jt) {
        if (not(array[it->first][jt->first] == jt->second))
          return false;
      }
    }

    return true;
  };

  /**
   *
   */
  cofqueuestatsarray &operator+=(const cofqueuestatsarray &queues) {
    /*
     * this may replace existing queue descriptions
     */
    AcquireReadWriteLock lock(array_lock);
    for (std::map<uint32_t,
                  std::map<uint32_t, cofqueue_stats_reply>>::const_iterator it =
             queues.array.begin();
         it != queues.array.end(); ++it) {
      for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator jt =
               it->second.begin();
           jt != it->second.end(); ++jt) {
        this->array[it->first][jt->first] = jt->second;
      }
    }

    return *this;
  };

public:
  /**
   *
   */
  cofqueuestatsarray &set_version(uint8_t ofp_version) {
    this->ofp_version = ofp_version;
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
  std::list<uint32_t> keys(uint32_t port_id) const {
    AcquireReadLock rwlock(array_lock);
    std::list<uint32_t> ids;
    for (auto it : array.at(port_id)) {
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
  cofqueuestatsarray &clear() {
    AcquireReadWriteLock lock(array_lock);
    array.clear();
    return *this;
  };

public:
  /**
   *
   */
  cofqueue_stats_reply &add_queue_stats(uint32_t port_no, uint32_t queue_id) {
    AcquireReadWriteLock lock(array_lock);
    if (array[port_no].find(queue_id) != array[port_no].end()) {
      array[port_no].erase(queue_id);
    }
    return (array[port_no][queue_id] = cofqueue_stats_reply(ofp_version));
  };

  /**
   *
   */
  cofqueue_stats_reply &set_queue_stats(uint32_t port_no, uint32_t queue_id) {
    AcquireReadWriteLock lock(array_lock);
    if (array[port_no].find(queue_id) == array[port_no].end()) {
      array[port_no][queue_id] = cofqueue_stats_reply(ofp_version);
    }
    return array[port_no][queue_id];
  };

  /**
   *
   */
  const cofqueue_stats_reply &get_queue_stats(uint32_t port_no,
                                              uint32_t queue_id) const {
    AcquireReadLock lock(array_lock);
    if (array.find(port_no) == array.end()) {
      throw eQueueStatsNotFound();
    }
    if (array.at(port_no).find(queue_id) == array.at(port_no).end()) {
      throw eQueueStatsNotFound();
    }
    return array.at(port_no).at(queue_id);
  };

  /**
   *
   */
  bool drop_queue_stats(uint32_t port_no, uint32_t queue_id) {
    AcquireReadWriteLock lock(array_lock);
    if (array[port_no].find(queue_id) == array[port_no].end()) {
      return false;
    }
    array[port_no].erase(queue_id);
    return true;
  };

  /**
   *
   */
  bool has_queue_stats(uint32_t port_no, uint32_t queue_id) const {
    AcquireReadLock lock(array_lock);
    if (array.find(port_no) == array.end()) {
      return false;
    }
    return (not(array.at(port_no).find(queue_id) == array.at(port_no).end()));
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
                                  cofqueuestatsarray const &groupstatsarray) {
    os << "<cofqueuestatsarray #ports:" << (int)groupstatsarray.array.size()
       << " >" << std::endl;
    AcquireReadLock lock(groupstatsarray.array_lock);
    for (std::map<uint32_t,
                  std::map<uint32_t, cofqueue_stats_reply>>::const_iterator jt =
             groupstatsarray.array.begin();
         jt != groupstatsarray.array.end(); ++jt) {
      os << "<portno: " << (int)jt->first
         << " #queues: " << (int)jt->second.size() << " >" << std::endl;

      for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator it =
               jt->second.begin();
           it != jt->second.end(); ++it) {
        os << it->second;
      }
    }
    return os;
  }

private:
  uint8_t ofp_version;
  std::map<uint32_t, std::map<uint32_t, cofqueue_stats_reply>> array;
  mutable rofl::crwlock array_lock;
};

}; // namespace openflow
}; // namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFQUEUESTATSARRAY_H */
