/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofaction.h"

using namespace rofl::openflow;

/*static*/
std::ostream &cofaction::dump(std::ostream &os, const cofaction &action) {
  try {
    switch (action.get_type()) {
    case rofl::openflow::OFPAT_OUTPUT: {
      os << dynamic_cast<const cofaction_output &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_VLAN_VID: {
      os << dynamic_cast<const cofaction_set_vlan_vid &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_VLAN_PCP: {
      os << dynamic_cast<const cofaction_set_vlan_pcp &>(action);
    } break;
    case rofl::openflow::OFPAT_STRIP_VLAN: {
      os << dynamic_cast<const cofaction_strip_vlan &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_DL_SRC: {
      os << dynamic_cast<const cofaction_set_dl_src &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_DL_DST: {
      os << dynamic_cast<const cofaction_set_dl_dst &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_NW_SRC: {
      os << dynamic_cast<const cofaction_set_nw_src &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_NW_DST: {
      os << dynamic_cast<const cofaction_set_nw_dst &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_NW_TOS: {
      os << dynamic_cast<const cofaction_set_nw_tos &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_TP_SRC: {
      os << dynamic_cast<const cofaction_set_tp_src &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_TP_DST: {
      os << dynamic_cast<const cofaction_set_tp_dst &>(action);
    } break;
    case rofl::openflow::OFPAT_COPY_TTL_OUT: {
      os << dynamic_cast<const cofaction_copy_ttl_out &>(action);
    } break;
    case rofl::openflow::OFPAT_COPY_TTL_IN: {
      os << dynamic_cast<const cofaction_copy_ttl_in &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_MPLS_TTL: {
      os << dynamic_cast<const cofaction_set_mpls_ttl &>(action);
    } break;
    case rofl::openflow::OFPAT_DEC_MPLS_TTL: {
      os << dynamic_cast<const cofaction_dec_mpls_ttl &>(action);
    } break;
    case rofl::openflow::OFPAT_PUSH_VLAN: {
      os << dynamic_cast<const cofaction_push_vlan &>(action);
    } break;
    case rofl::openflow::OFPAT_POP_VLAN: {
      os << dynamic_cast<const cofaction_pop_vlan &>(action);
    } break;
    case rofl::openflow::OFPAT_PUSH_MPLS: {
      os << dynamic_cast<const cofaction_push_mpls &>(action);
    } break;
    case rofl::openflow::OFPAT_POP_MPLS: {
      os << dynamic_cast<const cofaction_pop_mpls &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_QUEUE: {
      os << dynamic_cast<const cofaction_set_queue &>(action);
    } break;
    case rofl::openflow::OFPAT_GROUP: {
      os << dynamic_cast<const cofaction_group &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_NW_TTL: {
      os << dynamic_cast<const cofaction_set_nw_ttl &>(action);
    } break;
    case rofl::openflow::OFPAT_DEC_NW_TTL: {
      os << dynamic_cast<const cofaction_dec_nw_ttl &>(action);
    } break;
    case rofl::openflow::OFPAT_SET_FIELD: {
      os << dynamic_cast<const cofaction_set_field &>(action);
    } break;
    case rofl::openflow::OFPAT_PUSH_PBB: {
      os << dynamic_cast<const cofaction_push_pbb &>(action);
    } break;
    case rofl::openflow::OFPAT_POP_PBB: {
      os << dynamic_cast<const cofaction_pop_pbb &>(action);
    } break;
    case rofl::openflow::OFPAT_EXPERIMENTER: {
      os << dynamic_cast<const cofaction_experimenter &>(action);
    } break;
    default: {
      os << action;
    }
    };

  } catch (...) {
    os << action;
  }
  return os;
}

size_t cofaction::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow::ofp_action));
  } break;
  default:
    throw eBadVersion("cofaction::length() invalid version");
  }
}

void cofaction::pack(uint8_t *buf, size_t buflen) {
  len = length(); // use final length

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction::length())
    throw eInvalid("cofaction::pack() buflen too short", __FILE__, __FUNCTION__,
                   __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow::ofp_action *hdr =
        (struct rofl::openflow::ofp_action *)buf;

    hdr->type = htobe16(type);
    hdr->len = htobe16(len);

  } break;
  default:
    throw eBadVersion("cofaction::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction::length())
    throw eInvalid("cofaction::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow::ofp_action *hdr =
        (struct rofl::openflow::ofp_action *)buf;

    type = be16toh(hdr->type);
    len = be16toh(hdr->len);

    if (len < sizeof(struct rofl::openflow::ofp_action))
      throw eBadActionBadLen("cofaction::unpack()", __FILE__, __FUNCTION__,
                             __LINE__);

  } break;
  default:
    throw eBadVersion("cofaction::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_output::check_prerequisites() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (0 == port_no) {
      throw eBadActionBadOutPort("cofaction_output::check_prerequisites()",
                                 __FILE__, __FUNCTION__, __LINE__);
    }
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    if ((rofl::openflow13::OFPP_ANY == port_no) || (0 == port_no)) {
      throw eBadActionBadOutPort("cofaction_output::check_prerequisites()",
                                 __FILE__, __FUNCTION__, __LINE__);
    }
  } break;
  }
}

size_t cofaction_output::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_output);
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_output);
  default:
    throw eBadVersion("cofaction_output::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_output::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_output::length())
    throw eInvalid("cofaction_output::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_output *hdr =
        (struct rofl::openflow10::ofp_action_output *)buf;

    hdr->port = htobe16((uint16_t)port_no);
    hdr->max_len = htobe16(max_len);

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_output *hdr =
        (struct rofl::openflow13::ofp_action_output *)buf;

    hdr->port = htobe32(port_no);
    hdr->max_len = htobe16(max_len);

  } break;
  default:
    throw eBadVersion("cofaction_output::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_output::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_output::length())
    throw eInvalid("cofaction_output::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_output *hdr =
        (struct rofl::openflow10::ofp_action_output *)buf;

    port_no = be16toh(hdr->port);
    max_len = be16toh(hdr->max_len);

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_output *hdr =
        (struct rofl::openflow13::ofp_action_output *)buf;

    port_no = be32toh(hdr->port);
    max_len = be16toh(hdr->max_len);

  } break;
  default:
    throw eBadVersion("cofaction_output::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_vlan_vid::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_vlan_vid);
  default:
    throw eBadVersion("cofaction_set_vlan_vid::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_vlan_vid::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_vlan_vid::length())
    throw eInvalid("cofaction_set_vlan_vid::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_vlan_vid *hdr =
        (struct rofl::openflow10::ofp_action_vlan_vid *)buf;

    hdr->vlan_vid = htobe16(vlan_vid);

  } break;
  default:
    throw eBadVersion("cofaction_set_vlan_vid::pack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_vlan_vid::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_vlan_vid::length())
    throw eInvalid("cofaction_set_vlan_vid::unpack() buflen too short",
                   __FILE__, __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_vlan_vid *hdr =
        (struct rofl::openflow10::ofp_action_vlan_vid *)buf;

    vlan_vid = be16toh(hdr->vlan_vid);

  } break;
  default:
    throw eBadVersion("cofaction_set_vlan_vid::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_vlan_pcp::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_vlan_pcp);
  default:
    throw eBadVersion("cofaction_set_vlan_pcp::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_vlan_pcp::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_vlan_pcp::length())
    throw eInvalid("cofaction_set_vlan_pcp::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_vlan_pcp *hdr =
        (struct rofl::openflow10::ofp_action_vlan_pcp *)buf;

    hdr->vlan_pcp = vlan_pcp;

  } break;
  default:
    throw eBadVersion("cofaction_set_vlan_pcp::pack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_vlan_pcp::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_vlan_pcp::length())
    throw eInvalid("cofaction_set_vlan_pcp::unpack() buflen too short",
                   __FILE__, __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_vlan_pcp *hdr =
        (struct rofl::openflow10::ofp_action_vlan_pcp *)buf;

    vlan_pcp = hdr->vlan_pcp;

  } break;
  default:
    throw eBadVersion("cofaction_set_vlan_pcp::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_strip_vlan::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_header);
  default:
    throw eBadVersion("cofaction_strip_vlan::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_strip_vlan::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_strip_vlan::length())
    throw eInvalid("cofaction_strip_vlan::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_strip_vlan::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_strip_vlan::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_strip_vlan::length())
    throw eInvalid("cofaction_strip_vlan::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_strip_vlan::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_dl_src::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_dl_addr);
  default:
    throw eBadVersion("cofaction_set_dl_src::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_dl_src::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_dl_src::length())
    throw eInvalid("cofaction_set_dl_src::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_dl_addr *hdr =
        (struct rofl::openflow10::ofp_action_dl_addr *)buf;

    macaddr.pack(hdr->dl_addr, OFP_ETH_ALEN);

  } break;
  default:
    throw eBadVersion("cofaction_set_dl_src::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_dl_src::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_dl_src::length())
    throw eInvalid("cofaction_set_dl_src::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_dl_addr *hdr =
        (struct rofl::openflow10::ofp_action_dl_addr *)buf;

    macaddr.unpack(hdr->dl_addr, OFP_ETH_ALEN);

  } break;
  default:
    throw eBadVersion("cofaction_set_dl_src::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_dl_dst::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_dl_addr);
  default:
    throw eBadVersion("cofaction_set_dl_dst::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_dl_dst::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_dl_dst::length())
    throw eInvalid("cofaction_set_dl_dst::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_dl_addr *hdr =
        (struct rofl::openflow10::ofp_action_dl_addr *)buf;

    macaddr.pack(hdr->dl_addr, OFP_ETH_ALEN);

  } break;
  default:
    throw eBadVersion("cofaction_set_dl_dst::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_dl_dst::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_dl_dst::length())
    throw eInvalid("cofaction_set_dl_dst::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_dl_addr *hdr =
        (struct rofl::openflow10::ofp_action_dl_addr *)buf;

    macaddr.unpack(hdr->dl_addr, OFP_ETH_ALEN);

  } break;
  default:
    throw eBadVersion("cofaction_set_dl_dst::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_nw_src::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_nw_addr);
  default:
    throw eBadVersion("cofaction_set_nw_src::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_src::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_src::length())
    throw eInvalid("cofaction_set_nw_src::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_nw_addr *hdr =
        (struct rofl::openflow10::ofp_action_nw_addr *)buf;

    addr.pack((uint8_t *)&(hdr->nw_addr), sizeof(hdr->nw_addr));

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_src::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_src::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_src::length())
    throw eInvalid("cofaction_set_nw_src::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_nw_addr *hdr =
        (struct rofl::openflow10::ofp_action_nw_addr *)buf;

    addr.unpack((uint8_t *)&(hdr->nw_addr), sizeof(hdr->nw_addr));

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_src::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_nw_dst::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_nw_addr);
  default:
    throw eBadVersion("cofaction_set_nw_dst::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_dst::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_dst::length())
    throw eInvalid("cofaction_set_nw_dst::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_nw_addr *hdr =
        (struct rofl::openflow10::ofp_action_nw_addr *)buf;

    addr.pack((uint8_t *)&(hdr->nw_addr), sizeof(hdr->nw_addr));

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_dst::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_dst::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_dst::length())
    throw eInvalid("cofaction_set_nw_dst::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_nw_addr *hdr =
        (struct rofl::openflow10::ofp_action_nw_addr *)buf;

    addr.unpack((uint8_t *)&(hdr->nw_addr), sizeof(hdr->nw_addr));

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_dst::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_nw_tos::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_nw_tos);
  default:
    throw eBadVersion("cofaction_set_nw_tos::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_tos::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_tos::length())
    throw eInvalid("cofaction_set_nw_tos::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_nw_tos *hdr =
        (struct rofl::openflow10::ofp_action_nw_tos *)buf;

    hdr->nw_tos = nw_tos;

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_tos::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_tos::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_tos::length())
    throw eInvalid("cofaction_set_nw_tos::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_nw_tos *hdr =
        (struct rofl::openflow10::ofp_action_nw_tos *)buf;

    nw_tos = hdr->nw_tos;

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_tos::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_tp_src::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_tp_port);
  default:
    throw eBadVersion("cofaction_set_tp_src::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_tp_src::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_tp_src::length())
    throw eInvalid("cofaction_set_tp_src::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_tp_port *hdr =
        (struct rofl::openflow10::ofp_action_tp_port *)buf;

    hdr->tp_port = htobe16(tp_src);

  } break;
  default:
    throw eBadVersion("cofaction_set_tp_src::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_tp_src::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_tp_src::length())
    throw eInvalid("cofaction_set_tp_src::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_tp_port *hdr =
        (struct rofl::openflow10::ofp_action_tp_port *)buf;

    tp_src = be16toh(hdr->tp_port);

  } break;
  default:
    throw eBadVersion("cofaction_set_tp_src::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_tp_dst::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_tp_port);
  default:
    throw eBadVersion("cofaction_set_tp_dst::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_tp_dst::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_tp_dst::length())
    throw eInvalid("cofaction_set_tp_dst::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_tp_port *hdr =
        (struct rofl::openflow10::ofp_action_tp_port *)buf;

    hdr->tp_port = htobe16(tp_dst);

  } break;
  default:
    throw eBadVersion("cofaction_set_tp_dst::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_tp_dst::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_tp_dst::length())
    throw eInvalid("cofaction_set_tp_dst::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_tp_port *hdr =
        (struct rofl::openflow10::ofp_action_tp_port *)buf;

    tp_dst = be16toh(hdr->tp_port);

  } break;
  default:
    throw eBadVersion("cofaction_set_tp_dst::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_enqueue::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct rofl::openflow10::ofp_action_enqueue);
  default:
    throw eBadVersion("cofaction_enqueue::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_enqueue::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_enqueue::length())
    throw eInvalid("cofaction_enqueue::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_enqueue *hdr =
        (struct rofl::openflow10::ofp_action_enqueue *)buf;

    hdr->port = htobe16(port_no);
    hdr->queue_id = htobe32(queue_id);

  } break;
  default:
    throw eBadVersion("cofaction_enqueue::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_enqueue::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_enqueue::length())
    throw eInvalid("cofaction_enqueue::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_enqueue *hdr =
        (struct rofl::openflow10::ofp_action_enqueue *)buf;

    port_no = be16toh(hdr->port);
    queue_id = be32toh(hdr->queue_id);

  } break;
  default:
    throw eBadVersion("cofaction_enqueue::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_vendor::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    size_t total_length =
        sizeof(struct rofl::openflow10::ofp_action_vendor_header) +
        exp_body.length();

    size_t pad = (0x7 & total_length);

    /* append padding if not a multiple of 8 */
    if (pad) {
      total_length += 8 - pad;
    }
    return total_length;
  } break;
  default:
    throw eBadVersion("cofaction_vendor::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_vendor::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_vendor::length())
    throw eInvalid("cofaction_vendor::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_vendor_header *hdr =
        (struct rofl::openflow10::ofp_action_vendor_header *)buf;

    hdr->vendor = htobe32(exp_id);

    exp_body.pack(hdr->data, exp_body.length());

  } break;
  default:
    throw eBadVersion("cofaction_vendor::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_vendor::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  exp_body.resize(0);

  if (buflen < cofaction_vendor::length())
    throw eInvalid("cofaction_vendor::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_action_vendor_header *hdr =
        (struct rofl::openflow10::ofp_action_vendor_header *)buf;

    exp_id = be32toh(hdr->vendor);

    if (get_length() >
        sizeof(struct rofl::openflow10::ofp_action_vendor_header)) {
      exp_body.unpack(
          hdr->data,
          get_length() -
              sizeof(struct rofl::openflow10::ofp_action_vendor_header));
    }

  } break;
  default:
    throw eBadVersion("cofaction_vendor::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_mpls_ttl::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_mpls_ttl);
  default:
    throw eBadVersion("cofaction_set_mpls_ttl::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_mpls_ttl::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_mpls_ttl::length())
    throw eInvalid("cofaction_set_mpls_ttl::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_mpls_ttl *hdr =
        (struct rofl::openflow13::ofp_action_mpls_ttl *)buf;

    hdr->mpls_ttl = mpls_ttl;

  } break;
  default:
    throw eBadVersion("cofaction_set_mpls_ttl::pack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_mpls_ttl::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_mpls_ttl::length())
    throw eInvalid("cofaction_set_mpls_ttl::unpack() buflen too short",
                   __FILE__, __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_mpls_ttl *hdr =
        (struct rofl::openflow13::ofp_action_mpls_ttl *)buf;

    mpls_ttl = hdr->mpls_ttl;

  } break;
  default:
    throw eBadVersion("cofaction_set_mpls_ttl::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_dec_mpls_ttl::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_header);
  default:
    throw eBadVersion("cofaction_dec_mpls_ttl::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_dec_mpls_ttl::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_dec_mpls_ttl::length())
    throw eInvalid("cofaction_dec_mpls_ttl::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_dec_mpls_ttl::pack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_dec_mpls_ttl::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_dec_mpls_ttl::length())
    throw eInvalid("cofaction_dec_mpls_ttl::unpack() buflen too short",
                   __FILE__, __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_dec_mpls_ttl::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_push_vlan::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_push);
  default:
    throw eBadVersion("cofaction_push_vlan::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_push_vlan::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_push_vlan::length())
    throw eInvalid("cofaction_push_vlan::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_push *hdr =
        (struct rofl::openflow13::ofp_action_push *)buf;

    hdr->ethertype = htobe16(eth_type);

  } break;
  default:
    throw eBadVersion("cofaction_push_vlan::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_push_vlan::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_push_vlan::length())
    throw eInvalid("cofaction_push_vlan::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_push *hdr =
        (struct rofl::openflow13::ofp_action_push *)buf;

    eth_type = be16toh(hdr->ethertype);

  } break;
  default:
    throw eBadVersion("cofaction_push_vlan::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_pop_vlan::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_header);
  default:
    throw eBadVersion("cofaction_pop_vlan::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_pop_vlan::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_pop_vlan::length())
    throw eInvalid("cofaction_pop_vlan::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_pop_vlan::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_pop_vlan::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_pop_vlan::length())
    throw eInvalid("cofaction_pop_vlan::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_pop_vlan::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_push_mpls::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_push);
  default:
    throw eBadVersion("cofaction_push_mpls::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_push_mpls::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_push_mpls::length())
    throw eInvalid("cofaction_push_mpls::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_push *hdr =
        (struct rofl::openflow13::ofp_action_push *)buf;

    hdr->ethertype = htobe16(eth_type);

  } break;
  default:
    throw eBadVersion("cofaction_push_mpls::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_push_mpls::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_push_mpls::length())
    throw eInvalid("cofaction_push_mpls::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_push *hdr =
        (struct rofl::openflow13::ofp_action_push *)buf;

    eth_type = be16toh(hdr->ethertype);

  } break;
  default:
    throw eBadVersion("cofaction_push_mpls::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_pop_mpls::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_pop_mpls);
  default:
    throw eBadVersion("cofaction_pop_mpls::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_pop_mpls::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_pop_mpls::length())
    throw eInvalid("cofaction_pop_mpls::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_pop_mpls *hdr =
        (struct rofl::openflow13::ofp_action_pop_mpls *)buf;

    hdr->ethertype = htobe16(eth_type);

  } break;
  default:
    throw eBadVersion("cofaction_pop_mpls::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_pop_mpls::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_pop_mpls::length())
    throw eInvalid("cofaction_pop_mpls::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_pop_mpls *hdr =
        (struct rofl::openflow13::ofp_action_pop_mpls *)buf;

    eth_type = be16toh(hdr->ethertype);

  } break;
  default:
    throw eBadVersion("cofaction_pop_mpls::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_group::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_group);
  default:
    throw eBadVersion("cofaction_group::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_group::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_group::length())
    throw eInvalid("cofaction_group::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_group *hdr =
        (struct rofl::openflow13::ofp_action_group *)buf;

    hdr->group_id = htobe32(group_id);

  } break;
  default:
    throw eBadVersion("cofaction_group::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_group::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_group::length())
    throw eInvalid("cofaction_group::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_group *hdr =
        (struct rofl::openflow13::ofp_action_group *)buf;

    group_id = be32toh(hdr->group_id);

  } break;
  default:
    throw eBadVersion("cofaction_group::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_nw_ttl::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_nw_ttl);
  default:
    throw eBadVersion("cofaction_set_nw_ttl::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_ttl::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_ttl::length())
    throw eInvalid("cofaction_set_nw_ttl::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_nw_ttl *hdr =
        (struct rofl::openflow13::ofp_action_nw_ttl *)buf;

    hdr->nw_ttl = nw_ttl;

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_ttl::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_nw_ttl::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_nw_ttl::length())
    throw eInvalid("cofaction_set_nw_ttl::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_nw_ttl *hdr =
        (struct rofl::openflow13::ofp_action_nw_ttl *)buf;

    nw_ttl = hdr->nw_ttl;

  } break;
  default:
    throw eBadVersion("cofaction_set_nw_ttl::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_dec_nw_ttl::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_header);
  default:
    throw eBadVersion("cofaction_dec_nw_ttl::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_dec_nw_ttl::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_dec_nw_ttl::length())
    throw eInvalid("cofaction_dec_nw_ttl::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_dec_nw_ttl::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_dec_nw_ttl::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_dec_nw_ttl::length())
    throw eInvalid("cofaction_dec_nw_ttl::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_dec_nw_ttl::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_copy_ttl_out::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_header);
  default:
    throw eBadVersion("cofaction_copy_ttl_out::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_copy_ttl_out::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_copy_ttl_out::length())
    throw eInvalid("cofaction_copy_ttl_out::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_copy_ttl_out::pack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_copy_ttl_out::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_copy_ttl_out::length())
    throw eInvalid("cofaction_copy_ttl_out::unpack() buflen too short",
                   __FILE__, __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_copy_ttl_out::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_copy_ttl_in::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_header);
  default:
    throw eBadVersion("cofaction_copy_ttl_in::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_copy_ttl_in::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_copy_ttl_in::length())
    throw eInvalid("cofaction_copy_ttl_in::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_copy_ttl_in::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_copy_ttl_in::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_copy_ttl_in::length())
    throw eInvalid("cofaction_copy_ttl_in::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_copy_ttl_in::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_queue::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_set_queue);
  default:
    throw eBadVersion("cofaction_set_queue::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_queue::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_queue::length())
    throw eInvalid("cofaction_set_queue::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_set_queue *hdr =
        (struct rofl::openflow13::ofp_action_set_queue *)buf;

    hdr->queue_id = htobe32(queue_id);

  } break;
  default:
    throw eBadVersion("cofaction_set_queue::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_queue::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_queue::length())
    throw eInvalid("cofaction_set_queue::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_set_queue *hdr =
        (struct rofl::openflow13::ofp_action_set_queue *)buf;

    queue_id = be32toh(hdr->queue_id);

  } break;
  default:
    throw eBadVersion("cofaction_set_queue::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_set_field::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    size_t total_length =
        sizeof(struct rofl::openflow::ofp_action_header) - 4 * sizeof(uint8_t);

    switch (oxm_set_field_type) {
    case OXM_TYPE_8: {
      total_length += oxm_8.length();
    } break;
    case OXM_TYPE_16: {
      total_length += oxm_16.length();
    } break;
    case OXM_TYPE_24: {
      total_length += oxm_24.length();
    } break;
    case OXM_TYPE_32: {
      total_length += oxm_32.length();
    } break;
    case OXM_TYPE_48: {
      total_length += oxm_48.length();
    } break;
    case OXM_TYPE_64: {
      total_length += oxm_64.length();
    } break;
    case OXM_TYPE_128: {
      total_length += oxm_128.length();
    } break;
    case OXM_TYPE_EXP: {
      total_length += oxm_exp.length();
    } break;
    default: {
    };
    }

    size_t pad = (0x7 & total_length);

    /* append padding if not a multiple of 8 */
    if (pad) {
      total_length += 8 - pad;
    }
    return total_length;
  } break;
  default:
    throw eBadVersion("cofaction_set_field::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_field::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_field::length())
    throw eInvalid("cofaction_set_field::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_set_field *hdr =
        (struct rofl::openflow13::ofp_action_set_field *)buf;

    switch (oxm_set_field_type) {
    case OXM_TYPE_8: {
      oxm_8.pack(hdr->field, oxm_8.length());
    } break;
    case OXM_TYPE_16: {
      oxm_16.pack(hdr->field, oxm_16.length());
    } break;
    case OXM_TYPE_24: {
      oxm_24.pack(hdr->field, oxm_24.length());
    } break;
    case OXM_TYPE_32: {
      oxm_32.pack(hdr->field, oxm_32.length());
    } break;
    case OXM_TYPE_48: {
      oxm_48.pack(hdr->field, oxm_48.length());
    } break;
    case OXM_TYPE_64: {
      oxm_64.pack(hdr->field, oxm_64.length());
    } break;
    case OXM_TYPE_128: {
      oxm_128.pack(hdr->field, oxm_128.length());
    } break;
    case OXM_TYPE_EXP: {
      oxm_exp.pack(hdr->field, oxm_exp.length());
    } break;
    default: {
    };
    }

  } break;
  default:
    throw eBadVersion("cofaction_set_field::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_set_field::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_set_field::length())
    throw eInvalid("cofaction_set_field::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_set_field *hdr =
        (struct rofl::openflow13::ofp_action_set_field *)buf;

    if (get_length() > buflen)
      throw eBadActionBadLen("cofaction_set_field::unpack()", __FILE__,
                             __FUNCTION__, __LINE__);

    struct rofl::openflow::ofp_oxm_hdr *oxm_hdr =
        (struct rofl::openflow::ofp_oxm_hdr *)hdr->field;

    size_t oxm_len =
        sizeof(struct rofl::openflow::ofp_oxm_hdr) + oxm_hdr->oxm_length;

    if (oxm_len >
        (get_length() - sizeof(struct rofl::openflow13::ofp_action_header) +
         4 * sizeof(uint8_t)))
      throw eBadActionBadLen("cofaction_set_field::unpack()", __FILE__,
                             __FUNCTION__, __LINE__);

    if (oxm_hdr->oxm_field & 0x01 /*has_mask?*/) {

      switch (be16toh(oxm_hdr->oxm_class)) {
      case rofl::openflow::OFPXMC_OPENFLOW_BASIC: {
        switch (oxm_hdr->oxm_length) {
        case 2 /*bytes*/: {
          set_oxm_8().unpack(hdr->field, oxm_len);
        } break;
        case 4 /*bytes*/: {
          set_oxm_16().unpack(hdr->field, oxm_len);
        } break;
        case 6 /*bytes*/: {
          set_oxm_24().unpack(hdr->field, oxm_len);
        } break;
        case 8 /*bytes*/: {
          set_oxm_32().unpack(hdr->field, oxm_len);
        } break;
        case 12 /*bytes*/: {
          set_oxm_48().unpack(hdr->field, oxm_len);
        } break;
        case 16 /*bytes*/: {
          set_oxm_64().unpack(hdr->field, oxm_len);
        } break;
        case 32 /*bytes*/: {
          set_oxm_128().unpack(hdr->field, oxm_len);
        } break;
        default: {
        };
        }

      } break;
      case rofl::openflow::OFPXMC_EXPERIMENTER: {
        set_oxm_exp().unpack(hdr->field, oxm_len);

      } break;
      }

    } else {

      switch (be16toh(oxm_hdr->oxm_class)) {
      case rofl::openflow::OFPXMC_OPENFLOW_BASIC: {
        switch (oxm_hdr->oxm_length) {
        case 1 /*byte*/: {
          set_oxm_8().unpack(hdr->field, oxm_len);
        } break;
        case 2 /*bytes*/: {
          set_oxm_16().unpack(hdr->field, oxm_len);
        } break;
        case 3 /*bytes*/: {
          set_oxm_24().unpack(hdr->field, oxm_len);
        } break;
        case 4 /*bytes*/: {
          set_oxm_32().unpack(hdr->field, oxm_len);
        } break;
        case 6 /*bytes*/: {
          set_oxm_48().unpack(hdr->field, oxm_len);
        } break;
        case 8 /*bytes*/: {
          set_oxm_64().unpack(hdr->field, oxm_len);
        } break;
        case 16 /*bytes*/: {
          set_oxm_128().unpack(hdr->field, oxm_len);
        } break;
        default: {
        };
        }

      } break;
      case rofl::openflow::OFPXMC_EXPERIMENTER: {
        set_oxm_exp().unpack(hdr->field, oxm_len);

      } break;
      }
    }

  } break;
  default:
    throw eBadVersion("cofaction_set_field::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_experimenter::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    size_t total_length =
        sizeof(struct rofl::openflow13::ofp_action_experimenter_header) +
        exp_body.length();

    size_t pad = (0x7 & total_length);

    /* append padding if not a multiple of 8 */
    if (pad) {
      total_length += 8 - pad;
    }
    return total_length;
  } break;
  default:
    throw eBadVersion("cofaction_experimenter::length() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_experimenter::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_experimenter::length())
    throw eInvalid("cofaction_experimenter::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_experimenter_header *hdr =
        (struct rofl::openflow13::ofp_action_experimenter_header *)buf;

    hdr->experimenter = htobe32(exp_id);

    exp_body.pack(hdr->data, exp_body.length());

  } break;
  default:
    throw eBadVersion("cofaction_experimenter::pack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofaction_experimenter::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  exp_body.resize(0);

  if (buflen < cofaction_experimenter::length())
    throw eInvalid("cofaction_experimenter::unpack() buflen too short",
                   __FILE__, __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_experimenter_header *hdr =
        (struct rofl::openflow13::ofp_action_experimenter_header *)buf;

    exp_id = be32toh(hdr->experimenter);

    if (get_length() > buflen)
      throw eBadActionBadLen("cofaction::experimenter()");

    if (get_length() >
        sizeof(struct rofl::openflow13::ofp_action_experimenter_header)) {
      exp_body.unpack(
          hdr->data,
          get_length() -
              sizeof(struct rofl::openflow13::ofp_action_experimenter_header));
    }

  } break;
  default:
    throw eBadVersion("cofaction_experimenter::unpack() invalid version",
                      __FILE__, __FUNCTION__, __LINE__);
  }
}

size_t cofaction_push_pbb::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_push);
  default:
    throw eBadVersion("cofaction_push_pbb::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_push_pbb::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_push_pbb::length())
    throw eInvalid("cofaction_push_pbb::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_push *hdr =
        (struct rofl::openflow13::ofp_action_push *)buf;

    hdr->ethertype = htobe16(eth_type);

  } break;
  default:
    throw eBadVersion("cofaction_push_pbb::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_push_pbb::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_push_pbb::length())
    throw eInvalid("cofaction_push_pbb::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow13::ofp_action_push *hdr =
        (struct rofl::openflow13::ofp_action_push *)buf;

    eth_type = be16toh(hdr->ethertype);

  } break;
  default:
    throw eBadVersion("cofaction_push_pbb::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

size_t cofaction_pop_pbb::length() const {
  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct rofl::openflow13::ofp_action_header);
  default:
    throw eBadVersion("cofaction_pop_pbb::length() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_pop_pbb::pack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_pop_pbb::length())
    throw eInvalid("cofaction_pop_pbb::pack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::pack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_pop_pbb::pack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}

void cofaction_pop_pbb::unpack(uint8_t *buf, size_t buflen) {
  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofaction_pop_pbb::length())
    throw eInvalid("cofaction_pop_pbb::unpack() buflen too short", __FILE__,
                   __FUNCTION__, __LINE__);

  cofaction::unpack(buf, sizeof(struct rofl::openflow::ofp_action_header));

  switch (get_version()) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

  } break;
  default:
    throw eBadVersion("cofaction_pop_pbb::unpack() invalid version", __FILE__,
                      __FUNCTION__, __LINE__);
  }
}
