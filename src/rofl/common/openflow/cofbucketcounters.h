/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofbucketcounters.h
 *
 *  Created on: 16.03.2014
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFBUCKETCOUNTERS_H
#define ROFL_COMMON_OPENFLOW_COFBUCKETCOUNTERS_H 1

#include <inttypes.h>

#include <iostream>
#include <map>

#include "rofl/common/locking.hpp"
#include "rofl/common/openflow/cofbucketcounter.h"

namespace rofl {
namespace openflow {

class cofbucket_counters {
public:
  /**
   *
   */
  virtual ~cofbucket_counters(){};

  /**
   *
   */
  cofbucket_counters(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : ofp_version(ofp_version){};

  /**
   *
   */
  cofbucket_counters(const cofbucket_counters &bcs) { *this = bcs; };

  /**
   *
   */
  cofbucket_counters &operator=(const cofbucket_counters &bcs) {
    if (this == &bcs)
      return *this;

    clear();
    ofp_version = bcs.ofp_version;
    for (std::map<uint32_t, cofbucket_counter>::const_iterator it =
             bcs.bucketcounters.begin();
         it != bcs.bucketcounters.end(); ++it) {
      add_bucket_counter(it->first) = it->second;
    }

    return *this;
  };

  /**
   *
   */
  bool operator==(const cofbucket_counters &bcs) const {
    if (bucketcounters.size() != bcs.bucketcounters.size())
      return false;

    for (std::map<uint32_t, cofbucket_counter>::const_iterator it =
             bcs.bucketcounters.begin();
         it != bcs.bucketcounters.end(); ++it) {
      if (not(bucketcounters.at(it->first) == it->second))
        return false;
    }

    return true;
  };

public:
  /**
   *
   */
  cofbucket_counters &set_version(uint8_t ofp_version) {
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
  void clear() {
    AcquireReadWriteLock rwlock(bcs_lock);
    bucketcounters.clear();
  };

  /**
   *
   */
  cofbucket_counter &add_bucket_counter(uint32_t bucket_counter_id) {
    AcquireReadWriteLock rwlock(bcs_lock);
    if (bucketcounters.find(bucket_counter_id) != bucketcounters.end()) {
      bucketcounters.erase(bucket_counter_id);
    }
    return (bucketcounters[bucket_counter_id] = cofbucket_counter(ofp_version));
  };

  /**
   *
   */
  cofbucket_counter &set_bucket_counter(uint32_t bucket_counter_id) {
    AcquireReadWriteLock rwlock(bcs_lock);
    if (bucketcounters.find(bucket_counter_id) == bucketcounters.end()) {
      bucketcounters[bucket_counter_id] = cofbucket_counter(ofp_version);
    }
    return bucketcounters[bucket_counter_id];
  };

  /**
   *
   */
  const cofbucket_counter &
  get_bucket_counter(uint32_t bucket_counter_id) const {
    AcquireReadLock rwlock(bcs_lock);
    if (bucketcounters.find(bucket_counter_id) == bucketcounters.end()) {
      throw eBucketCounterNotFound();
    }
    return bucketcounters.at(bucket_counter_id);
  };

  /**
   *
   */
  bool drop_bucket_counter(uint32_t bucket_counter_id) {
    AcquireReadWriteLock rwlock(bcs_lock);
    if (bucketcounters.find(bucket_counter_id) == bucketcounters.end()) {
      return false;
    }
    bucketcounters.erase(bucket_counter_id);
    return true;
  };

  /**
   *
   */
  bool has_bucket_counter(uint32_t bucket_counter_id) const {
    AcquireReadLock rwlock(bcs_lock);
    return (
        not(bucketcounters.find(bucket_counter_id) == bucketcounters.end()));
  };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofbucket_counters &bcs) {
    os << "<cofbucket_counters #bucket-counters:" << bcs.bucketcounters.size()
       << " >" << std::endl;

    for (std::map<uint32_t, cofbucket_counter>::const_iterator it =
             bcs.bucketcounters.begin();
         it != bcs.bucketcounters.end(); ++it) {
      os << it->second;
    }
    return os;
  };

private:
  uint8_t ofp_version;
  std::map<uint32_t, cofbucket_counter> bucketcounters;
  rofl::crwlock bcs_lock;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFBUCKETCOUNTERS_H */
