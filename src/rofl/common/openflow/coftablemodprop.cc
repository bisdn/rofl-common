/*
 * coftablemodprop.cc
 *
 *  Created on: 31.03.2015
 *      Author: andi
 */

#include "rofl/common/openflow/coftablemodprop.h"

using namespace rofl::openflow;

void
coftablemod_prop::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < coftablemod_prop::length()) // take length() method from this base class
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow14::OFP_VERSION: {

		struct rofl::openflow14::ofp_table_mod_prop_header* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_header*)buf;

		len = length(); // take virtual length() method

		hdr->type	= htobe16(type);
		hdr->length	= htobe16(len);

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftablemod_prop::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < coftablemod_prop::length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow14::OFP_VERSION: {

		struct rofl::openflow14::ofp_table_mod_prop_header* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_header*)buf;

		type	= be16toh(hdr->type);
		len		= be16toh(hdr->length);

		if (len < sizeof(struct rofl::openflow14::ofp_table_mod_prop_header))
			throw eInval();

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftablemod_prop_eviction::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < coftablemod_prop_eviction::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		coftablemod_prop::pack(buf, buflen);

		struct rofl::openflow14::ofp_table_mod_prop_eviction* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_eviction*)buf;

		hdr->flags = htobe32(flags);

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftablemod_prop_eviction::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow14::ofp_table_mod_prop_eviction))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		coftablemod_prop::unpack(buf, buflen);

		if (get_length() < sizeof(struct rofl::openflow14::ofp_table_mod_prop_eviction))
			throw eInval();

		struct rofl::openflow14::ofp_table_mod_prop_eviction* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_eviction*)buf;

		flags = be32toh(hdr->flags);

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftablemod_prop_vacancy::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < coftablemod_prop_vacancy::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		coftablemod_prop::pack(buf, buflen);

		struct rofl::openflow14::ofp_table_mod_prop_vacancy* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_vacancy*)buf;

		hdr->vacancy_down = vacancy_down;
		hdr->vacancy_up   = vacancy_up;
		hdr->vacancy      = vacancy;

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftablemod_prop_vacancy::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow14::ofp_table_mod_prop_vacancy))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		coftablemod_prop::unpack(buf, buflen);

		if (get_length() < sizeof(struct rofl::openflow14::ofp_table_mod_prop_vacancy))
			throw eInval();

		struct rofl::openflow14::ofp_table_mod_prop_vacancy* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_vacancy*)buf;

		vacancy_down = hdr->vacancy_down;
		vacancy_up   = hdr->vacancy_up;
		vacancy      = hdr->vacancy;

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftablemod_prop_experimenter::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < coftablemod_prop_experimenter::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		coftablemod_prop::pack(buf, buflen);

		struct rofl::openflow14::ofp_table_mod_prop_experimenter* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_experimenter*)buf;

		hdr->experimenter	= htobe32(exp_id);
		hdr->exp_type		= htobe32(exp_type);
		exp_body.pack((uint8_t*)hdr->experimenter_data, exp_body.memlen());

	} break;
	default:
		throw eBadVersion();
	}
}



void
coftablemod_prop_experimenter::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow14::ofp_table_mod_prop_experimenter))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		coftablemod_prop::unpack(buf, buflen);

		if (buflen < get_length())
			throw eInval();

		struct rofl::openflow14::ofp_table_mod_prop_experimenter* hdr =
				(struct rofl::openflow14::ofp_table_mod_prop_experimenter*)buf;

		exp_id	= be32toh(hdr->experimenter);
		exp_type = be32toh(hdr->exp_type);
		exp_body.unpack((uint8_t*)hdr->experimenter_data, get_length() - sizeof(struct rofl::openflow14::ofp_table_mod_prop_experimenter));

	} break;
	default:
		throw eBadVersion();
	}
}


