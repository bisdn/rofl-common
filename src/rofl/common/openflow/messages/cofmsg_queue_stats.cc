/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_queue_stats.h"

using namespace rofl::openflow;

cofmsg_queue_stats_request::~cofmsg_queue_stats_request()
{}



cofmsg_queue_stats_request::cofmsg_queue_stats_request(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofqueue_stats_request& queue_stats) :
				cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_QUEUE, stats_flags),
				queue_stats(queue_stats)
{
	this->queue_stats.set_version(version);
}



cofmsg_queue_stats_request::cofmsg_queue_stats_request(
		const cofmsg_queue_stats_request& msg)
{
	*this = msg;
}



cofmsg_queue_stats_request&
cofmsg_queue_stats_request::operator= (
		const cofmsg_queue_stats_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_request::operator= (msg);
	queue_stats = msg.queue_stats;
	return *this;
}



size_t
cofmsg_queue_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request) + queue_stats.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_request) + queue_stats.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + queue_stats.length());
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
	return 0;
}



void
cofmsg_queue_stats_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_queue_stats_request::length())
		throw eMsgInval("cofmsg_queue_stats_request::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		queue_stats.pack(hdr->body, queue_stats.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_multipart_request* hdr =
				(struct rofl::openflow13::ofp_multipart_request*)buf;
		queue_stats.pack(hdr->body, queue_stats.length());
	};
	}
}



void
cofmsg_queue_stats_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::unpack(buf, buflen);

	queue_stats.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_queue_stats_request::length())
		throw eBadRequestBadLen("cofmsg_queue_stats_request::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_stats_type() != rofl::openflow10::OFPST_QUEUE)
			throw eMsgInval("cofmsg_queue_stats_request::unpack() invalid statistics type");

		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		if (buflen > sizeof(struct rofl::openflow10::ofp_stats_request)) {
			queue_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_request));
		}
	} break;
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_QUEUE)
			throw eMsgInval("cofmsg_queue_stats_request::unpack() invalid statistics type");

		struct rofl::openflow13::ofp_multipart_request* hdr =
				(struct rofl::openflow13::ofp_multipart_request*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			queue_stats.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_request));
		}
	};
	}

	if (get_length() < cofmsg_queue_stats_request::length())
		throw eBadRequestBadLen("cofmsg_queue_stats_request::unpack() buf too short");
}













cofmsg_queue_stats_reply::~cofmsg_queue_stats_reply()
{}



cofmsg_queue_stats_reply::cofmsg_queue_stats_reply(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		const rofl::openflow::cofqueuestatsarray& queuestatsarray) :
				cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_QUEUE, stats_flags),
				queuestatsarray(queuestatsarray)
{
	this->queuestatsarray.set_version(version);
}



cofmsg_queue_stats_reply::cofmsg_queue_stats_reply(
		const cofmsg_queue_stats_reply& msg)
{
	*this = msg;
}



cofmsg_queue_stats_reply&
cofmsg_queue_stats_reply::operator= (
		const cofmsg_queue_stats_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_reply::operator= (msg);
	queuestatsarray = msg.queuestatsarray;
	return *this;
}



size_t
cofmsg_queue_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) + queuestatsarray.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + queuestatsarray.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + queuestatsarray.length());
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
	return 0;
}



void
cofmsg_queue_stats_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_queue_stats_reply::length())
		throw eMsgInval("cofmsg_queue_stats_reply::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		queuestatsarray.pack(hdr->body, queuestatsarray.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		queuestatsarray.pack(hdr->body, queuestatsarray.length());
	};
	}
}



void
cofmsg_queue_stats_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::unpack(buf, buflen);

	queuestatsarray.clear();
	queuestatsarray.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_queue_stats_reply::length())
		throw eBadRequestBadLen("cofmsg_queue_stats_reply::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_stats_type() != rofl::openflow10::OFPST_QUEUE)
			throw eMsgInval("cofmsg_queue_stats_reply::unpack() invalid statistics type");

		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			queuestatsarray.unpack(hdr->body, buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));
		}
	} break;
	default: {
		if (get_stats_type() != rofl::openflow13::OFPMP_QUEUE)
			throw eMsgInval("cofmsg_queue_stats_reply::unpack() invalid statistics type");

		struct rofl::openflow13::ofp_multipart_reply* hdr =
				(struct rofl::openflow13::ofp_multipart_reply*)buf;
		if (buflen > sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			queuestatsarray.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));
		}
	};
	}

	if (get_length() < cofmsg_queue_stats_reply::length())
		throw eBadRequestBadLen("cofmsg_queue_stats_reply::unpack() buf too short");
}




