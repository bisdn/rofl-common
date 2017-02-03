/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofflowstats.h"

using namespace rofl::openflow;

size_t cofflow_stats_request::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return sizeof(struct rofl::openflow10::ofp_flow_stats_request);
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_flow_stats_request) -
            sizeof(struct rofl::openflow12::ofp_match) + match.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofflow_stats_request::pack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow::OFP_VERSION_UNKNOWN: {

  } break;
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < cofflow_stats_request::length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_flow_stats_request *req =
        (struct rofl::openflow10::ofp_flow_stats_request *)buf;
    req->table_id = table_id;
    req->out_port = htobe16((uint16_t)(out_port & 0x0000ffff));
    match.pack((uint8_t *)&(req->match),
               sizeof(struct rofl::openflow10::ofp_match));
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < cofflow_stats_request::length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_flow_stats_request *req =
        (struct rofl::openflow12::ofp_flow_stats_request *)buf;
    req->table_id = table_id;
    req->out_port = htobe32(out_port);
    req->out_group = htobe32(out_group);
    req->cookie = htobe64(cookie);
    req->cookie_mask = htobe64(cookie_mask);
    match.pack((uint8_t *)&(req->match),
               buflen -
                   sizeof(struct rofl::openflow12::ofp_flow_stats_request) +
                   sizeof(struct rofl::openflow12::ofp_match));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofflow_stats_request::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow::OFP_VERSION_UNKNOWN: {

  } break;
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow10::ofp_flow_stats_request))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_flow_stats_request *req =
        (struct rofl::openflow10::ofp_flow_stats_request *)buf;

    match.unpack((uint8_t *)&(req->match),
                 sizeof(struct rofl::openflow10::ofp_match));
    table_id = req->table_id;
    out_port = (uint32_t)(be16toh(req->out_port));
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_flow_stats_request))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_flow_stats_request *req =
        (struct rofl::openflow12::ofp_flow_stats_request *)buf;

    match.unpack((uint8_t *)&(req->match),
                 buflen -
                     sizeof(struct rofl::openflow12::ofp_flow_stats_request) +
                     sizeof(struct rofl::openflow12::ofp_match));
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

size_t cofflow_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct rofl::openflow10::ofp_flow_stats) + actions.length());
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_flow_stats) -
            sizeof(struct rofl::openflow12::ofp_match) + match.length() +
            instructions.length());
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_flow_stats) -
            sizeof(struct rofl::openflow13::ofp_match) + match.length() +
            instructions.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
  return 0;
}

void cofflow_stats_reply::pack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow::OFP_VERSION_UNKNOWN: {

  } break;
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_flow_stats *fs =
        (struct rofl::openflow10::ofp_flow_stats *)buf;

    fs->length = htobe16(length());
    fs->table_id = table_id;
    match.pack((uint8_t *)&(fs->match),
               sizeof(struct rofl::openflow10::ofp_match));
    fs->duration_sec = htobe32(duration_sec);
    fs->duration_nsec = htobe32(duration_nsec);
    fs->priority = htobe16(priority);
    fs->idle_timeout = htobe16(idle_timeout);
    fs->hard_timeout = htobe16(hard_timeout);
    fs->cookie = htobe64(cookie);
    fs->packet_count = htobe64(packet_count);
    fs->byte_count = htobe64(byte_count);
    actions.pack((uint8_t *)(fs->actions),
                 buflen - sizeof(struct rofl::openflow10::ofp_flow_stats));

  } break;
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_flow_stats *fs =
        (struct rofl::openflow12::ofp_flow_stats *)buf;

    fs->length = htobe16(length());
    fs->table_id = table_id;
    fs->duration_sec = htobe32(duration_sec);
    fs->duration_nsec = htobe32(duration_nsec);
    fs->priority = htobe16(priority);
    fs->idle_timeout = htobe16(idle_timeout);
    fs->hard_timeout = htobe16(hard_timeout);
    fs->cookie = htobe64(cookie);
    fs->packet_count = htobe64(packet_count);
    fs->byte_count = htobe64(byte_count);

    uint8_t *p_match = buf + sizeof(struct rofl::openflow12::ofp_flow_stats) -
                       sizeof(struct rofl::openflow12::ofp_match);

    match.pack(p_match, match.length());
    instructions.pack(p_match + match.length(), instructions.length());

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_flow_stats *fs =
        (struct rofl::openflow13::ofp_flow_stats *)buf;

    fs->length = htobe16(length());
    fs->table_id = table_id;
    fs->duration_sec = htobe32(duration_sec);
    fs->duration_nsec = htobe32(duration_nsec);
    fs->priority = htobe16(priority);
    fs->idle_timeout = htobe16(idle_timeout);
    fs->hard_timeout = htobe16(hard_timeout);
    fs->flags = htobe16(flags);
    fs->cookie = htobe64(cookie);
    fs->packet_count = htobe64(packet_count);
    fs->byte_count = htobe64(byte_count);

    uint8_t *p_match = buf + sizeof(struct rofl::openflow12::ofp_flow_stats) -
                       sizeof(struct rofl::openflow12::ofp_match);

    match.pack(p_match, match.length());
    instructions.pack(p_match + match.length(), instructions.length());

  } break;

  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofflow_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow::OFP_VERSION_UNKNOWN: {

  } break;
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow10::ofp_flow_stats))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_flow_stats *fs =
        (struct rofl::openflow10::ofp_flow_stats *)buf;

    table_id = fs->table_id;
    duration_sec = be32toh(fs->duration_sec);
    duration_nsec = be32toh(fs->duration_nsec);
    priority = be16toh(fs->priority);
    idle_timeout = be16toh(fs->idle_timeout);
    hard_timeout = be16toh(fs->hard_timeout);
    cookie = be64toh(fs->cookie);
    packet_count = be64toh(fs->packet_count);
    byte_count = be64toh(fs->byte_count);

    match.unpack((uint8_t *)&(fs->match),
                 sizeof(struct rofl::openflow10::ofp_match));
    actions.unpack((uint8_t *)fs->actions,
                   buflen - sizeof(struct rofl::openflow10::ofp_flow_stats));

  } break;
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_flow_stats))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_flow_stats *fs =
        (struct rofl::openflow12::ofp_flow_stats *)buf;

    table_id = fs->table_id;
    duration_sec = be32toh(fs->duration_sec);
    duration_nsec = be32toh(fs->duration_nsec);
    priority = be16toh(fs->priority);
    idle_timeout = be16toh(fs->idle_timeout);
    hard_timeout = be16toh(fs->hard_timeout);
    cookie = be64toh(fs->cookie);
    packet_count = be64toh(fs->packet_count);
    byte_count = be64toh(fs->byte_count);

    // derive length for match
    uint16_t matchlen = be16toh(fs->match.length);

    size_t pad = (0x7 & matchlen);
    /* append padding if not a multiple of 8 */
    if (pad) {
      matchlen += 8 - pad;
    }

    if (buflen < (sizeof(struct rofl::openflow12::ofp_flow_stats) -
                  sizeof(struct rofl::openflow12::ofp_match) + matchlen))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    uint8_t *p_match = buf + sizeof(struct rofl::openflow12::ofp_flow_stats) -
                       sizeof(struct rofl::openflow12::ofp_match);

    match.unpack(p_match, matchlen);
    instructions.unpack(
        (p_match + matchlen),
        buflen - sizeof(struct rofl::openflow12::ofp_flow_stats) +
            sizeof(struct rofl::openflow12::ofp_match) - matchlen);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_flow_stats))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_flow_stats *fs =
        (struct rofl::openflow13::ofp_flow_stats *)buf;

    table_id = fs->table_id;
    duration_sec = be32toh(fs->duration_sec);
    duration_nsec = be32toh(fs->duration_nsec);
    priority = be16toh(fs->priority);
    idle_timeout = be16toh(fs->idle_timeout);
    hard_timeout = be16toh(fs->hard_timeout);
    flags = be16toh(fs->flags);
    cookie = be64toh(fs->cookie);
    packet_count = be64toh(fs->packet_count);
    byte_count = be64toh(fs->byte_count);

    // derive length for match
    uint16_t matchlen = be16toh(fs->match.length);

    size_t pad = (0x7 & matchlen);
    /* append padding if not a multiple of 8 */
    if (pad) {
      matchlen += 8 - pad;
    }

    if (buflen < (sizeof(struct rofl::openflow12::ofp_flow_stats) -
                  sizeof(struct rofl::openflow12::ofp_match) + matchlen))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    uint8_t *p_match = buf + sizeof(struct rofl::openflow12::ofp_flow_stats) -
                       sizeof(struct rofl::openflow12::ofp_match);

    match.unpack(p_match, matchlen);
    instructions.unpack(
        (p_match + matchlen),
        buflen - sizeof(struct rofl::openflow12::ofp_flow_stats) +
            sizeof(struct rofl::openflow12::ofp_match) - matchlen);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}
