/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_port_mod.h"

using namespace rofl::openflow;

cofmsg_port_mod::~cofmsg_port_mod()
{}



cofmsg_port_mod::cofmsg_port_mod(
		uint8_t version,
		uint32_t xid,
		uint32_t portno,
		const rofl::caddress_ll& lladdr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise) :
				cofmsg(version, rofl::openflow::OFPT_PORT_MOD/*=16*/, xid),
				portno(portno),
				lladdr(lladdr),
				config(config),
				mask(mask),
				advertise(advertise)
{
	switch (version) {
	case openflow10::OFP_VERSION: {
		set_type(rofl::openflow10::OFPT_PORT_MOD/*=15*/);
	} break;
	default: {
		/* do nothing */
	};
	}
}



cofmsg_port_mod::cofmsg_port_mod(
		const cofmsg_port_mod& msg)
{
	*this = msg;
}



cofmsg_port_mod&
cofmsg_port_mod::operator= (
		const cofmsg_port_mod& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	portno = msg.portno;
	lladdr = msg.lladdr;
	config = msg.config;
	mask   = msg.mask;
	advertise = msg.advertise;
	return *this;
}



size_t
cofmsg_port_mod::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return sizeof(struct rofl::openflow10::ofp_port_mod);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return sizeof(struct rofl::openflow12::ofp_port_mod);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_port_mod);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_mod::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_mod::length())
		throw eMsgInval("cofmsg_port_mod::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_port_mod* hdr =
				(struct rofl::openflow10::ofp_port_mod*)buf;
		hdr->port_no = htobe16((uint16_t)(portno & 0x0000ffff));
		lladdr.pack(hdr->hw_addr, OFP_ETH_ALEN);
		hdr->config = htobe32(config);
		hdr->mask = htobe32(mask);
		hdr->advertise = htobe32(advertise);
	} break;
	default: {
		struct rofl::openflow13::ofp_port_mod* hdr =
				(struct rofl::openflow13::ofp_port_mod*)buf;
		hdr->port_no = htobe32(portno);
		lladdr.pack(hdr->hw_addr, OFP_ETH_ALEN);
		hdr->config = htobe32(config);
		hdr->mask = htobe32(mask);
		hdr->advertise = htobe32(advertise);
	};
	}
}



void
cofmsg_port_mod::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_mod::length())
		throw eBadSyntaxTooShort("cofmsg_port_mod::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_port_mod* hdr =
				(struct rofl::openflow10::ofp_port_mod*)buf;
		portno = be16toh(hdr->port_no);
		lladdr.unpack(hdr->hw_addr, OFP_ETH_ALEN);
		config = be32toh(hdr->config);
		mask = be32toh(hdr->mask);
		advertise = be32toh(hdr->advertise);
	} break;
	default: {
		struct rofl::openflow13::ofp_port_mod* hdr =
				(struct rofl::openflow13::ofp_port_mod*)buf;
		portno = be32toh(hdr->port_no);
		lladdr.unpack(hdr->hw_addr, OFP_ETH_ALEN);
		config = be32toh(hdr->config);
		mask = be32toh(hdr->mask);
		advertise = be32toh(hdr->advertise);
	};
	}

	if (get_length() < cofmsg_port_mod::length())
		throw eBadSyntaxTooShort("cofmsg_port_mod::unpack() buf too short");
}



