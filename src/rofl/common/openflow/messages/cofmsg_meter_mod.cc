/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_meter_mod.h"

using namespace rofl::openflow;

cofmsg_meter_mod::~cofmsg_meter_mod()
{}



cofmsg_meter_mod::cofmsg_meter_mod(
		uint8_t version,
		uint32_t xid,
		uint16_t command,
		uint16_t flags,
		uint32_t meter_id,
		const rofl::openflow::cofmeter_bands& meter_bands) :
				cofmsg(version, rofl::openflow::OFPT_METER_MOD, xid),
				command(command),
				flags(flags),
				meter_id(meter_id),
				meter_bands(meter_bands)
{
	this->meter_bands.set_version(version);
}



cofmsg_meter_mod::cofmsg_meter_mod(
		const cofmsg_meter_mod& msg)
{
	*this = msg;
}



cofmsg_meter_mod&
cofmsg_meter_mod::operator= (
		const cofmsg_meter_mod& msg)
{
	if (this == &msg)
		return *this;
	cofmsg::operator= (msg);
	command		= msg.command;
	flags		= msg.flags;
	meter_id	= msg.meter_id;
	meter_bands = msg.meter_bands;
	return *this;
}



size_t
cofmsg_meter_mod::length() const
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_mod) + meter_bands.length());
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
	return 0;
}



void
cofmsg_meter_mod::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_meter_mod::length())
		throw eMsgInval("cofmsg_meter_mod::pack() buf too short");

	switch (get_version()) {
	default: {
		struct rofl::openflow13::ofp_meter_mod* hdr =
				(struct rofl::openflow13::ofp_meter_mod*)buf;
		hdr->command = htobe16(command);
		hdr->flags = htobe16(flags);
		hdr->meter_id = htobe32(meter_id);
		meter_bands.pack((uint8_t*)hdr->bands, meter_bands.length());
	};
	}
}



void
cofmsg_meter_mod::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	meter_bands.clear();
	meter_bands.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_meter_mod::length())
		throw eBadRequestBadLen("cofmsg_meter_mod::unpack() buf too short");

	switch (get_version()) {
	default: {

		if (get_type() != rofl::openflow13::OFPT_METER_MOD)
			throw eMsgInval("cofmsg_meter_mod::unpack() invalid message type");

		struct rofl::openflow13::ofp_meter_mod* hdr =
				(struct rofl::openflow13::ofp_meter_mod*)buf;
		command = be16toh(hdr->command);
		flags = be16toh(hdr->flags);
		meter_id = be32toh(hdr->meter_id);
		if (buflen > sizeof(struct rofl::openflow13::ofp_meter_mod)) {
			meter_bands.unpack((uint8_t*)hdr->bands, buflen - sizeof(struct rofl::openflow13::ofp_meter_mod));
		}
	};
	}

	if (get_length() < cofmsg_meter_mod::length())
		throw eBadRequestBadLen("cofmsg_meter_mod::unpack() buf too short");
}




