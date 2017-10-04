/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_meter_config_stats.h
 *
 *  Created on: 28.05.2014
 *  Revised on: 01.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_METER_CONFIG_STATS_H_
#define COFMSG_METER_CONFIG_STATS_H_ 1

#include <inttypes.h>

#include "rofl/common/openflow/cofmeterconfigarray.h"
#include "rofl/common/openflow/messages/cofmsg_stats.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_meter_config_stats_request : public cofmsg_stats_request {
public:
  /**
   *
   */
  virtual ~cofmsg_meter_config_stats_request();

  /**
   *
   */
  cofmsg_meter_config_stats_request(uint8_t version, uint32_t xid,
                                    uint16_t stats_flags, uint32_t meter_id);

  /**
   *
   */
  cofmsg_meter_config_stats_request(
      uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN, uint32_t xid = 0,
      uint16_t stats_flags = 0,
      const rofl::openflow::cofmeter_config_request &request =
          rofl::openflow::cofmeter_config_request());

  /**
   *
   */
  cofmsg_meter_config_stats_request(
      const cofmsg_meter_config_stats_request &msg);

  /**
   *
   */
  cofmsg_meter_config_stats_request &
  operator=(const cofmsg_meter_config_stats_request &msg);

public:
  /** returns length of packet in packed state
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
  const rofl::openflow::cofmeter_config_request &get_meter_config() const {
    return meter_config;
  };

  /**
   *
   */
  rofl::openflow::cofmeter_config_request &set_meter_config() {
    return meter_config;
  };

public:
  friend std::ostream &
  operator<<(std::ostream &os, const cofmsg_meter_config_stats_request &msg) {
    os << dynamic_cast<const cofmsg_stats_request &>(msg);
    os << "<cofmsg_meter_config_stats_request >" << std::endl;
    os << msg.get_meter_config();
    return os;
  };

  virtual std::string str() const {
    std::stringstream ss;
    ss << cofmsg_stats_request::str() << "-Meter-Config-Stats-Request- ";
    return ss.str();
  };

private:
  rofl::openflow::cofmeter_config_request meter_config;
};

/**
 *
 */
class cofmsg_meter_config_stats_reply : public cofmsg_stats_reply {
public:
  /**
   *
   */
  virtual ~cofmsg_meter_config_stats_reply();

  /**
   *
   */
  cofmsg_meter_config_stats_reply(
      uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN, uint32_t xid = 0,
      uint16_t stats_flags = 0,
      const rofl::openflow::cofmeterconfigarray &array =
          rofl::openflow::cofmeterconfigarray());

  /**
   *
   */
  cofmsg_meter_config_stats_reply(const cofmsg_meter_config_stats_reply &msg);

  /**
   *
   */
  cofmsg_meter_config_stats_reply &
  operator=(const cofmsg_meter_config_stats_reply &msg);

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
  const rofl::openflow::cofmeterconfigarray &get_meter_config_array() const {
    return array;
  };

  /**
   *
   */
  rofl::openflow::cofmeterconfigarray &set_meter_config_array() {
    return array;
  };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmsg_meter_config_stats_reply const &msg) {
    os << dynamic_cast<cofmsg_stats_reply const &>(msg);
    os << "<cofmsg_meter_config_stats_reply >" << std::endl;
    os << msg.get_meter_config_array();
    return os;
  };

  virtual std::string str() const {
    std::stringstream ss;
    ss << cofmsg_stats_reply::str() << "-Meter-Config-Stats-Reply- ";
    return ss.str();
  };

private:
  rofl::openflow::cofmeterconfigarray array;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_METER_CONFIG_STATS_H_ */
