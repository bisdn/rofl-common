/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_desc_stats.h"

using namespace rofl::openflow;

cofmsg_desc_stats_request::~cofmsg_desc_stats_request()
{}



cofmsg_desc_stats_request::cofmsg_desc_stats_request(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags) :
				cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_DESC, stats_flags)
{
	switch (version) {
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_DESC);
	} break;
	default: {
		// do nothing
	};
	}
}



cofmsg_desc_stats_request::cofmsg_desc_stats_request(
		const cofmsg_desc_stats_request& msg)
{
	*this = msg;
}



cofmsg_desc_stats_request&
cofmsg_desc_stats_request::operator= (
		const cofmsg_desc_stats_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_request::operator= (msg);
	return *this;
}



size_t
cofmsg_desc_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request));
	} break;
	default: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request));
	};
	}
	return 0;
}



void
cofmsg_desc_stats_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_desc_stats_request::length())
		throw eMsgInval("cofmsg_desc_stats_request::pack() buf too short");
}



void
cofmsg_desc_stats_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_desc_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_desc_stats_request::unpack() buf too short");

	if (get_length() < cofmsg_desc_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_desc_stats_request::unpack() buf too short");
}





cofmsg_desc_stats_reply::~cofmsg_desc_stats_reply()
{}



cofmsg_desc_stats_reply::cofmsg_desc_stats_reply(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		const cofdesc_stats_reply& desc_stats) :
				cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_DESC, stats_flags),
				desc_stats(desc_stats)
{
	this->desc_stats.set_version(version);

	switch (version) {
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_DESC);
	} break;
	default: {
		// do nothing
	};
	}
}



cofmsg_desc_stats_reply::cofmsg_desc_stats_reply(
		const cofmsg_desc_stats_reply& msg)
{
	*this = msg;
}



cofmsg_desc_stats_reply&
cofmsg_desc_stats_reply::operator= (
		const cofmsg_desc_stats_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_reply::operator= (msg);
	desc_stats = msg.desc_stats;
	return *this;
}



size_t
cofmsg_desc_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) + desc_stats.length());
	} break;
	default: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + desc_stats.length());
	};
 	}
	return 0;
}



void
cofmsg_desc_stats_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_desc_stats_reply::length())
		throw eMsgInval("cofmsg_desc_stats_reply::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		desc_stats.pack(hdr->body, desc_stats.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		desc_stats.pack(hdr->body, desc_stats.length());
	};
	}
}



void
cofmsg_desc_stats_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::unpack(buf, buflen);

	desc_stats.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_desc_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_desc_stats_reply::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_type() != rofl::openflow10::OFPT_STATS_REPLY)
			throw eMsgInval("cofmsg_desc_stats_reply::unpack() invalid message type");

		if (get_stats_type() != rofl::openflow10::OFPST_DESC)
			throw eMsgInval("cofmsg_desc_stats_reply::unpack() invalid statistics type");

		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			desc_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));
		}
	} break;
	default: {
		if (get_type() != rofl::openflow13::OFPT_MULTIPART_REPLY)
			throw eMsgInval("cofmsg_desc_stats_reply::unpack() invalid message type");

		if (get_stats_type() != rofl::openflow13::OFPMP_DESC)
			throw eMsgInval("cofmsg_desc_stats_reply::unpack() invalid statistics type");

		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			desc_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
		}
	};
	}

	if (get_length() < cofmsg_desc_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_desc_stats_reply::unpack() buf too short");
}


