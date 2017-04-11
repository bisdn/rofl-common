/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofportstats.h"

using namespace rofl::openflow;

size_t cofport_stats_request::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return sizeof(struct rofl::openflow10::ofp_port_stats_request);
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return sizeof(struct rofl::openflow12::ofp_port_stats_request);
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return sizeof(struct rofl::openflow13::ofp_port_stats_request);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport_stats_request::pack(uint8_t *buf, size_t buflen) const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow10::ofp_port_stats_request))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_port_stats_request *req =
        (struct rofl::openflow10::ofp_port_stats_request *)buf;
    req->port_no = htobe16((uint16_t)(port_no & 0x0000ffff));

  } break;
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_port_stats_request))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_port_stats_request *req =
        (struct rofl::openflow12::ofp_port_stats_request *)buf;
    req->port_no = htobe32(port_no);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_port_stats_request))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_port_stats_request *req =
        (struct rofl::openflow13::ofp_port_stats_request *)buf;
    req->port_no = htobe32(port_no);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport_stats_request::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow10::ofp_port_stats_request))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_port_stats_request *req =
        (struct rofl::openflow10::ofp_port_stats_request *)buf;
    port_no = be16toh(req->port_no);

  } break;
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_port_stats_request))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_port_stats_request *req =
        (struct rofl::openflow12::ofp_port_stats_request *)buf;
    port_no = be32toh(req->port_no);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_port_stats_request))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_port_stats_request *req =
        (struct rofl::openflow13::ofp_port_stats_request *)buf;
    port_no = be32toh(req->port_no);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofport_stats_reply::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct rofl::openflow10::ofp_port_stats));
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_port_stats));
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_port_stats));
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
  return 0;
}

void cofport_stats_reply::pack(uint8_t *buf, size_t buflen) const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_port_stats *ps =
        (struct rofl::openflow10::ofp_port_stats *)buf;

    ps->port_no = htobe16((uint16_t)(port_no & 0x0000ffff));
    ps->rx_packets = htobe64(rx_packets);
    ps->tx_packets = htobe64(tx_packets);
    ps->rx_bytes = htobe64(rx_bytes);
    ps->tx_bytes = htobe64(tx_bytes);
    ps->rx_dropped = htobe64(rx_dropped);
    ps->tx_dropped = htobe64(tx_dropped);
    ps->rx_errors = htobe64(rx_errors);
    ps->tx_errors = htobe64(tx_errors);
    ps->rx_frame_err = htobe64(rx_frame_err);
    ps->rx_over_err = htobe64(rx_over_err);
    ps->rx_crc_err = htobe64(rx_crc_err);
    ps->collisions = htobe64(collisions);

  } break;
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_port_stats *ps =
        (struct rofl::openflow12::ofp_port_stats *)buf;

    ps->port_no = htobe32(port_no);
    ps->rx_packets = htobe64(rx_packets);
    ps->tx_packets = htobe64(tx_packets);
    ps->rx_bytes = htobe64(rx_bytes);
    ps->tx_bytes = htobe64(tx_bytes);
    ps->rx_dropped = htobe64(rx_dropped);
    ps->tx_dropped = htobe64(tx_dropped);
    ps->rx_errors = htobe64(rx_errors);
    ps->tx_errors = htobe64(tx_errors);
    ps->rx_frame_err = htobe64(rx_frame_err);
    ps->rx_over_err = htobe64(rx_over_err);
    ps->rx_crc_err = htobe64(rx_crc_err);
    ps->collisions = htobe64(collisions);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < length())
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_port_stats *ps =
        (struct rofl::openflow13::ofp_port_stats *)buf;

    ps->port_no = htobe32(port_no);
    ps->rx_packets = htobe64(rx_packets);
    ps->tx_packets = htobe64(tx_packets);
    ps->rx_bytes = htobe64(rx_bytes);
    ps->tx_bytes = htobe64(tx_bytes);
    ps->rx_dropped = htobe64(rx_dropped);
    ps->tx_dropped = htobe64(tx_dropped);
    ps->rx_errors = htobe64(rx_errors);
    ps->tx_errors = htobe64(tx_errors);
    ps->rx_frame_err = htobe64(rx_frame_err);
    ps->rx_over_err = htobe64(rx_over_err);
    ps->rx_crc_err = htobe64(rx_crc_err);
    ps->collisions = htobe64(collisions);
    ps->duration_sec = htobe32(duration_sec);
    ps->duration_nsec = htobe32(duration_nsec);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport_stats_reply::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow10::ofp_port_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow10::ofp_port_stats *ps =
        (struct rofl::openflow10::ofp_port_stats *)buf;
    port_no = (uint32_t)be16toh(ps->port_no);
    rx_packets = be64toh(ps->rx_packets);
    tx_packets = be64toh(ps->tx_packets);
    rx_bytes = be64toh(ps->rx_bytes);
    tx_bytes = be64toh(ps->tx_bytes);
    rx_dropped = be64toh(ps->rx_dropped);
    tx_dropped = be64toh(ps->tx_dropped);
    rx_errors = be64toh(ps->rx_errors);
    tx_errors = be64toh(ps->tx_errors);
    rx_frame_err = be64toh(ps->rx_frame_err);
    rx_over_err = be64toh(ps->rx_over_err);
    rx_crc_err = be64toh(ps->rx_crc_err);
    collisions = be64toh(ps->collisions);

  } break;
  case rofl::openflow12::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow12::ofp_port_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow12::ofp_port_stats *ps =
        (struct rofl::openflow12::ofp_port_stats *)buf;
    port_no = be32toh(ps->port_no);
    rx_packets = be64toh(ps->rx_packets);
    tx_packets = be64toh(ps->tx_packets);
    rx_bytes = be64toh(ps->rx_bytes);
    tx_bytes = be64toh(ps->tx_bytes);
    rx_dropped = be64toh(ps->rx_dropped);
    tx_dropped = be64toh(ps->tx_dropped);
    rx_errors = be64toh(ps->rx_errors);
    tx_errors = be64toh(ps->tx_errors);
    rx_frame_err = be64toh(ps->rx_frame_err);
    rx_over_err = be64toh(ps->rx_over_err);
    rx_crc_err = be64toh(ps->rx_crc_err);
    collisions = be64toh(ps->collisions);

  } break;
  case rofl::openflow13::OFP_VERSION: {
    if (buflen < sizeof(struct rofl::openflow13::ofp_port_stats))
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

    struct rofl::openflow13::ofp_port_stats *ps =
        (struct rofl::openflow13::ofp_port_stats *)buf;
    port_no = be32toh(ps->port_no);
    rx_packets = be64toh(ps->rx_packets);
    tx_packets = be64toh(ps->tx_packets);
    rx_bytes = be64toh(ps->rx_bytes);
    tx_bytes = be64toh(ps->tx_bytes);
    rx_dropped = be64toh(ps->rx_dropped);
    tx_dropped = be64toh(ps->tx_dropped);
    rx_errors = be64toh(ps->rx_errors);
    tx_errors = be64toh(ps->tx_errors);
    rx_frame_err = be64toh(ps->rx_frame_err);
    rx_over_err = be64toh(ps->rx_over_err);
    rx_crc_err = be64toh(ps->rx_crc_err);
    collisions = be64toh(ps->collisions);
    duration_sec = be32toh(ps->duration_sec);
    duration_nsec = be32toh(ps->duration_nsec);

  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}
