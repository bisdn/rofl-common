/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofbucketcounters.cc
 *
 *  Created on: 16.03.2014
 *      Author: andi
 */

#include "rofl/common/openflow/cofbucketcounters.h"

using namespace rofl::openflow;


size_t
cofbucket_counters::length() const
{
	size_t len = 0;
	for (std::map<uint32_t, cofbucket_counter>::const_iterator
			it = bucketcounters.begin(); it != bucketcounters.end(); ++it) {
		len += it->second.length();
	}
	return len;
}


void
cofbucket_counters::pack(uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		for (std::map<uint32_t, cofbucket_counter>::iterator
				it = bucketcounters.begin(); it != bucketcounters.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
			buflen -= it->second.length();
		}
	} break;
	default: {
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}
}


void
cofbucket_counters::unpack(uint8_t* buf, size_t buflen)
{
	clear();

	switch (ofp_version) {
	case rofl::openflow12::OFP_VERSION: {

		uint32_t bucket_counter_id = 0;

		while (buflen >= sizeof(struct rofl::openflow12::ofp_bucket_counter)) {
			add_bucket_counter(bucket_counter_id++).unpack(buf, sizeof(struct rofl::openflow12::ofp_bucket_counter));
			buf += sizeof(struct rofl::openflow12::ofp_bucket_counter);
			buflen -= sizeof(struct rofl::openflow12::ofp_bucket_counter);
		}

	} break;
	case rofl::openflow13::OFP_VERSION: {

		uint32_t bucket_counter_id = 0;

		while (buflen >= sizeof(struct rofl::openflow13::ofp_bucket_counter)) {
			add_bucket_counter(bucket_counter_id++).unpack(buf, sizeof(struct rofl::openflow13::ofp_bucket_counter));
			buf += sizeof(struct rofl::openflow13::ofp_bucket_counter);
			buflen -= sizeof(struct rofl::openflow13::ofp_bucket_counter);
		}

	} break;
	default: {
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}


}




