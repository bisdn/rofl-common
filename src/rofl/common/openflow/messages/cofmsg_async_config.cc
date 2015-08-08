/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmsg_async_config.h"

using namespace rofl::openflow;

cofmsg_get_async_config_request::~cofmsg_get_async_config_request()
{}



cofmsg_get_async_config_request::cofmsg_get_async_config_request(
		uint8_t version,
		uint32_t xid) :
				cofmsg(version, rofl::openflow::OFPT_GET_ASYNC_REQUEST, xid)
{}



cofmsg_get_async_config_request::cofmsg_get_async_config_request(
		const cofmsg_get_async_config_request& msg)
{
	*this = msg;
}



cofmsg_get_async_config_request&
cofmsg_get_async_config_request::operator= (
		const cofmsg_get_async_config_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	return *this;
}



size_t
cofmsg_get_async_config_request::length() const
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow::ofp_header));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



void
cofmsg_get_async_config_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_get_async_config_request::length())
		throw eMsgInval("cofmsg_get_async_config_request::pack() buf too short");
}



void
cofmsg_get_async_config_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_get_async_config_request::length())
		throw eBadSyntaxTooShort("cofmsg_get_async_config_request::unpack() buf too short");

	switch (get_version()) {
	default: {
		if (get_type() != rofl::openflow13::OFPT_GET_ASYNC_REQUEST)
			throw eMsgInval("cofmsg_get_async_config_request::unpack() invalid message type");
	};
	}

	if (get_length() < cofmsg_get_async_config_request::length())
		throw eBadSyntaxTooShort("cofmsg_get_async_config_request::unpack() buf too short");
}











cofmsg_get_async_config_reply::~cofmsg_get_async_config_reply()
{}



cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		uint8_t version,
		uint32_t xid,
		const rofl::openflow::cofasync_config& async_config) :
				cofmsg(version, rofl::openflow::OFPT_GET_ASYNC_REPLY, xid),
				async_config(async_config)
{
	this->async_config.set_version(version);
}



cofmsg_get_async_config_reply::cofmsg_get_async_config_reply(
		const cofmsg_get_async_config_reply& msg)
{
	*this = msg;
}



cofmsg_get_async_config_reply&
cofmsg_get_async_config_reply::operator= (
		const cofmsg_get_async_config_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	async_config = msg.async_config;
	return *this;
}



size_t
cofmsg_get_async_config_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + async_config.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_get_async_config_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_get_async_config_reply::length())
		throw eMsgInval("cofmsg_get_async_config_reply::pack() buf too short");

	switch (get_version()) {
	default: {
		async_config.pack(buf + sizeof(struct rofl::openflow::ofp_header), async_config.length());
	};
	}
}



void
cofmsg_get_async_config_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	async_config.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_get_async_config_reply::length())
		throw eBadSyntaxTooShort("cofmsg_get_async_config_reply::unpack() buf too short");

	switch (get_version()) {
	default: {

		if (get_type() != rofl::openflow13::OFPT_GET_ASYNC_REPLY)
			throw eMsgInval("cofmsg_get_async_config_request::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			async_config.unpack(buf + sizeof(struct rofl::openflow::ofp_header), buflen - sizeof(struct rofl::openflow::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_get_async_config_reply::length())
		throw eBadSyntaxTooShort("cofmsg_get_async_config_reply::unpack() buf too short");
}










cofmsg_set_async_config::~cofmsg_set_async_config()
{}



cofmsg_set_async_config::cofmsg_set_async_config(
		uint8_t version,
		uint32_t xid,
		const rofl::openflow::cofasync_config& async_config) :
				cofmsg(version, rofl::openflow::OFPT_SET_ASYNC, xid),
				async_config(async_config)
{
	this->async_config.set_version(version);
}



cofmsg_set_async_config::cofmsg_set_async_config(
		const cofmsg_set_async_config& msg)
{
	*this = msg;
}



cofmsg_set_async_config&
cofmsg_set_async_config::operator= (
		const cofmsg_set_async_config& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	async_config = msg.async_config;
	return *this;
}



size_t
cofmsg_set_async_config::length() const
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + async_config.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_set_async_config::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_set_async_config::length())
		throw eMsgInval("cofmsg_set_async_config::pack() buf too short");

	switch (get_version()) {
	default: {
		async_config.pack(buf + sizeof(struct rofl::openflow::ofp_header), async_config.length());
	};
	}
}



void
cofmsg_set_async_config::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	async_config.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_set_async_config::length())
		throw eBadSyntaxTooShort("cofmsg_set_async_config::unpack() buf too short");

	switch (get_version()) {
	default: {

		if (get_type() != rofl::openflow13::OFPT_SET_ASYNC)
			throw eMsgInval("cofmsg_get_async_config_request::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			async_config.unpack(buf + sizeof(struct rofl::openflow::ofp_header), buflen - sizeof(struct rofl::openflow::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_set_async_config::length())
		throw eBadSyntaxTooShort("cofmsg_set_async_config::unpack() buf too short");
}


