/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coftables.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/coftablestatsarray.h"

using namespace rofl::openflow;

size_t coftablestatsarray::length() const {
  size_t len = 0;
  for (std::map<uint8_t, coftable_stats_reply>::const_iterator it =
           array.begin();
       it != array.end(); ++it) {
    len += it->second.length();
  }
  return len;
}

void coftablestatsarray::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    for (std::map<uint8_t, coftable_stats_reply>::iterator it = array.begin();
         it != array.end(); ++it) {
      it->second.pack(buf, it->second.length());
      buf += it->second.length();
    }

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void coftablestatsarray::unpack(uint8_t *buf, size_t buflen) {
  array.clear();

  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION: {

    while (buflen >= sizeof(struct rofl::openflow10::ofp_table_stats)) {

      uint8_t table_id =
          ((struct rofl::openflow10::ofp_table_stats *)buf)->table_id;

      add_table_stats(table_id).unpack(
          buf, sizeof(struct rofl::openflow10::ofp_table_stats));

      buf += sizeof(struct rofl::openflow10::ofp_table_stats);
      buflen -= sizeof(struct rofl::openflow10::ofp_table_stats);
    }
  } break;
  case rofl::openflow12::OFP_VERSION: {

    while (buflen >= sizeof(struct rofl::openflow12::ofp_table_stats)) {

      uint8_t table_id =
          ((struct rofl::openflow12::ofp_table_stats *)buf)->table_id;

      add_table_stats(table_id).unpack(
          buf, sizeof(struct rofl::openflow12::ofp_table_stats));

      buf += sizeof(struct rofl::openflow12::ofp_table_stats);
      buflen -= sizeof(struct rofl::openflow12::ofp_table_stats);
    }
  } break;
  case rofl::openflow13::OFP_VERSION: {

    while (buflen >= sizeof(struct rofl::openflow13::ofp_table_stats)) {

      uint8_t table_id =
          ((struct rofl::openflow13::ofp_table_stats *)buf)->table_id;

      add_table_stats(table_id).unpack(
          buf, sizeof(struct rofl::openflow13::ofp_table_stats));

      buf += sizeof(struct rofl::openflow13::ofp_table_stats);
      buflen -= sizeof(struct rofl::openflow13::ofp_table_stats);
    }
  } break;
  default:
    throw eBadRequestBadVersion("eBadRequestBadVersion", __FILE__,
                                __PRETTY_FUNCTION__, __LINE__);
  }
}
