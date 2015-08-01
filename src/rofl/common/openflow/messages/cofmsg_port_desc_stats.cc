/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_port_desc_stats.h"

using namespace rofl::openflow;

cofmsg_port_desc_stats_request::~cofmsg_port_desc_stats_request()
{}



cofmsg_port_desc_stats_request::cofmsg_port_desc_stats_request(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags) :
				cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_PORT_DESC, stats_flags)
{}



cofmsg_port_desc_stats_request::cofmsg_port_desc_stats_request(
		const cofmsg_port_desc_stats_request& msg)
{
	*this = msg;
}



cofmsg_port_desc_stats_request&
cofmsg_port_desc_stats_request::operator= (
		const cofmsg_port_desc_stats_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_request::operator= (msg);
	return *this;
}



size_t
cofmsg_port_desc_stats_request::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_desc_stats_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_desc_stats_request::length())
		throw eMsgInval("cofmsg_port_desc_stats_request::pack() buf too short");
}



void
cofmsg_port_desc_stats_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_desc_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_port_desc_stats_request::unpack() buf too short");

	if (get_length() < cofmsg_port_desc_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_port_desc_stats_request::unpack() buf too short");
}
















cofmsg_port_desc_stats_reply::~cofmsg_port_desc_stats_reply()
{}



cofmsg_port_desc_stats_reply::cofmsg_port_desc_stats_reply(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofports& ports) :
				cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_PORT_DESC, stats_flags),
				ports(ports)
{
	this->ports.set_version(version);
}



cofmsg_port_desc_stats_reply::cofmsg_port_desc_stats_reply(
		const cofmsg_port_desc_stats_reply& msg)
{
	*this = msg;
}



cofmsg_port_desc_stats_reply&
cofmsg_port_desc_stats_reply::operator= (
		const cofmsg_port_desc_stats_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_reply::operator= (msg);
	ports = msg.ports;
	return *this;
}



size_t
cofmsg_port_desc_stats_reply::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_multipart_reply) + ports.length();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_desc_stats_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_desc_stats_reply::length())
		throw eMsgInval("cofmsg_port_desc_stats_reply::pack() buf too short");

	switch (get_version()) {
	default: {
		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		ports.pack(hdr->body, ports.length());
	};
	}
}



void
cofmsg_port_desc_stats_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::unpack(buf, buflen);

	ports.clear();
	ports.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_desc_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_port_desc_stats_reply::unpack() buf too short");

	switch (get_version()) {
	default: {
		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			ports.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
		}
	};
	}

	if (get_length() < cofmsg_port_desc_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_port_desc_stats_reply::unpack() buf too short");
}





