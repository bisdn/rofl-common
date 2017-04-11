/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofinstruction.h"

using namespace rofl::openflow;

size_t cofinstruction::length() const {
  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct ofp_instruction));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct ofp_instruction *hdr = (struct ofp_instruction *)buf;

    hdr->type = htobe16(type);
    hdr->len = htobe16(length()); // take virtual length()

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct ofp_instruction *hdr = (struct ofp_instruction *)buf;

    type = be16toh(hdr->type);

    if (be16toh(hdr->len) < cofinstruction::length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofinstruction_actions::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_instruction_actions) +
            actions.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_actions::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction_actions::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::pack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_actions *hdr =
        (struct rofl::openflow13::ofp_instruction_actions *)buf;

    actions.pack((uint8_t *)hdr->actions, actions.length());

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_actions::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  actions.clear();

  if (buflen < cofinstruction_actions::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::unpack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_actions *hdr =
        (struct rofl::openflow13::ofp_instruction_actions *)buf;

    if (be16toh(hdr->len) < cofinstruction_actions::length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    actions.unpack(
        (uint8_t *)hdr->actions,
        be16toh(hdr->len) -
            sizeof(struct rofl::openflow13::ofp_instruction_actions));

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofinstruction_goto_table::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_instruction_goto_table));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_goto_table::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction_goto_table::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::pack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_goto_table *hdr =
        (struct rofl::openflow13::ofp_instruction_goto_table *)buf;

    hdr->table_id = table_id;

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_goto_table::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < sizeof(struct rofl::openflow13::ofp_instruction_goto_table))
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::unpack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_goto_table *hdr =
        (struct rofl::openflow13::ofp_instruction_goto_table *)buf;

    if (be16toh(hdr->len) < cofinstruction_goto_table::length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    table_id = hdr->table_id;

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_goto_table::check_prerequisites() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (rofl::openflow13::OFPTT_ALL == get_table_id()) {
      throw eBadInstBadTableId();
    }
  } break;
  default: {
    // do nothing
  };
  }
};

size_t cofinstruction_write_metadata::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_instruction_write_metadata));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_write_metadata::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction_write_metadata::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::pack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_write_metadata *hdr =
        (struct rofl::openflow13::ofp_instruction_write_metadata *)buf;

    hdr->metadata = htobe64(metadata);
    hdr->metadata_mask = htobe64(metadata_mask);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_write_metadata::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction_write_metadata::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::unpack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_write_metadata *hdr =
        (struct rofl::openflow13::ofp_instruction_write_metadata *)buf;

    if (be16toh(hdr->len) < cofinstruction_write_metadata::length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    metadata = be64toh(hdr->metadata);
    metadata_mask = be64toh(hdr->metadata_mask);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofinstruction_meter::length() const {
  switch (get_version()) {
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_instruction_meter));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_meter::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction_meter::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::pack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_meter *hdr =
        (struct rofl::openflow13::ofp_instruction_meter *)buf;

    hdr->meter_id = htobe32(meter_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_meter::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction_meter::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::unpack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_meter *hdr =
        (struct rofl::openflow13::ofp_instruction_meter *)buf;

    if (be16toh(hdr->len) < cofinstruction_meter::length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    meter_id = be32toh(hdr->meter_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofinstruction_experimenter::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_instruction_experimenter) +
            exp_body.memlen());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_experimenter::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofinstruction_experimenter::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::pack(buf, buflen);

    struct rofl::openflow13::ofp_instruction_experimenter *hdr =
        (struct rofl::openflow13::ofp_instruction_experimenter *)buf;

    hdr->experimenter = htobe32(exp_id);

    exp_body.pack(hdr->body, exp_body.memlen());

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofinstruction_experimenter::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  exp_body.clear();

  if (buflen < cofinstruction_experimenter::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    cofinstruction::unpack(buf, sizeof(struct ofp_instruction));

    struct rofl::openflow13::ofp_instruction_experimenter *hdr =
        (struct rofl::openflow13::ofp_instruction_experimenter *)buf;

    if (be16toh(hdr->len) < cofinstruction_experimenter::length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    exp_id = be32toh(hdr->experimenter);

    exp_body.unpack(
        hdr->body,
        be16toh(hdr->len) -
            sizeof(struct rofl::openflow13::ofp_instruction_experimenter));

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}
