/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofgroups.cc
 *
 *  Created on: 14.03.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofgroupstatsarray.h"

using namespace rofl::openflow;


size_t
cofgroupstatsarray::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofgroup_stats_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofgroupstatsarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<uint32_t, cofgroup_stats_reply>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
		}

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofgroupstatsarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow12::ofp_group_stats)) {

			size_t length = be16toh(((struct rofl::openflow12::ofp_group_stats*)buf)->length);

			if (length < sizeof(struct rofl::openflow12::ofp_group_stats))
				return;

			uint32_t group_id = be32toh(((struct rofl::openflow12::ofp_group_stats*)buf)->group_id);

			add_group_stats(group_id).unpack(buf, length);

			buf += length;
			buflen -= length;
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow13::ofp_group_stats)) {

			size_t length = be16toh(((struct rofl::openflow13::ofp_group_stats*)buf)->length);

			if (length < sizeof(struct rofl::openflow13::ofp_group_stats))
				return;

			uint32_t group_id = be32toh(((struct rofl::openflow13::ofp_group_stats*)buf)->group_id);

			add_group_stats(group_id).unpack(buf, length);

			buf += length;
			buflen -= length;
		}
	} break;
	default:
		throw eBadRequestBadVersion("eBadRequestBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}


