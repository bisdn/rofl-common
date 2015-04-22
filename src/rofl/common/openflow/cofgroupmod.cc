/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofgroupmod.h"

using namespace rofl::openflow;

size_t
cofgroupmod::length() const
{
	return sizeof(struct rofl::openflow12::ofp_group_mod)
			- sizeof(struct rofl::openflow::ofp_header)
			+ buckets.length();
}



void
cofgroupmod::pack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < length())
		throw eGroupModInval("cofgroupmod::pack()");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		throw eBadRequestBadVersion("cofgroupmod::pack()");
	} break;
	default: {
		struct rofl::openflow12::ofp_group_mod* hdr =
				(struct rofl::openflow12::ofp_group_mod*)
					(buf - sizeof(struct rofl::openflow::ofp_header));

		hdr->command  = htobe16(command);
		hdr->type     = type;
		hdr->group_id = htobe32(group_id);

		buckets.pack((uint8_t*)(hdr->buckets), buckets.length());
	};
	}
}



void
cofgroupmod::unpack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < (sizeof(struct rofl::openflow12::ofp_group_mod)
						- sizeof(struct rofl::openflow::ofp_header)))
		throw eGroupModInval("cofgroupmod::unpack()");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		throw eBadRequestBadVersion("cofgroupmod::unpack()");
	} break;
	default: {
		struct rofl::openflow12::ofp_group_mod* hdr =
				(struct rofl::openflow12::ofp_group_mod*)
					(buf - sizeof(struct rofl::openflow::ofp_header));

		command  = be16toh(hdr->command);
		type     = hdr->type;
		group_id = be32toh(hdr->group_id);

		size_t bucketslen = buflen
				- sizeof(struct rofl::openflow12::ofp_group_mod)
				+ sizeof(struct rofl::openflow::ofp_header);

		if (bucketslen > 0) {
			buckets.unpack((uint8_t*)(hdr->buckets), bucketslen);
		}
	};
	}
}


