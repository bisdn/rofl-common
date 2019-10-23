/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_group_features_stats.h"

using namespace rofl::openflow;

cofmsg_group_features_stats_request::~cofmsg_group_features_stats_request() {}

cofmsg_group_features_stats_request::cofmsg_group_features_stats_request(
    uint8_t version, uint32_t xid, uint16_t flags)
    : cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_GROUP_FEATURES,
                           flags) {}

cofmsg_group_features_stats_request::cofmsg_group_features_stats_request(
    const cofmsg_group_features_stats_request &msg) {
  *this = msg;
}

cofmsg_group_features_stats_request &cofmsg_group_features_stats_request::
operator=(const cofmsg_group_features_stats_request &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_request::operator=(msg);
  return *this;
}

size_t cofmsg_group_features_stats_request::length() const {
  switch (get_version()) {
  default: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_request));
  };
  }
  return 0;
}

void cofmsg_group_features_stats_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_features_stats_request::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);
}

void cofmsg_group_features_stats_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_features_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_GROUP_FEATURES)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
  };
  }

  if (get_length() < cofmsg_group_features_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}

cofmsg_group_features_stats_reply::~cofmsg_group_features_stats_reply() {}

cofmsg_group_features_stats_reply::cofmsg_group_features_stats_reply(
    uint8_t version, uint32_t xid, uint16_t stats_flags,
    const rofl::openflow::cofgroup_features_stats_reply &group_features_stats)
    : cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_GROUP_FEATURES,
                         stats_flags),
      group_features_stats(group_features_stats) {
  this->group_features_stats.set_version(version);
}

cofmsg_group_features_stats_reply::cofmsg_group_features_stats_reply(
    const cofmsg_group_features_stats_reply &msg) {
  *this = msg;
}

cofmsg_group_features_stats_reply &cofmsg_group_features_stats_reply::
operator=(const cofmsg_group_features_stats_reply &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_reply::operator=(msg);
  group_features_stats = msg.group_features_stats;
  return *this;
}

size_t cofmsg_group_features_stats_reply::length() const {
  switch (get_version()) {
  default: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_reply) +
            group_features_stats.length());
  };
  }
  return 0;
}

void cofmsg_group_features_stats_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_features_stats_reply::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  default: {
    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    group_features_stats.pack(hdr->body, group_features_stats.length());
  };
  }
}

void cofmsg_group_features_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::unpack(buf, buflen);

  group_features_stats.set_version(get_version());

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_features_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_GROUP_FEATURES)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
      group_features_stats.unpack(
          hdr->body,
          buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
    }
  };
  }

  if (get_length() < cofmsg_group_features_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}
