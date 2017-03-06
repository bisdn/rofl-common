/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofgroupmod.h"

using namespace rofl::openflow;

size_t cofgroupmod::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct ofp_group_mod) + buckets.length());
  } break;
  default:
    throw eBadVersion("cofgroupmod::length() no version defined");
  }
}

void cofgroupmod::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofgroupmod::length())
    throw eInvalid("cofgroupmod::pack() buflen too short", __FILE__,
                   __PRETTY_FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct ofp_group_mod *hdr = (struct ofp_group_mod *)buf;

    hdr->command = htobe16(command);
    hdr->type = type;
    hdr->group_id = htobe32(group_id);

    buckets.pack((uint8_t *)hdr->buckets,
                 buflen - sizeof(struct ofp_group_mod));

  } break;
  default:
    throw eBadVersion("cofgroupmod::pack() unsupported version");
  }
}

void cofgroupmod::unpack(uint8_t *buf, size_t buflen) {
  buckets.set_version(get_version());
  buckets.clear();

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofgroupmod::length())
    throw eInvalid("cofgroupmod::unpack() buflen too short", __FILE__,
                   __PRETTY_FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct ofp_group_mod *hdr = (struct ofp_group_mod *)buf;

    command = be16toh(hdr->command);
    type = hdr->type;
    group_id = be32toh(hdr->group_id);

    if (buflen > sizeof(struct ofp_group_mod)) {
      buckets.unpack((uint8_t *)hdr->buckets,
                     buflen - sizeof(struct ofp_group_mod));
    }

  } break;
  default:
    throw eBadVersion("cofgroupmod::unpack() unsupported version");
  }
}
