/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmatch.h
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#ifndef COXMATCH_H_
#define COXMATCH_H_

#include <string>
#include <ostream>
#include <endian.h>
#include <pthread.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/exception.hpp"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"


namespace rofl {
namespace openflow {

/* error classes */
class eOxmBase : public exception {
public:
	eOxmBase(
			const std::string& __arg) :
				exception(__arg)
	{};
};
class eOxmInval : public eOxmBase {
public:
	eOxmInval(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};
class eOxmNotFound : public eOxmBase {
public:
	eOxmNotFound(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};
class eOxmBadLen : public eOxmBase {
public:
	eOxmBadLen(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};
class eOxmInvalType	: public eOxmBase {
public:
	eOxmInvalType(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};
class eOxmInvalClass : public eOxmBase {
public:
	eOxmInvalClass(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};
class eOxmHeaderInval : public eOxmBase {
public:
	eOxmHeaderInval(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};
class eOxmActionNotFound : public eOxmBase {
public:
	eOxmActionNotFound(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};
class eOxmBadExperimenter : public eOxmBase {
public:
	eOxmBadExperimenter(
			const std::string& __arg) :
				eOxmBase(__arg)
	{};
};








/**
 *
 */
class coxmatch {
public:

	/**
	 *
	 */
	virtual
	~coxmatch()
	{};

	/**
	 *
	 */
	coxmatch() :
		oxm_id(0)
	{};

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id) :
				oxm_id(oxm_id)
	{};

	/**
	 *
	 */
	coxmatch(
			const coxmatch& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch&
	operator= (
			const coxmatch& oxm)
	{
		if (this == &oxm)
			return *this;
		oxm_id = oxm.oxm_id;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const coxmatch& oxm) const
	{ return (oxm_id == oxm.oxm_id); };

	/**
	 *
	 */
	bool
	operator!= (
			const coxmatch& oxm) const
	{ return (oxm_id != oxm.oxm_id); };

	/**
	 *
	 */
	bool
	operator< (
			const coxmatch& oxm)
	{ return (oxm_id < oxm.oxm_id); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{ return sizeof(struct rofl::openflow::ofp_oxm_hdr); };

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch::pack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_tlv_hdr* oxm = (struct rofl::openflow::ofp_oxm_tlv_hdr*)buf;
		oxm->oxm_id = htobe32(oxm_id);
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		if (buflen < coxmatch::length()) {
			throw eOxmBadLen("coxmatch::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_tlv_hdr* oxm = (struct rofl::openflow::ofp_oxm_tlv_hdr*)buf;
		oxm_id = be32toh(oxm->oxm_id);
	};

public:

	/**
	 *
	 */
	void
	set_oxm_id(
			uint32_t oxm_id)
	{ this->oxm_id = oxm_id; };

	/**
	 *
	 */
	uint32_t
	get_oxm_id() const
	{ return oxm_id; };

public:

	/**
	 * @brief	Returns type (=class+field) without (hasmask+length), no shift
	 */
	uint32_t
	get_oxm_type() const
	{ return (oxm_id & 0xfffffe00); };

	/**
	 * @brief	Set type (=class+field) without (hasmask+length), no shift
	 */
	void
	set_oxm_type(
			uint32_t oxm_type)
	{ oxm_id = ((oxm_id & 0x000001ff) | (uint32_t)oxm_type); };

public:

	/**
	 *
	 */
	uint16_t
	get_oxm_class() const
	{ return ((oxm_id & 0xffff0000) >> 16); };

	/**
	 *
	 */
	void
	set_oxm_class(
			uint16_t oxm_class)
	{ oxm_id = ((oxm_id & 0x0000ffff) | ((uint32_t)oxm_class << 16)); };

	/**
	 *
	 */
	uint8_t
	get_oxm_field() const
	{ return ((oxm_id & 0x0000fe00) >> 9); };

	/**
	 *
	 */
	void
	set_oxm_field(
			uint8_t oxm_field)
	{ oxm_id = ((oxm_id & 0xffff01ff) | ((uint32_t)oxm_field << 9)); };

	/**
	 *
	 */
	bool
	get_oxm_hasmask() const
	{ return ((oxm_id & 0x00000100) >> 8); };

	/**
	 *
	 */
	void
	set_oxm_hasmask(
			bool hasmask)
	{ oxm_id = ((oxm_id & 0xfffffeff) | ((uint32_t)hasmask << 8)); };

	/**
	 *
	 */
	uint8_t
	get_oxm_length() const
	{ return (oxm_id & 0x000000ff); };

	/**
	 *
	 */
	void
	set_oxm_length(
			uint8_t oxm_len)
	{ oxm_id = ((oxm_id & 0xffffff00) | (uint32_t)oxm_len); };

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch& oxm) {
		os  << "<coxmatch ";
		os << "oxm_id: 0x" << std::hex << (unsigned int)oxm.get_oxm_id() << std::dec << " ";
		os << "class: 0x" << std::hex << (int)oxm.get_oxm_class() << std::dec << " ";
		os << "field: 0x" << std::hex << (int)oxm.get_oxm_field() << std::dec << " ";
		os << "hasmask: " << oxm.get_oxm_hasmask() << " ";
		os << "length: " << (int)oxm.get_oxm_length() << " ";
		os << ">" << std::endl;
		return os;
	};

private:

	uint32_t 		oxm_id;
};




class coxmatch_exp : public coxmatch {
public:

	/**
	 *
	 */
	virtual
	~coxmatch_exp()
	{};

	/**
	 *
	 */
	coxmatch_exp() :
		exp_id(0)
	{};

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{};

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint8_t value) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u8value(value); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint8_t value, uint8_t mask) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u8value(value); set_u8mask(mask); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint16_t value) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u16value(value); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint16_t value, uint16_t mask) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u16value(value); set_u16mask(mask); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint32_t value) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u32value(value); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint32_t value, uint32_t mask) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u32value(value); set_u32mask(mask); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, const rofl::caddress_ll& value) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u48value(value); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, const rofl::caddress_ll& value, const rofl::caddress_ll& mask) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u48value(value); set_u48mask(mask); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint64_t value) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u64value(value); };

	/**
	 *
	 */
	coxmatch_exp(
			uint32_t oxm_id, uint32_t exp_id, uint64_t value, uint64_t mask) :
				coxmatch(oxm_id),
				exp_id(exp_id)
	{ set_u64value(value); set_u64mask(mask); };

	/**
	 *
	 */
	coxmatch_exp(
			const coxmatch_exp& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_exp&
	operator= (
			const coxmatch_exp& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		exp_id = oxm.exp_id;
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		if (get_oxm_hasmask() && ((value.length() != mask.length()))) {
			throw eOxmInval("coxmatch_exp::length() value and mask differ in size");
		}
		return (sizeof(struct rofl::openflow::ofp_oxm_experimenter_header) + value.length() + mask.length());
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		if (get_oxm_hasmask() && ((value.length() != mask.length()))) {
			throw eOxmInval("coxmatch_exp::pack() value and mask differ in size");
		}

		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_exp::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_experimenter_header* oxm = (struct rofl::openflow::ofp_oxm_experimenter_header*)buf;
		oxm->experimenter = htobe32(exp_id);
		if (not value.empty()) {
			memcpy(oxm->data, value.somem(), value.length());
		}
		if (get_oxm_hasmask() && (not mask.empty())) {
			memcpy(oxm->data + value.length(), mask.somem(), mask.length());
		}
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		value.clear();
		mask.clear();

		coxmatch::unpack(buf, buflen);
		if (buflen < sizeof(struct rofl::openflow::ofp_oxm_experimenter_header)) {
			throw eOxmBadLen("coxmatch_exp::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_experimenter_header* oxm = (struct rofl::openflow::ofp_oxm_experimenter_header*)buf;
		exp_id = be32toh(oxm->experimenter);
		if (buflen < (sizeof(struct rofl::openflow::ofp_oxm_hdr) + oxm->oxm_header.oxm_length)) {
			throw eOxmBadLen("coxmatch_exp::unpack() invalid oxm_length field");
		}
		size_t bodylen = oxm->oxm_header.oxm_length - /*sizeof(exp_id)=*/sizeof(uint32_t);

		/* mask flag is false => single value, no mask */
		if (not get_oxm_hasmask()) {
			value.assign(oxm->data, bodylen);

		/* mask flag is true => value and mask */
		} else {

			/* sanity check: length must be an even value */
			if (bodylen % 2) {
				throw eOxmBadLen("coxmatch_exp::unpack() has_mask is true, but data length cannot be divided by 2");
			}

			size_t len = bodylen / 2;
			value.assign(oxm->data, len);
			mask.assign(oxm->data + len, len);
		}
	};

public:

	/**
	 *
	 */
	uint32_t
	get_oxm_exp_id() const
	{ return exp_id; };

	/**
	 *
	 */
	void
	set_oxm_exp_id(
			uint32_t exp_id)
	{ this->exp_id = exp_id; };

public:

	const rofl::cmemory&
	get_value() const
	{ return value; };

	coxmatch_exp&
	set_value(
			const rofl::cmemory& value)
	{ this->value = value; adjust_oxm_length(); return *this; };

	const rofl::cmemory&
	get_mask() const
	{ return mask; };

	coxmatch_exp&
	set_mask(
			const rofl::cmemory& mask)
	{ this->mask = mask; set_oxm_hasmask(true); adjust_oxm_length(); return *this; };

public:

	uint8_t
	get_u8masked_value() const
	{ return (get_u8value() & get_u8mask()); };

	uint16_t
	get_u16masked_value() const
	{ return (get_u16value() & get_u16mask()); };

	uint32_t
	get_u32masked_value() const
	{ return (get_u32value() & get_u32mask()); };

	rofl::caddress_ll
	get_u48masked_value() const
	{ return (get_u48value() & get_u48mask()); };

	uint64_t
	get_u64masked_value() const
	{ return (get_u64value() & get_u64mask()); };

public:

	uint8_t
	get_u8value() const
	{
		if (value.length() < sizeof(uint8_t))
			throw eOxmInval("coxmatch_exp::get_u8value() body too short");
		return *((uint8_t*)(value.somem()));
	};

	coxmatch_exp&
	set_u8value(
			uint8_t u8value)
	{
		value.resize(sizeof(uint8_t));
		uint8_t tmp = u8value;
		memcpy(value.somem(), (uint8_t*)&tmp, sizeof(tmp));
		adjust_oxm_length();
		return *this;
	};

	uint8_t
	get_u8mask() const
	{
		if (mask.length() < sizeof(uint8_t))
			return 0xff;
		return *((uint8_t*)(mask.somem()));
	};

	coxmatch_exp&
	set_u8mask(
			uint8_t u8mask)
	{
		mask.resize(sizeof(uint8_t));
		uint8_t tmp = u8mask;
		memcpy(mask.somem(), (uint8_t*)&tmp, sizeof(tmp));
		set_oxm_hasmask(true);
		adjust_oxm_length();
		return *this;
	};

public:

	uint16_t
	get_u16value() const
	{
		if (value.length() < sizeof(uint16_t))
			throw eOxmInval("coxmatch_exp::get_u16value() body too short");
		return be16toh(*((uint16_t*)(value.somem())));
	};

	coxmatch_exp&
	set_u16value(
			uint16_t u16value)
	{
		value.resize(sizeof(uint16_t));
		uint16_t tmp = htobe16(u16value);
		memcpy(value.somem(), (uint8_t*)&tmp, sizeof(tmp));
		adjust_oxm_length();
		return *this;
	};

	uint16_t
	get_u16mask() const
	{
		if (mask.length() < sizeof(uint16_t))
			return 0xffff;
		return be16toh(*((uint16_t*)(mask.somem())));
	};

	coxmatch_exp&
	set_u16mask(
			uint16_t u16mask)
	{
		mask.resize(sizeof(uint16_t));
		uint16_t tmp = htobe16(u16mask);
		memcpy(mask.somem(), (uint8_t*)&tmp, sizeof(tmp));
		set_oxm_hasmask(true);
		adjust_oxm_length();
		return *this;
	};

public:

	uint32_t
	get_u32value() const
	{
		if (value.length() < sizeof(uint32_t))
			throw eOxmInval("coxmatch_exp::get_u32value() body too short");
		return be32toh(*((uint32_t*)(value.somem())));
	};

	coxmatch_exp&
	set_u32value(
			uint32_t u32value)
	{
		value.resize(sizeof(uint32_t));
		uint32_t tmp = htobe32(u32value);
		memcpy(value.somem(), (uint8_t*)&tmp, sizeof(tmp));
		adjust_oxm_length();
		return *this;
	};

	uint32_t
	get_u32mask() const
	{
		if (mask.length() < sizeof(uint32_t))
			return 0xffffffff;
		return be32toh(*((uint32_t*)(mask.somem())));
	};

	coxmatch_exp&
	set_u32mask(
			uint32_t u32mask)
	{
		mask.resize(sizeof(uint32_t));
		uint32_t tmp = htobe32(u32mask);
		memcpy(mask.somem(), (uint8_t*)&tmp, sizeof(tmp));
		set_oxm_hasmask(true);
		adjust_oxm_length();
		return *this;
	};

public:

	rofl::caddress_ll
	get_u48value() const
	{
		if (value.length() < OFP_ETH_ALEN)
			throw eOxmInval("coxmatch_exp::get_u48value() body too short");
		return rofl::caddress_ll(value.somem(), OFP_ETH_ALEN);
	};

	coxmatch_exp&
	set_u48value(
			const rofl::caddress_ll& lladdr)
	{
		value.assign(lladdr.somem(), lladdr.length());
		adjust_oxm_length();
		return *this;
	};

	rofl::caddress_ll
	get_u48mask() const
	{
		if (mask.length() < OFP_ETH_ALEN)
			return rofl::caddress_ll("ff:ff:ff:ff:ff:ff");
		return rofl::caddress_ll(mask.somem(), OFP_ETH_ALEN);
	};

	coxmatch_exp&
	set_u48mask(
			const rofl::caddress_ll& lladdr)
	{
		mask.assign(lladdr.somem(), lladdr.length());
		set_oxm_hasmask(true);
		adjust_oxm_length();
		return *this;
	};

public:

	uint64_t
	get_u64value() const
	{
		if (value.length() < sizeof(uint64_t))
			throw eOxmInval("coxmatch_exp::get_u64value() body too short");
		return be64toh(*((uint64_t*)(value.somem())));
	};

	coxmatch_exp&
	set_u64value(
			uint64_t u64value)
	{
		value.resize(sizeof(uint64_t));
		uint64_t tmp = htobe64(u64value);
		memcpy(value.somem(), (uint8_t*)&tmp, sizeof(tmp));
		adjust_oxm_length();
		return *this;
	};

	uint64_t
	get_u64mask() const
	{
		if (mask.length() < sizeof(uint64_t))
			return 0xffffffffffffffff;
		return be64toh(*((uint64_t*)(mask.somem())));
	};

	coxmatch_exp&
	set_u64mask(
			uint64_t u64mask)
	{
		mask.resize(sizeof(uint64_t));
		uint64_t tmp = htobe64(u64mask);
		memcpy(mask.somem(), (uint8_t*)&tmp, sizeof(tmp));
		set_oxm_hasmask(true);
		adjust_oxm_length();
		return *this;
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_exp& oxm) {
		os  << "<coxmatch_exp ";
		os << "exp_id: 0x" << std::hex << oxm.get_oxm_exp_id() << std::dec << " ";
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		
		os << "<value: >" << std::endl << oxm.get_value();
		os << "<mask: >" << std::endl << oxm.get_mask();
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(
					sizeof(uint32_t) + value.length() + mask.length());
		} else {
			coxmatch::set_oxm_length(
					sizeof(uint32_t) + value.length());
		}
	};

private:

	uint32_t		exp_id;
	rofl::cmemory   value;
	rofl::cmemory   mask;
};



class coxmatch_8 :
		public coxmatch
{
public:

	/**
	 *
	 */
	virtual
	~coxmatch_8()
	{};

	/**
	 *
	 */
	coxmatch_8(
			uint32_t oxm_id) :
		coxmatch(oxm_id),
		value(0),
		mask(0xff)
	{};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_8(
			uint32_t oxm_id, uint8_t value) :
				coxmatch(oxm_id),
				value(value),
				mask(0xff)
	{};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_8(
			uint32_t oxm_id, uint8_t value, uint8_t mask) :
				coxmatch(oxm_id),
				value(value),
				mask(mask)
	{ set_oxm_hasmask(true); };

	/**
	 *
	 */
	coxmatch_8(
			const coxmatch_8& oxm) :
				coxmatch(oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_8&
	operator= (
			const coxmatch_8& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	uint8_t
	get_u8value() const
	{ return value; };

	coxmatch_8&
	set_u8value(
			uint8_t value)
	{ this->value = value; return *this; };

	uint8_t
	get_u8mask() const
	{ return mask; };

	coxmatch_8&
	set_u8mask(
			uint8_t mask)
	{ this->mask = mask; set_oxm_hasmask(true); return *this; };

public:

	uint8_t
	get_u8masked_value() const
	{ return (value & mask); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		return sizeof(struct rofl::openflow::ofp_oxm_hdr) +
			(get_oxm_hasmask() ? 2*sizeof(uint8_t) : 1*sizeof(uint8_t));
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		adjust_oxm_length();
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_8::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint8_t*)buf;
		oxm->byte = value;
		if (get_oxm_hasmask())
			oxm->mask = mask;
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		coxmatch::unpack(buf, buflen);
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_8::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint8_t*)buf;
		value = oxm->byte;
		mask = (get_oxm_hasmask() ? oxm->mask : 0xff);
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_8& oxm) {
		os  << "<coxmatch_8 ";
		os << "value: 0x" << std::hex << (int)oxm.get_u8value() << std::dec << " ";
		if (oxm.get_oxm_hasmask()) {
			os << "/ mask: 0x" << std::hex << (int)oxm.get_u8mask() << std::dec << " ";
		}
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(2*sizeof(uint8_t));
		} else {
			coxmatch::set_oxm_length(1*sizeof(uint8_t));
		}
	};

private:

	uint8_t value;
	uint8_t mask;
};



class coxmatch_16 :
		public coxmatch
{
public:

	/**
	 *
	 */
	virtual
	~coxmatch_16()
	{};

	/**
	 *
	 */
	coxmatch_16(
			uint32_t oxm_id) :
		coxmatch(oxm_id),
		value(0),
		mask(0xffff)
	{};

	/**
	 * @brief	coxmatch base class for single 16bit values
	 */
	coxmatch_16(
			uint32_t oxm_id, uint16_t value) :
				coxmatch(oxm_id),
				value(value),
				mask(0xffff)
	{};

	/**
	 * @brief	coxmatch base class for masked 16bit values
	 */
	coxmatch_16(
			uint32_t oxm_id, uint16_t value, uint16_t mask) :
				coxmatch(oxm_id),
				value(value),
				mask(mask)
	{ set_oxm_hasmask(true); };

	/**
	 *
	 */
	coxmatch_16(
			const coxmatch_16& oxm) :
				coxmatch(oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_16&
	operator= (
			const coxmatch_16& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	uint16_t
	get_u16value() const
	{ return value; };

	coxmatch_16&
	set_u16value(
			uint16_t value)
	{ this->value = value; return *this; };

	uint16_t
	get_u16mask() const
	{ return mask; };

	coxmatch_16&
	set_u16mask(
			uint16_t mask)
	{ this->mask = mask; set_oxm_hasmask(true); return *this; };

public:

	uint16_t
	get_u16masked_value() const
	{ return (value & mask); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		return sizeof(struct rofl::openflow::ofp_oxm_hdr) +
			(get_oxm_hasmask() ? 2*sizeof(uint16_t) : 1*sizeof(uint16_t));
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		adjust_oxm_length();
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_16::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint16_t*)buf;
		oxm->word = htobe16(value);
		if (get_oxm_hasmask())
			oxm->mask = htobe16(mask);
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		coxmatch::unpack(buf, buflen);
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_16::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint16_t*)buf;
		value = be16toh(oxm->word);
		mask = (get_oxm_hasmask() ? be16toh(oxm->mask) : 0xffff);
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_16& oxm) {
		os  << "<coxmatch_16 ";
		os << "value: 0x" << std::hex << oxm.get_u16value() << std::dec << " ";
		if (oxm.get_oxm_hasmask()) {
			os << "/ mask: 0x" << std::hex << oxm.get_u16mask() << std::dec << " ";
		}
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(2*sizeof(uint16_t));
		} else {
			coxmatch::set_oxm_length(1*sizeof(uint16_t));
		}
	};

private:

	uint16_t value;
	uint16_t mask;
};



class coxmatch_24 :
		public coxmatch
{
public:

	/**
	 *
	 */
	virtual
	~coxmatch_24()
	{};

	/**
	 *
	 */
	coxmatch_24(
			uint32_t oxm_id) :
		coxmatch(oxm_id),
		value(0),
		mask(0xffffff)
	{};

	/**
	 * @brief	coxmatch base class for single 24bit values
	 */
	coxmatch_24(
			uint32_t oxm_id, uint32_t value) :
				coxmatch(oxm_id),
				value(value),
				mask(0xffffff)
	{};

	/**
	 * @brief	coxmatch base class for masked 24bit values
	 */
	coxmatch_24(
			uint32_t oxm_id, uint32_t value, uint32_t mask) :
				coxmatch(oxm_id),
				value(value),
				mask(mask)
	{ set_oxm_hasmask(true); };

	/**
	 *
	 */
	coxmatch_24(
			const coxmatch_24& oxm) :
				coxmatch(oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_24&
	operator= (
			const coxmatch_24& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	uint32_t
	get_u24value() const
	{ return value; };

	coxmatch_24&
	set_u24value(
			uint32_t value)
	{ this->value = value; return *this; };

	uint32_t
	get_u24mask() const
	{ return mask; };

	coxmatch_24&
	set_u24mask(
			uint32_t mask)
	{ this->mask = mask; set_oxm_hasmask(true); return *this; };

public:

	uint32_t
	get_u24masked_value() const
	{ return (value & mask); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		return sizeof(struct rofl::openflow::ofp_oxm_hdr) +
			(get_oxm_hasmask() ? 2*3*sizeof(uint8_t) : 1*3*sizeof(uint8_t));
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		adjust_oxm_length();
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_24::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint24_t*)buf;
		oxm->word[2] = (uint8_t)((value >>  0) & 0x000000ff);
		oxm->word[1] = (uint8_t)((value >>  8) & 0x000000ff);
		oxm->word[0] = (uint8_t)((value >> 16) & 0x000000ff);
		if (get_oxm_hasmask()) {
			oxm->mask[2] = (uint8_t)((mask >>  0) & 0x000000ff);
			oxm->mask[1] = (uint8_t)((mask >>  8) & 0x000000ff);
			oxm->mask[0] = (uint8_t)((mask >> 16) & 0x000000ff);
		}
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		coxmatch::unpack(buf, buflen);
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_24::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint24_t*)buf;
		value = (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
		if (get_oxm_hasmask())
			mask = (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
		else
			mask = 0xffffff;
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_24& oxm) {
		os  << "<coxmatch_24 ";
		os << "value: 0x" << std::hex << oxm.get_u24value() << std::dec << " ";
		if (oxm.get_oxm_hasmask()) {
			os << "/ mask: 0x" << std::hex << oxm.get_u24mask() << std::dec << " ";
		}
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(2*3*sizeof(uint8_t));
		} else {
			coxmatch::set_oxm_length(1*3*sizeof(uint8_t));
		}
	};

private:

	uint32_t value;
	uint32_t mask;
};



class coxmatch_32 :
		public coxmatch
{
public:

	/**
	 *
	 */
	virtual
	~coxmatch_32()
	{};

	/**
	 *
	 */
	coxmatch_32(
			uint32_t oxm_id) :
		coxmatch(oxm_id),
		value(0),
		mask(0xffffffff)
	{};

	/**
	 * @brief	coxmatch base class for single 32bit values
	 */
	coxmatch_32(
			uint32_t oxm_id, uint32_t value) :
				coxmatch(oxm_id),
				value(value),
				mask(0xffffffff)
	{};

	/**
	 * @brief	coxmatch base class for masked 32bit values
	 */
	coxmatch_32(
			uint32_t oxm_id, uint32_t value, uint32_t mask) :
				coxmatch(oxm_id),
				value(value),
				mask(mask)
	{ set_oxm_hasmask(true); };

	/**
	 * @brief	coxmatch base class for single 32bit values
	 */
	coxmatch_32(
			uint32_t oxm_id, const rofl::caddress_in4& value) :
				coxmatch(oxm_id),
				value(0),
				mask(0xffffffff)
	{ set_u32value(value); };

	/**
	 * @brief	coxmatch base class for masked 32bit values
	 */
	coxmatch_32(
			uint32_t oxm_id, const rofl::caddress_in4& value, const rofl::caddress_in4& mask) :
				coxmatch(oxm_id),
				value(0),
				mask(0xffffffff)
	{ set_u32value(value); set_u32mask(mask); };

	/**
	 *
	 */
	coxmatch_32(
			const coxmatch_32& oxm) :
				coxmatch(oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_32&
	operator= (
			const coxmatch_32& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	uint32_t
	get_u32value() const
	{ return value; };

	coxmatch_32&
	set_u32value(
			uint32_t value)
	{ this->value = value; return *this; };

	uint32_t
	get_u32mask() const
	{ return mask; };

	coxmatch_32&
	set_u32mask(
			uint32_t mask)
	{ this->mask = mask; set_oxm_hasmask(true); return *this; };

public:

	uint32_t
	get_u32masked_value() const
	{ return (value & mask); };

public:

	rofl::caddress_in4
	get_u32value_as_addr() const
	{ caddress_in4 addr; addr.set_addr_hbo(value); return addr; };

	coxmatch_32&
	set_u32value(
			const rofl::caddress_in4& addr)
	{ value = addr.get_addr_hbo(); return *this; };

	rofl::caddress_in4
	get_u32mask_as_addr() const
	{ caddress_in4 addr; addr.set_addr_hbo(mask); return addr; };

	coxmatch_32&
	set_u32mask(
			const rofl::caddress_in4& addr)
	{ mask = addr.get_addr_hbo(); set_oxm_hasmask(true); return *this; };

public:

	rofl::caddress_in4
	get_u32masked_value_as_addr() const
	{ return (get_u32value_as_addr() & get_u32mask_as_addr()); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		return sizeof(struct rofl::openflow::ofp_oxm_hdr) +
			(get_oxm_hasmask() ? 2*sizeof(uint32_t) : 1*sizeof(uint32_t));
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		adjust_oxm_length();
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_32::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint32_t*)buf;
		oxm->dword = htobe32(value);
		if (get_oxm_hasmask())
			oxm->mask = htobe32(mask);
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		coxmatch::unpack(buf, buflen);
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_32::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint32_t*)buf;
		value = be32toh(oxm->dword);
		mask = (get_oxm_hasmask() ? be32toh(oxm->mask) : 0xffffffff);
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_32& oxm) {
		os  << "<coxmatch_32 ";
		os << "value: 0x" << std::hex << oxm.get_u32value() << std::dec << " ";
		if (oxm.get_oxm_hasmask()) {
			os << "/ mask: 0x" << std::hex << oxm.get_u32mask() << std::dec << " ";
		}
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(2*sizeof(uint32_t));
		} else {
			coxmatch::set_oxm_length(1*sizeof(uint32_t));
		}
	};

private:

	uint32_t value;
	uint32_t mask;
};



class coxmatch_48 :
		public coxmatch
{
public:

	/**
	 *
	 */
	virtual
	~coxmatch_48()
	{};

	/**
	 *
	 */
	coxmatch_48(
			uint32_t oxm_id) :
		coxmatch(oxm_id),
		value(0),
		mask(0xffffffffffff)
	{};

	/**
	 * @brief	coxmatch base class for single 48bit values
	 */
	coxmatch_48(
			uint32_t oxm_id, uint64_t value) :
				coxmatch(oxm_id),
				value(value),
				mask(0xffffffffffff)
	{};

	/**
	 * @brief	coxmatch base class for masked 48bit values
	 */
	coxmatch_48(
			uint32_t oxm_id, uint64_t value, uint64_t mask) :
				coxmatch(oxm_id),
				value(value),
				mask(mask)
	{ set_oxm_hasmask(true); };

	/**
	 * @brief	coxmatch base class for single 48bit values
	 */
	coxmatch_48(
			uint32_t oxm_id, const rofl::caddress_ll& value) :
				coxmatch(oxm_id),
				value(0),
				mask(0xffffffffffff)
	{ set_u48value(value); };

	/**
	 * @brief	coxmatch base class for masked 48bit values
	 */
	coxmatch_48(
			uint32_t oxm_id, const rofl::caddress_ll& value, const rofl::caddress_ll& mask) :
				coxmatch(oxm_id),
				value(0),
				mask(0xffffffffffff)
	{ set_u48value(value); set_u48mask(mask); };

	/**
	 *
	 */
	coxmatch_48(
			const coxmatch_48& oxm) :
				coxmatch(oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_48&
	operator= (
			const coxmatch_48& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	uint64_t
	get_u48value() const
	{ return value; };

	coxmatch_48&
	set_u48value(
			uint32_t value)
	{ this->value = value; return *this; };

	uint64_t
	get_u48mask() const
	{ return mask; };

	coxmatch_48&
	set_u48mask(
			uint32_t mask)
	{ this->mask = mask; set_oxm_hasmask(true); return *this; };

public:

	uint32_t
	get_u48masked_value() const
	{ return (value & mask); };

public:

	rofl::caddress_ll
	get_u48value_as_lladdr() const
	{
		rofl::caddress_ll lladdr;
		lladdr[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
		lladdr[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
		lladdr[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
		lladdr[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
		lladdr[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
		lladdr[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
		return lladdr;
	};

	coxmatch_48&
	set_u48value(
			const rofl::caddress_ll& lladdr)
	{
		value = (((uint64_t)lladdr[0] << 40) |
				 ((uint64_t)lladdr[1] << 32) |
				 ((uint64_t)lladdr[2] << 24) |
				 ((uint64_t)lladdr[3] << 16) |
				 ((uint64_t)lladdr[4] <<  8) |
				 ((uint64_t)lladdr[5] <<  0));
		return *this;
	};

	rofl::caddress_ll
	get_u48mask_as_lladdr() const
	{
		rofl::caddress_ll lladdr;
		lladdr[5] = (uint8_t)((mask >>  0) & 0x00000000000000ff);
		lladdr[4] = (uint8_t)((mask >>  8) & 0x00000000000000ff);
		lladdr[3] = (uint8_t)((mask >> 16) & 0x00000000000000ff);
		lladdr[2] = (uint8_t)((mask >> 24) & 0x00000000000000ff);
		lladdr[1] = (uint8_t)((mask >> 32) & 0x00000000000000ff);
		lladdr[0] = (uint8_t)((mask >> 40) & 0x00000000000000ff);
		return lladdr;
	};

	coxmatch_48&
	set_u48mask(
			const rofl::caddress_ll& lladdr)
	{
		mask = (((uint64_t)lladdr[0] << 40) |
				((uint64_t)lladdr[1] << 32) |
				((uint64_t)lladdr[2] << 24) |
				((uint64_t)lladdr[3] << 16) |
				((uint64_t)lladdr[4] <<  8) |
				((uint64_t)lladdr[5] <<  0));
		set_oxm_hasmask(true);
		return *this;
	};

public:

	rofl::caddress_ll
	get_u48masked_value_as_lladdr() const
	{ return (get_u48value_as_lladdr() & get_u48mask_as_lladdr()); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		return sizeof(struct rofl::openflow::ofp_oxm_hdr) +
			(get_oxm_hasmask() ? 2*6*sizeof(uint8_t) : 1*6*sizeof(uint8_t));
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		adjust_oxm_length();
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_48::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint48_t*)buf;
		oxm->value[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
		oxm->value[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
		oxm->value[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
		oxm->value[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
		oxm->value[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
		oxm->value[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
		if (get_oxm_hasmask()) {
			oxm->mask[5] = (uint8_t)((mask >>  0) & 0x00000000000000ff);
			oxm->mask[4] = (uint8_t)((mask >>  8) & 0x00000000000000ff);
			oxm->mask[3] = (uint8_t)((mask >> 16) & 0x00000000000000ff);
			oxm->mask[2] = (uint8_t)((mask >> 24) & 0x00000000000000ff);
			oxm->mask[1] = (uint8_t)((mask >> 32) & 0x00000000000000ff);
			oxm->mask[0] = (uint8_t)((mask >> 40) & 0x00000000000000ff);
		}
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		coxmatch::unpack(buf, buflen);
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_48::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint48_t*)buf;
		value = (((uint64_t)oxm->value[0] << 40) |
				 ((uint64_t)oxm->value[1] << 32) |
				 ((uint64_t)oxm->value[2] << 24) |
				 ((uint64_t)oxm->value[3] << 16) |
				 ((uint64_t)oxm->value[4] <<  8) |
				 ((uint64_t)oxm->value[5] <<  0));
		if (get_oxm_hasmask()) {
			mask = (((uint64_t)oxm->mask[0] << 40) |
					((uint64_t)oxm->mask[1] << 32) |
					((uint64_t)oxm->mask[2] << 24) |
					((uint64_t)oxm->mask[3] << 16) |
					((uint64_t)oxm->mask[4] <<  8) |
					((uint64_t)oxm->mask[5] <<  0));
		} else {
			mask = 0xffffffffffffffff;
		}
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_48& oxm) {
		os  << "<coxmatch_48 ";
		os << "value: 0x" << std::hex << oxm.get_u48value() << std::dec << " ";
		if (oxm.get_oxm_hasmask()) {
			os << "/ mask: 0x" << std::hex << oxm.get_u48mask() << std::dec << " ";
		}
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(2*6*sizeof(uint8_t));
		} else {
			coxmatch::set_oxm_length(1*6*sizeof(uint8_t));
		}
	};

private:

	uint64_t value;
	uint64_t mask;
};



class coxmatch_64 :
		public coxmatch
{
public:

	/**
	 *
	 */
	virtual
	~coxmatch_64()
	{};

	/**
	 *
	 */
	coxmatch_64(
			uint32_t oxm_id) :
		coxmatch(oxm_id),
		value(0),
		mask(0xffffffffffffffff)
	{};

	/**
	 * @brief	coxmatch base class for single 64bit values
	 */
	coxmatch_64(
			uint32_t oxm_id, uint64_t value) :
				coxmatch(oxm_id),
				value(value),
				mask(0xffffffffffffffff)
	{};

	/**
	 * @brief	coxmatch base class for masked 64bit values
	 */
	coxmatch_64(
			uint32_t oxm_id, uint64_t value, uint64_t mask) :
				coxmatch(oxm_id),
				value(value),
				mask(mask)
	{ set_oxm_hasmask(true); };

	/**
	 *
	 */
	coxmatch_64(
			const coxmatch_64& oxm) :
				coxmatch(oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_64&
	operator= (
			const coxmatch_64& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	uint64_t
	get_u64value() const
	{ return value; };

	coxmatch_64&
	set_u64value(
			uint64_t value)
	{ this->value = value; return *this; };

	uint64_t
	get_u64mask() const
	{ return mask; };

	coxmatch_64&
	set_u64mask(
			uint64_t mask)
	{ this->mask = mask; set_oxm_hasmask(true); return *this; };

public:

	uint64_t
	get_u64masked_value() const
	{ return (value & mask); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		return sizeof(struct rofl::openflow::ofp_oxm_hdr) +
			(get_oxm_hasmask() ? 2*sizeof(uint64_t) : 1*sizeof(uint64_t));
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		adjust_oxm_length();
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_64::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint64_t*)buf;
		oxm->word = htobe64(value);
		if (get_oxm_hasmask())
			oxm->mask = htobe64(mask);
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		coxmatch::unpack(buf, buflen);
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_64::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm = (struct rofl::openflow::ofp_oxm_ofb_uint64_t*)buf;
		value = be64toh(oxm->word);
		mask = (get_oxm_hasmask() ? be64toh(oxm->mask) : 0xffffffffffffffff);
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_64& oxm) {
		os  << "<coxmatch_64 ";
		os << "value: 0x" << std::hex << oxm.get_u64value() << std::dec << " ";
		if (oxm.get_oxm_hasmask()) {
			os << "/ mask: 0x" << std::hex << oxm.get_u64mask() << std::dec << " ";
		}
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(2*sizeof(uint64_t));
		} else {
			coxmatch::set_oxm_length(1*sizeof(uint64_t));
		}
	};

private:

	uint64_t value;
	uint64_t mask;
};



class coxmatch_128 :
		public coxmatch
{
public:

	/**
	 *
	 */
	virtual
	~coxmatch_128()
	{};

	/**
	 *
	 */
	coxmatch_128(
			uint32_t oxm_id) :
		coxmatch(oxm_id),
		mask("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
	{};

	/**
	 * @brief	coxmatch base class for single 128bit values
	 */
	coxmatch_128(
			uint32_t oxm_id, const rofl::caddress_in6& value) :
				coxmatch(oxm_id),
				value(value),
				mask("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff")
	{};

	/**
	 * @brief	coxmatch base class for masked 128bit values
	 */
	coxmatch_128(
			uint32_t oxm_id, const rofl::caddress_in6& value, const rofl::caddress_in6& mask) :
				coxmatch(oxm_id),
				value(value),
				mask(mask)
	{ set_oxm_hasmask(true); };

	/**
	 *
	 */
	coxmatch_128(
			const coxmatch_128& oxm) :
				coxmatch(oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_128&
	operator= (
			const coxmatch_128& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch::operator= (oxm);
		value = oxm.value;
		mask = oxm.mask;
		return *this;
	};

public:

	const rofl::caddress_in6&
	get_u128value() const
	{ return value; };

	coxmatch_128&
	set_u128value(
			const rofl::caddress_in6& value)
	{ this->value = value; return *this; };

	const rofl::caddress_in6&
	get_u128mask() const
	{ return mask; };

	coxmatch_128&
	set_u128mask(
			const rofl::caddress_in6& mask)
	{ this->mask = mask; set_oxm_hasmask(true); return *this; };

public:

	rofl::caddress_in6
	get_u128masked_value() const
	{ return (value & mask); };

public:

	/**
	 *
	 */
	virtual size_t
	length() const
	{
		return sizeof(struct rofl::openflow::ofp_oxm_hdr) +
			(get_oxm_hasmask() ? 2*6*sizeof(uint8_t) : 1*6*sizeof(uint8_t));
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		adjust_oxm_length();
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_128::pack() buf too short");
		}
		coxmatch::pack(buf, buflen);
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm = (struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)buf;
		memcpy(oxm->addr, value.somem(), 16);
		if (get_oxm_hasmask()) {
			memcpy(oxm->mask, mask.somem(), 16);
		}
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		coxmatch::unpack(buf, buflen);
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch_128::unpack() buf too short");
		}
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm = (struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)buf;
		memcpy(value.somem(), oxm->addr, 16);
		if (get_oxm_hasmask()) {
			memcpy(mask.somem(), oxm->mask, 16);
		}
	 };

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_128& oxm) {
		os  << "<coxmatch_128 ";
		os << "value: 0x" << std::hex << oxm.get_u128value().str() << std::dec << " ";
		if (oxm.get_oxm_hasmask()) {
			os << "/ mask: 0x" << std::hex << oxm.get_u128mask().str() << std::dec << " ";
		}
		os << " >" << std::endl;
		
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};

private:

	/**
	 *
	 */
	void
	adjust_oxm_length() {
		if (coxmatch::get_oxm_hasmask()) {
			coxmatch::set_oxm_length(2*16*sizeof(uint8_t));
		} else {
			coxmatch::set_oxm_length(1*16*sizeof(uint8_t));
		}
	};

private:

	rofl::caddress_in6 value;
	rofl::caddress_in6 mask;
};








/*
 *
 *
 * specific matches defined by OpenFlow
 *
 *
 *
 */





/**
 * @brief	OXM_OF_IN_PORT
 */
class coxmatch_ofb_in_port : public coxmatch_32 {
public:
	coxmatch_ofb_in_port() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IN_PORT)
	{};
	coxmatch_ofb_in_port(
			uint32_t port_no) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IN_PORT, port_no)
	{};
	coxmatch_ofb_in_port(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_in_port()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_in_port& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << "<in-port: 0x" << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IN_PHY_PORT
 */
class coxmatch_ofb_in_phy_port : public coxmatch_32 {
public:
	coxmatch_ofb_in_phy_port() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT)
	{};
	coxmatch_ofb_in_phy_port(
			uint32_t port_no) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT, port_no)
	{};
	coxmatch_ofb_in_phy_port(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_in_phy_port()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_in_phy_port& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << "<in-phy-port: 0x" << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_METADATA
 */
class coxmatch_ofb_metadata : public coxmatch_64 {
public:
	coxmatch_ofb_metadata() :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_METADATA)
	{};
	coxmatch_ofb_metadata(
			uint64_t metadata) :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_METADATA, metadata)
	{};
	coxmatch_ofb_metadata(
			uint64_t metadata, uint64_t mask) :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_METADATA_MASK, metadata, mask)
	{};
	coxmatch_ofb_metadata(
			const coxmatch_64& oxm) :
				coxmatch_64(oxm)
	{};
	virtual
	~coxmatch_ofb_metadata()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_metadata& oxm) {
		os << dynamic_cast<const coxmatch_64&>(oxm);
		os << std::hex;
		os << "<metadata: ";
		os << "0x" << (unsigned long long)oxm.get_u64value() << " / ";
		os << "0x" << (unsigned long long)oxm.get_u64mask() << " ";
		os << std::dec << ">" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ETH_DST
 */
class coxmatch_ofb_eth_dst : public coxmatch_48 {
public:
	coxmatch_ofb_eth_dst() :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_DST)
	{};
	coxmatch_ofb_eth_dst(
			const rofl::caddress_ll& maddr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_DST, maddr)
	{};
	coxmatch_ofb_eth_dst(
			const rofl::caddress_ll& maddr, const rofl::caddress_ll& mask) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK, maddr, mask)
	{};
	coxmatch_ofb_eth_dst(
			const coxmatch_48& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_dst& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << "<eth-dst: "
					<< oxm.get_u48value_as_lladdr().str() << "/"
					<< oxm.get_u48mask_as_lladdr().str()
					<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ETH_SRC
 */
class coxmatch_ofb_eth_src : public coxmatch_48 {
public:
	coxmatch_ofb_eth_src() :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_SRC)
	{};
	coxmatch_ofb_eth_src(
			const rofl::caddress_ll& maddr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_SRC, maddr)
	{};
	coxmatch_ofb_eth_src(
			const rofl::caddress_ll& maddr, const rofl::caddress_ll& mask) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK, maddr, mask)
	{};
	coxmatch_ofb_eth_src(
			const coxmatch_48& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_src& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << "<eth-src: "
					<< oxm.get_u48value_as_lladdr().str() << "/"
					<< oxm.get_u48mask_as_lladdr().str()
					<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ETH_TYPE
 */
class coxmatch_ofb_eth_type : public coxmatch_16 {
public:
	coxmatch_ofb_eth_type() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE)
	{};
	coxmatch_ofb_eth_type(
			uint16_t dl_type) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE, dl_type)
	{};
	coxmatch_ofb_eth_type(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_type& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<eth-type: 0x" << std::hex << (int)oxm.get_u16value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_vid : public coxmatch_16 {
public:
	coxmatch_ofb_vlan_vid() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_VLAN_VID)
	{};
	coxmatch_ofb_vlan_vid(
			uint16_t vid) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_VLAN_VID, vid)
	{};
	coxmatch_ofb_vlan_vid(
			uint16_t vid, uint16_t mask) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK, vid, mask)
	{};
	coxmatch_ofb_vlan_vid(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_vid()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_vid& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os  << "<coxmatch_ofb_vlan_vid >" << std::endl;
		os  << "<vlan-vid: "
				<< "0x" << std::hex << (int)oxm.get_u16value() << std::dec << " / "
				<< "0x" << std::hex << (int)oxm.get_u16mask() << std::dec << " "
				<< ">" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_untagged : public coxmatch_16 {
public:
	coxmatch_ofb_vlan_untagged() :
		coxmatch_16(rofl::openflow::OXM_TLV_BASIC_VLAN_VID, (uint16_t)rofl::openflow::OFPVID_NONE)
	{};
	coxmatch_ofb_vlan_untagged(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_untagged()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_untagged& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os  << "<vlan-untagged >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_present : public coxmatch_16 {
public:
	coxmatch_ofb_vlan_present() :
		coxmatch_16(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK,
				(uint16_t)rofl::openflow::OFPVID_PRESENT,
				(uint16_t)rofl::openflow::OFPVID_PRESENT)
	{};
	coxmatch_ofb_vlan_present(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_present()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_present& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os  << "<vlan-present >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_PCP
 */
class coxmatch_ofb_vlan_pcp : public coxmatch_8 {
public:
	coxmatch_ofb_vlan_pcp() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)
	{};
	coxmatch_ofb_vlan_pcp(
			uint8_t pcp) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP, pcp)
	{};
	coxmatch_ofb_vlan_pcp(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_pcp()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_pcp& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<vlan-pcp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_DSCP
 */
class coxmatch_ofb_ip_dscp : public coxmatch_8 {
public:
	coxmatch_ofb_ip_dscp() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_DSCP)
	{};
	coxmatch_ofb_ip_dscp(
			uint8_t dscp) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_DSCP, dscp)
	{};
	coxmatch_ofb_ip_dscp(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_dscp()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_dscp& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<ip-dscp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_ECN
 */
class coxmatch_ofb_ip_ecn : public coxmatch_8 {
public:
	coxmatch_ofb_ip_ecn() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_ECN)
	{};
	coxmatch_ofb_ip_ecn(
			uint8_t ecn) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_ECN, ecn)
	{};
	coxmatch_ofb_ip_ecn(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_ecn()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_ecn& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<ip-ecn: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_PROTO
 */
class coxmatch_ofb_ip_proto : public coxmatch_8 {
public:
	coxmatch_ofb_ip_proto() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)
	{};
	coxmatch_ofb_ip_proto(
			uint8_t proto) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_PROTO, proto)
	{};
	coxmatch_ofb_ip_proto(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_proto()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_proto& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<ip-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV4_SRC
 */
class coxmatch_ofb_ipv4_src : public coxmatch_32 {
public:
	coxmatch_ofb_ipv4_src() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC)
	{};
	coxmatch_ofb_ipv4_src(
			uint32_t src) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src)
	{};
	coxmatch_ofb_ipv4_src(
			uint32_t src, uint32_t mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv4_src(
			const rofl::caddress_in4& src) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src)
	{};
	coxmatch_ofb_ipv4_src(
			const rofl::caddress_in4& src, const rofl::caddress_in4& mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv4_src(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_src& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << "<ipv4-src: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV4_DST
 */
class coxmatch_ofb_ipv4_dst : public coxmatch_32 {
public:
	coxmatch_ofb_ipv4_dst() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_DST)
	{};
	coxmatch_ofb_ipv4_dst(
			uint32_t dst) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst)
	{};
	coxmatch_ofb_ipv4_dst(
			uint32_t dst, uint32_t mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv4_dst(
			const caddress_in4& dst) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst)
	{};
	coxmatch_ofb_ipv4_dst(
			const caddress_in4& dst, const caddress_in4& mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv4_dst(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_dst& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << "<ipv4-dst: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_SRC
 */
class coxmatch_ofb_ipv6_src : public coxmatch_128 {
public:
	coxmatch_ofb_ipv6_src() :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC)
	{};
	coxmatch_ofb_ipv6_src(
			const rofl::caddress_in6& src) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC, src)
	{};
	coxmatch_ofb_ipv6_src(
			const rofl::caddress_in6& src, const rofl::caddress_in6& mask) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv6_src(
			const coxmatch_128& oxm) :
				coxmatch_128(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_src& oxm) {
		os << dynamic_cast<const coxmatch_128&>(oxm);
		os << "<ipv6-src: " << oxm.get_u128value().str() << "/";
		os << oxm.get_u128mask().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_DST
 */
class coxmatch_ofb_ipv6_dst : public coxmatch_128 {
public:
	coxmatch_ofb_ipv6_dst() :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_DST)
	{};
	coxmatch_ofb_ipv6_dst(
			const rofl::caddress_in6& dst) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_DST, dst)
	{};
	coxmatch_ofb_ipv6_dst(
			const rofl::caddress_in6& dst, const rofl::caddress_in6& mask) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv6_dst(
			const coxmatch_128& oxm) :
				coxmatch_128(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_dst& oxm) {
		os << dynamic_cast<const coxmatch_128&>(oxm);
		os << "<ipv6-dst: " << oxm.get_u128value().str() << "/";
		os << oxm.get_u128mask().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_ND_TARGET
 */
class coxmatch_ofb_ipv6_nd_target : public coxmatch_128 {
public:
	coxmatch_ofb_ipv6_nd_target() :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET)
	{};
	coxmatch_ofb_ipv6_nd_target(
			const rofl::caddress_in6& nd_target) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET, nd_target)
	{};
	coxmatch_ofb_ipv6_nd_target(
			const coxmatch_128& oxm) :
				coxmatch_128(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_target()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_target& oxm) {
		os << dynamic_cast<const coxmatch_128&>(oxm);
		os  << "<ipv6-nd-target: " << oxm.get_u128value().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TCP_SRC
 */
class coxmatch_ofb_tcp_src : public coxmatch_16 {
public:
	coxmatch_ofb_tcp_src() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_TCP_SRC)
	{};
	coxmatch_ofb_tcp_src(
			uint16_t src) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_TCP_SRC, src)
	{};
	coxmatch_ofb_tcp_src(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_tcp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tcp_src& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<tcp-src: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TCP_DST
 */
class coxmatch_ofb_tcp_dst : public coxmatch_16 {
public:
	coxmatch_ofb_tcp_dst() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_TCP_DST)
	{};
	coxmatch_ofb_tcp_dst(
			uint16_t dst) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_TCP_DST, dst)
	{};
	coxmatch_ofb_tcp_dst(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_tcp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tcp_dst& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<tcp-dst: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_UDP_SRC
 */
class coxmatch_ofb_udp_src : public coxmatch_16 {
public:
	coxmatch_ofb_udp_src() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_UDP_SRC)
	{};
	coxmatch_ofb_udp_src(
			uint16_t src) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_UDP_SRC, src)
	{};
	coxmatch_ofb_udp_src(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_udp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_udp_src& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<udp-src: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_UDP_DST
 */
class coxmatch_ofb_udp_dst : public coxmatch_16 {
public:
	coxmatch_ofb_udp_dst() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_UDP_DST)
	{};
	coxmatch_ofb_udp_dst(
			uint16_t dst) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_UDP_DST, dst)
	{};
	coxmatch_ofb_udp_dst(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_udp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_udp_dst& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<udp-dst: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_SCTP_SRC
 */
class coxmatch_ofb_sctp_src : public coxmatch_16 {
public:
	coxmatch_ofb_sctp_src() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC)
	{};
	coxmatch_ofb_sctp_src(
			uint16_t src) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC, src)
	{};
	coxmatch_ofb_sctp_src(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_sctp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_sctp_src& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<sctp-src: " << (int)oxm.get_u16value()	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_SCTP_DST
 */
class coxmatch_ofb_sctp_dst : public coxmatch_16 {
public:
	coxmatch_ofb_sctp_dst() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_SCTP_DST)
	{};
	coxmatch_ofb_sctp_dst(
			uint16_t dst) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_SCTP_DST, dst)
	{};
	coxmatch_ofb_sctp_dst(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_sctp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_sctp_dst& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<sctp-dst: " << (int)oxm.get_u16value()	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV4_TYPE
 */
class coxmatch_ofb_icmpv4_type : public coxmatch_8 {
public:
	coxmatch_ofb_icmpv4_type() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE)
	{};
	coxmatch_ofb_icmpv4_type(
			uint8_t type) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE, type)
	{};
	coxmatch_ofb_icmpv4_type(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv4_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv4_type& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<icmpv4-type: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV4_CODE
 */
class coxmatch_ofb_icmpv4_code : public coxmatch_8 {
public:
	coxmatch_ofb_icmpv4_code() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE)
	{};
	coxmatch_ofb_icmpv4_code(
			uint8_t code) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE, code)
	{};
	coxmatch_ofb_icmpv4_code(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv4_code()
	{};
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv4_code& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<icmpv4-code: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ARP_OP
 */
class coxmatch_ofb_arp_opcode : public coxmatch_16 {
public:
	coxmatch_ofb_arp_opcode() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_ARP_OP)
	{};
	coxmatch_ofb_arp_opcode(
			uint16_t opcode) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_ARP_OP, opcode)
	{};
	coxmatch_ofb_arp_opcode(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_opcode()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_opcode& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<arp-opcode: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_SPA
 */
class coxmatch_ofb_arp_spa : public coxmatch_32 {
public:
	coxmatch_ofb_arp_spa() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_SPA)
	{};
	coxmatch_ofb_arp_spa(
			uint32_t spa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa)
	{};
	coxmatch_ofb_arp_spa(
			uint32_t spa, uint32_t mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK, spa, mask)
	{};
	coxmatch_ofb_arp_spa(
			const caddress_in4& spa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa)
	{};
	coxmatch_ofb_arp_spa(
			const caddress_in4& spa, const caddress_in4& mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK, spa, mask)
	{};
	coxmatch_ofb_arp_spa(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_spa()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_spa& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << "<arp-spa: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_TPA
 */
class coxmatch_ofb_arp_tpa : public coxmatch_32 {
public:
	coxmatch_ofb_arp_tpa() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_TPA)
	{};
	coxmatch_ofb_arp_tpa(
			uint32_t tpa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa)
	{};
	coxmatch_ofb_arp_tpa(
			uint32_t tpa, uint32_t mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK, tpa, mask)
	{};
	coxmatch_ofb_arp_tpa(
			const caddress_in4& tpa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa)
	{};
	coxmatch_ofb_arp_tpa(
			const caddress_in4& tpa, const caddress_in4& mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK, tpa, mask)
	{};
	coxmatch_ofb_arp_tpa(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tpa()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tpa& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << "<arp-tpa: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_SHA
 */
class coxmatch_ofb_arp_sha : public coxmatch_48 {
public:
	coxmatch_ofb_arp_sha() :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_SHA)
	{};
	coxmatch_ofb_arp_sha(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, lladdr)
	{};
	coxmatch_ofb_arp_sha(
			uint64_t lladdr, uint64_t llmask) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_sha(
			const cmacaddr& lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, lladdr)
	{};

	coxmatch_ofb_arp_sha(
			const cmacaddr& lladdr, const cmacaddr& llmask) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_sha(
			const coxmatch_48& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_sha()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_sha& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << "<arp-sha: "
				<< oxm.get_u48value_as_lladdr().str() << "/"
				<< oxm.get_u48mask_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_THA
 */
class coxmatch_ofb_arp_tha : public coxmatch_48 {
public:
	coxmatch_ofb_arp_tha() :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_THA)
	{};
	coxmatch_ofb_arp_tha(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_THA, lladdr)
	{};
	coxmatch_ofb_arp_tha(
			uint64_t lladdr, uint64_t llmask) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_tha(
			const cmacaddr& lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_THA, lladdr)
	{};
	coxmatch_ofb_arp_tha(
			const rofl::caddress_ll& lladdr, const rofl::caddress_ll& llmask) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_tha(
			const coxmatch_48& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tha()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tha& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << "<arp-tha: "
				<< oxm.get_u48value_as_lladdr().str() << "/"
				<< oxm.get_u48mask_as_lladdr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_FLABEL
 */
class coxmatch_ofb_ipv6_flabel : public coxmatch_32 {
public:
	coxmatch_ofb_ipv6_flabel() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL)
	{};
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL, flow_label)
	{};
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label, uint32_t mask) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL_MASK, flow_label, mask)
	{};
	coxmatch_ofb_ipv6_flabel(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_flabel()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_flabel& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << "<ipv6-flabel: 0x" << std::hex
						<< (int)oxm.get_u32value() << "/0x" << (int)oxm.get_u32mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ICMPV6_TYPE
 */
class coxmatch_ofb_icmpv6_type : public coxmatch_8 {
public:
	coxmatch_ofb_icmpv6_type() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE)
	{};
	coxmatch_ofb_icmpv6_type(
			uint8_t type) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE, type)
	{};
	coxmatch_ofb_icmpv6_type(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv6_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv6_type& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<icmpv6-type: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV6_CODE
 */
class coxmatch_ofb_icmpv6_code : public coxmatch_8 {
public:
	coxmatch_ofb_icmpv6_code() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE)
	{};
	coxmatch_ofb_icmpv6_code(
			uint8_t code) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE, code)
	{};
	coxmatch_ofb_icmpv6_code(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv6_code()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv6_code& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<icmpv6-code: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_ND_SLL
 */
class coxmatch_ofb_ipv6_nd_sll : public coxmatch_48 {
public:
	coxmatch_ofb_ipv6_nd_sll() :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL)
	{};
	coxmatch_ofb_ipv6_nd_sll(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL, lladdr)
	{};
	coxmatch_ofb_ipv6_nd_sll(
			const cmacaddr& addr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL, addr)
	{};
	coxmatch_ofb_ipv6_nd_sll(
			const coxmatch_48& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_sll()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_sll& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << "<ipv6-nd-sll: "
				<< oxm.get_u48value_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_ND_TLL
 */
class coxmatch_ofb_ipv6_nd_tll : public coxmatch_48 {
public:
	coxmatch_ofb_ipv6_nd_tll() :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL)
	{};
	coxmatch_ofb_ipv6_nd_tll(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL, lladdr)
	{};
	coxmatch_ofb_ipv6_nd_tll(
			const cmacaddr& addr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL, addr)
	{};
	coxmatch_ofb_ipv6_nd_tll(
			const coxmatch_48& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_tll()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_tll& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << "<ipv6-nd-tll: "
				<< oxm.get_u48value_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_LABEL
 */
class coxmatch_ofb_mpls_label : public coxmatch_32 {
public:
	coxmatch_ofb_mpls_label() :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL)
	{};
	coxmatch_ofb_mpls_label(
			uint32_t mpls_label) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL, mpls_label)
	{};
	coxmatch_ofb_mpls_label(
			const coxmatch_32& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_label()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_label& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << "<mpls-label: 0x" << std::hex << (int)oxm.get_u32value() << std::dec	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_TC
 */
class coxmatch_ofb_mpls_tc : public coxmatch_8 {
public:
	coxmatch_ofb_mpls_tc() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_MPLS_TC)
	{};
	coxmatch_ofb_mpls_tc(
			uint8_t mpls_tc) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_MPLS_TC, mpls_tc)
	{};
	coxmatch_ofb_mpls_tc(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_tc()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_tc& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<mpls-tc: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_BOS
 */
class coxmatch_ofb_mpls_bos : public coxmatch_8 {
public:
	coxmatch_ofb_mpls_bos() :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS)
	{};
	coxmatch_ofb_mpls_bos(
			uint8_t mpls_bos) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS, mpls_bos)
	{};
	coxmatch_ofb_mpls_bos(
			const coxmatch_8& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_bos()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_bos& oxm) {
		os << dynamic_cast<const coxmatch_8&>(oxm);
		os << "<mpls-bos: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_TUNNEL_ID
 */
class coxmatch_ofb_tunnel_id : public coxmatch_64 {
public:
	coxmatch_ofb_tunnel_id() :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID)
	{};
	coxmatch_ofb_tunnel_id(
			uint64_t tunnel_id) :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID, tunnel_id)
	{};
	coxmatch_ofb_tunnel_id(
			uint64_t tunnel_id, uint64_t mask) :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID_MASK, tunnel_id, mask)
	{};
	coxmatch_ofb_tunnel_id(
			const coxmatch_64& oxm) :
				coxmatch_64(oxm)
	{};
	virtual
	~coxmatch_ofb_tunnel_id()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tunnel_id& oxm) {
		os << dynamic_cast<const coxmatch_64&>(oxm);
		os << "<tunnel-id: 0x"
						<< std::hex
						<< (unsigned long long)oxm.get_u64value()
						<< " / 0x" << (unsigned long long)oxm.get_u64mask()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_PBB_ISID
 */
class coxmatch_ofb_pbb_isid : public coxmatch_24 {
public:
	coxmatch_ofb_pbb_isid() :
				coxmatch_24(rofl::openflow::OXM_TLV_BASIC_PBB_ISID)
	{};
	coxmatch_ofb_pbb_isid(
			uint32_t pbb_isid) :
				coxmatch_24(rofl::openflow::OXM_TLV_BASIC_PBB_ISID, pbb_isid)
	{};
	coxmatch_ofb_pbb_isid(
			uint32_t pbb_isid, uint32_t mask) :
				coxmatch_24(rofl::openflow::OXM_TLV_BASIC_PBB_ISID_MASK, pbb_isid, mask)
	{};
	coxmatch_ofb_pbb_isid(
			const coxmatch_24& oxm) :
				coxmatch_24(oxm)
	{};
	virtual
	~coxmatch_ofb_pbb_isid()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_pbb_isid& oxm) {
		os << dynamic_cast<const coxmatch_24&>(oxm);
		os << "<pbb-isid: 0x"
						<< std::hex
						<< (unsigned int)oxm.get_u24value() << "/0x" << (unsigned int)oxm.get_u24mask()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_EXTHDR
 */
class coxmatch_ofb_ipv6_exthdr : public coxmatch_16 {
public:
	coxmatch_ofb_ipv6_exthdr() :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR)
	{};
	coxmatch_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR, ipv6_exthdr)
	{};
	coxmatch_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr, uint16_t mask) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR_MASK, ipv6_exthdr, mask)
	{};
	coxmatch_ofb_ipv6_exthdr(
			const coxmatch_16& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_exthdr()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_exthdr& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << "<ipv6-exthdr: "
						<< (int)oxm.get_u16value() << "/" << oxm.get_u16mask()
						<< " >" << std::endl;
		return os;
	};
};





/**
 * @brief	OXM_OF_NW_PROTO (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_proto : public coxmatch_exp {
public:
	coxmatch_ofx_nw_proto() :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO, ROFL_EXP_ID)
	{};
	coxmatch_ofx_nw_proto(
			uint8_t proto) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO, ROFL_EXP_ID)
	{ set_u8value(proto); };
	coxmatch_ofx_nw_proto(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_proto()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_proto& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<nw-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_NW_TOS (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_tos : public coxmatch_exp {
public:
	coxmatch_ofx_nw_tos() :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS, ROFL_EXP_ID)
	{};
	coxmatch_ofx_nw_tos(
			uint8_t tos) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS, ROFL_EXP_ID)
	{ set_u8value(tos); };
	coxmatch_ofx_nw_tos(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_tos()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_tos& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<nw-tos: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_NW_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_src : public coxmatch_exp {
public:
	coxmatch_ofx_nw_src() :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, ROFL_EXP_ID)
	{};
	coxmatch_ofx_nw_src(
			uint32_t src) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, ROFL_EXP_ID)
	{ set_u32value(src); };
	coxmatch_ofx_nw_src(
			uint32_t src, uint32_t mask) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, ROFL_EXP_ID)
	{ set_u32value(src); set_u32mask(mask); };
	coxmatch_ofx_nw_src(
			const caddress_in4& src) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, ROFL_EXP_ID)
	{ set_u32value(src.get_addr_hbo()); };
	coxmatch_ofx_nw_src(
			const caddress_in4& src, const caddress_in4& mask) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, ROFL_EXP_ID)
	{ set_u32value(src.get_addr_hbo()); set_u32mask(mask.get_addr_hbo()); };
	coxmatch_ofx_nw_src(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_src& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<nw-src: "
				<< (unsigned int)oxm.get_u32value() << "/"
				<< (unsigned int)oxm.get_u32mask() << " >" << std::endl;
		return os;
	};
};





/**
 * @brief	OXM_OF_NW_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_dst : public coxmatch_exp {
public:
	coxmatch_ofx_nw_dst() :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, ROFL_EXP_ID)
	{};
	coxmatch_ofx_nw_dst(
			uint32_t dst) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, ROFL_EXP_ID)
	{ set_u32value(dst); };
	coxmatch_ofx_nw_dst(
			uint32_t dst, uint32_t mask) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, ROFL_EXP_ID)
	{ set_u32value(dst); set_u32mask(mask); };
	coxmatch_ofx_nw_dst(
			const caddress_in4& dst) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, ROFL_EXP_ID)
	{ set_u32value(dst.get_addr_hbo()); };
	coxmatch_ofx_nw_dst(
			const caddress_in4& dst, const caddress_in4& mask) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, ROFL_EXP_ID)
	{ set_u32value(dst.get_addr_hbo()); set_u32mask(mask.get_addr_hbo()); };
	coxmatch_ofx_nw_dst(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_dst& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<nw-dst: "
				<< (unsigned int)oxm.get_u32value() << "/"
				<< (unsigned int)oxm.get_u32mask() << " >" << std::endl;
		return os;
	};
};






/**
 * @brief	OXM_OF_TP_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_tp_src : public coxmatch_exp {
public:
	coxmatch_ofx_tp_src() :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC, ROFL_EXP_ID)
	{};
	coxmatch_ofx_tp_src(
			uint16_t src) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC, ROFL_EXP_ID)
	{ set_u16value(src); };
	coxmatch_ofx_tp_src(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_tp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_tp_src& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<tp-src: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TP_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_tp_dst : public coxmatch_exp {
public:
	coxmatch_ofx_tp_dst() :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST, ROFL_EXP_ID)
	{};
	coxmatch_ofx_tp_dst(
			uint16_t dst) :
				coxmatch_exp(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST, ROFL_EXP_ID)
	{ set_u16value(dst); };
	coxmatch_ofx_tp_dst(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_tp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_tp_dst& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<tp-dst: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};



class coxmatch_find_type
{
	uint16_t oxm_class;
	uint8_t  oxm_field;
public:
	/**
	 *
	 */
	coxmatch_find_type(
			uint16_t oxm_class,
			uint8_t oxm_field) :
				oxm_class(oxm_class), oxm_field(oxm_field) {};
	/**
	 *
	 */
	bool
	operator() (
			uint16_t __oxm_class,
			uint8_t __oxm_field) const
	{
		return ((oxm_class == __oxm_class) && (oxm_field == __oxm_field));
	};

	/**
	 *
	 */
	bool
	operator() (
			coxmatch const& match) const
	{
		return ((oxm_class == match.get_oxm_class()) &&
				 (oxm_field == match.get_oxm_field()));
	};
};





}; // end of namespace openflow
}; // end of namespace rofl




#endif /* COXMATCH_H_ */
