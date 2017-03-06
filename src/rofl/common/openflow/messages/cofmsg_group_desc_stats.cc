/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmsg_group_desc_stats.h"

using namespace rofl::openflow;

cofmsg_group_desc_stats_request::~cofmsg_group_desc_stats_request() {}

cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
    uint8_t of_version, uint32_t xid, uint16_t flags)
    : cofmsg_stats_request(of_version, xid, rofl::openflow::OFPMP_GROUP_DESC,
                           flags) {}

cofmsg_group_desc_stats_request::cofmsg_group_desc_stats_request(
    const cofmsg_group_desc_stats_request &msg) {
  *this = msg;
}

cofmsg_group_desc_stats_request &cofmsg_group_desc_stats_request::
operator=(const cofmsg_group_desc_stats_request &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_request::operator=(msg);
  return *this;
}

size_t cofmsg_group_desc_stats_request::length() const {
  switch (get_version()) {
  default: { return (sizeof(struct rofl::openflow13::ofp_multipart_request)); };
  }
  return 0;
}

void cofmsg_group_desc_stats_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_desc_stats_request::length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}

void cofmsg_group_desc_stats_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_desc_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_GROUP_DESC)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);
  };
  }

  if (get_length() < cofmsg_group_desc_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);
}

cofmsg_group_desc_stats_reply::~cofmsg_group_desc_stats_reply() {}

cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
    uint8_t version, uint32_t xid, uint16_t flags,
    const rofl::openflow::cofgroupdescstatsarray &groupdescstatsarray)
    : cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_GROUP_DESC, flags),
      groupdescstatsarray(groupdescstatsarray) {
  this->groupdescstatsarray.set_version(version);
}

cofmsg_group_desc_stats_reply::cofmsg_group_desc_stats_reply(
    const cofmsg_group_desc_stats_reply &msg) {
  *this = msg;
}

cofmsg_group_desc_stats_reply &cofmsg_group_desc_stats_reply::
operator=(const cofmsg_group_desc_stats_reply &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_reply::operator=(msg);
  groupdescstatsarray = msg.groupdescstatsarray;
  return *this;
}

size_t cofmsg_group_desc_stats_reply::length() const {
  switch (get_version()) {
  default: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_reply) +
            groupdescstatsarray.length());
  };
  }
  return 0;
}

void cofmsg_group_desc_stats_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_desc_stats_reply::length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  switch (get_version()) {
  default: {
    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    groupdescstatsarray.pack(hdr->body, groupdescstatsarray.length());
  };
  }
}

void cofmsg_group_desc_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::unpack(buf, buflen);

  groupdescstatsarray.set_version(get_version());
  groupdescstatsarray.clear();

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_group_desc_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_GROUP_DESC)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
      groupdescstatsarray.unpack(
          hdr->body,
          buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
    }
  };
  }

  if (get_length() < cofmsg_group_desc_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);
}
