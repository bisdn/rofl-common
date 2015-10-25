/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_role.h"

using namespace rofl::openflow;

cofmsg_role_request::~cofmsg_role_request()
{}



cofmsg_role_request::cofmsg_role_request(
		uint8_t version,
		uint32_t xid,
		const rofl::openflow::cofrole& role) :
				cofmsg(version, rofl::openflow::OFPT_ROLE_REQUEST, xid),
				role(role)
{
	this->role.set_version(version);
}



cofmsg_role_request::cofmsg_role_request(
		const cofmsg_role_request& msg)
{
	*this = msg;
}



cofmsg_role_request&
cofmsg_role_request::operator= (
		const cofmsg_role_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	role = msg.role;
	return *this;
}



size_t
cofmsg_role_request::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_header) + role.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + role.length());
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
	return 0;
}



void
cofmsg_role_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_role_request::length())
		throw eInvalid("eInvalid").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);

	switch (get_version()) {
	default: {
		role.pack(buf + sizeof(struct rofl::openflow13::ofp_header), role.length());
	};
	}
}



void
cofmsg_role_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	role.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_role_request::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	default: {

		if (get_type() != rofl::openflow13::OFPT_ROLE_REQUEST)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		if (buflen > sizeof(struct rofl::openflow13::ofp_header)) {
			role.unpack(buf + sizeof(struct rofl::openflow13::ofp_header), buflen - sizeof(struct rofl::openflow13::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_role_request::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}









cofmsg_role_reply::~cofmsg_role_reply()
{}



cofmsg_role_reply::cofmsg_role_reply(
		uint8_t version,
		uint32_t xid,
		const rofl::openflow::cofrole& role) :
				cofmsg(version, rofl::openflow::OFPT_ROLE_REPLY, xid),
				role(role)
{
	this->role.set_version(version);
}



cofmsg_role_reply::cofmsg_role_reply(
		const cofmsg_role_reply& msg)
{
	*this = msg;
}



cofmsg_role_reply&
cofmsg_role_reply::operator= (
		const cofmsg_role_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	role = msg.role;
	return *this;
}



size_t
cofmsg_role_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_header) + role.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_header) + role.length());
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
	return 0;
}



void
cofmsg_role_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_role_reply::length())
		throw eInvalid("eInvalid").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);

	switch (get_version()) {
	default: {
		role.pack(buf + sizeof(struct rofl::openflow13::ofp_header), role.length());
	};
	}
}



void
cofmsg_role_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	role.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_role_reply::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	default: {

		if (get_type() != rofl::openflow13::OFPT_ROLE_REPLY)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		if (buflen > sizeof(struct rofl::openflow13::ofp_header)) {
			role.unpack(buf + sizeof(struct rofl::openflow13::ofp_header), buflen - sizeof(struct rofl::openflow13::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_role_reply::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}




