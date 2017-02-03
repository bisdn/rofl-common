/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/coftablefeatures.h"

using namespace rofl::openflow;

size_t coftable_features::length() const {
  switch (get_version()) {
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_table_features) +
            properties.length());
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void coftable_features::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  switch (ofp_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < coftable_features::length())
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_table_features *hdr =
        (struct rofl::openflow13::ofp_table_features *)buf;

    hdr->length = htobe16(length());
    hdr->table_id = table_id;
    strncpy(hdr->name, name.c_str(), OFP_MAX_TABLE_NAME_LEN);
    hdr->metadata_match = htobe64(metadata_match);
    hdr->metadata_write = htobe64(metadata_write);
    hdr->config = htobe32(config);
    hdr->max_entries = htobe32(max_entries);

    properties.pack(buf + sizeof(struct rofl::openflow13::ofp_table_features),
                    buflen -
                        sizeof(struct rofl::openflow13::ofp_table_features));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void coftable_features::unpack(uint8_t *buf, size_t buflen) {
  switch (ofp_version) {
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_table_features))
      throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_table_features *hdr =
        (struct rofl::openflow13::ofp_table_features *)buf;

    table_id = hdr->table_id;
    name.assign(hdr->name, OFP_MAX_TABLE_NAME_LEN);
    metadata_match = be64toh(hdr->metadata_match);
    metadata_write = be64toh(hdr->metadata_write);
    config = be32toh(hdr->config);
    max_entries = be32toh(hdr->max_entries);

    /* real length in buffer */
    size_t rlen = buflen - sizeof(struct rofl::openflow13::ofp_table_features);
    /* specified length in structure */
    size_t slen = be16toh(hdr->length) -
                  sizeof(struct rofl::openflow13::ofp_table_features);
    /* get smaller specified length, in any case not more bytes than available
     * in the buffer */
    size_t proplen = slen < rlen ? slen : rlen;

    properties.unpack((uint8_t *)&(hdr->properties), proplen);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}
