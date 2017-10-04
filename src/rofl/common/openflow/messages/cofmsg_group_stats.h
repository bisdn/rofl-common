/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_group_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 26.07.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_GROUP_STATS_H_
#define COFMSG_GROUP_STATS_H_ 1

#include "rofl/common/openflow/cofgroupstats.h"
#include "rofl/common/openflow/cofgroupstatsarray.h"
#include "rofl/common/openflow/messages/cofmsg_stats.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_group_stats_request : public cofmsg_stats_request {
public:
  /**
   *
   */
  virtual ~cofmsg_group_stats_request();

  /**
   *
   */
  cofmsg_group_stats_request(
      uint8_t version = 0, uint32_t xid = 0, uint16_t stats_flags = 0,
      const rofl::openflow::cofgroup_stats_request &group_stats =
          rofl::openflow::cofgroup_stats_request());

  /**
   *
   */
  cofmsg_group_stats_request(const cofmsg_group_stats_request &msg);

  /**
   *
   */
  cofmsg_group_stats_request &operator=(const cofmsg_group_stats_request &msg);

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf = nullptr, size_t buflen = 0);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  const rofl::openflow::cofgroup_stats_request &get_group_stats() const {
    return group_stats;
  };

  /**
   *
   */
  rofl::openflow::cofgroup_stats_request &set_group_stats() {
    return group_stats;
  };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofmsg_group_stats_request &msg) {
    os << dynamic_cast<const cofmsg_stats_request &>(msg);
    os << "<cofmsg_group_stats_request >" << std::endl;

    os << msg.group_stats;
    return os;
  };

  virtual std::string str() const {
    std::stringstream ss;
    ss << cofmsg_stats_request::str() << "-Group-Stats-Request- ";
    return ss.str();
  };

private:
  rofl::openflow::cofgroup_stats_request group_stats;
};

/**
 *
 */
class cofmsg_group_stats_reply : public cofmsg_stats_reply {
public:
  /**
   *
   */
  virtual ~cofmsg_group_stats_reply();

  /**
   *
   */
  cofmsg_group_stats_reply(uint8_t of_version = 0, uint32_t xid = 0,
                           uint16_t flags = 0,
                           const rofl::openflow::cofgroupstatsarray &groups =
                               rofl::openflow::cofgroupstatsarray());

  /**
   *
   */
  cofmsg_group_stats_reply(const cofmsg_group_stats_reply &msg);

  /**
   *
   */
  cofmsg_group_stats_reply &operator=(const cofmsg_group_stats_reply &msg);

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf = nullptr, size_t buflen = 0);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  const rofl::openflow::cofgroupstatsarray &get_group_stats_array() const {
    return groupstatsarray;
  };

  /**
   *
   */
  rofl::openflow::cofgroupstatsarray &set_group_stats_array() {
    return groupstatsarray;
  };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofmsg_group_stats_reply &msg) {
    os << dynamic_cast<const cofmsg_stats_reply &>(msg);
    os << "<cofmsg_group_stats_reply >" << std::endl;

    os << msg.groupstatsarray;
    return os;
  };

  virtual std::string str() const {
    std::stringstream ss;
    ss << cofmsg_stats_reply::str() << "-Group-Stats-Reply- ";
    return ss.str();
  };

private:
  rofl::openflow::cofgroupstatsarray groupstatsarray;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_GROUP_STATS_H_ */
