/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * csegment.hpp
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#ifndef CSEGMENT_HPP_
#define CSEGMENT_HPP_

#include <inttypes.h>

#include <iostream>

#include "rofl/common/ctimespec.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/messages/cofmsg_aggr_stats.h"
#include "rofl/common/openflow/messages/cofmsg_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_flow_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_config_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_stats.h"
#include "rofl/common/openflow/messages/cofmsg_queue_stats.h"
#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_stats.h"

namespace rofl {

class eSegmentedMessageBase : public exception {
public:
  eSegmentedMessageBase(const std::string &__arg) : exception(__arg){};
};
class eSegmentedMessageInvalid : public eSegmentedMessageBase {
public:
  eSegmentedMessageInvalid(const std::string &__arg)
      : eSegmentedMessageBase(__arg){};
};
class eSegmentedMessageNotFound : public eSegmentedMessageBase {
public:
  eSegmentedMessageNotFound(const std::string &__arg)
      : eSegmentedMessageBase(__arg){};
};

class csegment {
public:
  /**
   *
   */
  virtual ~csegment() {
    if (nullptr != msg) {
      delete msg;
    }
  };

  /**
   *
   */
  csegment() : xid(0), msg(nullptr), msg_type(0), msg_multipart_type(0){};

  /**
   *
   */
  csegment(uint32_t xid, const ctimespec &tspec, uint8_t msg_type,
           uint16_t msg_multipart_type = 0)
      : tspec(tspec), xid(xid), msg(nullptr), msg_type(msg_type),
        msg_multipart_type(msg_multipart_type){};

  /**
   *
   */
  csegment(const csegment &segment) { *this = segment; }

  /**
   *
   */
  csegment &operator=(const csegment &segment) {
    if (this == &segment)
      return *this;
    tspec = segment.tspec;
    xid = segment.xid;
    if (NULL != segment.msg) {
      csegment::clone(*(segment.msg));
    } else {
      msg = NULL;
    }
    msg_type = segment.msg_type;
    msg_multipart_type = segment.msg_multipart_type;
    return *this;
  };

public:
  /**
   *
   */
  bool is_expired() const { return tspec.is_expired(); };

  /**
   *
   */
  uint32_t get_xid() const { return xid; };

  /**
   *
   */
  const rofl::openflow::cofmsg &get_msg() const {
    if (0 == msg)
      throw eInvalid("csegment::get_msg()")
          .set_func(__FUNCTION__)
          .set_line(__LINE__);
    return *msg;
  };

public:
  /**
   * @brief	Merges payload from msg within this->msg. Checks stats sub-type
   * first.
   */
  void store_and_merge_msg(const rofl::openflow::cofmsg &msg);

  /**
   * @brief	Returns pointer to this->msg and sets this->msg to NULL. The
   * object resides on heap and must be destroyed by the calling entity.
   */
  rofl::openflow::cofmsg *retrieve_and_detach_msg();

private:
  /**
   *
   */
  void clone(const rofl::openflow::cofmsg &msg);

public:
  /**
   *
   */
  class csegment_is_expired {
  public:
    csegment_is_expired(){};
    bool operator()(const std::pair<uint32_t, csegment> &p) const {
      return p.second.is_expired();
    };
  };

public:
  friend std::ostream &operator<<(std::ostream &os, const csegment &msg) {
    os << "<csegment"
       << " >" << std::endl;
    os << "<expires: >" << std::endl;
    { os << msg.tspec; }
    os << "<xid: 0x" << std::hex << (int)msg.xid << std::dec << " >"
       << std::endl;

    uint16_t stats_type = 0;
    if (dynamic_cast<const rofl::openflow::cofmsg_stats_request *>((msg.msg))) {
      stats_type =
          dynamic_cast<const rofl::openflow::cofmsg_stats_request &>(*(msg.msg))
              .get_stats_type();
    } else if (dynamic_cast<const rofl::openflow::cofmsg_stats_reply *>(
                   (msg.msg))) {
      stats_type =
          dynamic_cast<const rofl::openflow::cofmsg_stats_reply &>(*(msg.msg))
              .get_stats_type();
    }

    switch (msg.msg->get_version()) {
    case rofl::openflow13::OFP_VERSION: {

      switch (msg.msg->get_type()) {
      case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

        switch (stats_type) {
        case rofl::openflow13::OFPMP_DESC: {
          os << dynamic_cast<rofl::openflow::cofmsg_desc_stats_request const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_FLOW: {
          os << dynamic_cast<rofl::openflow::cofmsg_flow_stats_request const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_AGGREGATE: {
          os << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_request const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_TABLE: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_table_stats_request const &>(*(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_PORT_STATS: {
          os << dynamic_cast<rofl::openflow::cofmsg_port_stats_request const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_QUEUE: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_queue_stats_request const &>(*(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_group_stats_request const &>(*(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP_DESC: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_group_desc_stats_request const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP_FEATURES: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_group_features_stats_request const &>(
              *(msg.msg));
        } break;
        // TODO: meters
        case rofl::openflow13::OFPMP_TABLE_FEATURES: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_table_features_stats_request const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_PORT_DESC: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_port_desc_stats_request const &>(
              *(msg.msg));
        } break;
        default: {
          os << *(msg.msg);
        };
        }

      } break;
      case rofl::openflow13::OFPT_MULTIPART_REPLY: {

        switch (stats_type) {
        case rofl::openflow13::OFPMP_DESC: {
          os << dynamic_cast<rofl::openflow::cofmsg_desc_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_FLOW: {
          os << dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_AGGREGATE: {
          os << dynamic_cast<rofl::openflow::cofmsg_aggr_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_TABLE: {
          os << dynamic_cast<rofl::openflow::cofmsg_table_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_PORT_STATS: {
          os << dynamic_cast<rofl::openflow::cofmsg_port_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_QUEUE: {
          os << dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP: {
          os << dynamic_cast<rofl::openflow::cofmsg_group_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP_DESC: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_group_desc_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP_FEATURES: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_group_features_stats_reply const &>(
              *(msg.msg));
        } break;
        // TODO: meters
        case rofl::openflow13::OFPMP_TABLE_FEATURES: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_table_features_stats_reply const &>(
              *(msg.msg));
        } break;
        case rofl::openflow13::OFPMP_PORT_DESC: {
          os << dynamic_cast<
              rofl::openflow::cofmsg_port_desc_stats_reply const &>(*(msg.msg));
        } break;
        default: {
          os << *(msg.msg);
        };
        }

      } break;
      default: {
        // oops, do nothing here
      };
      }

    } break;
    }
    return os;
  };

private:
  // time this fragmented OpenFlow message will expire
  ctimespec tspec;

  // transaction id used by this fragmented OpenFlow message
  uint32_t xid;

  // stitched multipart message, allocated on heap
  rofl::openflow::cofmsg *msg;

  // openflow message type
  uint8_t msg_type;

  // openflow multipart message subtype (or 0, if no multipart message)
  uint16_t msg_multipart_type;
};

}; // end of namespace rofl

#endif /* CSEGMENT_HPP_ */
