/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_table_mod.h"

using namespace rofl::openflow;

cofmsg_table_mod::~cofmsg_table_mod()
{}



cofmsg_table_mod::cofmsg_table_mod(
		uint8_t version,
		uint32_t xid,
		uint8_t  table_id,
		uint32_t config) :
				cofmsg(version, rofl::openflow::OFPT_TABLE_MOD, xid),
				table_id(table_id),
				config(config)
{}



cofmsg_table_mod::cofmsg_table_mod(
		const cofmsg_table_mod& msg)
{
	*this = msg;
}



cofmsg_table_mod&
cofmsg_table_mod::operator= (
		const cofmsg_table_mod& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	table_id = msg.table_id;
	config = msg.config;
	return *this;
}



size_t
cofmsg_table_mod::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_table_mod));
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
	return 0;
}



void
cofmsg_table_mod::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_table_mod::length())
		throw eInvalid("eInvalid").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);

	switch (get_version()) {
	default: {
		struct rofl::openflow13::ofp_table_mod* hdr =
				(struct rofl::openflow13::ofp_table_mod*)buf;
		hdr->table_id = table_id;
		hdr->config = htobe32(config);
	};
	}
}



void
cofmsg_table_mod::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_table_mod::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	default: {

		if (get_type() != rofl::openflow13::OFPT_TABLE_MOD)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_table_mod* hdr =
				(struct rofl::openflow13::ofp_table_mod*)buf;
		table_id = hdr->table_id;
		config = be32toh(hdr->config);
	};
	}

	if (get_length() < cofmsg_table_mod::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}




