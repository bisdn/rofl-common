/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofdescstats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_COFDESCSTATS_H_
#define ROFL_COMMON_COFDESCSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class cofdesc_stats_reply {
public:
  /**
   *
   */
  virtual ~cofdesc_stats_reply(){};

  /**
   *
   */
  cofdesc_stats_reply(uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : of_version(of_version){};

  /**
   *
   */
  cofdesc_stats_reply(uint8_t of_version, const std::string &mfr_desc,
                      const std::string &hw_desc, const std::string &sw_desc,
                      const std::string &serial_num, const std::string &dp_desc)
      : of_version(of_version), mfr_desc(mfr_desc), hw_desc(hw_desc),
        sw_desc(sw_desc), serial_num(serial_num), dp_desc(dp_desc){};

  /**
   *
   */
  cofdesc_stats_reply(const cofdesc_stats_reply &descstats) {
    *this = descstats;
  };

  /**
   *
   */
  cofdesc_stats_reply &operator=(const cofdesc_stats_reply &descstats) {
    if (this == &descstats)
      return *this;

    of_version = descstats.of_version;
    mfr_desc = descstats.mfr_desc;
    hw_desc = descstats.hw_desc;
    sw_desc = descstats.sw_desc;
    serial_num = descstats.serial_num;
    dp_desc = descstats.dp_desc;

    return *this;
  };

public:
  /**
   *
   */
  cofdesc_stats_reply &clear() {
    mfr_desc.clear();
    hw_desc.clear();
    sw_desc.clear();
    serial_num.clear();
    dp_desc.clear();
    return *this;
  };

  /**
   *
   */
  cofdesc_stats_reply &set_version(uint8_t of_version) {
    this->of_version = of_version;
    return *this;
  };

  /**
   *
   */
  uint8_t get_version() const { return of_version; };

  /**
   *
   */
  cofdesc_stats_reply &set_mfr_desc(const std::string &mfr_desc) {
    this->mfr_desc = mfr_desc;
    return *this;
  };

  /**
   *
   */
  const std::string &get_mfr_desc() const { return mfr_desc; };

  /**
   *
   */
  cofdesc_stats_reply &set_hw_desc(const std::string &hw_desc) {
    this->hw_desc = hw_desc;
    return *this;
  };

  /**
   *
   */
  const std::string &get_hw_desc() const { return hw_desc; };

  /**
   *
   */
  cofdesc_stats_reply &set_sw_desc(const std::string &sw_desc) {
    this->sw_desc = sw_desc;
    return *this;
  };

  /**
   *
   */
  const std::string &get_sw_desc() const { return sw_desc; };

  /**
   *
   */
  cofdesc_stats_reply &set_serial_num(const std::string &serial_num) {
    this->serial_num = serial_num;
    return *this;
  };

  /**
   *
   */
  const std::string &get_serial_num() const { return serial_num; };

  /**
   *
   */
  cofdesc_stats_reply &set_dp_desc(const std::string &dp_desc) {
    this->dp_desc = dp_desc;
    return *this;
  };

  /**
   *
   */
  const std::string &get_dp_desc() const { return dp_desc; };

public:
  /**
   *
   */
  size_t length() const;

  /**
   *
   */
  void pack(uint8_t *buf, size_t buflen) const;

  /**
   *
   */
  void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofdesc_stats_reply const &reply) {
    switch (reply.get_version()) {
    case rofl::openflow::OFP_VERSION_UNKNOWN: {
      os << "<cofdesc_stats_reply ofp-version:" << (int)reply.get_version()
         << " >" << std::endl;

    } break;
    case rofl::openflow10::OFP_VERSION: {
      os << "<cofdesc_stats_reply ofp-version:" << (int)reply.get_version()
         << " >" << std::endl;
      os << "<mfr-desc: " << reply.mfr_desc << " >" << std::endl;
      os << "<hw-desc: " << reply.hw_desc << " >" << std::endl;
      os << "<sw-desc: " << reply.sw_desc << " >" << std::endl;
      os << "<serial-num: " << reply.serial_num << " >" << std::endl;
      os << "<dp-desc: " << reply.dp_desc << " >" << std::endl;

    } break;
    case rofl::openflow12::OFP_VERSION:
    case rofl::openflow13::OFP_VERSION: {
      os << "<cofdesc_stats_reply ofp-version:" << (int)reply.get_version()
         << " >" << std::endl;
      os << "<mfr-desc: " << reply.mfr_desc << " >" << std::endl;
      os << "<hw-desc: " << reply.hw_desc << " >" << std::endl;
      os << "<sw-desc: " << reply.sw_desc << " >" << std::endl;
      os << "<serial-num: " << reply.serial_num << " >" << std::endl;
      os << "<dp-desc: " << reply.dp_desc << " >" << std::endl;

    } break;
    default: {
      throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    };
    }
    return os;
  };

private:
  uint8_t of_version;
  std::string mfr_desc;
  std::string hw_desc;
  std::string sw_desc;
  std::string serial_num;
  std::string dp_desc;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* ROFL_COMMON_COFDESCSTATS_H_ */
