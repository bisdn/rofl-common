/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofaggrstats.h"

using namespace rofl::openflow;

void cofaggr_stats_request::pack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < (sizeof(struct openflow10::ofp_flow_stats_request)))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow10::ofp_flow_stats_request *req =
        (struct openflow10::ofp_flow_stats_request *)buf;
    req->table_id = table_id;
    req->out_port = htobe16((uint16_t)(out_port & 0x0000ffff));
    match.pack((uint8_t *)&(req->match), sizeof(struct openflow10::ofp_match));

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < (sizeof(struct openflow12::ofp_flow_stats_request) -
                  sizeof(struct openflow12::ofp_match) + match.length()))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow12::ofp_flow_stats_request *req =
        (struct openflow12::ofp_flow_stats_request *)buf;
    req->table_id = table_id;
    req->out_port = htobe32(out_port);
    req->out_group = htobe32(out_group);
    req->cookie = htobe64(cookie);
    req->cookie_mask = htobe64(cookie_mask);
    match.pack((uint8_t *)&(req->match),
               buflen - sizeof(struct openflow12::ofp_flow_stats_request) +
                   sizeof(struct openflow12::ofp_match));

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofaggr_stats_request::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct openflow10::ofp_flow_stats_request))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow10::ofp_flow_stats_request *req =
        (struct openflow10::ofp_flow_stats_request *)buf;

    match.unpack((uint8_t *)&(req->match),
                 sizeof(struct openflow10::ofp_match));
    table_id = req->table_id;
    out_port = (uint32_t)(be16toh(req->out_port));

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct openflow12::ofp_flow_stats_request))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow12::ofp_flow_stats_request *req =
        (struct openflow12::ofp_flow_stats_request *)buf;

    match.unpack((uint8_t *)&(req->match),
                 buflen - sizeof(struct openflow12::ofp_flow_stats_request) +
                     sizeof(struct openflow12::ofp_match));
    table_id = req->table_id;
    out_port = be32toh(req->out_port);
    out_group = be32toh(req->out_group);
    cookie = be64toh(req->cookie);
    cookie_mask = be64toh(req->cookie_mask);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

size_t cofaggr_stats_request::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return sizeof(struct openflow10::ofp_flow_stats_request);
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct openflow12::ofp_flow_stats_request) -
            sizeof(struct openflow12::ofp_match) + match.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofaggr_stats_reply::pack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow10::ofp_aggregate_stats_reply *as =
        (struct openflow10::ofp_aggregate_stats_reply *)buf;
    as->packet_count = htobe64(packet_count);
    as->byte_count = htobe64(byte_count);
    as->flow_count = htobe32(flow_count);

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow12::ofp_aggregate_stats_reply *as =
        (struct openflow12::ofp_aggregate_stats_reply *)buf;
    as->packet_count = htobe64(packet_count);
    as->byte_count = htobe64(byte_count);
    as->flow_count = htobe32(flow_count);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofaggr_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct openflow10::ofp_aggregate_stats_reply))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow10::ofp_aggregate_stats_reply *as =
        (struct openflow10::ofp_aggregate_stats_reply *)buf;
    packet_count = be64toh(as->packet_count);
    byte_count = be64toh(as->byte_count);
    flow_count = be32toh(as->flow_count);

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct openflow12::ofp_aggregate_stats_reply))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct openflow12::ofp_aggregate_stats_reply *as =
        (struct openflow12::ofp_aggregate_stats_reply *)buf;
    packet_count = be64toh(as->packet_count);
    byte_count = be64toh(as->byte_count);
    flow_count = be32toh(as->flow_count);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

size_t cofaggr_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct openflow10::ofp_aggregate_stats_reply));
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct openflow12::ofp_aggregate_stats_reply));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
  return 0;
}
