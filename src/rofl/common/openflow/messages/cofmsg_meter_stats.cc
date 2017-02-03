/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_meter_stats.h"

using namespace rofl::openflow;

cofmsg_meter_stats_request::~cofmsg_meter_stats_request() {}

cofmsg_meter_stats_request::cofmsg_meter_stats_request(uint8_t version,
                                                       uint32_t xid,
                                                       uint16_t stats_flags,
                                                       uint32_t meter_id)
    : cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_METER,
                           stats_flags) {
  meter_stats.set_version(version);
  meter_stats.set_meter_id(meter_id);
}

cofmsg_meter_stats_request::cofmsg_meter_stats_request(
    uint8_t version, uint32_t xid, uint16_t stats_flags,
    const rofl::openflow::cofmeter_stats_request &meter_stats)
    : cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_METER,
                           stats_flags),
      meter_stats(meter_stats) {
  this->meter_stats.set_version(version);
}

cofmsg_meter_stats_request::cofmsg_meter_stats_request(
    const cofmsg_meter_stats_request &msg) {
  *this = msg;
}

cofmsg_meter_stats_request &cofmsg_meter_stats_request::
operator=(const cofmsg_meter_stats_request &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_request::operator=(msg);
  meter_stats = msg.meter_stats;
  return *this;
}

size_t cofmsg_meter_stats_request::length() const {
  switch (get_version()) {
  default: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_request) +
            meter_stats.length());
  };
  }
}

void cofmsg_meter_stats_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_stats_request::length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  switch (get_version()) {
  default: {
    struct rofl::openflow13::ofp_multipart_request *hdr =
        (struct rofl::openflow13::ofp_multipart_request *)buf;
    meter_stats.pack(hdr->body, meter_stats.length());
  };
  }
}

void cofmsg_meter_stats_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_request::unpack(buf, buflen);

  meter_stats.set_version(get_version());

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_METER)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_multipart_request *hdr =
        (struct rofl::openflow13::ofp_multipart_request *)buf;
    if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_request)) {
      meter_stats.unpack(
          hdr->body,
          buflen - sizeof(struct rofl::openflow13::ofp_multipart_request));
    }
  };
  }

  if (get_length() < cofmsg_meter_stats_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);
}

cofmsg_meter_stats_reply::~cofmsg_meter_stats_reply() {}

cofmsg_meter_stats_reply::cofmsg_meter_stats_reply(
    uint8_t version, uint32_t xid, uint16_t stats_flags,
    const rofl::openflow::cofmeterstatsarray &meterstatsarray)
    : cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_METER,
                         stats_flags),
      meterstatsarray(meterstatsarray) {
  this->meterstatsarray.set_version(version);
}

cofmsg_meter_stats_reply::cofmsg_meter_stats_reply(
    const cofmsg_meter_stats_reply &msg) {
  *this = msg;
}

cofmsg_meter_stats_reply &cofmsg_meter_stats_reply::
operator=(const cofmsg_meter_stats_reply &msg) {
  if (this == &msg)
    return *this;
  cofmsg_stats_reply::operator=(msg);
  meterstatsarray = msg.meterstatsarray;
  return *this;
}

size_t cofmsg_meter_stats_reply::length() const {
  switch (get_version()) {
  default: {
    return (sizeof(struct rofl::openflow13::ofp_multipart_reply) +
            meterstatsarray.length());
  };
  }
}

void cofmsg_meter_stats_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_stats_reply::length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  switch (get_version()) {
  default: {
    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    meterstatsarray.pack(hdr->body, meterstatsarray.length());
  };
  }
}

void cofmsg_meter_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg_stats_reply::unpack(buf, buflen);

  meterstatsarray.set_version(get_version());
  meterstatsarray.clear();

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_meter_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {
    if (get_stats_type() != rofl::openflow13::OFPMP_METER)
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_multipart_reply *hdr =
        (struct rofl::openflow13::ofp_multipart_reply *)buf;
    if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
      meterstatsarray.unpack(
          hdr->body,
          buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
    }
  };
  }

  if (get_length() < cofmsg_meter_stats_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);
}
