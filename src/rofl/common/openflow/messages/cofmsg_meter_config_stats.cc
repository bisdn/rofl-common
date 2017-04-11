/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_meter_config_stats.h"

using namespace rofl::openflow;

cofmsg_meter_config_stats_request::~cofmsg_meter_config_stats_request() {}

cofmsg_meter_config_stats_request::cofmsg_meter_config_stats_request(
    uint8_t version, uint32_t xid, uint16_t stats_flags, uint32_t meter_id)
    : cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_METER_CONFIG,
                           stats_flags),
      meter_config(version) {
  meter_config.set_meter_id(meter_id);
}

cofmsg_meter_config_stats_request::cofmsg_meter_config_stats_request(
    uint8_t version, uint32_t xid, uint16_t stats_flags,
    const rofl::openflow::cofmeter_config_request &meter_config)
    : cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_METER_CONFIG,
                           stats_flags),
      meter_config(meter_config) {
  this->meter_config.set_version(version);
}

cofmsg_meter_config_stats_request::cofmsg_meter_config_stats_request(
    const cofmsg_meter_config_stats_request &msg) {
  *this = msg;
}

cofmsg_meter_config_stats_request &cofmsg_meter_config_stats_request::
operator=(const cofmsg_meter_config_stats_request &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_request::operator=(msg);
  meter_config = msg.meter_config;
  return *this;
}

size_t cofmsg_meter_config_stats_request::length() const {
  switch (get_version()) {
  case openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_request) +
            meter_config.length());
  } break;
  default:
    throw eBadVersion("eBadVersion").set_func(__FUNCTION__).set_line(__LINE__);
  }
}

void cofmsg_meter_config_stats_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_config_stats_request::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  default: {
    struct rofl::openflow13::ofp_multipart_request *hdr =
        (struct rofl::openflow13::ofp_multipart_request *)buf;
    meter_config.pack(hdr->body, meter_config.length());
  };
  }
}

void cofmsg_meter_config_stats_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::unpack(buf, buflen);

  meter_config.set_version(get_version());

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_config_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_METER_CONFIG)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_multipart_request *hdr =
        (struct rofl::openflow13::ofp_multipart_request *)buf;
    if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_request)) {
      meter_config.unpack(
          hdr->body,
          buflen - sizeof(struct rofl::openflow13::ofp_multipart_request));
    }
  };
  }

  if (get_length() < cofmsg_meter_config_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}

cofmsg_meter_config_stats_reply::~cofmsg_meter_config_stats_reply() {}

cofmsg_meter_config_stats_reply::cofmsg_meter_config_stats_reply(
    uint8_t version, uint32_t xid, uint16_t stats_flags,
    const rofl::openflow::cofmeterconfigarray &array)
    : cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_METER_CONFIG,
                         stats_flags),
      array(array) {
  this->array.set_version(version);
}

cofmsg_meter_config_stats_reply::cofmsg_meter_config_stats_reply(
    const cofmsg_meter_config_stats_reply &msg) {
  *this = msg;
}

cofmsg_meter_config_stats_reply &cofmsg_meter_config_stats_reply::
operator=(const cofmsg_meter_config_stats_reply &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_reply::operator=(msg);
  array = msg.array;
  return *this;
}

size_t cofmsg_meter_config_stats_reply::length() const {
  switch (get_version()) {
  case openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_reply) +
            array.length());
  } break;
  default:
    throw eBadVersion("eBadVersion").set_func(__FUNCTION__).set_line(__LINE__);
  }
}

void cofmsg_meter_config_stats_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_config_stats_reply::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  default: {
    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    array.pack(hdr->body, array.length());
  };
  }
}

void cofmsg_meter_config_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::unpack(buf, buflen);

  array.clear();
  array.set_version(get_version());

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_config_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_METER_CONFIG)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
      array.unpack(hdr->body,
                   buflen -
                       sizeof(struct rofl::openflow13::ofp_multipart_reply));
    }
  };
  }

  if (get_length() < cofmsg_meter_config_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}
