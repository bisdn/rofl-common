/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofmeterconfig.h"

using namespace rofl::openflow;

size_t cofmeter_config_request::length() const {
  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    return sizeof(struct rofl::openflow13::ofp_meter_multipart_request);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofmeter_config_request::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_multipart_request *req =
        (struct rofl::openflow13::ofp_meter_multipart_request *)buf;

    req->meter_id = htobe32(meter_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofmeter_config_request::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_multipart_request *req =
        (struct rofl::openflow13::ofp_meter_multipart_request *)buf;

    meter_id = be32toh(req->meter_id);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofmeter_config_reply::length() const {
  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_meter_config) +
            mbands.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
  return 0;
}

void cofmeter_config_reply::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_config *meter_config =
        (struct rofl::openflow13::ofp_meter_config *)buf;

    meter_config->flags = htobe16(flags);
    meter_config->meter_id = htobe32(meter_id);
    meter_config->length = htobe16(length());

    mbands.pack((uint8_t *)(meter_config->bands), mbands.length());

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofmeter_config_reply::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (of_version) {
  case rofl::openflow13::OFP_VERSION: {
    mbands.clear();

    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_meter_config *meter_config =
        (struct rofl::openflow13::ofp_meter_config *)buf;

    if (be16toh(meter_config->length) <
        sizeof(struct rofl::openflow13::ofp_meter_config)) {
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);
    }

    flags = be16toh(meter_config->flags);
    meter_id = be32toh(meter_config->meter_id);

    uint16_t mbands_len = be16toh(meter_config->length) -
                          sizeof(struct rofl::openflow13::ofp_meter_config);

    if (mbands_len > 0) {
      mbands.unpack((uint8_t *)(meter_config->bands), mbands_len);
    }

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}
