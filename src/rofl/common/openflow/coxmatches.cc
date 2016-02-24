/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#include <stdexcept>

#include "rofl/common/openflow/coxmatches.h"

using namespace rofl::openflow;

coxmatches::coxmatches()
{

}


coxmatches::~coxmatches()
{
	clear();
}


coxmatches::coxmatches(
		coxmatches const& oxmlist)
{
	*this = oxmlist;
}


coxmatches&
coxmatches::operator= (
		coxmatches const& oxms)
{
	if (this == &oxms)
		return *this;
	clear();
	copy_matches(oxms);
	return *this;
}




bool
coxmatches::operator== (coxmatches const& oxms) const
{
	if (matches.size() != oxms.matches.size()) {
		return false;
	}
	for (std::map<uint64_t, coxmatch*>::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		if (oxms.matches.find(it->first) == oxms.matches.end()) {
			return false;
		}
		if (*it->second != *oxms.matches.at(it->first)) {
			return false;
		}
	}
	return true;
}





void
coxmatches::unpack(
		uint8_t* buf,
		size_t buflen)
{
	clear();

	while (true) {

		if (buflen < sizeof(struct openflow::ofp_oxm_hdr)) {
			return; // not enough bytes to parse an entire ofp_oxm_hdr, possibly padding bytes found
		}

		struct openflow::ofp_oxm_hdr *hdr = (struct openflow::ofp_oxm_hdr*)buf;

		switch (be16toh(hdr->oxm_class)) {
		case rofl::openflow::OFPXMC_OPENFLOW_BASIC: {

			if (buflen < sizeof(struct openflow::ofp_oxm_hdr)) {
				throw eOxmBadLen("coxmatches::unpack() buflen too short");
			}

			struct rofl::openflow::ofp_oxm_tlv_hdr* oxm = (struct rofl::openflow::ofp_oxm_tlv_hdr*)buf;

			uint64_t oxm_type = OXM_ROFL_TYPE(be32toh(oxm->oxm_id));

			switch (oxm_type) {
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT): {
				add_ofb_in_port().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT): {
				add_ofb_in_phy_port().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA): {
				add_ofb_metadata().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST): {
				add_ofb_eth_dst().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC): {
				add_ofb_eth_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE): {
				add_ofb_eth_type().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID): {
				add_ofb_vlan_vid().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP): {
				add_ofb_vlan_pcp().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP): {
				add_ofb_ip_dscp().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN): {
				add_ofb_ip_ecn().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO): {
				add_ofb_ip_proto().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC): {
				add_ofb_ipv4_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST): {
				add_ofb_ipv4_dst().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC): {
				add_ofb_tcp_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST): {
				add_ofb_tcp_dst().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC): {
				add_ofb_udp_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST): {
				add_ofb_udp_dst().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC): {
				add_ofb_sctp_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST): {
				add_ofb_sctp_dst().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE): {
				add_ofb_icmpv4_type().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE): {
				add_ofb_icmpv4_code().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP): {
				add_ofb_arp_opcode().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA): {
				add_ofb_arp_spa().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA): {
				add_ofb_arp_tpa().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA): {
				add_ofb_arp_sha().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA): {
				add_ofb_arp_tha().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC): {
				add_ofb_ipv6_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST): {
				add_ofb_ipv6_dst().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL): {
				add_ofb_ipv6_flabel().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE): {
				add_ofb_icmpv6_type().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE): {
				add_ofb_icmpv6_code().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET): {
				add_ofb_ipv6_nd_target().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL): {
				add_ofb_ipv6_nd_sll().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL): {
				add_ofb_ipv6_nd_tll().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL): {
				add_ofb_mpls_label().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC): {
				add_ofb_mpls_tc().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS): {
				add_ofb_mpls_bos().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID): {
				add_ofb_pbb_isid().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID): {
				add_ofb_mpls_bos().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR): {
				add_ofb_ipv6_exthdr().unpack(buf, buflen);
			} break;
			default: {

			};
			}


		} break;
		case rofl::openflow::OFPXMC_EXPERIMENTER: {

			if (buflen < sizeof(struct openflow::ofp_oxm_experimenter_header)) {
				throw eOxmBadLen("coxmatches::unpack() buflen too short");
			}

			struct rofl::openflow::ofp_oxm_experimenter_header* oxm =
					(struct rofl::openflow::ofp_oxm_experimenter_header*)buf;

			uint64_t oxm_type = ((uint64_t)be32toh(oxm->experimenter)) << 32;

			struct rofl::openflow::ofp_oxm_tlv_hdr* hdr =
					(struct rofl::openflow::ofp_oxm_tlv_hdr*)buf;
			oxm_type |= OXM_ROFL_TYPE(be32toh(hdr->oxm_id));

			uint32_t exp_id = be32toh(oxm->experimenter);
			uint32_t oxm_id = be32toh(hdr->oxm_id);

			switch (oxm_type) {
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC): {
				add_ofx_nw_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST): {
				add_ofx_nw_dst().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO): {
				add_ofx_nw_proto().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS): {
				add_ofx_nw_tos().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC): {
				add_ofx_tp_src().unpack(buf, buflen);
			} break;
			case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST): {
				add_ofx_tp_dst().unpack(buf, buflen);
			} break;
			default: {
				add_exp_match(exp_id, oxm_id).unpack(buf, buflen);
			};
			}

		} break;
		}

		if (buflen >= (sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length)) {
			buflen -= ((sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length));
			buf += ((sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length));
		} else {
			return;
		}
	}
}



void
coxmatches::pack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < length()) {
		throw eBadMatchBadLen("eBadMatchBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
	for (std::map<uint64_t, coxmatch*>::iterator
			jt = matches.begin(); jt != matches.end(); ++jt) {

		coxmatch *match = (matches[jt->first]);

		match->pack(buf, match->length());

		buf += match->length();
	}
}



size_t
coxmatches::length() const
{
	size_t len = 0;
	for (std::map<uint64_t, coxmatch*>::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		len += it->second->length();
	}
	return len;
}




bool
coxmatches::contains(
	coxmatches const& oxms,
	bool strict /* = false (default) */)
{
	/*
	 * strict:
	 * all elemens in *this and oxl must be identical (number of and value of elements)
	 *
	 * non-strict:
	 * all elements in oxl must be present in *this (value of elements), but *this may contain additional OXM TLVs
	 */

	if (strict) {
		// strict: # of elems for an ofm_class must be the same in oxl for the specific ofm_class
		if (matches.size() != oxms.matches.size()) {
			return false;
		}
	} else  /* non-strict*/ {
		// non-strict: # of elems for a class must be larger (or equal) the number of elements in oxl
		if (matches.size() > oxms.matches.size()) {
			return false;
		}
	}

	// strict: check all TLVs for specific class in oxl.matches => must exist and have same value
	for (std::map<uint64_t, coxmatch*>::iterator
			jt = matches.begin(); jt != matches.end(); ++jt) {

		coxmatch *lmatch = (jt->second);

		// keep in mind: match.get_oxm_id() & 0xfffffe00 == jt->first

		// strict: all OXM TLVs must also exist in oxl
		uint64_t id = (__UINT64_C(0xfffffe00) & lmatch->get_oxm_id()) << 32 |
				0/*(lmatch->is_experimenter() ? lmatch->get_oxm_exp_id() : 0)*/;
		if (oxms.matches.find(id) == oxms.matches.end()) {
			return false;
		}

		coxmatch const *rmatch = (oxms.matches.find(jt->first)->second);

		// strict: both OXM TLVs must have identical values
		if (*lmatch != *rmatch) {
			return false;
		}
	}

	return true;
}




bool
coxmatches::is_part_of(
		coxmatches const& oxms,
		uint16_t& exact_hits,
		uint16_t& wildcard_hits,
		uint16_t& missed)
{
	bool result = true;

	for (std::map<uint64_t, coxmatch*>::const_iterator
			jt = oxms.matches.begin(); jt != oxms.matches.end(); ++jt) {

		coxmatch const *rmatch = (jt->second);

		// keep in mind: match.get_oxm_id() & 0xfffffe00 == jt->first

		uint64_t id = (__UINT64_C(0xfffffe00) & rmatch->get_oxm_id()) << 32 |
				0/*(rmatch->is_experimenter() ? rmatch->get_oxm_exp_id() : 0)*/;
		if (matches.find(id) == matches.end()) {
			wildcard_hits++; continue;
		}

		if (*matches[jt->first] != *rmatch) {
			missed++; result = false; continue;
		}

		exact_hits++;
	}

	return result;
}



void
coxmatches::copy_matches(
			const coxmatches& oxmatches)
{
	clear();
	std::vector<uint64_t> ids = oxmatches.get_ids();
	for (std::vector<uint64_t>::const_iterator
			it = ids.begin(); it != ids.end(); ++it) {
		switch (*it) {
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PORT): {
			add_ofb_in_port() = oxmatches.get_ofb_in_port();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT): {
			add_ofb_in_phy_port() = oxmatches.get_ofb_in_phy_port();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_METADATA): {
			add_ofb_metadata() = oxmatches.get_ofb_metadata();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_DST): {
			add_ofb_eth_dst() = oxmatches.get_ofb_eth_dst();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_SRC): {
			add_ofb_eth_src() = oxmatches.get_ofb_eth_src();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE): {
			add_ofb_eth_type() = oxmatches.get_ofb_eth_type();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_VID): {
			add_ofb_vlan_vid() = oxmatches.get_ofb_vlan_vid();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP): {
			add_ofb_vlan_pcp() = oxmatches.get_ofb_vlan_pcp();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_DSCP): {
			add_ofb_ip_dscp() = oxmatches.get_ofb_ip_dscp();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_ECN): {
			add_ofb_ip_ecn() = oxmatches.get_ofb_ip_ecn();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IP_PROTO): {
			add_ofb_ip_proto() = oxmatches.get_ofb_ip_proto();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC): {
			add_ofb_ipv4_src() = oxmatches.get_ofb_ipv4_src();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV4_DST): {
			add_ofb_ipv4_dst() = oxmatches.get_ofb_ipv4_dst();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_SRC): {
			add_ofb_tcp_src() = oxmatches.get_ofb_tcp_src();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TCP_DST): {
			add_ofb_tcp_dst() = oxmatches.get_ofb_tcp_dst();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_SRC): {
			add_ofb_udp_src() = oxmatches.get_ofb_udp_src();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_UDP_DST): {
			add_ofb_udp_dst() = oxmatches.get_ofb_udp_dst();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC): {
			add_ofb_sctp_src() = oxmatches.get_ofb_sctp_src();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_SCTP_DST): {
			add_ofb_sctp_dst() = oxmatches.get_ofb_sctp_dst();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE): {
			add_ofb_icmpv4_type() = oxmatches.get_ofb_icmpv4_type();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE): {
			add_ofb_icmpv4_code() = oxmatches.get_ofb_icmpv4_code();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_OP): {
			add_ofb_arp_opcode() = oxmatches.get_ofb_arp_opcode();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SPA): {
			add_ofb_arp_spa() = oxmatches.get_ofb_arp_spa();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_TPA): {
			add_ofb_arp_tpa() = oxmatches.get_ofb_arp_tpa();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_SHA): {
			add_ofb_arp_sha() = oxmatches.get_ofb_arp_sha();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ARP_THA): {
			add_ofb_arp_tha() = oxmatches.get_ofb_arp_tha();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC): {
			add_ofb_ipv6_src() = oxmatches.get_ofb_ipv6_src();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_DST): {
			add_ofb_ipv6_dst() = oxmatches.get_ofb_ipv6_dst();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL): {
			add_ofb_ipv6_flabel() = oxmatches.get_ofb_ipv6_flabel();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE): {
			add_ofb_icmpv6_type() = oxmatches.get_ofb_icmpv6_type();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE): {
			add_ofb_icmpv6_code() = oxmatches.get_ofb_icmpv6_code();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET): {
			add_ofb_ipv6_nd_target() = oxmatches.get_ofb_ipv6_nd_target();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL): {
			add_ofb_ipv6_nd_sll() = oxmatches.get_ofb_ipv6_nd_sll();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL): {
			add_ofb_ipv6_nd_tll() = oxmatches.get_ofb_ipv6_nd_tll();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL): {
			add_ofb_mpls_label() = oxmatches.get_ofb_mpls_label();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_TC): {
			add_ofb_mpls_tc() = oxmatches.get_ofb_mpls_tc();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS): {
			add_ofb_mpls_bos() = oxmatches.get_ofb_mpls_bos();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_PBB_ISID): {
			add_ofb_pbb_isid() = oxmatches.get_ofb_pbb_isid();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID): {
			add_ofb_tunnel_id() = oxmatches.get_ofb_tunnel_id();
		} break;
		case OXM_ROFL_OFB_TYPE(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR): {
			add_ofb_ipv6_exthdr() = oxmatches.get_ofb_ipv6_exthdr();
		} break;
		case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC): {
			add_ofx_nw_src() = oxmatches.get_ofx_nw_src();
		} break;
		case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST): {
			add_ofx_nw_dst() = oxmatches.get_ofx_nw_dst();
		} break;
		case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO): {
			add_ofx_nw_proto() = oxmatches.get_ofx_nw_proto();
		} break;
		case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS): {
			add_ofx_nw_tos() = oxmatches.get_ofx_nw_tos();
		} break;
		case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC): {
			add_ofx_tp_src() = oxmatches.get_ofx_tp_src();
		} break;
		case OXM_ROFL_OFX_TYPE(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST): {
			add_ofx_tp_dst() = oxmatches.get_ofx_tp_dst();
		} break;
		default: {
			if (OXM_ROFL_CLASS(*it) == ((uint64_t)rofl::openflow::OFPXMC_EXPERIMENTER << 16)) {
				uint32_t exp_id = (*it & 0xffffffff00000000) >> 32;
				uint32_t oxm_id = (*it & 0x00000000fffffe00) >>  0);
				add_exp_match(exp_id, oxm_id) = oxmatches.get_exp_match(exp_id, oxm_id);
			}
		};
		}
	}
}

