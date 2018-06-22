/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofdescstats.h"

using namespace rofl::openflow;

size_t cofdesc_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct openflow10::ofp_desc_stats));
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct openflow12::ofp_desc_stats));
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct openflow13::ofp_desc));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
  return 0;
}

void cofdesc_stats_reply::pack(uint8_t *buf, size_t buflen) const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct openflow10::ofp_desc_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct openflow10::ofp_desc_stats *desc =
        (struct openflow10::ofp_desc_stats *)buf;

    snprintf(desc->mfr_desc, DESC_STR_LEN, mfr_desc.c_str(), mfr_desc.length());
    snprintf(desc->hw_desc, DESC_STR_LEN, hw_desc.c_str(), hw_desc.length());
    snprintf(desc->sw_desc, DESC_STR_LEN, sw_desc.c_str(), sw_desc.length());
    snprintf(desc->serial_num, SERIAL_NUM_LEN, serial_num.c_str(),
             serial_num.length());
    snprintf(desc->dp_desc, DESC_STR_LEN, dp_desc.c_str(), dp_desc.length());
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct openflow12::ofp_desc_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct openflow12::ofp_desc_stats *desc =
        (struct openflow12::ofp_desc_stats *)buf;

    snprintf(desc->mfr_desc, DESC_STR_LEN, mfr_desc.c_str(), mfr_desc.length());
    snprintf(desc->hw_desc, DESC_STR_LEN, hw_desc.c_str(), hw_desc.length());
    snprintf(desc->sw_desc, DESC_STR_LEN, sw_desc.c_str(), sw_desc.length());
    snprintf(desc->serial_num, SERIAL_NUM_LEN, serial_num.c_str(),
             serial_num.length());
    snprintf(desc->dp_desc, DESC_STR_LEN, dp_desc.c_str(), dp_desc.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofdesc_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct openflow10::ofp_desc_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct openflow10::ofp_desc_stats *desc =
        (struct openflow10::ofp_desc_stats *)buf;

    mfr_desc.assign(desc->mfr_desc, strnlen(desc->mfr_desc, DESC_STR_LEN));
    hw_desc.assign(desc->hw_desc, strnlen(desc->hw_desc, DESC_STR_LEN));
    sw_desc.assign(desc->sw_desc, strnlen(desc->sw_desc, DESC_STR_LEN));
    serial_num.assign(desc->serial_num, strnlen(desc->serial_num, SERIAL_NUM_LEN));
    dp_desc.assign(desc->dp_desc, strnlen(desc->dp_desc, DESC_STR_LEN));

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct openflow12::ofp_desc_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct openflow12::ofp_desc_stats *desc =
        (struct openflow12::ofp_desc_stats *)buf;

    mfr_desc.assign(desc->mfr_desc, DESC_STR_LEN);
    hw_desc.assign(desc->hw_desc, DESC_STR_LEN);
    sw_desc.assign(desc->sw_desc, DESC_STR_LEN);
    serial_num.assign(desc->serial_num, SERIAL_NUM_LEN);
    dp_desc.assign(desc->dp_desc, DESC_STR_LEN);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}
