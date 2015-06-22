/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofgroupmod.h"

using namespace rofl::openflow;


cofgroupmod::cofgroupmod(
		uint8_t ofp_version,
		uint16_t command,
		uint8_t type,
		uint32_t group_id) :
		ofp_version(ofp_version),
		command(command),
		type(type),
		group_id(group_id),
		buckets(ofp_version)
{}


cofgroupmod::~cofgroupmod()
{}


cofgroupmod&
cofgroupmod::operator= (const cofgroupmod& ge)
{
	if (this == &ge)
		return *this;

	this->ofp_version 		= ge.ofp_version;
	this->command           = ge.command;
	this->type              = ge.type;
	this->group_id          = ge.group_id;
	this->buckets 			= ge.buckets;

	return *this;
}


void
cofgroupmod::clear()
{
	command = htobe16(openflow13::OFPGC_ADD);  // = 0
	type = openflow13::OFPGT_ALL; // = 0
	group_id = 0;

	buckets.clear();
}



size_t
cofgroupmod::length() const
{
	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct ofp_group_mod) + buckets.length());
	} break;
	default:
		throw eBadVersion("cofgroupmod::length() no version defined");
	}
}



void
cofgroupmod::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval("cofgroupmod::pack() buflen too short");

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct ofp_group_mod* hdr = (struct ofp_group_mod*)buf;

		hdr->command  = htobe16(command);
		hdr->type     = type;
		hdr->group_id = htobe32(group_id);

		buckets.pack((uint8_t*)hdr->buckets, buflen - sizeof(struct ofp_group_mod));

	} break;
	default:
		throw eBadVersion("cofgroupmod::pack() unsupported version");
	}
}



void
cofgroupmod::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	buckets.clear();

	switch (get_version()) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < sizeof(struct ofp_group_mod))
			throw eInval("cofgroupmod::unpack() buflen too short");

		struct ofp_group_mod* hdr = (struct ofp_group_mod*)buf;

		command  = be16toh(hdr->command);
		type     = hdr->type;
		group_id = be32toh(hdr->group_id);

		size_t buckets_len = buflen - sizeof(struct ofp_group_mod);

		if (buckets_len > 0)
			buckets.unpack((uint8_t*)hdr->buckets, buckets_len);

	} break;
	default:
		throw eBadVersion("cofgroupmod::unpack() unsupported version");
	}
}


