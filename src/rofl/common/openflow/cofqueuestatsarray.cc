/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofqueues.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofqueuestatsarray.h"

using namespace rofl::openflow;

size_t cofqueuestatsarray::length() const {
  size_t len = 0;
  for (std::map<uint32_t,
                std::map<uint32_t, cofqueue_stats_reply>>::const_iterator it =
           array.begin();
       it != array.end(); ++it) {
    for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator jt =
             it->second.begin();
         jt != it->second.end(); ++jt) {
      len += jt->second.length();
    }
  }
  return len;
}

void cofqueuestatsarray::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    for (std::map<uint32_t,
                  std::map<uint32_t, cofqueue_stats_reply>>::const_iterator it =
             array.begin();
         it != array.end(); ++it) {
      for (std::map<uint32_t, cofqueue_stats_reply>::const_iterator jt =
               it->second.begin();
           jt != it->second.end(); ++jt) {
        jt->second.pack(buf, jt->second.length());
        buf += jt->second.length();
      }
    }

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofqueuestatsarray::unpack(uint8_t *buf, size_t buflen) {
  array.clear();

  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION: {

    while (buflen >= sizeof(struct rofl::openflow12::ofp_queue_stats)) {

      uint32_t port_no =
          be32toh(((struct rofl::openflow12::ofp_queue_stats *)buf)->port_no);
      uint32_t queue_id =
          be32toh(((struct rofl::openflow12::ofp_queue_stats *)buf)->queue_id);

      add_queue_stats(port_no, queue_id)
          .unpack(buf, sizeof(struct rofl::openflow12::ofp_queue_stats));

      buf += sizeof(struct rofl::openflow12::ofp_queue_stats);
      buflen -= sizeof(struct rofl::openflow12::ofp_queue_stats);
    }
  } break;
  case rofl::openflow13::OFP_VERSION: {

    while (buflen >= sizeof(struct rofl::openflow13::ofp_queue_stats)) {

      uint32_t port_no =
          be32toh(((struct rofl::openflow13::ofp_queue_stats *)buf)->port_no);
      uint32_t queue_id =
          be32toh(((struct rofl::openflow13::ofp_queue_stats *)buf)->queue_id);

      add_queue_stats(port_no, queue_id)
          .unpack(buf, sizeof(struct rofl::openflow13::ofp_queue_stats));

      buf += sizeof(struct rofl::openflow13::ofp_queue_stats);
      buflen -= sizeof(struct rofl::openflow13::ofp_queue_stats);
    }
  } break;
  default:
    throw eBadRequestBadVersion("eBadRequestBadVersion", __FILE__,
                                __PRETTY_FUNCTION__, __LINE__);
  }
}
