/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_stats.h"

using namespace rofl::openflow;

cofmsg_stats_request::~cofmsg_stats_request() {}

cofmsg_stats_request::cofmsg_stats_request(uint8_t version, uint32_t xid,
                                           uint16_t stats_type,
                                           uint16_t stats_flags)
    : cofmsg(version, rofl::openflow13::OFPT_MULTIPART_REQUEST, xid),
      stats_type(stats_type), stats_flags(stats_flags) {
  switch (version) {
  case rofl::openflow10::OFP_VERSION: {
    set_type(rofl::openflow10::OFPT_STATS_REQUEST);
  } break;
  default: {
    // do nothing
  };
  }
}

cofmsg_stats_request::cofmsg_stats_request(const cofmsg_stats_request &msg)
    : cofmsg(msg), stats_type(msg.stats_type), stats_flags(msg.stats_flags) {}

cofmsg_stats_request &cofmsg_stats_request::
operator=(const cofmsg_stats_request &msg) {
  if (this == &msg)
    return *this;
  cofmsg::operator=(msg);
  stats_type = msg.stats_type;
  stats_flags = msg.stats_flags;
  return *this;
}

size_t cofmsg_stats_request::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    return sizeof(struct rofl::openflow10::ofp_stats_request);
  } break;
  default: { return sizeof(struct rofl::openflow13::ofp_multipart_request); };
  }
  return 0;
}

void cofmsg_stats_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg::pack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_stats_request::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    struct rofl::openflow10::ofp_stats_request *hdr =
        (struct rofl::openflow10::ofp_stats_request *)buf;
    hdr->type = htobe16(stats_type);
    hdr->flags = htobe16(stats_flags);
  } break;
  default: {
    struct rofl::openflow13::ofp_multipart_request *hdr =
        (struct rofl::openflow13::ofp_multipart_request *)buf;
    hdr->type = htobe16(stats_type);
    hdr->flags = htobe16(stats_flags);
  };
  }
}

void cofmsg_stats_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_type() != rofl::openflow10::OFPT_STATS_REQUEST)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow10::ofp_stats_request *hdr =
        (struct rofl::openflow10::ofp_stats_request *)buf;
    stats_type = be16toh(hdr->type);
    stats_flags = be16toh(hdr->flags);
  } break;
  default: {
    if (get_type() != rofl::openflow13::OFPT_MULTIPART_REQUEST)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_multipart_request *hdr =
        (struct rofl::openflow13::ofp_multipart_request *)buf;
    stats_type = be16toh(hdr->type);
    stats_flags = be16toh(hdr->flags);
  };
  }

  if (get_length() < cofmsg_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}

cofmsg_stats_reply::~cofmsg_stats_reply() {}

cofmsg_stats_reply::cofmsg_stats_reply(uint8_t version, uint32_t xid,
                                       uint16_t stats_type,
                                       uint16_t stats_flags)
    : cofmsg(version, rofl::openflow::OFPT_MULTIPART_REPLY, xid),
      stats_type(stats_type), stats_flags(stats_flags) {
  switch (version) {
  case rofl::openflow10::OFP_VERSION: {
    set_type(rofl::openflow10::OFPT_STATS_REPLY);
  } break;
  default: {
    // do nothing
  };
  }
}

cofmsg_stats_reply::cofmsg_stats_reply(const cofmsg_stats_reply &msg)
    : cofmsg(msg), stats_type(msg.stats_type), stats_flags(msg.stats_flags) {}

cofmsg_stats_reply &cofmsg_stats_reply::
operator=(const cofmsg_stats_reply &msg) {
  if (this == &msg)
    return *this;
  cofmsg::operator=(msg);
  stats_type = msg.stats_type;
  stats_flags = msg.stats_flags;
  return *this;
}

size_t cofmsg_stats_reply::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    return sizeof(struct rofl::openflow10::ofp_stats_reply);
  } break;
  default: { return sizeof(struct rofl::openflow13::ofp_multipart_reply); };
  }
  return 0;
}

void cofmsg_stats_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg::pack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_stats_reply::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    struct rofl::openflow10::ofp_stats_reply *hdr =
        (struct rofl::openflow10::ofp_stats_reply *)buf;
    hdr->type = htobe16(stats_type);
    hdr->flags = htobe16(stats_flags);
  } break;
  default: {
    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    hdr->type = htobe16(stats_type);
    hdr->flags = htobe16(stats_flags);
  };
  }
}

void cofmsg_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_type() != rofl::openflow10::OFPT_STATS_REPLY)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow10::ofp_stats_reply *hdr =
        (struct rofl::openflow10::ofp_stats_reply *)buf;
    stats_type = be16toh(hdr->type);
    stats_flags = be16toh(hdr->flags);
  } break;
  default: {
    if (get_type() != rofl::openflow13::OFPT_MULTIPART_REPLY)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    stats_type = be16toh(hdr->type);
    stats_flags = be16toh(hdr->flags);
  };
  }

  if (get_length() < cofmsg_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}
