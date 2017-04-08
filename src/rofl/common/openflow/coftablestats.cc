/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/coftablestats.h"

using namespace rofl::openflow;

size_t coftable_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct rofl::openflow10::ofp_table_stats));
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_table_stats));
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_table_stats));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void coftable_stats_reply::pack(uint8_t *buf, size_t buflen) const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow10::ofp_table_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_table_stats *table_stats =
        (struct rofl::openflow10::ofp_table_stats *)buf;

    table_stats->table_id = table_id;
    snprintf(table_stats->name, OFP_MAX_TABLE_NAME_LEN, name.c_str(),
             name.length());
    table_stats->wildcards =
        htobe32((uint32_t)(wildcards & 0x00000000ffffffff));
    table_stats->max_entries = htobe32(max_entries);
    table_stats->active_count = htobe32(active_count);
    table_stats->lookup_count = htobe64(lookup_count);
    table_stats->matched_count = htobe64(matched_count);

  } break;
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_table_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_table_stats *table_stats =
        (struct rofl::openflow12::ofp_table_stats *)buf;

    table_stats->table_id = table_id;
    snprintf(table_stats->name, OFP_MAX_TABLE_NAME_LEN, name.c_str(),
             name.length());
    table_stats->match = htobe64(match);
    table_stats->wildcards = htobe64(wildcards);
    table_stats->write_actions = htobe32(write_actions);
    table_stats->apply_actions = htobe32(apply_actions);
    table_stats->write_setfields = htobe64(write_setfields);
    table_stats->apply_setfields = htobe64(apply_setfields);
    table_stats->metadata_match = htobe64(metadata_match);
    table_stats->metadata_write = htobe64(metadata_write);
    table_stats->instructions = htobe32(instructions);
    table_stats->config = htobe32(config);
    table_stats->max_entries = htobe32(max_entries);
    table_stats->active_count = htobe32(active_count);
    table_stats->lookup_count = htobe64(lookup_count);
    table_stats->matched_count = htobe64(matched_count);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_table_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_table_stats *table_stats =
        (struct rofl::openflow13::ofp_table_stats *)buf;

    table_stats->table_id = table_id;
    table_stats->active_count = htobe32(active_count);
    table_stats->lookup_count = htobe64(lookup_count);
    table_stats->matched_count = htobe64(matched_count);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void coftable_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow10::ofp_table_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_table_stats *table_stats =
        (struct rofl::openflow10::ofp_table_stats *)buf;

    table_id = table_stats->table_id;
    name = std::string(table_stats->name,
                       strnlen(table_stats->name, OFP_MAX_TABLE_NAME_LEN));
    wildcards = be32toh(table_stats->wildcards);
    max_entries = be32toh(table_stats->max_entries);
    active_count = be32toh(table_stats->active_count);
    lookup_count = be64toh(table_stats->lookup_count);
    matched_count = be64toh(table_stats->matched_count);

  } break;
  case openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_table_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_table_stats *table_stats =
        (struct rofl::openflow12::ofp_table_stats *)buf;

    table_id = table_stats->table_id;
    name = std::string(table_stats->name,
                       strnlen(table_stats->name, OFP_MAX_TABLE_NAME_LEN));
    match = be64toh(table_stats->match);
    wildcards = be64toh(table_stats->wildcards);
    write_actions = be32toh(table_stats->write_actions);
    apply_actions = be32toh(table_stats->apply_actions);
    write_setfields = be64toh(table_stats->write_setfields);
    apply_setfields = be64toh(table_stats->apply_setfields);
    metadata_match = be64toh(table_stats->metadata_match);
    metadata_write = be64toh(table_stats->metadata_write);
    instructions = be32toh(table_stats->instructions);
    config = be32toh(table_stats->config);
    max_entries = be32toh(table_stats->max_entries);
    active_count = be32toh(table_stats->active_count);
    lookup_count = be64toh(table_stats->lookup_count);
    matched_count = be64toh(table_stats->matched_count);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_table_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_table_stats *table_stats =
        (struct rofl::openflow13::ofp_table_stats *)buf;

    table_id = table_stats->table_id;
    active_count = be32toh(table_stats->active_count);
    lookup_count = be64toh(table_stats->lookup_count);
    matched_count = be64toh(table_stats->matched_count);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}
