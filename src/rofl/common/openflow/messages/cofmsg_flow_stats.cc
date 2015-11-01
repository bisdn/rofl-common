/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmsg_flow_stats.h"

using namespace rofl::openflow;

cofmsg_flow_stats_request::~cofmsg_flow_stats_request()
{}



cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofflow_stats_request& flow_stats) :
				cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_FLOW, stats_flags),
				flow_stats(flow_stats)
{
	this->flow_stats.set_version(version);
}



cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		const cofmsg_flow_stats_request& msg)
{
	*this = msg;
}



cofmsg_flow_stats_request&
cofmsg_flow_stats_request::operator= (
		const cofmsg_flow_stats_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_request::operator= (msg);
	flow_stats = msg.flow_stats;
	return *this;
}



size_t
cofmsg_flow_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request) + flow_stats.length());
	} break;
	default: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + flow_stats.length());
	};
	}
	return 0;
}



void
cofmsg_flow_stats_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_flow_stats_request::length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		flow_stats.pack(hdr->body, flow_stats.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_multipart_request* hdr =
				(struct rofl::openflow13::ofp_multipart_request*)buf;
		flow_stats.pack(hdr->body, flow_stats.length());
	};
	}
}



void
cofmsg_flow_stats_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::unpack(buf, buflen);

	flow_stats.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_flow_stats_request::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_stats_type() != rofl::openflow10::OFPST_FLOW)
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		if (buflen > sizeof(struct rofl::openflow10::ofp_stats_request)) {
			flow_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_request));
		}
	} break;
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_FLOW)
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_multipart_request* hdr =
				(struct rofl::openflow13::ofp_multipart_request*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			flow_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_request));
		}
	};
	}

	if (get_length() < cofmsg_flow_stats_request::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}







cofmsg_flow_stats_reply::~cofmsg_flow_stats_reply()
{}



cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		uint8_t version,
		uint32_t xid,
		uint16_t flags,
		const rofl::openflow::cofflowstatsarray& flowstatsarray) :
				cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_FLOW, flags),
				flowstatsarray(flowstatsarray)
{
	this->flowstatsarray.set_version(version);
}



cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		const cofmsg_flow_stats_reply& msg)
{
	*this = msg;
}



cofmsg_flow_stats_reply&
cofmsg_flow_stats_reply::operator= (
		const cofmsg_flow_stats_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_reply::operator= (msg);
	flowstatsarray = msg.flowstatsarray;
	return *this;
}



size_t
cofmsg_flow_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) + flowstatsarray.length());
	} break;
	default: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + flowstatsarray.length());
	};
	}
	return 0;
}



void
cofmsg_flow_stats_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_flow_stats_reply::length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		flowstatsarray.pack(hdr->body, flowstatsarray.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_multipart_request* hdr =
				(struct rofl::openflow13::ofp_multipart_request*)buf;
		flowstatsarray.pack(hdr->body, flowstatsarray.length());
	};
	}
}



void
cofmsg_flow_stats_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::unpack(buf, buflen);

	flowstatsarray.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_flow_stats_reply::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_stats_type() != rofl::openflow10::OFPST_FLOW)
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			flowstatsarray.unpack(hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));
		}
	} break;
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_FLOW)
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			flowstatsarray.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
		}
	};
	}

	if (get_length() < cofmsg_flow_stats_reply::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}


