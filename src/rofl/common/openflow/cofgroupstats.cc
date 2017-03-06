/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofgroupstats.h"

using namespace rofl::openflow;

size_t cofgroup_stats_request::length() const {
  switch (of_version) {
  // no OpenFLow 1.0 group stats
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_group_stats_request));
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_group_stats_request));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
  return 0;
}

void cofgroup_stats_request::pack(uint8_t *buf, size_t buflen) const {
  switch (of_version) {
  // no OpenFLow 1.0 group stats
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_group_stats_request))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_group_stats_request *stats =
        (struct rofl::openflow12::ofp_group_stats_request *)buf;

    stats->group_id = htobe32(group_id);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_group_stats_request))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_group_stats_request *stats =
        (struct rofl::openflow13::ofp_group_stats_request *)buf;

    stats->group_id = htobe32(group_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofgroup_stats_request::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  // no OpenFLow 1.0 group stats
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_group_stats_request)) {
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    struct rofl::openflow12::ofp_group_stats_request *stats =
        (struct rofl::openflow12::ofp_group_stats_request *)buf;

    group_id = be32toh(stats->group_id);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_group_stats_request)) {
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    struct rofl::openflow13::ofp_group_stats_request *stats =
        (struct rofl::openflow13::ofp_group_stats_request *)buf;

    group_id = be32toh(stats->group_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

size_t cofgroup_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_group_stats) +
            bucket_counters.length());
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_group_stats) +
            bucket_counters.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
  return 0;
}

void cofgroup_stats_reply::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  switch (of_version) {
  case rofl::openflow12::OFP_VERSION: {

    struct rofl::openflow12::ofp_group_stats *stats =
        (struct rofl::openflow12::ofp_group_stats *)buf;

    stats->length = htobe16(length());
    stats->group_id = htobe32(group_id);
    stats->ref_count = htobe32(ref_count);
    stats->packet_count = htobe64(packet_count);
    stats->byte_count = htobe64(byte_count);

    bucket_counters.pack((uint8_t *)(stats->bucket_stats),
                         bucket_counters.length());

  } break;
  case rofl::openflow13::OFP_VERSION: {

    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_group_stats *stats =
        (struct rofl::openflow13::ofp_group_stats *)buf;

    stats->length = htobe16(length());
    stats->group_id = htobe32(group_id);
    stats->ref_count = htobe32(ref_count);
    stats->packet_count = htobe64(packet_count);
    stats->byte_count = htobe64(byte_count);
    stats->duration_sec = htobe32(duration_sec);
    stats->duration_nsec = htobe32(duration_nsec);

    bucket_counters.pack((uint8_t *)(stats->bucket_stats),
                         bucket_counters.length());

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofgroup_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  bucket_counters.clear();

  switch (of_version) {
  case rofl::openflow12::OFP_VERSION: {

    if (buflen < sizeof(struct rofl::openflow12::ofp_group_stats))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_group_stats *stats =
        (struct rofl::openflow12::ofp_group_stats *)buf;

    if (be16toh(stats->length) > buflen)
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    group_id = be32toh(stats->group_id);
    ref_count = be32toh(stats->ref_count);
    packet_count = be64toh(stats->packet_count);
    byte_count = be64toh(stats->byte_count);

    uint16_t length = be16toh(stats->length);

    buf += sizeof(struct rofl::openflow12::ofp_group_stats);
    length -= sizeof(struct rofl::openflow12::ofp_group_stats);

    uint32_t bucket_counter_id = 0;

    while (length >= sizeof(struct rofl::openflow12::ofp_bucket_counter)) {
      bucket_counters.add_bucket_counter(bucket_counter_id++)
          .unpack(buf, sizeof(struct rofl::openflow12::ofp_bucket_counter));
      buf += sizeof(struct rofl::openflow12::ofp_bucket_counter);
      length -= sizeof(struct rofl::openflow12::ofp_bucket_counter);
    }

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_group_stats))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_group_stats *stats =
        (struct rofl::openflow13::ofp_group_stats *)buf;

    if (be16toh(stats->length) > buflen)
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    group_id = be32toh(stats->group_id);
    ref_count = be32toh(stats->ref_count);
    packet_count = be64toh(stats->packet_count);
    byte_count = be64toh(stats->byte_count);
    duration_sec = be32toh(stats->duration_sec);
    duration_nsec = be32toh(stats->duration_nsec);

    uint16_t length = be16toh(stats->length);

    buf += sizeof(struct rofl::openflow13::ofp_group_stats);
    length -= sizeof(struct rofl::openflow13::ofp_group_stats);

    uint32_t bucket_counter_id = 0;

    while (length >= sizeof(struct rofl::openflow13::ofp_bucket_counter)) {
      bucket_counters.add_bucket_counter(bucket_counter_id++)
          .unpack(buf, sizeof(struct rofl::openflow13::ofp_bucket_counter));
      buf += sizeof(struct rofl::openflow13::ofp_bucket_counter);
      length -= sizeof(struct rofl::openflow13::ofp_bucket_counter);
    }

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}
