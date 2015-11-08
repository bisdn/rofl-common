/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofgroupdescstats.h"

using namespace rofl::openflow;


size_t
cofgroup_desc_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_group_desc_stats) + buckets.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_group_desc) + buckets.length());
	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
	return 0;
}



void
cofgroup_desc_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow12::ofp_group_desc_stats) + buckets.length()))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_group_desc_stats *stats = (struct rofl::openflow12::ofp_group_desc_stats*)buf;

		stats->length		= htobe16(sizeof(struct rofl::openflow12::ofp_group_desc_stats) + buckets.length());
		stats->type			= type;
		stats->group_id		= htobe32(group_id);

		buckets.pack((uint8_t*)(stats->buckets), buckets.length());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < (sizeof(struct rofl::openflow13::ofp_group_desc) + buckets.length()))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_group_desc *stats = (struct rofl::openflow13::ofp_group_desc*)buf;

		stats->length		= htobe16(sizeof(struct rofl::openflow13::ofp_group_desc) + buckets.length());
		stats->type			= type;
		stats->group_id		= htobe32(group_id);

		buckets.pack((uint8_t*)(stats->buckets), buckets.length());

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofgroup_desc_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow12::ofp_group_desc_stats))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_group_desc_stats *stats = (struct rofl::openflow12::ofp_group_desc_stats*)buf;

		if (be16toh(stats->length) > buflen)
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		group_id		= be32toh(stats->group_id);
		type			= stats->type;

		buckets.unpack((uint8_t*)stats->buckets, buflen - sizeof(struct rofl::openflow12::ofp_group_desc_stats));

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_group_desc))
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_group_desc *stats = (struct rofl::openflow13::ofp_group_desc*)buf;

		if (be16toh(stats->length) > buflen)
			throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		group_id		= be32toh(stats->group_id);
		type			= stats->type;

		buckets.unpack((uint8_t*)stats->buckets, buflen - sizeof(struct rofl::openflow13::ofp_group_desc));

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



