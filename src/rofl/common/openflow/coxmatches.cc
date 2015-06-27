/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#include <stdexcept>

#include "rofl/common/openflow/coxmatches.h"

using namespace rofl::openflow;

coxmatches::coxmatches()
{

}


coxmatches::~coxmatches()
{
	clear();
}


coxmatches::coxmatches(
		coxmatches const& oxmlist)
{
	*this = oxmlist;
}


coxmatches&
coxmatches::operator= (
		coxmatches const& oxms)
{
	if (this == &oxms)
		return *this;

	clear();

	for (std::map<uint64_t, coxmatch*>::const_iterator
			jt = oxms.matches.begin(); jt != oxms.matches.end(); ++jt) {
		add_match(new coxmatch(jt->second->somem(), jt->second->memlen()));
	}

	return *this;
}




void
coxmatches::clear()
{
	for (std::map<uint64_t, coxmatch*>::iterator
			it = matches.begin(); it != matches.end(); ++it) {
		delete it->second;
		it->second = NULL;
		//matches.clear();
	}
	matches.clear();
}



bool
coxmatches::operator== (coxmatches const& oxms) const
{
	if (matches.size() != oxms.matches.size()) {
		return false;
	}
	for (std::map<uint64_t, coxmatch*>::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		if (oxms.matches.find(it->first) == oxms.matches.end()) {
			return false;
		}
		if (*it->second != *oxms.matches.at(it->first)) {
			return false;
		}
	}
	return true;
}





void
coxmatches::unpack(
		uint8_t* buf,
		size_t buflen)
{
	clear();

	// xxx treat experimenter?

	// sanity check: oxm_len must be of size at least of ofp_oxm_hdr
	if (buflen < (int)sizeof(struct rofl::openflow::ofp_oxm_hdr)) {
		throw eBadMatchBadLen();
	}


	while (buflen > 0) {
		struct openflow::ofp_oxm_hdr *hdr = (struct openflow::ofp_oxm_hdr*)buf;

		if ((buflen < sizeof(struct openflow::ofp_oxm_hdr)) || (0 == hdr->oxm_length)) {
			return; // not enough bytes to parse an entire ofp_oxm_hdr, possibly padding bytes found
		}

		if (hdr->oxm_length > (sizeof(struct openflow::ofp_oxm_hdr) + buflen))
			throw eBadMatchBadLen();

		add_match(new coxmatch(buf, sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length));

		buflen -= ((sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length));
		buf += ((sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length));
	}
}



void
coxmatches::pack(
		uint8_t* buf, size_t buflen)
{
	if (buflen < length()) {
		throw eBadMatchBadLen();
	}
	for (std::map<uint64_t, coxmatch*>::iterator
			jt = matches.begin(); jt != matches.end(); ++jt) {

		coxmatch *match = (matches[jt->first]);

		match->pack(buf, match->length());

		buf += match->length();
	}
}


coxmatch*
coxmatches::add_match(coxmatch *oxm)
{
	// keep class and field, hide mask and length
	uint64_t id = (__UINT64_C(0xfffffe00) & oxm->get_oxm_id()) << 32 | (
			oxm->is_experimenter() ? oxm->get_oxm_exp_id() : 0);

	std::map<uint64_t, coxmatch*>::iterator it = matches.find(id);
	if (it != matches.end()) {
		delete it->second;
		matches.erase(it);
	}
	return (matches[id] = oxm);
}


coxmatch*
coxmatches::add_match(uint32_t oxm_id)
{
	// todo experimenter currently not supported here

	// keep class and field, hide mask and length
	uint64_t id = (__UINT64_C(0xfffffe00) & oxm_id) << 32;
	std::map<uint64_t, coxmatch*>::iterator it = matches.find(id);
	if (it != matches.end()) {
		delete it->second;
		matches.erase(it);
	}
	return (matches[id] = new coxmatch(oxm_id));
}


coxmatch*
coxmatches::set_match(uint32_t oxm_id)
{
	uint64_t oid = (__UINT64_C(0xfffffe00) & oxm_id) << 32; // keep class and field, hide mask and length
	if (matches.find(oid) == matches.end()) {
		matches[oid] = new coxmatch(oxm_id);
	}
	return matches[oid];
}


coxmatch const*
coxmatches::get_match(uint32_t oxm_id) const
{
	uint64_t oid = (__UINT64_C(0xfffffe00) & oxm_id) << 32; // keep class and field, hide mask and length
	if (matches.find(oid) == matches.end()) {
		throw eOxmNotFound("coxmatches::get_match() oxm-id not found");
	}
	return matches.at(oid);
}


void
coxmatches::drop_match(uint32_t oxm_id, uint32_t exp_id)
{
	// keep class and field, hide mask and length
	uint64_t id = (__UINT64_C(0xfffffe00) & oxm_id) << 32 |
			(coxmatch::is_experimenter(oxm_id) ? exp_id : 0);
	std::map<uint64_t, coxmatch*>::iterator it = matches.find(id);
	if (it == matches.end()) {
		return;
	}
	delete it->second;
	matches.erase(id);
}


bool
coxmatches::has_match(uint32_t oxm_id) const
{
	uint64_t oid = (__UINT64_C(0xfffffe00) & oxm_id) << 32; // keep class and field, hide mask and length
	return (not (matches.find(oid) == matches.end()));
}



size_t
coxmatches::length() const
{
	size_t len = 0;
	for (std::map<uint64_t, coxmatch*>::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		len += it->second->length();
	}
	return len;
}




bool
coxmatches::contains(
	coxmatches const& oxms,
	bool strict /* = false (default) */)
{
	/*
	 * strict:
	 * all elemens in *this and oxl must be identical (number of and value of elements)
	 *
	 * non-strict:
	 * all elements in oxl must be present in *this (value of elements), but *this may contain additional OXM TLVs
	 */

	if (strict) {
		// strict: # of elems for an ofm_class must be the same in oxl for the specific ofm_class
		if (matches.size() != oxms.matches.size()) {
			return false;
		}
	} else  /* non-strict*/ {
		// non-strict: # of elems for a class must be larger (or equal) the number of elements in oxl
		if (matches.size() > oxms.matches.size()) {
			return false;
		}
	}

	// strict: check all TLVs for specific class in oxl.matches => must exist and have same value
	for (std::map<uint64_t, coxmatch*>::iterator
			jt = matches.begin(); jt != matches.end(); ++jt) {

		coxmatch *lmatch = (jt->second);

		// keep in mind: match.get_oxm_id() & 0xfffffe00 == jt->first

		// strict: all OXM TLVs must also exist in oxl
		uint64_t id = (__UINT64_C(0xfffffe00) & lmatch->get_oxm_id()) << 32 |
				(lmatch->is_experimenter() ? lmatch->get_oxm_exp_id() : 0);
		if (oxms.matches.find(id) == oxms.matches.end()) {
			return false;
		}

		coxmatch const *rmatch = (oxms.matches.find(jt->first)->second);

		// strict: both OXM TLVs must have identical values
		if (*lmatch != *rmatch) {
			return false;
		}
	}

	return true;
}




bool
coxmatches::is_part_of(
		coxmatches const& oxms,
		uint16_t& exact_hits,
		uint16_t& wildcard_hits,
		uint16_t& missed)
{
	bool result = true;

	for (std::map<uint64_t, coxmatch*>::const_iterator
			jt = oxms.matches.begin(); jt != oxms.matches.end(); ++jt) {

		coxmatch const *rmatch = (jt->second);

		// keep in mind: match.get_oxm_id() & 0xfffffe00 == jt->first

		uint64_t id = (__UINT64_C(0xfffffe00) & rmatch->get_oxm_id()) << 32 |
				(rmatch->is_experimenter() ? rmatch->get_oxm_exp_id() : 0);
		if (matches.find(id) == matches.end()) {
			wildcard_hits++; continue;
		}

		if (*matches[jt->first] != *rmatch) {
			missed++; result = false; continue;
		}

		exact_hits++;
	}

	return result;
}


