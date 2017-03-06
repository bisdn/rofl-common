/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmatch.h"

using namespace rofl::openflow;

cofmatch::cofmatch(uint8_t of_version, uint16_t type)
    : of_version(of_version), type(type) {}

cofmatch::~cofmatch() {}

cofmatch::cofmatch(cofmatch const &match) { *this = match; }

cofmatch &cofmatch::operator=(const cofmatch &match) {
  if (this == &match)
    return *this;

  of_version = match.of_version;
  matches = match.matches;
  type = match.type;

  return *this;
}

bool cofmatch::operator==(const cofmatch &match) const {
  return ((of_version == match.of_version) && (matches == match.matches) &&
          (type == match.type));
}

size_t cofmatch::length() const {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION: {
    return sizeof(struct rofl::openflow10::ofp_match);
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    size_t total_length =
        2 * sizeof(uint16_t) +
        matches.length(); // type-field + length-field + OXM-TLV list

    size_t pad = (0x7 & total_length);
    /* append padding if not a multiple of 8 */
    if (pad) {
      total_length += 8 - pad;
    }
    return total_length;
  }
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
  return 0;
}

void cofmatch::pack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION:
    return pack_of10(buf, buflen);
    break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    return pack_of13(buf, buflen);
    break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofmatch::unpack(uint8_t *buf, size_t buflen) {
  switch (of_version) {
  case rofl::openflow10::OFP_VERSION:
    unpack_of10(buf, buflen);
    break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
    unpack_of13(buf, buflen);
    break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }
}

void cofmatch::pack_of10(uint8_t *buf, size_t buflen) {
  if (buflen < length()) {
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }

  uint32_t wildcards = 0;

  memset(buf, 0, buflen);

  struct rofl::openflow10::ofp_match *m =
      (struct rofl::openflow10::ofp_match *)buf;

  // in_port
  if (matches.has_ofb_in_port()) {
    m->in_port = htobe16(matches.get_ofb_in_port().get_u32value() & 0x0000ffff);
  } else {
    wildcards |= rofl::openflow10::OFPFW_IN_PORT;
  }

  // dl_src
  if (matches.has_ofb_eth_src()) {
    matches.get_ofb_eth_src().get_u48value_as_lladdr().pack(m->dl_src,
                                                            OFP_ETH_ALEN);
  } else {
    wildcards |= rofl::openflow10::OFPFW_DL_SRC;
  }

  // dl_dst
  if (matches.has_ofb_eth_dst()) {
    matches.get_ofb_eth_dst().get_u48value_as_lladdr().pack(m->dl_dst,
                                                            OFP_ETH_ALEN);
  } else {
    wildcards |= rofl::openflow10::OFPFW_DL_DST;
  }

  // dl_vlan
  if (matches.has_ofb_vlan_vid()) {
    m->dl_vlan = htobe16(matches.get_ofb_vlan_vid().get_u16value());
  } else {
    wildcards |= rofl::openflow10::OFPFW_DL_VLAN;
  }

  // dl_vlan_pcp
  if (matches.has_ofb_vlan_pcp()) {
    m->dl_vlan_pcp = matches.get_ofb_vlan_pcp().get_u8value();
  } else {
    wildcards |= rofl::openflow10::OFPFW_DL_VLAN_PCP;
  }

  // dl_type
  if (matches.has_ofb_eth_type()) {
    m->dl_type = htobe16(matches.get_ofb_eth_type().get_u16value());
  } else {
    wildcards |= rofl::openflow10::OFPFW_DL_TYPE;
  }

  // nw_tos
  if (matches.has_ofx_nw_tos()) {
    m->nw_tos = matches.get_ofx_nw_tos().get_u8value();
  } else {
    wildcards |= rofl::openflow10::OFPFW_NW_TOS;
  }

  // nw_proto
  if (matches.has_ofx_nw_proto()) {
    m->nw_proto = matches.get_ofx_nw_proto().get_u8value();
  } else {
    wildcards |= rofl::openflow10::OFPFW_NW_PROTO;
  }

  // nw_src
  if (matches.has_ofx_nw_src()) {
    m->nw_src = htobe32(matches.get_ofx_nw_src().get_u32value());
    if (matches.get_ofx_nw_src().get_oxm_hasmask()) {
      std::bitset<32> mask(matches.get_ofx_nw_src().get_u32mask());
      wildcards |=
          ((32 - mask.count()) << rofl::openflow10::OFPFW_NW_SRC_SHIFT) &
          rofl::openflow10::OFPFW_NW_SRC_MASK;
    }
  } else {
    wildcards |= rofl::openflow10::OFPFW_NW_SRC_ALL;
  }

  // nw_dst
  if (matches.has_ofx_nw_dst()) {
    m->nw_dst = htobe32(matches.get_ofx_nw_dst().get_u32value());
    if (matches.get_ofx_nw_dst().get_oxm_hasmask()) {
      std::bitset<32> mask(matches.get_ofx_nw_dst().get_u32mask());
      wildcards |=
          ((32 - mask.count()) << rofl::openflow10::OFPFW_NW_DST_SHIFT) &
          rofl::openflow10::OFPFW_NW_DST_MASK;
    }
  } else {
    wildcards |= rofl::openflow10::OFPFW_NW_DST_ALL;
  }

  // tp_src
  if (matches.has_ofx_tp_src()) {
    m->tp_src = htobe16(matches.get_ofx_tp_src().get_u16value());
  } else {
    wildcards |= rofl::openflow10::OFPFW_TP_SRC;
  }

  // tp_dst
  if (matches.has_ofx_tp_dst()) {
    m->tp_dst = htobe16(matches.get_ofx_tp_dst().get_u16value());
  } else {
    wildcards |= rofl::openflow10::OFPFW_TP_DST;
  }

  m->wildcards = htobe32(wildcards);
}

void cofmatch::unpack_of10(uint8_t *buf, size_t buflen) {
  matches.clear();

  if (buflen < sizeof(struct rofl::openflow10::ofp_match)) {
    throw eBadMatchBadLen("eBadMatchBadLen", __FILE__, __PRETTY_FUNCTION__,
                          __LINE__);
  }

  struct rofl::openflow10::ofp_match *m =
      (struct rofl::openflow10::ofp_match *)buf;

  uint32_t wildcards = be32toh(m->wildcards);

  // in_port
  if (!(wildcards & rofl::openflow10::OFPFW_IN_PORT)) {
    matches.add_ofb_in_port(be16toh(m->in_port));
  }

  // dl_src
  if (!(wildcards & rofl::openflow10::OFPFW_DL_SRC)) {
    matches.add_ofb_eth_src(rofl::caddress_ll(m->dl_src, OFP_ETH_ALEN));
  }

  // dl_dst
  if (!(wildcards & rofl::openflow10::OFPFW_DL_DST)) {
    matches.add_ofb_eth_dst(rofl::caddress_ll(m->dl_dst, OFP_ETH_ALEN));
  }

  // dl_vlan
  if (!(wildcards & rofl::openflow10::OFPFW_DL_VLAN)) {
    matches.add_ofb_vlan_vid(be16toh(m->dl_vlan));
  }

  // dl_vlan_pcp
  if (!(wildcards & rofl::openflow10::OFPFW_DL_VLAN_PCP) &&
      m->dl_vlan !=
          0xffff) { // 0xFFFF value is used to indicate that no VLAN id eas set.
    matches.add_ofb_vlan_pcp(m->dl_vlan_pcp);
  }

  // dl_type
  if (!(wildcards & rofl::openflow10::OFPFW_DL_TYPE)) {
    const uint16_t dl_type = be16toh(m->dl_type);
    matches.add_ofb_eth_type(dl_type);

    // The following text is added in the OpenFlow 1.0.1 specification:
    // Protocol-specific fields within ofp_match must be ignored when the
    // corresponding protocol
    // is not specified in the match. The IP header and transport header fields
    // must be ignored
    // unless the Ethertype is specified as either IPv4 or ARP. The tp_src and
    // tp_dst fields must
    // be ignored unless the network protocol is set to TCP, UDP or ICMP. The
    // dl_vlan_pcp field
    // must be ignored when the OFPFW_DL_VLAN wildcard bit is set or when the
    // dl_vlan value is
    // set to OFP_VLAN_NONE. Fields that are ignored don't need to be wildcarded
    // and should be
    // set to 0.

    if (dl_type == 0x0806 /* ARP */ || dl_type == 0x0800 /* IPv4 */) {
      // nw_src
      {
        uint64_t num_of_bits =
            (wildcards & rofl::openflow10::OFPFW_NW_SRC_MASK) >>
            openflow10::OFPFW_NW_SRC_SHIFT;
        if (num_of_bits > 32)
          num_of_bits = 32;
        uint64_t u_mask = ~((1UL << num_of_bits) - 1UL);
        rofl::caddress_in4 addr;
        addr.set_addr_nbo(m->nw_src);
        rofl::caddress_in4 mask;
        mask.set_addr_nbo(htobe32((uint32_t)u_mask));
        if (num_of_bits <= 32) {
          matches.add_ofx_nw_src(addr, mask);
        }
      }

      // nw_dst
      {
        uint64_t num_of_bits =
            (wildcards & rofl::openflow10::OFPFW_NW_DST_MASK) >>
            openflow10::OFPFW_NW_DST_SHIFT;
        if (num_of_bits > 32)
          num_of_bits = 32;
        uint64_t u_mask = ~((1UL << num_of_bits) - 1UL);
        rofl::caddress_in4 addr;
        addr.set_addr_nbo(m->nw_dst);
        rofl::caddress_in4 mask;
        mask.set_addr_nbo(htobe32((uint32_t)u_mask));
        if (num_of_bits <= 32) {
          matches.add_ofx_nw_dst(addr, mask);
        }
      }

      if (dl_type == 0x0800 /* IPv4 */) {
        // nw_tos
        if (!(wildcards & rofl::openflow10::OFPFW_NW_TOS)) {
          matches.add_ofx_nw_tos(m->nw_tos);
        }

        // nw_proto
        if (!(wildcards & rofl::openflow10::OFPFW_NW_PROTO)) {
          const uint8_t nw_proto = m->nw_proto;
          matches.add_ofx_nw_proto(nw_proto);

          if (nw_proto == 6 /* TCP */ || nw_proto == 17 /* UDP */ ||
              nw_proto == 1 /* ICMP */) {
            // tp_src
            if (!(wildcards & rofl::openflow10::OFPFW_TP_SRC)) {
              matches.add_ofx_tp_src(be16toh(m->tp_src));
            }

            // tp_dst
            if (!(wildcards & rofl::openflow10::OFPFW_TP_DST)) {
              matches.add_ofx_tp_dst(be16toh(m->tp_dst));
            }
          }
        }
      }

      if (dl_type == 0x0806 /* ARP */) {
        // arp_opcode
        if (!(wildcards & rofl::openflow10::OFPFW_NW_PROTO)) {
          const uint8_t arp_opcode = m->nw_proto;
          matches.add_ofb_arp_opcode(arp_opcode);
        }
      }
    }
  }
}

void cofmatch::pack_of13(uint8_t *buf, size_t buflen) {
  if (buflen < length()) {
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }

  struct rofl::openflow13::ofp_match *m =
      (struct rofl::openflow13::ofp_match *)buf;

  m->type = htobe16(type);
  m->length = htobe16(2 * sizeof(uint16_t) +
                      matches.length()); // real length without padding

  matches.pack(m->oxm_fields, matches.length());
}

void cofmatch::unpack_of13(uint8_t *buf, size_t buflen) {
  matches.clear();

  if (buflen < 2 * sizeof(uint16_t)) {
    throw eBadMatchBadLen("eBadMatchBadLen", __FILE__, __PRETTY_FUNCTION__,
                          __LINE__);
  }

  struct rofl::openflow13::ofp_match *m =
      (struct rofl::openflow13::ofp_match *)buf;

  type = be16toh(m->type);

  if (rofl::openflow13::OFPMT_OXM != type) {
    throw eBadMatchBadType("eBadMatchBadType", __FILE__, __PRETTY_FUNCTION__,
                           __LINE__);
  }

  buflen -= 2 * sizeof(uint16_t);

  if (buflen > 0) {
    matches.unpack(m->oxm_fields, buflen);
  }
}

void cofmatch::check_prerequisites() const {
  switch (of_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    /*
     * these are generic prerequisites as defined in OF 1.3, section 7.2.3.6,
     * page 53 ff.
     */
    if (matches.has_ofb_in_phy_port()) {
      if (not matches.has_ofb_in_port()) {
        std::cerr << "[rofl][match] rejecting ofp_match: IN-PHY-PORT defined "
                     "while no IN-PORT is present"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_vlan_pcp()) {
      if (openflow::OFPVID_NONE == matches.get_ofb_vlan_vid().get_u16value()) {
        std::cerr << "[rofl][match] rejecting ofp_match: VLAN-PCP defined "
                     "while VID is set to OFPVID-NONE"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ip_dscp()) {
      if ((not matches.has_ofb_eth_type()) ||
          ((matches.get_ofb_eth_type().get_u16value() != 0x0800) &&
           (matches.get_ofb_eth_type().get_u16value() != 0x86dd))) {
        std::cerr << "[rofl][match] rejecting ofp_match: IP-DSCP defined while "
                     "ETH-TYPE is not IPv4/IPv6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ip_ecn()) {
      if ((not matches.has_ofb_eth_type()) ||
          ((matches.get_ofb_eth_type().get_u16value() != 0x0800) &&
           (matches.get_ofb_eth_type().get_u16value() != 0x86dd))) {
        std::cerr << "[rofl][match] rejecting ofp_match: IP-ECN defined while "
                     "ETH-TYPE is not IPv4/IPv6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ip_proto()) {
      if ((not matches.has_ofb_eth_type()) ||
          ((matches.get_ofb_eth_type().get_u16value() != 0x0800) &&
           (matches.get_ofb_eth_type().get_u16value() != 0x86dd))) {
        std::cerr << "[rofl][match] rejecting ofp_match: IP-PROTO defined "
                     "while ETH-TYPE is not IPv4/IPv6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv4_src()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x0800)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPV4-SRC defined "
                     "while ETH-TYPE is not IPv4"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv4_dst()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x0800)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPV4-DST defined "
                     "while ETH-TYPE is not IPv4"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_tcp_src()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 6)) {
        std::cerr << "[rofl][match] rejecting ofp_match: TCP-SRC defined while "
                     "IP-PROTO is not TCP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_tcp_dst()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 6)) {
        std::cerr << "[rofl][match] rejecting ofp_match: TCP-DST defined while "
                     "IP-PROTO is not TCP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_udp_src()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 17)) {
        std::cerr << "[rofl][match] rejecting ofp_match: UDP-SRC defined while "
                     "IP-PROTO is not UDP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_udp_dst()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 17)) {
        std::cerr << "[rofl][match] rejecting ofp_match: UDP-DST defined while "
                     "IP-PROTO is not UDP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_sctp_src()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 132)) {
        std::cerr << "[rofl][match] rejecting ofp_match: SCTP-SRC defined "
                     "while IP-PROTO is not SCTP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_sctp_dst()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 132)) {
        std::cerr << "[rofl][match] rejecting ofp_match: SCTP-DST defined "
                     "while IP-PROTO is not SCTP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_icmpv4_type()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 1)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ICMPV4-TYPE defined "
                     "while IP-PROTO is not ICMPV4"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_icmpv4_code()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 1)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ICMPV4-CODE defined "
                     "while IP-PROTO is not ICMPV4"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_arp_opcode()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x0806)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ARP-OPCODE defined "
                     "while ETH-TYPE is not ARP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_arp_spa()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x0806)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ARP-SPA defined while "
                     "ETH-TYPE is not ARP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_arp_tpa()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x0806)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ARP-TPA defined while "
                     "ETH-TYPE is not ARP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_arp_sha()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x0806)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ARP-SHA defined while "
                     "ETH-TYPE is not ARP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_arp_tha()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x0806)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ARP-THA defined while "
                     "ETH-TYPE is not ARP"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv6_src()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x86dd)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPV6-SRC defined "
                     "while ETH-TYPE is not IPv6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv6_dst()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x86dd)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPV6-DST defined "
                     "while ETH-TYPE is not IPv6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv6_flabel()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x86dd)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPV6-FLABEL defined "
                     "while ETH-TYPE is not IPv6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_icmpv6_type()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 58)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ICMPV6-TYPE defined "
                     "while IP-PROTO is not ICMPV6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_icmpv6_code()) {
      if ((not matches.has_ofb_ip_proto()) ||
          (matches.get_ofb_ip_proto().get_u8value() != 58)) {
        std::cerr << "[rofl][match] rejecting ofp_match: ICMPV6-CODE defined "
                     "while IP-PROTO is not ICMPV6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv6_nd_target()) {
      if ((not matches.has_ofb_icmpv6_type()) ||
          ((matches.get_ofb_icmpv6_type().get_u8value() != 135) &&
           (matches.get_ofb_icmpv6_type().get_u8value() != 136))) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPv6-ND-TARGET "
                     "defined while ICMPV6-TYPE is not ND-SOLICITATION or "
                     "ND-ADVERTISEMENT"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv6_nd_sll()) {
      if ((not matches.has_ofb_icmpv6_type()) ||
          (matches.get_ofb_icmpv6_type().get_u8value() != 135)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPv6-ND-SLL defined "
                     "while ICMPV6-TYPE is not ND-SOLICITATION"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv6_nd_tll()) {
      if ((not matches.has_ofb_icmpv6_type()) ||
          (matches.get_ofb_icmpv6_type().get_u8value() != 136)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPv6-ND-TLL defined "
                     "while ICMPV6-TYPE is not ND-ADVERTISEMENT"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_mpls_label()) {
      if ((not matches.has_ofb_eth_type()) ||
          ((matches.get_ofb_eth_type().get_u16value() != 0x8847) &&
           (matches.get_ofb_eth_type().get_u16value() != 0x8848))) {
        std::cerr << "[rofl][match] rejecting ofp_match: MPLS-LABEL defined "
                     "while ETH-TYPE is not MPLS/MPLS-UPSTREAM"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_mpls_tc()) {
      if ((not matches.has_ofb_eth_type()) ||
          ((matches.get_ofb_eth_type().get_u16value() != 0x8847) &&
           (matches.get_ofb_eth_type().get_u16value() != 0x8848))) {
        std::cerr << "[rofl][match] rejecting ofp_match: MPLS-TC defined while "
                     "ETH-TYPE is not MPLS/MPLS-UPSTREAM"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_mpls_bos()) {
      if ((not matches.has_ofb_eth_type()) ||
          ((matches.get_ofb_eth_type().get_u16value() != 0x8847) &&
           (matches.get_ofb_eth_type().get_u16value() != 0x8848))) {
        std::cerr << "[rofl][match] rejecting ofp_match: MPLS-BOS defined "
                     "while ETH-TYPE is not MPLS/MPLS-UPSTREAM"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_pbb_isid()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x88e7)) {
        std::cerr << "[rofl][match] rejecting ofp_match: PBB-ISID defined "
                     "while ETH-TYPE is not PBB"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

    if (matches.has_ofb_ipv6_exthdr()) {
      if ((not matches.has_ofb_eth_type()) ||
          (matches.get_ofb_eth_type().get_u16value() != 0x86dd)) {
        std::cerr << "[rofl][match] rejecting ofp_match: IPV6-EXTHDR defined "
                     "while ETH-TYPE is not IPv6"
                  << std::endl
                  << matches;
        throw eBadMatchBadPrereq("eBadMatchBadPrereq", __FILE__,
                                 __PRETTY_FUNCTION__, __LINE__);
      }
    }

  } break;
  default: {
    // do nothing
  };
  }
}
