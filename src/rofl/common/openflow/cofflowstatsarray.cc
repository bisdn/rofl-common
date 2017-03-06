/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofflows.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofflowstatsarray.h"

using namespace rofl::openflow;

size_t cofflowstatsarray::length() const {
  size_t len = 0;
  for (std::map<uint32_t, cofflow_stats_reply>::const_iterator it =
           array.begin();
       it != array.end(); ++it) {
    len += it->second.length();
  }
  return len;
}

void cofflowstatsarray::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    for (std::map<uint32_t, cofflow_stats_reply>::iterator it = array.begin();
         it != array.end(); ++it) {
      it->second.pack(buf, it->second.length());
      buf += it->second.length();
    }

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofflowstatsarray::unpack(uint8_t *buf, size_t buflen) {
  array.clear();

  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION: {

    uint32_t flow_id = 0;

    while (buflen >= sizeof(struct rofl::openflow10::ofp_flow_stats)) {

      add_flow_stats(flow_id++).unpack(
          buf, sizeof(struct rofl::openflow10::ofp_flow_stats));

      buf += sizeof(struct rofl::openflow10::ofp_flow_stats);
      buflen -= sizeof(struct rofl::openflow10::ofp_flow_stats);
    }
  } break;
  case rofl::openflow12::OFP_VERSION: {

    uint32_t flow_id = 0;

    while (buflen >= sizeof(struct rofl::openflow12::ofp_flow_stats)) {

      struct rofl::openflow12::ofp_flow_stats *flow_stats =
          (struct rofl::openflow12::ofp_flow_stats *)buf;

      uint16_t length = be16toh(flow_stats->length);

      if (length < sizeof(struct rofl::openflow12::ofp_flow_stats))
        throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

      add_flow_stats(flow_id++).unpack(buf, length);

      buf += length;
      buflen -= length;
    }

  } break;
  case rofl::openflow13::OFP_VERSION: {

    uint32_t flow_id = 0;

    while (buflen >= sizeof(struct rofl::openflow13::ofp_flow_stats)) {

      struct rofl::openflow13::ofp_flow_stats *flow_stats =
          (struct rofl::openflow13::ofp_flow_stats *)buf;

      uint16_t length = be16toh(flow_stats->length);

      if (length < sizeof(struct rofl::openflow13::ofp_flow_stats))
        throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

      add_flow_stats(flow_id++).unpack(buf, length);

      buf += length;
      buflen -= length;
    }

  } break;
  default:
    throw eBadRequestBadVersion("eBadRequestBadVersion", __FILE__,
                                __PRETTY_FUNCTION__, __LINE__);
  }
}
