/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofbucketcounter.cc
 *
 *  Created on: 16.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofbucketcounter.h"

using namespace rofl::openflow;

size_t cofbucket_counter::length() const {
  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION: {
    return sizeof(struct rofl::openflow12::ofp_bucket_counter);
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return sizeof(struct rofl::openflow13::ofp_bucket_counter);
  } break;
  default: {
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  };
  }
}

void cofbucket_counter::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen)) {
    return;
  }

  if (buflen < length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION: {
    struct rofl::openflow12::ofp_bucket_counter *bc =
        (struct rofl::openflow12::ofp_bucket_counter *)buf;
    bc->packet_count = htobe64(packet_count);
    bc->byte_count = htobe64(byte_count);
  } break;
  case rofl::openflow13::OFP_VERSION: {
    struct rofl::openflow13::ofp_bucket_counter *bc =
        (struct rofl::openflow13::ofp_bucket_counter *)buf;
    bc->packet_count = htobe64(packet_count);
    bc->byte_count = htobe64(byte_count);
  } break;
  default: {
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  };
  }
}

void cofbucket_counter::unpack(uint8_t *buf, size_t buflen) {
  if (buflen < length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION: {
    struct rofl::openflow12::ofp_bucket_counter *bc =
        (struct rofl::openflow12::ofp_bucket_counter *)buf;
    packet_count = be64toh(bc->packet_count);
    byte_count = be64toh(bc->byte_count);
  } break;
  case rofl::openflow13::OFP_VERSION: {
    struct rofl::openflow13::ofp_bucket_counter *bc =
        (struct rofl::openflow13::ofp_bucket_counter *)buf;
    packet_count = be64toh(bc->packet_count);
    byte_count = be64toh(bc->byte_count);
  } break;
  default: {
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  };
  }
}
