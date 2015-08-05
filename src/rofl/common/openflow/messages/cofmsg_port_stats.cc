/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_port_stats.h"

using namespace rofl::openflow;

cofmsg_port_stats_request::~cofmsg_port_stats_request()
{}



cofmsg_port_stats_request::cofmsg_port_stats_request(
		uint8_t version,
		uint32_t xid,
		uint16_t flags,
		const rofl::openflow::cofport_stats_request& port_stats) :
				cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_PORT_STATS, flags),
				port_stats(port_stats)
{
	this->port_stats.set_version(version);
}



cofmsg_port_stats_request::cofmsg_port_stats_request(
		const cofmsg_port_stats_request& msg)
{
	*this = msg;
}



cofmsg_port_stats_request&
cofmsg_port_stats_request::operator= (
		const cofmsg_port_stats_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_request::operator= (msg);
	port_stats = msg.port_stats;
	return *this;
}



size_t
cofmsg_port_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request) +
				sizeof(struct rofl::openflow10::ofp_port_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_request) +
				sizeof(struct rofl::openflow12::ofp_port_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) +
				sizeof(struct rofl::openflow13::ofp_port_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_stats_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_stats_request::length())
		throw eMsgInval("cofmsg_meter_config_stats_request::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		port_stats.pack(hdr->body, port_stats.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_multipart_request* hdr =
				(struct rofl::openflow13::ofp_multipart_request*)buf;
		port_stats.pack(hdr->body, port_stats.length());
	};
	}
}



void
cofmsg_port_stats_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::unpack(buf, buflen);

	port_stats.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_port_stats_request::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_stats_type() != rofl::openflow10::OFPST_PORT)
			throw eMsgInval("cofmsg_port_stats_request::unpack() invalid statistics type");

		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		if (buflen > sizeof(struct rofl::openflow10::ofp_stats_request)) {
			port_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_request));
		}
	} break;
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_PORT_STATS)
			throw eMsgInval("cofmsg_port_stats_request::unpack() invalid statistics type");

		struct rofl::openflow13::ofp_multipart_request* hdr =
				(struct rofl::openflow13::ofp_multipart_request*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			port_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_request));
		}
	};
	}

	if (get_length() < cofmsg_port_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_port_stats_request::unpack() buf too short");
}










cofmsg_port_stats_reply::~cofmsg_port_stats_reply()
{}



cofmsg_port_stats_reply::cofmsg_port_stats_reply(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofportstatsarray& portstatsarray) :
				cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_PORT_STATS, stats_flags),
				portstatsarray(portstatsarray)
{
	this->portstatsarray.set_version(version);
}



cofmsg_port_stats_reply::cofmsg_port_stats_reply(
		const cofmsg_port_stats_reply& msg)
{
	*this = msg;
}



cofmsg_port_stats_reply&
cofmsg_port_stats_reply::operator= (
		const cofmsg_port_stats_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_reply::operator= (msg);
	portstatsarray = msg.portstatsarray;
	return *this;
}



size_t
cofmsg_port_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) + portstatsarray.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + portstatsarray.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + portstatsarray.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_port_stats_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_stats_reply::length())
		throw eMsgInval("cofmsg_port_stats_reply::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		portstatsarray.pack(hdr->body, portstatsarray.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		portstatsarray.pack(hdr->body, portstatsarray.length());
	};
	}
}



void
cofmsg_port_stats_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::unpack(buf, buflen);

	portstatsarray.set_version(get_version());
	portstatsarray.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_port_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_port_stats_reply::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_stats_type() != rofl::openflow10::OFPST_PORT)
			throw eMsgInval("cofmsg_port_stats_reply::unpack() invalid statistics type");

		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			portstatsarray.unpack(hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));
		}
	} break;
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_PORT_STATS)
			throw eMsgInval("cofmsg_port_stats_reply::unpack() invalid statistics type");

		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			portstatsarray.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
		}
	};
	}

	if (get_length() < cofmsg_port_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_port_stats_reply::unpack() buf too short");
}






