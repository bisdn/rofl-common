/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFMATCH_H
#define COFMATCH_H 1

#include <bitset>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <string>
#ifndef be32toh
#include "../endian_conversion.h"
#endif

#include "rofl/common/caddress.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/coxmatches.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class cofmatch {
  uint8_t of_version;
  coxmatches matches;
  uint16_t type;

public:
  /**
   *
   */
  cofmatch(uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
           uint16_t type = rofl::openflow::OFPMT_OXM);

  /**
   *
   */
  cofmatch(const cofmatch &match);

  /**
   *
   */
  virtual ~cofmatch();

  /**
   *
   */
  cofmatch &operator=(const cofmatch &match);

  /**
   *
   */
  bool operator==(const cofmatch &m) const;

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  void check_prerequisites() const;

  /**
   *
   */
  void clear() { matches.clear(); };

  /**
   *
   */
  bool contains(cofmatch const &match, bool strict = false) {
    return matches.contains(match.get_matches(), strict);
  };

  /**
   *
   */
  bool is_part_of(cofmatch const &match, uint16_t &exact_hits,
                  uint16_t &wildcard_hits, uint16_t &missed) {
    return matches.is_part_of(match.get_matches(), exact_hits, wildcard_hits,
                              missed);
  };

public:
  /**
   *
   */
  void set_version(uint8_t ofp_version) { this->of_version = ofp_version; };

  /**
   *
   */
  uint8_t get_version() const { return of_version; };

  /**
   *
   */
  void set_type(uint16_t type) { this->type = type; };

  /**
   *
   */
  uint16_t get_type() const { return type; };

  /**
   *
   */
  coxmatches &set_matches() { return matches; };

  /**
   *
   */
  coxmatches const &get_matches() const { return matches; };

private:
  /**
   *
   */
  void pack_of10(uint8_t *buf, size_t buflen);

  /**
   *
   */
  void unpack_of10(uint8_t *buf, size_t buflen);

  /**
   *
   */
  void pack_of13(uint8_t *buf, size_t buflen);

  /**
   *
   */
  void unpack_of13(uint8_t *m, size_t mlen);

public:
  friend std::ostream &operator<<(std::ostream &os, cofmatch const &m) {
    os << "<cofmatch ofp-version:" << (int)m.get_version() << " >" << std::endl;
    switch (m.of_version) {
    case rofl::openflow12::OFP_VERSION:
    case rofl::openflow13::OFP_VERSION: {
      os << "<type: " << m.type << " >" << std::endl;
    } break;
    }

    os << m.matches;
    return os;
  };

public:
  /*
   * old API: to be or not to be ... deprecated (???)
   */

  // OF10
  uint8_t get_nw_proto() const {
    return matches.get_ofx_nw_proto().get_u8value();
  };
  uint8_t get_nw_tos() const { return matches.get_ofx_nw_tos().get_u8value(); };
  caddress_in4 get_nw_src() const {
    rofl::caddress_in4 addr;
    addr.set_addr_hbo(matches.get_ofx_nw_src().get_u32masked_value());
    return addr;
  };
  caddress_in4 get_nw_src_value() const {
    rofl::caddress_in4 addr;
    addr.set_addr_hbo(matches.get_ofx_nw_src().get_u32value());
    return addr;
  };
  caddress_in4 get_nw_src_mask() const {
    rofl::caddress_in4 addr;
    addr.set_addr_hbo(matches.get_ofx_nw_src().get_u32mask());
    return addr;
  };
  caddress_in4 get_nw_dst() const {
    rofl::caddress_in4 addr;
    addr.set_addr_hbo(matches.get_ofx_nw_dst().get_u32masked_value());
    return addr;
  };
  caddress_in4 get_nw_dst_value() const {
    rofl::caddress_in4 addr;
    addr.set_addr_hbo(matches.get_ofx_nw_dst().get_u32value());
    return addr;
  };
  caddress_in4 get_nw_dst_mask() const {
    rofl::caddress_in4 addr;
    addr.set_addr_hbo(matches.get_ofx_nw_dst().get_u32mask());
    return addr;
  };
  uint16_t get_tp_src() const {
    return matches.get_ofx_tp_src().get_u16value();
  };
  uint16_t get_tp_dst() const {
    return matches.get_ofx_tp_dst().get_u16value();
  };

  // OF12
  uint32_t get_in_port() const {
    return matches.get_ofb_in_port().get_u32value();
  };
  uint32_t get_in_phy_port() const {
    return matches.get_ofb_in_phy_port().get_u32value();
  };
  uint64_t get_metadata() const {
    return matches.get_ofb_metadata().get_u64masked_value();
  };
  uint64_t get_metadata_value() const {
    return matches.get_ofb_metadata().get_u64value();
  };
  uint64_t get_metadata_mask() const {
    return matches.get_ofb_metadata().get_u64mask();
  };
  cmacaddr get_eth_dst() const {
    return matches.get_ofb_eth_dst().get_u48masked_value_as_lladdr();
  };
  cmacaddr get_eth_dst_addr() const {
    return matches.get_ofb_eth_dst().get_u48value_as_lladdr();
  };
  cmacaddr get_eth_dst_mask() const {
    return matches.get_ofb_eth_dst().get_u48mask_as_lladdr();
  };
  cmacaddr get_eth_src() const {
    return matches.get_ofb_eth_src().get_u48masked_value_as_lladdr();
  };
  cmacaddr get_eth_src_addr() const {
    return matches.get_ofb_eth_src().get_u48value_as_lladdr();
  };
  cmacaddr get_eth_src_mask() const {
    return matches.get_ofb_eth_src().get_u48mask_as_lladdr();
  };
  uint16_t get_eth_type() const {
    return matches.get_ofb_eth_type().get_u16value();
  };
  uint16_t get_vlan_vid() const {
    return matches.get_ofb_vlan_vid().get_u16masked_value();
  };
  uint16_t get_vlan_vid_value() const {
    return matches.get_ofb_vlan_vid().get_u16value();
  };
  uint16_t get_vlan_vid_mask() const {
    return matches.get_ofb_vlan_vid().get_u16mask();
  };
  uint8_t get_vlan_pcp() const {
    return matches.get_ofb_vlan_pcp().get_u8value();
  };
  uint32_t get_mpls_label() const {
    return matches.get_ofb_mpls_label().get_u32value();
  };
  uint8_t get_mpls_tc() const {
    return matches.get_ofb_mpls_tc().get_u8value();
  };
  caddress_in4 get_ipv4_src() const {
    return matches.get_ofb_ipv4_src().get_u32masked_value_as_addr();
  };
  caddress_in4 get_ipv4_src_value() const {
    return matches.get_ofb_ipv4_src().get_u32value_as_addr();
  };
  caddress_in4 get_ipv4_src_mask() const {
    return matches.get_ofb_ipv4_src().get_u32mask_as_addr();
  };
  caddress_in4 get_ipv4_dst() const {
    return matches.get_ofb_ipv4_dst().get_u32masked_value_as_addr();
  };
  caddress_in4 get_ipv4_dst_value() const {
    return matches.get_ofb_ipv4_dst().get_u32value_as_addr();
  };
  caddress_in4 get_ipv4_dst_mask() const {
    return matches.get_ofb_ipv4_dst().get_u32mask_as_addr();
  };
  uint16_t get_arp_opcode() const {
    return matches.get_ofb_arp_opcode().get_u16value();
  };
  cmacaddr get_arp_sha() const {
    return matches.get_ofb_arp_sha().get_u48masked_value_as_lladdr();
  };
  cmacaddr get_arp_sha_addr() const {
    return matches.get_ofb_arp_sha().get_u48value_as_lladdr();
  };
  cmacaddr get_arp_sha_mask() const {
    return matches.get_ofb_arp_sha().get_u48mask_as_lladdr();
  };
  cmacaddr get_arp_tha() const {
    return matches.get_ofb_arp_tha().get_u48masked_value_as_lladdr();
  };
  cmacaddr get_arp_tha_addr() const {
    return matches.get_ofb_arp_tha().get_u48value_as_lladdr();
  };
  cmacaddr get_arp_tha_mask() const {
    return matches.get_ofb_arp_tha().get_u48mask_as_lladdr();
  };
  caddress_in4 get_arp_spa() const {
    return matches.get_ofb_arp_spa().get_u32masked_value_as_addr();
  };
  caddress_in4 get_arp_spa_value() const {
    return matches.get_ofb_arp_spa().get_u32value_as_addr();
  };
  caddress_in4 get_arp_spa_mask() const {
    return matches.get_ofb_arp_spa().get_u32mask_as_addr();
  };
  caddress_in4 get_arp_tpa() const {
    return matches.get_ofb_arp_tpa().get_u32masked_value_as_addr();
  };
  caddress_in4 get_arp_tpa_value() const {
    return matches.get_ofb_arp_tpa().get_u32value_as_addr();
  };
  caddress_in4 get_arp_tpa_mask() const {
    return matches.get_ofb_arp_tpa().get_u32mask_as_addr();
  };
  caddress_in6 get_ipv6_src() const {
    return matches.get_ofb_ipv6_src().get_u128masked_value();
  };
  caddress_in6 get_ipv6_src_value() const {
    return matches.get_ofb_ipv6_src().get_u128value();
  };
  caddress_in6 get_ipv6_src_mask() const {
    return matches.get_ofb_ipv6_src().get_u128mask();
  };
  caddress_in6 get_ipv6_dst() const {
    return matches.get_ofb_ipv6_dst().get_u128masked_value();
  };
  caddress_in6 get_ipv6_dst_value() const {
    return matches.get_ofb_ipv6_dst().get_u128value();
  };
  caddress_in6 get_ipv6_dst_mask() const {
    return matches.get_ofb_ipv6_dst().get_u128mask();
  };
  uint8_t get_ip_proto() const {
    return matches.get_ofb_ip_proto().get_u8value();
  };
  uint8_t get_ip_dscp() const {
    return matches.get_ofb_ip_dscp().get_u8value();
  };
  uint8_t get_ip_ecn() const { return matches.get_ofb_ip_ecn().get_u8value(); };
  uint8_t get_icmpv4_type() const {
    return matches.get_ofb_icmpv4_type().get_u8value();
  };
  uint8_t get_icmpv4_code() const {
    return matches.get_ofb_icmpv4_code().get_u8value();
  };
  uint8_t get_icmpv6_type() const {
    return matches.get_ofb_icmpv6_type().get_u8value();
  };
  uint8_t get_icmpv6_code() const {
    return matches.get_ofb_icmpv6_code().get_u8value();
  };
  uint32_t get_ipv6_flabel() const {
    return matches.get_ofb_ipv6_flabel().get_u32masked_value();
  };
  uint32_t get_ipv6_flabel_value() const {
    return matches.get_ofb_ipv6_flabel().get_u32value();
  };
  uint32_t get_ipv6_flabel_mask() const {
    return matches.get_ofb_ipv6_flabel().get_u32mask();
  };
  cmacaddr get_ipv6_nd_sll() const {
    return matches.get_ofb_ipv6_nd_sll().get_u48masked_value_as_lladdr();
  };
  cmacaddr get_ipv6_nd_tll() const {
    return matches.get_ofb_ipv6_nd_tll().get_u48masked_value_as_lladdr();
  };
  caddress_in6 get_ipv6_nd_target() const {
    return matches.get_ofb_ipv6_nd_target().get_u128masked_value();
  };
  uint16_t get_udp_src() const {
    return matches.get_ofb_udp_src().get_u16value();
  };
  uint16_t get_udp_dst() const {
    return matches.get_ofb_udp_dst().get_u16value();
  };
  uint16_t get_tcp_src() const {
    return matches.get_ofb_tcp_src().get_u16value();
  };
  uint16_t get_tcp_dst() const {
    return matches.get_ofb_tcp_dst().get_u16value();
  };
  uint16_t get_sctp_src() const {
    return matches.get_ofb_sctp_src().get_u16value();
  };
  uint16_t get_sctp_dst() const {
    return matches.get_ofb_sctp_dst().get_u16value();
  };

  // OF13
  bool get_mpls_bos() const {
    return matches.get_ofb_mpls_bos().get_u8value();
  };
  uint64_t get_tunnel_id() const {
    return matches.get_ofb_tunnel_id().get_u64masked_value();
  };
  uint64_t get_tunnel_id_value() const {
    return matches.get_ofb_tunnel_id().get_u64value();
  };
  uint64_t get_tunnel_id_mask() const {
    return matches.get_ofb_tunnel_id().get_u64mask();
  };
  uint32_t get_pbb_isid() const {
    return matches.get_ofb_pbb_isid().get_u24masked_value();
  };
  uint32_t get_pbb_isid_value() const {
    return matches.get_ofb_pbb_isid().get_u24value();
  };
  uint32_t get_pbb_isid_mask() const {
    return matches.get_ofb_pbb_isid().get_u24mask();
  };
  uint16_t get_ipv6_exthdr() const {
    return matches.get_ofb_ipv6_exthdr().get_u16masked_value();
  };
  uint16_t get_ipv6_exthdr_value() const {
    return matches.get_ofb_ipv6_exthdr().get_u16value();
  };
  uint16_t get_ipv6_exthdr_mask() const {
    return matches.get_ofb_ipv6_exthdr().get_u16mask();
  };

  // OF10
  void set_nw_proto(uint8_t proto) { matches.add_ofx_nw_proto(proto); };
  void set_nw_tos(uint8_t tos) { matches.add_ofx_nw_tos(tos); };
  void set_nw_src(const caddress_in4 &src) { matches.add_ofx_nw_src(src); };
  void set_nw_src(const caddress_in4 &src, const caddress_in4 &mask) {
    matches.add_ofx_nw_src(src, mask);
  };
  void set_nw_dst(const caddress_in4 &dst) { matches.add_ofx_nw_dst(dst); };
  void set_nw_dst(const caddress_in4 &dst, const caddress_in4 &mask) {
    matches.add_ofx_nw_dst(dst, mask);
  };
  void set_tp_src(uint16_t src_port) { matches.add_ofx_tp_src(src_port); };
  void set_tp_dst(uint16_t dst_port) { matches.add_ofx_tp_dst(dst_port); };

  // OF12
  void set_in_port(uint32_t in_port) { matches.add_ofb_in_port(in_port); };
  void set_in_phy_port(uint32_t in_phy_port) {
    matches.add_ofb_in_phy_port(in_phy_port);
  };
  void set_metadata(uint64_t metadata) { matches.add_ofb_metadata(metadata); };
  void set_metadata(uint64_t metadata, uint64_t mask) {
    matches.add_ofb_metadata(metadata, mask);
  };
  void set_eth_dst(const cmacaddr &maddr) { matches.add_ofb_eth_dst(maddr); };
  void set_eth_dst(const cmacaddr &maddr, const cmacaddr &mmask) {
    matches.add_ofb_eth_dst(maddr, mmask);
  };
  void set_eth_src(const cmacaddr &maddr) { matches.add_ofb_eth_src(maddr); };
  void set_eth_src(const cmacaddr &maddr, const cmacaddr &mmask) {
    matches.add_ofb_eth_src(maddr, mmask);
  };
  void set_eth_type(uint16_t dl_type) { matches.add_ofb_eth_type(dl_type); };
  void set_vlan_vid(uint16_t vid) { matches.add_ofb_vlan_vid(vid); };
  void set_vlan_vid(uint16_t vid, uint16_t mask) {
    matches.add_ofb_vlan_vid(vid, mask);
  };
  void set_vlan_present() {
    matches.add_ofb_vlan_vid(rofl::openflow::OFPVID_PRESENT,
                             rofl::openflow::OFPVID_PRESENT);
  };
  void set_vlan_untagged() {
    matches.add_ofb_vlan_vid(rofl::openflow::OFPVID_NONE);
  };
  void set_vlan_pcp(uint8_t pcp) { matches.add_ofb_vlan_pcp(pcp); };
  void set_ip_dscp(uint8_t dscp) { matches.add_ofb_ip_dscp(dscp); };
  void set_ip_ecn(uint8_t ecn) { matches.add_ofb_ip_ecn(ecn); };
  void set_ip_proto(uint8_t proto) { matches.add_ofb_ip_proto(proto); };
  void set_ipv4_src(const caddress_in4 &src) { matches.add_ofb_ipv4_src(src); };
  void set_ipv4_src(const caddress_in4 &src, const caddress_in4 &mask) {
    matches.add_ofb_ipv4_src(src, mask);
  };
  void set_ipv4_dst(const caddress_in4 &dst) { matches.add_ofb_ipv4_dst(dst); };
  void set_ipv4_dst(const caddress_in4 &dst, const caddress_in4 &mask) {
    matches.add_ofb_ipv4_dst(dst, mask);
  };
  void set_tcp_src(uint16_t src_port) { matches.add_ofb_tcp_src(src_port); };
  void set_tcp_dst(uint16_t dst_port) { matches.add_ofb_tcp_dst(dst_port); };
  void set_udp_src(uint16_t src_port) { matches.add_ofb_udp_src(src_port); };
  void set_udp_dst(uint16_t dst_port) { matches.add_ofb_udp_dst(dst_port); };
  void set_sctp_src(uint16_t src_port) { matches.add_ofb_sctp_src(src_port); };
  void set_sctp_dst(uint16_t dst_port) { matches.add_ofb_sctp_dst(dst_port); };
  void set_icmpv4_type(uint8_t type) { matches.add_ofb_icmpv4_type(type); };
  void set_icmpv4_code(uint8_t code) { matches.add_ofb_icmpv4_code(code); };
  void set_arp_opcode(uint16_t opcode) { matches.add_ofb_arp_opcode(opcode); };
  void set_arp_spa(const caddress_in4 &spa) {
    matches.add_ofb_arp_spa(spa.get_addr_hbo());
  };
  void set_arp_spa(const caddress_in4 &spa, const caddress_in4 &mask) {
    matches.add_ofb_arp_spa(spa.get_addr_hbo(), mask.get_addr_hbo());
  };
  void set_arp_tpa(const caddress_in4 &tpa) {
    matches.add_ofb_arp_tpa(tpa.get_addr_hbo());
  };
  void set_arp_tpa(const caddress_in4 &tpa, const caddress_in4 &mask) {
    matches.add_ofb_arp_tpa(tpa.get_addr_hbo(), mask.get_addr_hbo());
  };
  void set_arp_sha(const cmacaddr &sha) { matches.add_ofb_arp_sha(sha); };
  void set_arp_sha(const cmacaddr &sha, const cmacaddr &mmask) {
    matches.add_ofb_arp_sha(sha, mmask);
  };
  void set_arp_tha(const cmacaddr &tha) { matches.add_ofb_arp_tha(tha); };
  void set_arp_tha(const cmacaddr &tha, const cmacaddr &mmask) {
    matches.add_ofb_arp_tha(tha, mmask);
  };
  void set_ipv6_src(const caddress_in6 &addr) {
    matches.add_ofb_ipv6_src(addr);
  };
  void set_ipv6_src(const caddress_in6 &addr, const caddress_in6 &mask) {
    matches.add_ofb_ipv6_src(addr, mask);
  };
  void set_ipv6_dst(const caddress_in6 &addr) {
    matches.add_ofb_ipv6_dst(addr);
  };
  void set_ipv6_dst(const caddress_in6 &addr, const caddress_in6 &mask) {
    matches.add_ofb_ipv6_dst(addr, mask);
  };
  void set_ipv6_flabel(uint32_t flabel) {
    matches.add_ofb_ipv6_flabel(flabel);
  };
  void set_ipv6_flabel(uint32_t flabel, uint32_t mask) {
    matches.add_ofb_ipv6_flabel(flabel, mask);
  };
  void set_icmpv6_type(uint8_t type) { matches.add_ofb_icmpv6_type(type); };
  void set_icmpv6_code(uint8_t code) { matches.add_ofb_icmpv6_code(code); };
  void set_ipv6_nd_target(const caddress_in6 &addr) {
    matches.add_ofb_ipv6_nd_target(addr);
  };
  void set_ipv6_nd_sll(const cmacaddr &maddr) {
    matches.add_ofb_ipv6_nd_sll(maddr);
  };
  void set_ipv6_nd_tll(const cmacaddr &maddr) {
    matches.add_ofb_ipv6_nd_tll(maddr);
  };
  void set_mpls_label(uint32_t label) { matches.add_ofb_mpls_label(label); };
  void set_mpls_tc(uint8_t tc) { matches.add_ofb_mpls_tc(tc); };

  // OF13
  void set_mpls_bos(bool bos) { matches.add_ofb_mpls_bos(bos); };
  void set_tunnel_id(uint64_t tunnel_id) {
    matches.add_ofb_tunnel_id(tunnel_id);
  };
  void set_tunnel_id(uint64_t tunnel_id, uint64_t mask) {
    matches.add_ofb_tunnel_id(tunnel_id, mask);
  };
  void set_pbb_isid(uint32_t pbb_isid) { matches.add_ofb_pbb_isid(pbb_isid); };
  void set_pbb_isid(uint32_t pbb_isid, uint32_t mask) {
    matches.add_ofb_pbb_isid(pbb_isid, mask);
  };
  void set_ipv6_exthdr(uint16_t ipv6_exthdr) {
    matches.add_ofb_ipv6_exthdr(ipv6_exthdr);
  };
  void set_ipv6_exthdr(uint16_t ipv6_exthdr, uint16_t mask) {
    matches.add_ofb_ipv6_exthdr(ipv6_exthdr, mask);
  };

public:
  /*
   * old API: to be or not to be ... deprecated (???)
   */

  // OF10
  bool has_nw_proto() const { return matches.has_ofx_nw_proto(); };
  bool has_nw_tos() const { return matches.has_ofx_nw_tos(); };
  bool has_nw_src() const { return matches.has_ofx_nw_src(); };
  bool has_nw_src_value() const { return matches.has_ofx_nw_src(); };
  bool has_nw_src_mask() const {
    return (matches.has_ofx_nw_src() &&
            matches.get_ofx_nw_src().get_oxm_hasmask());
  };
  bool has_nw_dst() const { return matches.has_ofx_nw_dst(); };
  bool has_nw_dst_value() const { return matches.has_ofx_nw_dst(); };
  bool has_nw_dst_mask() const {
    return (matches.has_ofx_nw_dst() &&
            matches.get_ofx_nw_dst().get_oxm_hasmask());
  };
  bool has_tp_src() const { return matches.has_ofx_tp_src(); };
  bool has_tp_dst() const { return matches.has_ofx_tp_dst(); };

  // OF12
  bool has_in_port() const { return matches.has_ofb_in_port(); };
  bool has_in_phy_port() const { return matches.has_ofb_in_phy_port(); };
  bool has_metadata() const { return matches.has_ofb_metadata(); };
  bool has_metadata_value() const { return matches.has_ofb_metadata(); };
  bool has_metadata_mask() const {
    return (matches.has_ofb_metadata() &&
            matches.get_ofb_metadata().get_oxm_hasmask());
  };
  bool has_eth_dst() const { return matches.has_ofb_eth_dst(); };
  bool has_eth_dst_addr() const { return matches.has_ofb_eth_dst(); };
  bool has_eth_dst_mask() const {
    return (matches.has_ofb_eth_dst() &&
            matches.get_ofb_eth_dst().get_oxm_hasmask());
  };
  bool has_eth_src() const { return matches.has_ofb_eth_src(); };
  bool has_eth_src_addr() const { return matches.has_ofb_eth_src(); };
  bool has_eth_src_mask() const {
    return (matches.has_ofb_eth_src() &&
            matches.get_ofb_eth_src().get_oxm_hasmask());
  };
  bool has_eth_type() const { return matches.has_ofb_eth_type(); };
  bool has_vlan_vid() const { return matches.has_ofb_vlan_vid(); };
  bool has_vlan_vid_value() const { return matches.has_ofb_vlan_vid(); };
  bool has_vlan_vid_mask() const {
    return (matches.has_ofb_vlan_vid() &&
            matches.get_ofb_vlan_vid().get_oxm_hasmask());
  };
  bool has_vlan_pcp() const { return matches.has_ofb_vlan_pcp(); };
  bool has_mpls_label() const { return matches.has_ofb_mpls_label(); };
  bool has_mpls_tc() const { return matches.has_ofb_mpls_tc(); };
  bool has_ipv4_src() const { return matches.has_ofb_ipv4_src(); };
  bool has_ipv4_src_value() const { return matches.has_ofb_ipv4_src(); };
  bool has_ipv4_src_mask() const {
    return (matches.has_ofb_ipv4_src() &&
            matches.get_ofb_ipv4_src().get_oxm_hasmask());
  };
  bool has_ipv4_dst() const { return matches.has_ofb_ipv4_dst(); };
  bool has_ipv4_dst_value() const { return matches.has_ofb_ipv4_dst(); };
  bool has_ipv4_dst_mask() const {
    return (matches.has_ofb_ipv4_dst() &&
            matches.get_ofb_ipv4_dst().get_oxm_hasmask());
  };
  bool has_arp_opcode() const { return matches.has_ofb_arp_opcode(); };
  bool has_arp_sha() const { return matches.has_ofb_arp_sha(); };
  bool has_arp_sha_addr() const { return matches.has_ofb_arp_sha(); };
  bool has_arp_sha_mask() const {
    return (matches.has_ofb_arp_sha() &&
            matches.get_ofb_arp_sha().get_oxm_hasmask());
  };
  bool has_arp_tha() const { return matches.has_ofb_arp_tha(); };
  bool has_arp_tha_addr() const { return matches.has_ofb_arp_tha(); };
  bool has_arp_tha_mask() const {
    return (matches.has_ofb_arp_tha() &&
            matches.get_ofb_arp_tha().get_oxm_hasmask());
  };
  bool has_arp_spa() const { return matches.has_ofb_arp_spa(); };
  bool has_arp_spa_value() const { return matches.has_ofb_arp_spa(); };
  bool has_arp_spa_mask() const {
    return (matches.has_ofb_arp_spa() &&
            matches.get_ofb_arp_spa().get_oxm_hasmask());
  };
  bool has_arp_tpa() const { return matches.has_ofb_arp_tpa(); };
  bool has_arp_tpa_value() const { return matches.has_ofb_arp_tpa(); };
  bool has_arp_tpa_mask() const {
    return (matches.has_ofb_arp_tpa() &&
            matches.get_ofb_arp_tpa().get_oxm_hasmask());
  };
  bool has_ipv6_src() const { return matches.has_ofb_ipv6_src(); };
  bool has_ipv6_src_value() const { return matches.has_ofb_ipv6_src(); };
  bool has_ipv6_src_mask() const {
    return (matches.has_ofb_ipv6_src() &&
            matches.get_ofb_ipv6_src().get_oxm_hasmask());
  };
  bool has_ipv6_dst() const { return matches.has_ofb_ipv6_dst(); };
  bool has_ipv6_dst_value() const { return matches.has_ofb_ipv6_dst(); };
  bool has_ipv6_dst_mask() const {
    return (matches.has_ofb_ipv6_dst() &&
            matches.get_ofb_ipv6_dst().get_oxm_hasmask());
  };
  bool has_ip_proto() const { return matches.has_ofb_ip_proto(); };
  bool has_ip_dscp() const { return matches.has_ofb_ip_dscp(); };
  bool has_ip_ecn() const { return matches.has_ofb_ip_ecn(); };
  bool has_icmpv4_type() const { return matches.has_ofb_icmpv4_type(); };
  bool has_icmpv4_code() const { return matches.has_ofb_icmpv4_code(); };
  bool has_icmpv6_type() const { return matches.has_ofb_icmpv6_type(); };
  bool has_icmpv6_code() const { return matches.has_ofb_icmpv6_code(); };
  bool has_ipv6_flabel() const { return matches.has_ofb_ipv6_flabel(); };
  bool has_ipv6_flabel_value() const { return matches.has_ofb_ipv6_flabel(); };
  bool has_ipv6_flabel_mask() const {
    return (matches.has_ofb_ipv6_flabel() &&
            matches.get_ofb_ipv6_flabel().get_oxm_hasmask());
  };
  bool has_ipv6_nd_sll() const { return matches.has_ofb_ipv6_nd_sll(); };
  bool has_ipv6_nd_tll() const { return matches.has_ofb_ipv6_nd_tll(); };
  bool has_ipv6_nd_target() const { return matches.has_ofb_ipv6_nd_target(); };
  bool has_udp_src() const { return matches.has_ofb_udp_src(); };
  bool has_udp_dst() const { return matches.has_ofb_udp_dst(); };
  bool has_tcp_src() const { return matches.has_ofb_tcp_src(); };
  bool has_tcp_dst() const { return matches.has_ofb_tcp_dst(); };
  bool has_sctp_src() const { return matches.has_ofb_sctp_src(); };
  bool has_sctp_dst() const { return matches.has_ofb_sctp_dst(); };

  // OF13
  bool has_mpls_bos() const { return matches.has_ofb_mpls_bos(); };
  bool has_tunnel_id() const { return matches.has_ofb_tunnel_id(); };
  bool has_tunnel_id_value() const { return matches.has_ofb_tunnel_id(); };
  bool has_tunnel_id_mask() const {
    return (matches.has_ofb_tunnel_id() &&
            matches.get_ofb_tunnel_id().get_oxm_hasmask());
  };
  bool has_pbb_isid() const { return matches.has_ofb_pbb_isid(); };
  bool has_pbb_isid_value() const { return matches.has_ofb_pbb_isid(); };
  bool has_pbb_isid_mask() const {
    return (matches.has_ofb_pbb_isid() &&
            matches.get_ofb_pbb_isid().get_oxm_hasmask());
  };
  bool has_ipv6_exthdr() const { return matches.has_ofb_ipv6_exthdr(); };
  bool has_ipv6_exthdr_value() const { return matches.has_ofb_ipv6_exthdr(); };
  bool has_ipv6_exthdr_mask() const {
    return (matches.has_ofb_ipv6_exthdr() &&
            matches.get_ofb_ipv6_exthdr().get_oxm_hasmask());
  };
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
