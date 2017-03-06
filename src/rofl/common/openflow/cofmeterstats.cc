/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofmeterstats.h"

using namespace rofl::openflow;

size_t cofmeter_stats_request::length() const {
  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    return sizeof(struct rofl::openflow13::ofp_meter_multipart_request);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofmeter_stats_request::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_multipart_request *req =
        (struct rofl::openflow13::ofp_meter_multipart_request *)buf;

    req->meter_id = htobe32(meter_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofmeter_stats_request::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_multipart_request *req =
        (struct rofl::openflow13::ofp_meter_multipart_request *)buf;

    meter_id = be32toh(req->meter_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

size_t cofmeter_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_meter_stats) +
            mbstats.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
  return 0;
}

void cofmeter_stats_reply::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_stats *meter_stats =
        (struct rofl::openflow13::ofp_meter_stats *)buf;

    meter_stats->meter_id = htobe32(meter_id);
    meter_stats->len = htobe16(length());
    meter_stats->flow_count = htobe32(flow_count);
    meter_stats->packet_in_count = htobe64(packet_in_count);
    meter_stats->byte_in_count = htobe64(byte_in_count);
    meter_stats->duration_sec = htobe32(duration_sec);
    meter_stats->duration_nsec = htobe32(duration_nsec);

    mbstats.pack((uint8_t *)(meter_stats->band_stats), mbstats.length());

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofmeter_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    mbstats.clear();

    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_stats *meter_stats =
        (struct rofl::openflow13::ofp_meter_stats *)buf;

    if (be16toh(meter_stats->len) <
        sizeof(struct rofl::openflow13::ofp_meter_stats)) {
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }

    meter_id = be32toh(meter_stats->meter_id);
    flow_count = be32toh(meter_stats->flow_count);
    packet_in_count = be64toh(meter_stats->packet_in_count);
    byte_in_count = be64toh(meter_stats->byte_in_count);
    duration_sec = be32toh(meter_stats->duration_sec);
    duration_nsec = be32toh(meter_stats->duration_nsec);

    uint16_t mbstats_len = be16toh(meter_stats->len) -
                           sizeof(struct rofl::openflow13::ofp_meter_stats);

    if (mbstats_len > 0) {
      mbstats.unpack((uint8_t *)(meter_stats->band_stats), mbstats_len);
    }

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}
