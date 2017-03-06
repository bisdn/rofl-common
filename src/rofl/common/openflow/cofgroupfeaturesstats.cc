/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofgroupfeaturesstats.h"

using namespace rofl::openflow;

size_t cofgroup_features_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_group_features_stats));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
  return 0;
}

void cofgroup_features_stats_reply::pack(uint8_t *buf, size_t buflen) const {
  switch (of_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    struct rofl::openflow12::ofp_group_features_stats *fs =
        (struct rofl::openflow12::ofp_group_features_stats *)buf;
    fs->types = htobe32(types);
    fs->capabilities = htobe32(capabilities);
    for (unsigned int i = 0; i < 4; i++) {
      fs->max_groups[i] = htobe32(max_groups[i]);
      fs->actions[i] = htobe32(actions[i]);
    }
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofgroup_features_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_group_features_stats))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_group_features_stats *fs =
        (struct rofl::openflow12::ofp_group_features_stats *)buf;
    types = be32toh(fs->types);
    capabilities = be32toh(fs->capabilities);
    for (unsigned int i = 0; i < 4; i++) {
      max_groups[i] = be32toh(fs->max_groups[i]);
      actions[i] = be32toh(fs->actions[i]);
    }
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}
