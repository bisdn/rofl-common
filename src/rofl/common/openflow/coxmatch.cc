/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmatch.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#include "rofl/common/openflow/coxmatch.h"

using namespace rofl::openflow;




/*
 * oxm-id
 */
void
coxmatch::set_oxm_id(uint32_t oxm_id) {
	if (memlen() < sizeof(struct ofp_oxm_tlv_hdr))
		throw eOxmBadLen("coxmatch::set_oxm_id() invalid length");
	struct rofl::openflow::ofp_oxm_tlv_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_tlv_hdr*)somem();
	oxm->oxm_id = htobe32(oxm_id);
}



uint32_t
coxmatch::get_oxm_id() const {
	if (memlen() < sizeof(struct ofp_oxm_tlv_hdr))
		throw eOxmBadLen("coxmatch::get_oxm_id() invalid length");
	struct rofl::openflow::ofp_oxm_tlv_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_tlv_hdr*)somem();
	return be32toh(oxm->oxm_id);
}

size_t
coxmatch::length() const {
	return memlen();
}

void
coxmatch::pack(
		uint8_t* buf,
		size_t buflen) {
	if (buflen < length()) {
		throw eOxmBadLen("coxmatch::pack() invalid length");
	}
	memcpy(buf, somem(), memlen());
}

void
coxmatch::unpack(
		uint8_t* buf,
		size_t buflen) {
	clear();
	if (buflen > memlen()) {
		resize(buflen);
	}
	assign(buf, buflen);
}

void
coxmatch::set_oxm_class(
		uint16_t oxm_class) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_class = htobe16(oxm_class);
}

uint16_t
coxmatch::get_oxm_class() const {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return be16toh(oxm->oxm_class);
}


void
coxmatch::set_oxm_field(
		uint8_t oxm_field) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_field = (oxm->oxm_field & 0x01) | ((oxm_field & 0x7f) << 1);
}


uint8_t
coxmatch::get_oxm_field() const {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return ((oxm->oxm_field) >> 1);
}


void
coxmatch::set_oxm_hasmask(
		bool hasmask) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_field = (oxm->oxm_field & 0xfe) | hasmask;
}


bool
coxmatch::get_oxm_hasmask() const {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return (oxm->oxm_field & 0x01);
}


void
coxmatch::set_oxm_length(
		uint8_t oxm_len) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_length = oxm_len;
}


uint8_t
coxmatch::get_oxm_length() {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return oxm->oxm_length;
}









/*
 * 16 bits
 */
coxmatch_16::coxmatch_16(uint32_t oxm_id, uint16_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_16BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u16value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


coxmatch_16::coxmatch_16(uint32_t oxm_id, uint16_t value, uint16_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_16BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 4*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u16value(value);
		set_u16mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


void
coxmatch_16::set_u16value(uint16_t value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint16_t))
		throw eOxmBadLen("coxmatch::set_u16value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	oxm->word = htobe16(value);
}


void
coxmatch_16::set_u16mask(uint16_t mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t))
		throw eOxmBadLen("coxmatch::set_u16mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	oxm->mask = htobe16(mask);
}


uint16_t
coxmatch_16::get_u16value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint16_t))
		throw eOxmBadLen("coxmatch::set_u16value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	return be16toh(oxm->word);
}


uint16_t
coxmatch_16::get_u16mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t))
		return 0xffff;
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	return be16toh(oxm->mask);
}


uint16_t
coxmatch_16::get_u16masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t))
		return get_u16value();
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	return be16toh((oxm->word & oxm->mask));
}



/*
 * 24 bits
 */
coxmatch_24::coxmatch_24(
		uint32_t oxm_id, uint32_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_24BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 3*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u24value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch_24::coxmatch_24() invalid OXM width");
	};
	}
}

coxmatch_24::coxmatch_24(
		uint32_t oxm_id, uint32_t value, uint32_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_24BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 6*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u24value(value);
		set_u24mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch_24::coxmatch_24() invalid OXM width");
	};
	}
}

void
coxmatch_24::set_u24value(uint32_t value)
{
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 3*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u24value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
	oxm->word[2] = (uint8_t)((value >>  0) & 0x000000ff);
	oxm->word[1] = (uint8_t)((value >>  8) & 0x000000ff);
	oxm->word[0] = (uint8_t)((value >> 16) & 0x000000ff);
}

void
coxmatch_24::set_u24mask(uint32_t mask)
{
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u24mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
	oxm->mask[2] = (uint8_t)((mask >>  0) & 0x000000ff);
	oxm->mask[1] = (uint8_t)((mask >>  8) & 0x000000ff);
	oxm->mask[0] = (uint8_t)((mask >> 16) & 0x000000ff);
}

uint32_t
coxmatch_24::get_u24value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 3*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::get_u24value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
	return (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
}

uint32_t
coxmatch_24::get_u24mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 6*sizeof(uint8_t))
		return 0x00ffffff;
	struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
	return ((((uint32_t)oxm->mask[0] << 16) | ((uint32_t)oxm->mask[1] << 8) | ((uint32_t)oxm->mask[2] << 0)) & 0x00ffffff);
}


uint32_t
coxmatch_24::get_u24masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch_24::get_u24masked_value() invalid length");
	return (get_u24value() & get_u24mask());
}



/*
 * IPv4
 */
coxmatch_32::coxmatch_32(
		uint32_t oxm_id, const rofl::caddress_in4& value)
{
	resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) +  4*sizeof(uint8_t));
	set_u32value(value);
	set_oxm_id(oxm_id);
}


coxmatch_32::coxmatch_32(
		uint32_t oxm_id, const rofl::caddress_in4& value, const rofl::caddress_in4& mask)
{

	resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2* 4*sizeof(uint8_t));
	set_u32value(value);
	set_u32mask(mask);
	set_oxm_id(oxm_id);
}

/*
 * 32 bits
 */
coxmatch_32::coxmatch_32(
		uint32_t oxm_id, uint32_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_32BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 4*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u32value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch_32::coxmatch_32() invalid OXM width");
	};
	}
}

coxmatch_32::coxmatch_32(
		uint32_t oxm_id, uint32_t value, uint32_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_32BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 8*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u32value(value);
		set_u32mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch_32::coxmatch_32() invalid OXM width");
	};
	}
}

void
coxmatch_32::set_u32value(uint32_t value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::set_u32value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	oxm->dword = htobe32(value);
}


void
coxmatch_32::set_u32mask(uint32_t mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::set_u32mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	oxm->mask = htobe32(mask);
}


uint32_t
coxmatch_32::get_u32value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::get_u32value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	return be32toh(oxm->dword);
}


uint32_t
coxmatch_32::get_u32mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return 0xffffffff;
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	return be32toh(oxm->mask);
}


uint32_t
coxmatch_32::get_u32masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return get_u32value();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	return be32toh((oxm->dword & oxm->mask));
}


void
coxmatch_32::set_u32value(rofl::caddress_in4 const& addr) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::set_u32value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	oxm->dword = addr.get_addr_nbo();
}


void
coxmatch_32::set_u32mask(rofl::caddress_in4 const& mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::set_u32mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	oxm->mask = mask.get_addr_nbo();
}


rofl::caddress_in4
coxmatch_32::get_u32value_as_addr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::get_u32value_as_addr() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	caddress_in4 addr; addr.set_addr_nbo(oxm->dword);
	return addr;
}


rofl::caddress_in4
coxmatch_32::get_u32mask_as_addr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return rofl::caddress_in4("255.255.255.255");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	caddress_in4 mask; mask.set_addr_nbo(oxm->mask);
	return mask;
}


rofl::caddress_in4
coxmatch_32::get_u32masked_value_as_addr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return get_u32value_as_addr();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	caddress_in4 masked; masked.set_addr_nbo(oxm->dword & oxm->mask);
	return masked;
}


/*
 * 48 bits
 */
coxmatch_48::coxmatch_48(
		uint32_t oxm_id, uint64_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_48BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 6*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u48value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


coxmatch_48::coxmatch_48(
		uint32_t oxm_id, uint64_t value, uint64_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_48BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 12*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u48value(value);
		set_u48mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}

/*
 * 48 bits
 */
coxmatch_48::coxmatch_48(
		uint32_t oxm_id, rofl::cmacaddr const& value)
{
	resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + OFP_ETH_ALEN);
	set_oxm_id(oxm_id);
	set_u48value(value);
}


coxmatch_48::coxmatch_48(
		uint32_t oxm_id, rofl::cmacaddr const& value, rofl::cmacaddr const& mask)
{
	resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN);
	set_oxm_id(oxm_id);
	set_u48value(value);
	set_u48mask(mask);
}



void
coxmatch_48::set_u48value(uint64_t value)
{
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u48value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	oxm->value[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
	oxm->value[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
	oxm->value[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
	oxm->value[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
	oxm->value[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
	oxm->value[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
}


void
coxmatch_48::set_u48mask(uint64_t mask)
{
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 12*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u48mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	oxm->mask[5] = (uint8_t)((mask >>  0) & 0x00000000000000ff);
	oxm->mask[4] = (uint8_t)((mask >>  8) & 0x00000000000000ff);
	oxm->mask[3] = (uint8_t)((mask >> 16) & 0x00000000000000ff);
	oxm->mask[2] = (uint8_t)((mask >> 24) & 0x00000000000000ff);
	oxm->mask[1] = (uint8_t)((mask >> 32) & 0x00000000000000ff);
	oxm->mask[0] = (uint8_t)((mask >> 40) & 0x00000000000000ff);
}


uint64_t
coxmatch_48::get_u48value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::get_u48value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	return (((uint64_t)oxm->value[0] << 40) | ((uint64_t)oxm->value[1] << 32) | ((uint64_t)oxm->value[2] << 24) |
			((uint64_t)oxm->value[3] << 16) | ((uint64_t)oxm->value[4] <<  8) | ((uint64_t)oxm->value[5] <<  0));
}


uint64_t
coxmatch_48::get_u48mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 12*sizeof(uint8_t))
		return get_u48value();
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	return (((uint64_t)oxm->mask[0] << 40) | ((uint64_t)oxm->mask[1] << 32) | ((uint64_t)oxm->mask[2] << 24) |
			((uint64_t)oxm->mask[3] << 16) | ((uint64_t)oxm->mask[4] <<  8) | ((uint64_t)oxm->mask[5] <<  0));
}


uint64_t
coxmatch_48::get_u48masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 12*sizeof(uint8_t))
		return get_u48value();
	return (get_u48value() & get_u48mask());
}

void
coxmatch_48::set_u48value(rofl::cmacaddr const& value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + OFP_ETH_ALEN)
		throw eOxmInval("coxmatch::set_u48value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	memcpy(oxm->value, value.somem(), OFP_ETH_ALEN);
}


void
coxmatch_48::set_u48mask(rofl::cmacaddr const& mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN)
		throw eOxmInval("coxmatch::set_u48mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	memcpy(oxm->mask, mask.somem(), OFP_ETH_ALEN);
}


rofl::cmacaddr
coxmatch_48::get_u48value_as_lladdr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + OFP_ETH_ALEN)
		throw eOxmInval("coxmatch::get_u48value_as_lladdr() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	cmacaddr addr;
	memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
	return addr;
}


rofl::cmacaddr
coxmatch_48::get_u48mask_as_lladdr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN)
		return rofl::cmacaddr("ff:ff:ff:ff:ff:ff");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	cmacaddr mask;
	memcpy(mask.somem(), oxm->mask, OFP_ETH_ALEN);
	return mask;
}


rofl::cmacaddr
coxmatch_48::get_u48masked_value_as_lladdr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN)
		return get_u48value();
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	cmacaddr addr;
	memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
	cmacaddr mask;
	memcpy(mask.somem(), oxm->mask, OFP_ETH_ALEN);
	return (addr & mask);
}


/*
 * 64 bits
 */
coxmatch_64::coxmatch_64(
		uint32_t oxm_id, uint64_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_64BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 8*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u64value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


coxmatch_64::coxmatch_64(
		uint32_t oxm_id, uint64_t value, uint64_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_64BIT: {
		rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 16*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_u64value(value);
		set_u64mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}

void
coxmatch_64::set_u64value(uint64_t value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint64_t))
		throw eOxmBadLen("coxmatch::set_u64value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	oxm->word = htobe64(value);
}


void
coxmatch_64::set_u64mask(uint64_t mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t))
		throw eOxmBadLen("coxmatch::set_u64mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	oxm->mask = htobe64(mask);
}


uint64_t
coxmatch_64::get_u64value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint64_t))
		throw eOxmBadLen("coxmatch::get_u64value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	return be64toh(oxm->word);
}


uint64_t
coxmatch_64::get_u64mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t))
		return 0xffffffffffffffffULL;
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	return be64toh(oxm->mask);
}


uint64_t
coxmatch_64::get_u64masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t))
		return get_u64value();
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	return be64toh((oxm->word & oxm->mask));
}


/*
 * 16 bytes
 */

//
// for caddress based constructors, see above
//

/*
 * IPv6
 */
coxmatch_128::coxmatch_128(
		uint32_t oxm_id, rofl::caddress_in6 const& value)
{
	resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 16*sizeof(uint8_t));
	set_u128value(value);
	set_oxm_id(oxm_id);
}


coxmatch_128::coxmatch_128(
		uint32_t oxm_id, rofl::caddress_in6 const& value, rofl::caddress_in6 const& mask)
{
	resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t));
	set_u128value(value);
	set_u128mask(mask);
	set_oxm_id(oxm_id);
}

void
coxmatch_128::set_u128value(rofl::caddress_in6 const& value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 16*sizeof(uint8_t))
		throw eOxmInval("coxmatch::set_u128value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	memcpy(oxm->addr, value.somem(), 16);
}


void
coxmatch_128::set_u128mask(rofl::caddress_in6 const& mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t))
		throw eOxmInval("coxmatch::set_u128mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	memcpy(oxm->mask, mask.somem(), 16);
}


rofl::caddress_in6
coxmatch_128::get_u128value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 16*sizeof(uint8_t))
		throw eOxmInval("coxmatch::get_u128value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	caddress_in6 addr;
	memcpy(addr.somem(), oxm->addr, 16);
	return addr;
}


rofl::caddress_in6
coxmatch_128::get_u128mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t))
		return rofl::caddress_in6("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	caddress_in6 mask;
	memcpy(mask.somem(), oxm->mask, 16);
	return mask;
}


rofl::caddress_in6
coxmatch_128::get_u128masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t))
		return get_u128value();
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	caddress_in6 addr;
	memcpy(addr.somem(), oxm->addr, 16);
	caddress_in6 mask;
	memcpy(mask.somem(), oxm->mask, 16);
	return (addr & mask);
}

// EXPERIMETER


/*
 * 16 bits
 */
coxmatch_16_exp::coxmatch_16_exp(uint32_t oxm_id, uint32_t exp_id, uint16_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_16BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u16value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


coxmatch_16_exp::coxmatch_16_exp(uint32_t oxm_id, uint32_t exp_id, uint16_t value, uint16_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_16BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 4*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u16value(value);
		set_u16mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


void
coxmatch_16_exp::set_u16value(uint16_t value) {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + sizeof(uint16_t))
		throw eOxmBadLen("coxmatch::set_u16value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
	oxm->word = htobe16(value);
}


void
coxmatch_16_exp::set_u16mask(uint16_t mask) {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint16_t))
		throw eOxmBadLen("coxmatch::set_u16mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
	oxm->mask = htobe16(mask);
}


uint16_t
coxmatch_16_exp::get_u16value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + sizeof(uint16_t))
		throw eOxmBadLen("coxmatch::set_u16value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
	return be16toh(oxm->word);
}


uint16_t
coxmatch_16_exp::get_u16mask() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint16_t))
		return 0xffff;
	struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
	return be16toh(oxm->mask);
}


uint16_t
coxmatch_16_exp::get_u16masked_value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint16_t))
		return get_u16value();
	struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
	return be16toh((oxm->word & oxm->mask));
}



/*
 * 24 bits
 */
coxmatch_24_exp::coxmatch_24_exp(
		uint32_t oxm_id, uint32_t exp_id, uint32_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_24BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 3*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u24value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch_24_exp::coxmatch_24_exp() invalid OXM width");
	};
	}
}

coxmatch_24_exp::coxmatch_24_exp(
		uint32_t oxm_id, uint32_t exp_id, uint32_t value, uint32_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_24BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 6*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u24value(value);
		set_u24mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch_24_exp::coxmatch_24_exp() invalid OXM width");
	};
	}
}

void
coxmatch_24_exp::set_u24value(uint32_t value)
{
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 3*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u24value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
	oxm->word[2] = (uint8_t)((value >>  0) & 0x000000ff);
	oxm->word[1] = (uint8_t)((value >>  8) & 0x000000ff);
	oxm->word[0] = (uint8_t)((value >> 16) & 0x000000ff);
}

void
coxmatch_24_exp::set_u24mask(uint32_t mask)
{
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u24mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
	oxm->mask[2] = (uint8_t)((mask >>  0) & 0x000000ff);
	oxm->mask[1] = (uint8_t)((mask >>  8) & 0x000000ff);
	oxm->mask[0] = (uint8_t)((mask >> 16) & 0x000000ff);
}

uint32_t
coxmatch_24_exp::get_u24value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 3*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::get_u24value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
	return (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
}

uint32_t
coxmatch_24_exp::get_u24mask() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 6*sizeof(uint8_t))
		return 0x00ffffff;
	struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
	return ((((uint32_t)oxm->mask[0] << 16) | ((uint32_t)oxm->mask[1] << 8) | ((uint32_t)oxm->mask[2] << 0)) & 0x00ffffff);
}


uint32_t
coxmatch_24_exp::get_u24masked_value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch_24_exp::get_u24masked_value() invalid length");
	return (get_u24value() & get_u24mask());
}

/*
 * 32 bits
 */
coxmatch_32_exp::coxmatch_32_exp(
		uint32_t oxm_id, uint32_t exp_id, uint32_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_32BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 4*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u32value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch_32_exp::coxmatch_32_exp() invalid OXM width");
	};
	}
}

coxmatch_32_exp::coxmatch_32_exp(
		uint32_t oxm_id, uint32_t exp_id, uint32_t value, uint32_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_32BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 8*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u32value(value);
		set_u32mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch_32_exp::coxmatch_32_exp() invalid OXM width");
	};
	}
}

void
coxmatch_32_exp::set_u32value(uint32_t value) {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::set_u32value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
	oxm->dword = htobe32(value);
}


void
coxmatch_32_exp::set_u32mask(uint32_t mask) {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::set_u32mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
	oxm->mask = htobe32(mask);
}


uint32_t
coxmatch_32_exp::get_u32value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + sizeof(uint32_t))
		throw eOxmBadLen("coxmatch::get_u32value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
	return be32toh(oxm->dword);
}


uint32_t
coxmatch_32_exp::get_u32mask() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint32_t))
		return 0xffffffff;
	struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
	return be32toh(oxm->mask);
}


uint32_t
coxmatch_32_exp::get_u32masked_value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint32_t))
		return get_u32value();
	struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
	return be32toh((oxm->dword & oxm->mask));
}

/*
 * 48 bits
 */
coxmatch_48_exp::coxmatch_48_exp(
		uint32_t oxm_id, uint32_t exp_id, uint64_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_48BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 6*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u48value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


coxmatch_48_exp::coxmatch_48_exp(
		uint32_t oxm_id, uint32_t exp_id, uint64_t value, uint64_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_48BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 12*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u48value(value);
		set_u48mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}

void
coxmatch_48_exp::set_u48value(uint64_t value)
{
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u48value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
	oxm->value[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
	oxm->value[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
	oxm->value[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
	oxm->value[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
	oxm->value[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
	oxm->value[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
}


void
coxmatch_48_exp::set_u48mask(uint64_t mask)
{
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 12*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::set_u48mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
	oxm->mask[5] = (uint8_t)((mask >>  0) & 0x00000000000000ff);
	oxm->mask[4] = (uint8_t)((mask >>  8) & 0x00000000000000ff);
	oxm->mask[3] = (uint8_t)((mask >> 16) & 0x00000000000000ff);
	oxm->mask[2] = (uint8_t)((mask >> 24) & 0x00000000000000ff);
	oxm->mask[1] = (uint8_t)((mask >> 32) & 0x00000000000000ff);
	oxm->mask[0] = (uint8_t)((mask >> 40) & 0x00000000000000ff);
}


uint64_t
coxmatch_48_exp::get_u48value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 6*sizeof(uint8_t))
		throw eOxmBadLen("coxmatch::get_u48value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
	return (((uint64_t)oxm->value[0] << 40) | ((uint64_t)oxm->value[1] << 32) | ((uint64_t)oxm->value[2] << 24) |
			((uint64_t)oxm->value[3] << 16) | ((uint64_t)oxm->value[4] <<  8) | ((uint64_t)oxm->value[5] <<  0));
}


uint64_t
coxmatch_48_exp::get_u48mask() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 12*sizeof(uint8_t))
		return __UINT64_C(0xffffffffffff);
	struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
	return (((uint64_t)oxm->mask[0] << 40) | ((uint64_t)oxm->mask[1] << 32) | ((uint64_t)oxm->mask[2] << 24) |
			((uint64_t)oxm->mask[3] << 16) | ((uint64_t)oxm->mask[4] <<  8) | ((uint64_t)oxm->mask[5] <<  0));
}


uint64_t
coxmatch_48_exp::get_u48masked_value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 12*sizeof(uint8_t))
		return get_u48value();
	return (get_u48value() & get_u48mask());
}

/*
 * 64 bits
 */
coxmatch_64_exp::coxmatch_64_exp(
		uint32_t oxm_id, uint32_t exp_id, uint64_t value, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_64BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 8*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u64value(value);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}


coxmatch_64_exp::coxmatch_64_exp(
		uint32_t oxm_id, uint32_t exp_id, uint64_t value, uint64_t mask, enum coxmatch_bit_t bits)
{
	switch (bits) {
	case COXMATCH_64BIT: {
		rofl::cmemory::resize(sizeof(struct ofp_oxm_experimenter_header) + 16*sizeof(uint8_t));
		set_oxm_id(oxm_id);
		set_oxm_exp_id(exp_id);
		set_u64value(value);
		set_u64mask(mask);
	} break;
	default: {
		throw eOxmInval("coxmatch::coxmatch() invalid OXM width");
	};
	}
}

void
coxmatch_64_exp::set_u64value(uint64_t value) {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + sizeof(uint64_t))
		throw eOxmBadLen("coxmatch::set_u64value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
	oxm->word = htobe64(value);
}


void
coxmatch_64_exp::set_u64mask(uint64_t mask) {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint64_t))
		throw eOxmBadLen("coxmatch::set_u64mask() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
	oxm->mask = htobe64(mask);
}


uint64_t
coxmatch_64_exp::get_u64value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + sizeof(uint64_t))
		throw eOxmBadLen("coxmatch::get_u64value() invalid length");
	struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
	return be64toh(oxm->word);
}


uint64_t
coxmatch_64_exp::get_u64mask() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint64_t))
		return 0xffffffffffffffffULL;
	struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
	return be64toh(oxm->mask);
}


uint64_t
coxmatch_64_exp::get_u64masked_value() const {
	if (memlen() < sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint64_t))
		return get_u64value();
	struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
	return be64toh((oxm->word & oxm->mask));
}


