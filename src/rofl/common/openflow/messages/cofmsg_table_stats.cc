/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_table_stats.h"

using namespace rofl::openflow;

cofmsg_table_stats_request::~cofmsg_table_stats_request() {}

cofmsg_table_stats_request::cofmsg_table_stats_request(uint8_t version,
                                                       uint32_t xid,
                                                       uint16_t stats_flags)
    : cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_TABLE,
                           stats_flags) {}

cofmsg_table_stats_request::cofmsg_table_stats_request(
    const cofmsg_table_stats_request &msg) {
  *this = msg;
}

cofmsg_table_stats_request &cofmsg_table_stats_request::
operator=(const cofmsg_table_stats_request &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_request::operator=(msg);
  return *this;
}

size_t cofmsg_table_stats_request::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct rofl::openflow10::ofp_stats_request));
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_stats_request));
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_request));
  } break;
  default:
    throw eBadVersion("eBadVersion").set_func(__FUNCTION__).set_line(__LINE__);
  }
  return 0;
}

void cofmsg_table_stats_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_table_stats_request::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);
}

void cofmsg_table_stats_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_table_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_stats_type() != rofl::openflow10::OFPST_TABLE)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);

  } break;
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_TABLE)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
  };
  }

  if (get_length() < cofmsg_table_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}

cofmsg_table_stats_reply::~cofmsg_table_stats_reply() {}

cofmsg_table_stats_reply::cofmsg_table_stats_reply(
    uint8_t version, uint32_t xid, uint16_t flags,
    const rofl::openflow::coftablestatsarray &tablestatsarray)
    : cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_TABLE, flags),
      tablestatsarray(tablestatsarray) {
  this->tablestatsarray.set_version(version);
}

cofmsg_table_stats_reply::cofmsg_table_stats_reply(
    const cofmsg_table_stats_reply &msg) {
  *this = msg;
}

cofmsg_table_stats_reply &cofmsg_table_stats_reply::
operator=(const cofmsg_table_stats_reply &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_reply::operator=(msg);
  tablestatsarray = msg.tablestatsarray;
  return *this;
}

size_t cofmsg_table_stats_reply::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct rofl::openflow10::ofp_stats_reply) +
            tablestatsarray.length());
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_stats_reply) +
            tablestatsarray.length());
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_reply) +
            tablestatsarray.length());
  } break;
  default:
    throw eBadVersion("eBadVersion").set_func(__FUNCTION__).set_line(__LINE__);
  }
  return 0;
}

void cofmsg_table_stats_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_table_stats_reply::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    struct rofl::openflow10::ofp_stats_reply *hdr =
        (struct rofl::openflow10::ofp_stats_reply *)buf;
    tablestatsarray.pack(hdr->body, tablestatsarray.length());
  } break;
  default: {
    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    tablestatsarray.pack(hdr->body, tablestatsarray.length());
  };
  }
}

void cofmsg_table_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::unpack(buf, buflen);

  tablestatsarray.clear();
  tablestatsarray.set_version(get_version());

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_table_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_stats_type() != rofl::openflow10::OFPST_TABLE)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow10::ofp_stats_reply *hdr =
        (struct rofl::openflow10::ofp_stats_reply *)buf;
    if (buflen > sizeof(struct rofl::openflow10::ofp_stats_reply)) {
      tablestatsarray.unpack(
          hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));
    }
  } break;
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_TABLE)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
      tablestatsarray.unpack(
          hdr->body,
          buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
    }
  };
  }

  if (get_length() < cofmsg_table_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}
