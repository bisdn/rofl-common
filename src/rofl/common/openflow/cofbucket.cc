/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofbucket.h"

using namespace rofl::openflow;

uint8_t *cofbucket::pack(uint8_t *bucket, size_t bclen) {
  switch (ofp_version) {
  case openflow12::OFP_VERSION: {
    return pack_of12(bucket, bclen);
  } break;
  case openflow13::OFP_VERSION: {
    return pack_of13(bucket, bclen);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofbucket::unpack(uint8_t *bucket, size_t bclen) {
  switch (ofp_version) {
  case openflow12::OFP_VERSION: {
    unpack_of12(bucket, bclen);
  } break;
  case openflow13::OFP_VERSION: {
    unpack_of13(bucket, bclen);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

uint8_t *cofbucket::pack_of12(uint8_t *buf, size_t buflen) {
  struct openflow12::ofp_bucket *bucket = (struct openflow12::ofp_bucket *)buf;
  size_t __bclen = sizeof(struct openflow12::ofp_bucket) + actions.length();

  if (buflen < __bclen)
    throw eBucketBadLen();

  bucket->len = htobe16(__bclen);
  bucket->weight = htobe16(weight);
  bucket->watch_port = htobe32(watch_port);
  bucket->watch_group = htobe32(watch_group);

  size_t aclen = buflen - sizeof(struct openflow12::ofp_bucket);

  actions.pack((uint8_t *)bucket->actions, aclen);

  return buf;
}

void cofbucket::unpack_of12(uint8_t *buf, size_t buflen) {
  if (buflen < sizeof(struct openflow12::ofp_bucket))
    return;

  struct openflow12::ofp_bucket *bucket = (struct openflow12::ofp_bucket *)buf;

  weight = be16toh(bucket->weight);
  watch_port = be32toh(bucket->watch_port);
  watch_group = be32toh(bucket->watch_group);

  size_t aclen = buflen - sizeof(struct openflow12::ofp_bucket);

  if (aclen >= sizeof(struct openflow12::ofp_action_header)) {
    actions.unpack((uint8_t *)bucket->actions, aclen);
  }
}

uint8_t *cofbucket::pack_of13(uint8_t *buf, size_t buflen) {
  struct openflow13::ofp_bucket *bucket = (struct openflow13::ofp_bucket *)buf;
  size_t __bclen = sizeof(struct openflow13::ofp_bucket) + actions.length();

  if (buflen < __bclen)
    throw eBucketBadLen();

  bucket->len = htobe16(__bclen);
  bucket->weight = htobe16(weight);
  bucket->watch_port = htobe32(watch_port);
  bucket->watch_group = htobe32(watch_group);

  size_t aclen = buflen - sizeof(struct openflow13::ofp_bucket);

  actions.pack((uint8_t *)bucket->actions, aclen);

  return buf;
}

void cofbucket::unpack_of13(uint8_t *buf, size_t buflen) {
  if (buflen < sizeof(struct openflow13::ofp_bucket))
    return;

  struct openflow13::ofp_bucket *bucket = (struct openflow13::ofp_bucket *)buf;

  weight = be16toh(bucket->weight);
  watch_port = be32toh(bucket->watch_port);
  watch_group = be32toh(bucket->watch_group);

  size_t aclen = buflen - sizeof(struct openflow13::ofp_bucket);

  if (aclen >= sizeof(struct openflow13::ofp_action_header)) {
    actions.unpack((uint8_t *)bucket->actions, aclen);
  }
}

size_t cofbucket::length() const {
  size_t total_length = 16 * sizeof(uint8_t) + actions.length();

  size_t pad = (0x7 & total_length);

  /* append padding if not a multiple of 8 */
  if (pad) {
    total_length += 8 - pad;
  }

  return (total_length);
}

void cofbucket::get_bucket_stats(cmemory &body) {
  cmemory bstats(sizeof(struct openflow12::ofp_bucket_counter));
  struct openflow12::ofp_bucket_counter *bucket_counter =
      (struct openflow12::ofp_bucket_counter *)bstats.somem();

  bucket_counter->packet_count = htobe64(packet_count);
  bucket_counter->byte_count = htobe64(byte_count);

  body += bstats;
}

void cofbucket::check_prerequisites() const { actions.check_prerequisites(); }
