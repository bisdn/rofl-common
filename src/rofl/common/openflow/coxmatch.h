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
#include "rofl/common/croflexception.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/fframe.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"

namespace rofl {
namespace openflow {

/* error classes */
class eOxmBase 				: public RoflException {
public:
	eOxmBase(const std::string& __arg) : RoflException(__arg) {};
};
class eOxmInval 			: public eOxmBase {
public:
	eOxmInval(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmNotFound 			: public eOxmBase {
public:
	eOxmNotFound(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmBadLen 			: public eOxmBase {
public:
	eOxmBadLen(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmInvalType 		: public eOxmBase {
public:
	eOxmInvalType(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmInvalClass 		: public eOxmBase {
public:
	eOxmInvalClass(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmHeaderInval 		: public eOxmBase {
public:
	eOxmHeaderInval(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmActionNotFound 	: public eOxmBase {
public:
	eOxmActionNotFound(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmBadExperimenter 	: public eOxmBase {
public:
	eOxmBadExperimenter(const std::string& __arg) : eOxmBase(__arg) {};
};


enum rofl_exp_id_t {
	ROFL_EXP_ID = 0, // TODO
};


/**
 *
 */
class coxmatch :
	public rofl::cmemory
{
	PthreadRwLock		oxmlock;

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
	coxmatch()
	{};

	/**
	 *
	 */
	coxmatch(
			size_t oxm_len, uint32_t oxm_id) :
				rofl::cmemory(oxm_len)
	{ set_oxm_id(oxm_id); };

	/**
	 *
	 */
	coxmatch(
			uint8_t* oxm_hdr, size_t oxm_len) :
				rofl::cmemory(oxm_hdr, oxm_len)
	{};

	/**
	 *
	 */
	coxmatch(
			const coxmatch& oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr)) {
			throw eOxmInval("coxmatch::coxmatch() buf too short");
		}
		*this = oxm;
	};

	/**
	 *
	 */
	coxmatch&
	operator= (
			const coxmatch& oxm)
	{
		if (this == &oxm)
			return *this;
		cmemory::operator= (oxm);
		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const coxmatch& oxm) const
	{ return cmemory::operator== (oxm); };

	/**
	 *
	 */
	bool
	operator!= (
			const coxmatch& oxm) const
	{ return cmemory::operator!= (oxm); };

	/**
	 *
	 */
	bool
	operator< (
			const coxmatch& oxm)
	{
		if (get_oxm_id() != oxm.get_oxm_id()) {
			return (get_oxm_id() < oxm.get_oxm_id());
		}
		size_t len = (memlen() < oxm.memlen()) ? memlen() : oxm.memlen();
		int rc = memcmp(somem(), oxm.somem(), len);
		if (rc < 0) {
			return true;
		} else if (rc > 0) {
			return false;
		}
		return false;
	};

public:

	/**
	 *
	 */
	virtual void
	clear()
	{ rofl::cmemory::clear(); };

	/**
	 *
	 */
	virtual size_t
	length() const
	{ return memlen(); };

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen)
	{
		if (buflen < length()) {
			throw eOxmBadLen("coxmatch::pack() invalid length");
		}
		memcpy(buf, somem(), memlen());
	};

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen)
	 {
		clear();
		if (buflen > memlen()) {
			resize(buflen);
		}
		assign(buf, buflen);
	};

public:

	/**
	 *
	 */
	void
	set_oxm_id(
			uint32_t oxm_id)
	{
		struct rofl::openflow::ofp_oxm_tlv_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_tlv_hdr*)somem();
		oxm->oxm_id = htobe32(oxm_id);
	};

	/**
	 *
	 */
	uint32_t
	get_oxm_id() const
	{
		struct rofl::openflow::ofp_oxm_tlv_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_tlv_hdr*)somem();
		return be32toh(oxm->oxm_id);
	};

public:

	/**
	 *
	 */
	uint16_t
	get_oxm_class() const
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		return be16toh(oxm->oxm_class);
	};

	/**
	 *
	 */
	void
	set_oxm_class(
			uint16_t oxm_class)
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		oxm->oxm_class = htobe16(oxm_class);
	};

	/**
	 *
	 */
	uint8_t
	get_oxm_field() const
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		return ((oxm->oxm_field) >> 1);
	};

	/**
	 *
	 */
	void
	set_oxm_field(
			uint8_t oxm_field)
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		oxm->oxm_field = (oxm->oxm_field & 0x01) | ((oxm_field & 0x7f) << 1);
	};

	/**
	 *
	 */
	void
	set_oxm_hasmask(
			bool hasmask)
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		oxm->oxm_field = (oxm->oxm_field & 0xfe) | hasmask;
	};

	/**
	 *
	 */
	bool
	get_oxm_hasmask() const
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		return (oxm->oxm_field & 0x01);
	};

	/**
	 *
	 */
	void
	set_oxm_length(
			uint8_t oxm_len)
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		oxm->oxm_length = oxm_len;
	};

	/**
	 *
	 */
	uint8_t
	get_oxm_length()
	{
		struct rofl::openflow::ofp_oxm_hdr* oxm =
				(struct rofl::openflow::ofp_oxm_hdr*)somem();
		return oxm->oxm_length;
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch& oxm) {
		os << rofl::indent(0) << "<coxmatch oxm-id: 0x" << std::hex << oxm.get_oxm_id() << std::dec << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<rofl::cmemory const&>( oxm );
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






class coxmatch_exp :
		public coxmatch
{
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
	coxmatch_exp()
	{};

	/**
	 *
	 */
	coxmatch_exp(
			size_t oxm_len, uint32_t oxm_id, uint32_t exp_id) :
				coxmatch(oxm_len, oxm_id)
	{ set_oxm_exp_id(exp_id); };

	/**
	 *
	 */
	coxmatch_exp(
			uint8_t* oxm_hdr, size_t oxm_len);

	/**
	 *
	 */
	coxmatch_exp(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_experimenter_header)) {
			throw eOxmInval("coxmatch_exp::coxmatch_exp() buf too short");
		}
		*this = oxm;
	};

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
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_oxm_exp_id(
			uint32_t exp_id)
	{
		struct ofp_oxm_experimenter_header* oxm =
				(struct ofp_oxm_experimenter_header*)somem();
		oxm->experimenter = htobe32(exp_id);
	};

	/**
	 *
	 */
	uint32_t
	get_oxm_exp_id() const
	{
		struct ofp_oxm_experimenter_header* oxm =
				(struct ofp_oxm_experimenter_header*)somem();
		return be32toh(oxm->experimenter);
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_exp& oxm) {
		os << rofl::indent(0) << "<coxmatch_exp ";
		os << "oxm_id: 0x" << std::hex << oxm.get_oxm_id() << std::dec << ", ";
		os << "exp_id: 0x" << std::hex << oxm.get_oxm_exp_id() << std::dec << " ";
		os << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const coxmatch&>( oxm );
		return os;
	};
};







class coxmatch_8 :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_8(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_8::coxmatch_8() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_8(
			uint32_t oxm_id, uint8_t value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint8_t), oxm_id)
	{ set_u8value(value); };

public:

	uint8_t
	get_u8value() const {
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
		return oxm->byte;
	};

	void
	set_u8value(
			uint8_t value) {
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
		oxm->byte = value;
	};
};





class coxmatch_8_masked :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_8_masked(
			const coxmatch& oxm) :
			coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_8_masked::coxmatch_8_masked() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_8_masked(
			uint32_t oxm_id, uint8_t value, uint8_t mask) :
			coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint8_t), oxm_id)
	{
			set_u8value(value);
			set_u8mask(mask);
	};

public:

	/**
	 *
	 */
	uint8_t
	get_u8value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
		return oxm->byte;
	};

	/**
	 *
	 */
	void
	set_u8value(uint8_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
		oxm->byte = value;
	};

	/**
	 *
	 */
	uint8_t
	get_u8mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
		return oxm->mask;
	};

	/**
	 *
	 */
	void
	set_u8mask(uint8_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
		oxm->mask = mask;
	};

	/**
	 *
	 */
	uint8_t
	get_u8masked_value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
		return (oxm->byte & oxm->mask);
	};
};





class coxmatch_16 :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_16(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint16_t)) {
			throw eOxmInval("coxmatch_16::coxmatch_16() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_16(
			uint32_t oxm_id, uint16_t value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint16_t), oxm_id)
	{ set_u16value(value); };

public:

	uint16_t
	get_u16value() const {
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
		return be16toh(oxm->word);
	};

	void
	set_u16value(
			uint16_t value) {
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
		oxm->word = htobe16(value);
	};
};





class coxmatch_16_masked :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_16_masked(
			const coxmatch& oxm) :
			coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t)) {
			throw eOxmInval("coxmatch_16_masked::coxmatch_16_masked() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_16_masked(
			uint32_t oxm_id, uint16_t value, uint16_t mask) :
			coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t), oxm_id)
	{
			set_u16value(value);
			set_u16mask(mask);
	};

public:

	/**
	 *
	 */
	uint16_t
	get_u16value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
		return be16toh(oxm->word);
	};

	/**
	 *
	 */
	void
	set_u16value(uint16_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
		oxm->word = htobe16(value);
	};

	/**
	 *
	 */
	uint16_t
	get_u16mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
		return be16toh(oxm->mask);
	};

	/**
	 *
	 */
	void
	set_u16mask(uint16_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
		oxm->mask = htobe16(mask);
	};

	/**
	 *
	 */
	uint16_t
	get_u16masked_value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
		return be16toh(oxm->word & oxm->mask);
	};
};




class coxmatch_24 :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_24(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 3*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_24::coxmatch_24() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_24(
			uint32_t oxm_id, uint32_t value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 3*sizeof(uint8_t), oxm_id)
	{ set_u24value(value); };

public:

	uint32_t
	get_u24value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
		return (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
	};

	void
	set_u24value(
			uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
		oxm->word[2] = (uint8_t)((value >>  0) & 0x000000ff);
		oxm->word[1] = (uint8_t)((value >>  8) & 0x000000ff);
		oxm->word[0] = (uint8_t)((value >> 16) & 0x000000ff);
	};
};



class coxmatch_24_masked :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_24_masked(
			const coxmatch& oxm) :
			coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*3*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_24_masked::coxmatch_24_masked() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_24_masked(
			uint32_t oxm_id, uint32_t value, uint32_t mask) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*3*sizeof(uint8_t), oxm_id)
	{
			set_u24value(value);
			set_u24mask(mask);
	};

public:

	uint32_t
	get_u24value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
		return (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
	};

	void
	set_u24value(
			uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
		oxm->word[2] = (uint8_t)((value >>  0) & 0x000000ff);
		oxm->word[1] = (uint8_t)((value >>  8) & 0x000000ff);
		oxm->word[0] = (uint8_t)((value >> 16) & 0x000000ff);
	};

	/**
	 *
	 */
	uint32_t
	get_u24mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
		return ((((uint32_t)oxm->mask[0] << 16) | ((uint32_t)oxm->mask[1] << 8) | ((uint32_t)oxm->mask[2] << 0)) & 0x00ffffff);
	};

	/**
	 *
	 */
	void
	set_u24mask(uint32_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint24_t*)somem();
		oxm->mask[2] = (uint8_t)((mask >>  0) & 0x000000ff);
		oxm->mask[1] = (uint8_t)((mask >>  8) & 0x000000ff);
		oxm->mask[0] = (uint8_t)((mask >> 16) & 0x000000ff);
	};

	/**
	 *
	 */
	uint32_t
	get_u24masked_value() const
	{
		return (get_u24value() & get_u24mask());
	};
};




class coxmatch_32 :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_32(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint32_t)) {
			throw eOxmInval("coxmatch_32::coxmatch_32() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_32(
			uint32_t oxm_id, uint32_t value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint32_t), oxm_id)
	{ set_u32value(value); };

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_32(
			uint32_t oxm_id, const rofl::caddress_in4& value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint32_t), oxm_id)
	{ set_u32value(value); };

public:

	/**
	 *
	 */
	uint32_t
	get_u32value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		return be32toh(oxm->dword);
	};

	/**
	 *
	 */
	void
	set_u32value(
			uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		oxm->dword = htobe32(value);
	};

	/**
	 *
	 */
	rofl::caddress_in4
	get_u32value_as_addr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		caddress_in4 addr; addr.set_addr_nbo(oxm->dword);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u32value(
			const rofl::caddress_in4& addr)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		oxm->dword = addr.get_addr_nbo();
	};
};





class coxmatch_32_masked :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_32_masked(
			const coxmatch& oxm) :
			coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t)) {
			throw eOxmInval("coxmatch_32_masked::coxmatch_32_masked() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_32_masked(
			uint32_t oxm_id, uint32_t value, uint32_t mask) :
			coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t), oxm_id)
	{
			set_u32value(value);
			set_u32mask(mask);
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_32_masked(
			uint32_t oxm_id, const rofl::caddress_in4& value, const rofl::caddress_in4& mask) :
			coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t), oxm_id)
	{
			set_u32value(value);
			set_u32mask(mask);
	};

public:

	/**
	 *
	 */
	uint32_t
	get_u32value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		return be32toh(oxm->dword);
	};

	/**
	 *
	 */
	void
	set_u32value(uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		oxm->dword = htobe32(value);
	};

	/**
	 *
	 */
	uint32_t
	get_u32mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		return be32toh(oxm->mask);
	};

	/**
	 *
	 */
	void
	set_u32mask(uint32_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		oxm->mask = htobe32(mask);
	};

	/**
	 *
	 */
	uint32_t
	get_u32masked_value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		return be32toh(oxm->dword & oxm->mask);
	};

	/**
	 *
	 */
	rofl::caddress_in4
	get_u32value_as_addr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		caddress_in4 addr; addr.set_addr_nbo(oxm->dword);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u32value(
			const rofl::caddress_in4& addr)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		oxm->dword = addr.get_addr_nbo();
	};

	/**
	 *
	 */
	rofl::caddress_in4
	get_u32mask_as_addr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		caddress_in4 mask; mask.set_addr_nbo(oxm->mask);
		return mask;
	};

	/**
	 *
	 */
	void
	set_u32mask(
			const rofl::caddress_in4& mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		oxm->mask = mask.get_addr_nbo();
	};

	/**
	 *
	 */
	rofl::caddress_in4
	get_u32masked_value_as_addr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
		caddress_in4 masked; masked.set_addr_nbo(oxm->dword & oxm->mask);
		return masked;
	};
};




class coxmatch_48 :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_48(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*6*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_48::coxmatch_48() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_48(
			uint32_t oxm_id, uint64_t value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*6*sizeof(uint8_t), oxm_id)
	{ set_u48value(value); };

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_48(
			uint32_t oxm_id, const rofl::caddress_ll& value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*6*sizeof(uint8_t), oxm_id)
	{ set_u48value(value); };

public:

	/**
	 *
	 */
	uint64_t
	get_u48value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		return (((uint64_t)oxm->value[0] << 40) | ((uint64_t)oxm->value[1] << 32) | ((uint64_t)oxm->value[2] << 24) |
				((uint64_t)oxm->value[3] << 16) | ((uint64_t)oxm->value[4] <<  8) | ((uint64_t)oxm->value[5] <<  0));
	};

	/**
	 *
	 */
	void
	set_u48value(
			uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		oxm->value[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
		oxm->value[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
		oxm->value[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
		oxm->value[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
		oxm->value[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
		oxm->value[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48value_as_lladdr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		rofl::caddress_ll addr;
		memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u48value(
			const rofl::caddress_ll& addr)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		memcpy(oxm->value, addr.somem(), OFP_ETH_ALEN);
	};
};





class coxmatch_48_masked :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_48_masked(
			const coxmatch& oxm) :
			coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*6*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_48_masked::coxmatch_48_masked() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_48_masked(
			uint32_t oxm_id, uint64_t value, uint64_t mask) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*6*sizeof(uint8_t), oxm_id)
	{
			set_u48value(value);
			set_u48mask(mask);
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_48_masked(
			uint32_t oxm_id, const rofl::caddress_ll& value, const rofl::caddress_ll& mask) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*6*sizeof(uint8_t), oxm_id)
	{
			set_u48value(value);
			set_u48mask(mask);
	};

public:

	/**
	 *
	 */
	uint64_t
	get_u48value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		return (((uint64_t)oxm->value[0] << 40) | ((uint64_t)oxm->value[1] << 32) | ((uint64_t)oxm->value[2] << 24) |
				((uint64_t)oxm->value[3] << 16) | ((uint64_t)oxm->value[4] <<  8) | ((uint64_t)oxm->value[5] <<  0));
	};

	/**
	 *
	 */
	void
	set_u48value(
			uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		oxm->value[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
		oxm->value[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
		oxm->value[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
		oxm->value[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
		oxm->value[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
		oxm->value[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48value_as_lladdr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		rofl::caddress_ll addr;
		memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u48value(
			const rofl::caddress_ll& addr)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		memcpy(oxm->value, addr.somem(), OFP_ETH_ALEN);
	};

	/**
	 *
	 */
	uint64_t
	get_u48mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		return (((uint64_t)oxm->mask[0] << 40) | ((uint64_t)oxm->mask[1] << 32) | ((uint64_t)oxm->mask[2] << 24) |
				((uint64_t)oxm->mask[3] << 16) | ((uint64_t)oxm->mask[4] <<  8) | ((uint64_t)oxm->mask[5] <<  0));
	};

	/**
	 *
	 */
	void
	set_u48mask(
			uint64_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		oxm->mask[5] = (uint8_t)((mask >>  0) & 0x00000000000000ff);
		oxm->mask[4] = (uint8_t)((mask >>  8) & 0x00000000000000ff);
		oxm->mask[3] = (uint8_t)((mask >> 16) & 0x00000000000000ff);
		oxm->mask[2] = (uint8_t)((mask >> 24) & 0x00000000000000ff);
		oxm->mask[1] = (uint8_t)((mask >> 32) & 0x00000000000000ff);
		oxm->mask[0] = (uint8_t)((mask >> 40) & 0x00000000000000ff);
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48mask_as_lladdr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		rofl::caddress_ll addr;
		memcpy(addr.somem(), oxm->mask, OFP_ETH_ALEN);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u48mask(
			const rofl::caddress_ll& addr)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
		memcpy(oxm->mask, addr.somem(), OFP_ETH_ALEN);
	};

	/**
	 *
	 */
	uint64_t
	get_u48masked_value() const
	{
		return (get_u48value() & get_u48mask());
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48masked_value_as_lladdr() const
	{
		return (get_u48value_as_lladdr() & get_u48mask_as_lladdr());
	};
};




class coxmatch_64 :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_64(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint64_t)) {
			throw eOxmInval("coxmatch_64::coxmatch_64() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_64(
			uint32_t oxm_id, uint64_t value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*sizeof(uint64_t), oxm_id)
	{ set_u64value(value); };

public:

	uint64_t
	get_u64value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
		return be64toh(oxm->word);
	};

	void
	set_u64value(
			uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
		oxm->word = htobe64(value);
	};
};



class coxmatch_64_masked :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_64_masked(
			const coxmatch& oxm) :
			coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t)) {
			throw eOxmInval("coxmatch_64_masked::coxmatch_64_masked() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_64_masked(
			uint32_t oxm_id, uint64_t value, uint64_t mask) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t), oxm_id)
	{
			set_u64value(value);
			set_u64mask(mask);
	};

public:

	uint64_t
	get_u64value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
		return be64toh(oxm->word);
	};

	void
	set_u64value(
			uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
		oxm->word = htobe64(value);
	};

	uint64_t
	get_u64mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
		return be64toh(oxm->mask);
	};

	void
	set_u64mask(
			uint64_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
		oxm->mask = htobe64(mask);
	};

	/**
	 *
	 */
	uint64_t
	get_u64masked_value() const
	{
		return (get_u64value() & get_u64mask());
	};
};





class coxmatch_128 :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_128(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*16*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_128::coxmatch_128() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_128(
			uint32_t oxm_id, const rofl::caddress_in6& value) :
				coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 1*16*sizeof(uint8_t), oxm_id)
	{ set_u128value(value); };

public:

	/**
	 *
	 */
	rofl::caddress_in6
	get_u128value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
		caddress_in6 addr;
		memcpy(addr.somem(), oxm->addr, 16);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u128value(
			const rofl::caddress_in6& value)
	{
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
		memcpy(oxm->addr, value.somem(), 16);
	};
};





class coxmatch_128_masked :
		public coxmatch
{
public:
	/**
	 *
	 */
	coxmatch_128_masked(
			const coxmatch& oxm) :
			coxmatch(oxm)
	{
		if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t)) {
			throw eOxmInval("coxmatch_128_masked::coxmatch_128_masked() buf too short");
		}
	};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_128_masked(
			uint32_t oxm_id, const rofl::caddress_in6& value, const rofl::caddress_in6& mask) :
			coxmatch(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t), oxm_id)
	{
			set_u128value(value);
			set_u128mask(mask);
	};

public:

	/**
	 *
	 */
	rofl::caddress_in6
	get_u128value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
		caddress_in6 addr;
		memcpy(addr.somem(), oxm->addr, 16);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u128value(
			const rofl::caddress_in6& value)
	{
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
		memcpy(oxm->addr, value.somem(), 16);
	};

	/**
	 *
	 */
	rofl::caddress_in6
	get_u128mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
		caddress_in6 addr;
		memcpy(addr.somem(), oxm->mask, 16);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u128mask(
			const rofl::caddress_in6& mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
		memcpy(oxm->mask, mask.somem(), 16);
	};

	/**
	 *
	 */
	rofl::caddress_in6
	get_u128masked_value() const
	{
		return (get_u128value() & get_u128mask());
	};
};












class coxmatch_8_exp :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_8_exp(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch_8_exp(
			uint32_t oxm_id, uint32_t exp_id, uint8_t value) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 1*sizeof(uint8_t), oxm_id, exp_id)
	{ set_u8value(value); };

	/**
	 *
	 */
	coxmatch_8_exp(
			const coxmatch_8_exp& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_8_exp&
	operator= (
			const coxmatch_8_exp& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint8_t
	get_u8value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t*)somem();
		return oxm->byte;
	};

	/**
	 *
	 */
	void
	set_u8value(uint8_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t*)somem();
		oxm->byte = value;
	};
};




class coxmatch_8_exp_masked :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_8_exp_masked(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch_8_exp_masked(
			uint32_t oxm_id, uint32_t exp_id, uint8_t value, uint8_t mask) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint8_t), oxm_id, exp_id)
	{
		set_u8value(value);
		set_u8mask(mask);
	};

	/**
	 *
	 */
	coxmatch_8_exp_masked(
			const coxmatch_8_exp_masked& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_8_exp_masked&
	operator= (
			const coxmatch_8_exp_masked& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint8_t
	get_u8value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t*)somem();
		return oxm->byte;
	};

	/**
	 *
	 */
	void
	set_u8value(uint8_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t*)somem();
		oxm->byte = value;
	};

	/**
	 *
	 */
	uint8_t
	get_u8mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t*)somem();
		return oxm->mask;
	};

	/**
	 *
	 */
	void
	set_u8mask(uint8_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t*)somem();
		oxm->mask = mask;
	};

	/**
	 *
	 */
	uint8_t
	get_u8masked_value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint8_t*)somem();
		return (oxm->byte & oxm->mask);
	};
};



class coxmatch_16_exp :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_16_exp(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for single 16bit values
	 */
	coxmatch_16_exp(
			uint32_t oxm_id, uint32_t exp_id, uint16_t value) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 1*sizeof(uint16_t), oxm_id, exp_id)
	{ set_u16value(value); };

	/**
	 *
	 */
	coxmatch_16_exp(
			const coxmatch_16_exp& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_16_exp&
	operator= (
			const coxmatch_16_exp& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_u16value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
		return be16toh(oxm->word);
	};

	/**
	 *
	 */
	void
	set_u16value(uint16_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
		oxm->word = htobe16(value);
	};
};




class coxmatch_16_exp_masked :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_16_exp_masked(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for masked 16bit values
	 */
	coxmatch_16_exp_masked(
			uint32_t oxm_id, uint32_t exp_id, uint16_t value, uint16_t mask) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint16_t), oxm_id, exp_id)
	{
		set_u16value(value);
		set_u16mask(mask);
	};

	/**
	 *
	 */
	coxmatch_16_exp_masked(
			const coxmatch_16_exp_masked& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_16_exp_masked&
	operator= (
			const coxmatch_16_exp_masked& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_u16value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
		return be16toh(oxm->word);
	};

	/**
	 *
	 */
	void
	set_u16value(uint16_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
		oxm->word = htobe16(value);
	};

	/**
	 *
	 */
	uint16_t
	get_u16mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
		return be16toh(oxm->mask);
	};

	/**
	 *
	 */
	void
	set_u16mask(uint16_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
		oxm->mask = htobe16(mask);
	};

	/**
	 *
	 */
	uint16_t
	get_u16masked_value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint16_t*)somem();
		return be16toh(oxm->word & oxm->mask);
	};
};



class coxmatch_24_exp :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_24_exp(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for single 16bit values
	 */
	coxmatch_24_exp(
			uint32_t oxm_id, uint32_t exp_id, uint32_t value) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 1*3*sizeof(uint8_t), oxm_id, exp_id)
	{ set_u24value(value); };

	/**
	 *
	 */
	coxmatch_24_exp(
			const coxmatch_24_exp& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_24_exp&
	operator= (
			const coxmatch_24_exp& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_u24value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
		return (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
	};

	/**
	 *
	 */
	void
	set_u24value(uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
		oxm->word[2] = (uint8_t)((value >>  0) & 0x000000ff);
		oxm->word[1] = (uint8_t)((value >>  8) & 0x000000ff);
		oxm->word[0] = (uint8_t)((value >> 16) & 0x000000ff);
	};
};




class coxmatch_24_exp_masked :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_24_exp_masked(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for masked 16bit values
	 */
	coxmatch_24_exp_masked(
			uint32_t oxm_id, uint32_t exp_id, uint32_t value, uint32_t mask) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 2*3*sizeof(uint8_t), oxm_id, exp_id)
	{
		set_u24value(value);
		set_u24mask(mask);
	};

	/**
	 *
	 */
	coxmatch_24_exp_masked(
			const coxmatch_24_exp_masked& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_24_exp_masked&
	operator= (
			const coxmatch_24_exp_masked& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_u24value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
		return (((uint32_t)oxm->word[0] << 16) | ((uint32_t)oxm->word[1] << 8) | ((uint32_t)oxm->word[2] << 0));
	};

	/**
	 *
	 */
	void
	set_u24value(uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
		oxm->word[2] = (uint8_t)((value >>  0) & 0x000000ff);
		oxm->word[1] = (uint8_t)((value >>  8) & 0x000000ff);
		oxm->word[0] = (uint8_t)((value >> 16) & 0x000000ff);
	};

	/**
	 *
	 */
	uint32_t
	get_u24mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
		return (((uint32_t)oxm->mask[0] << 16) | ((uint32_t)oxm->mask[1] << 8) | ((uint32_t)oxm->mask[2] << 0));
	};

	/**
	 *
	 */
	void
	set_u24mask(uint32_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint24_t*)somem();
		oxm->mask[2] = (uint8_t)((mask >>  0) & 0x000000ff);
		oxm->mask[1] = (uint8_t)((mask >>  8) & 0x000000ff);
		oxm->mask[0] = (uint8_t)((mask >> 16) & 0x000000ff);
	};

	/**
	 *
	 */
	uint32_t
	get_u24masked_value() const
	{
		return (get_u24value() & get_u24mask());
	};
};



class coxmatch_32_exp :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_32_exp(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for single 32bit values
	 */
	coxmatch_32_exp(
			uint32_t oxm_id, uint32_t exp_id, uint32_t value) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 1*sizeof(uint32_t), oxm_id, exp_id)
	{ set_u32value(value); };

	/**
	 *
	 */
	coxmatch_32_exp(
			const coxmatch_32_exp& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_32_exp&
	operator= (
			const coxmatch_32_exp& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_u32value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
		return be32toh(oxm->dword);
	};

	/**
	 *
	 */
	void
	set_u32value(uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
		oxm->dword = htobe32(value);
	};
};



class coxmatch_32_exp_masked :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_32_exp_masked(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for masked 32bit values
	 */
	coxmatch_32_exp_masked(
			uint32_t oxm_id, uint32_t exp_id, uint32_t value, uint32_t mask) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint32_t), oxm_id, exp_id)
	{
		set_u32value(value);
		set_u32mask(mask);
	};

	/**
	 *
	 */
	coxmatch_32_exp_masked(
			const coxmatch_32_exp_masked& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_32_exp_masked&
	operator= (
			const coxmatch_32_exp_masked& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint32_t
	get_u32value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
		return be32toh(oxm->dword);
	};

	/**
	 *
	 */
	void
	set_u32value(uint32_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
		oxm->dword = htobe32(value);
	};

	/**
	 *
	 */
	uint32_t
	get_u32mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
		return be32toh(oxm->mask);
	};

	/**
	 *
	 */
	void
	set_u32mask(uint32_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
		oxm->mask = htobe32(mask);
	};

	/**
	 *
	 */
	uint32_t
	get_u32masked_value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint32_t*)somem();
		return be32toh(oxm->dword & oxm->mask);
	};
};



class coxmatch_48_exp :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_48_exp(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for single 48bit values
	 */
	coxmatch_48_exp(
			uint32_t oxm_id, uint32_t exp_id, uint64_t value) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 1*sizeof(uint64_t), oxm_id, exp_id)
	{ set_u48value(value); };

	/**
	 * @brief	coxmatch base class for single 48bit values
	 */
	coxmatch_48_exp(
			uint32_t oxm_id, uint32_t exp_id, const rofl::caddress_ll& value) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 1*sizeof(uint64_t), oxm_id, exp_id)
	{ set_u48value(value); };

	/**
	 *
	 */
	coxmatch_48_exp(
			const coxmatch_48_exp& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_48_exp&
	operator= (
			const coxmatch_48_exp& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint64_t
	get_u48value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		return (((uint64_t)oxm->value[0] << 40) | ((uint64_t)oxm->value[1] << 32) | ((uint64_t)oxm->value[2] << 24) |
				((uint64_t)oxm->value[3] << 16) | ((uint64_t)oxm->value[4] <<  8) | ((uint64_t)oxm->value[5] <<  0));
	};

	/**
	 *
	 */
	void
	set_u48value(uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		oxm->value[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
		oxm->value[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
		oxm->value[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
		oxm->value[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
		oxm->value[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
		oxm->value[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48value_as_lladdr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		rofl::caddress_ll addr;
		memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u48value(const rofl::caddress_ll& value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		memcpy(oxm->value, value.somem(), OFP_ETH_ALEN);
	};
};



class coxmatch_48_exp_masked :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_48_exp_masked(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for masked 48bit values
	 */
	coxmatch_48_exp_masked(
			uint32_t oxm_id, uint32_t exp_id, uint64_t value, uint64_t mask) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint64_t), oxm_id, exp_id)
	{
		set_u48value(value);
		set_u48mask(mask);
	};

	/**
	 * @brief	coxmatch base class for masked 48bit values
	 */
	coxmatch_48_exp_masked(
			uint32_t oxm_id, uint32_t exp_id, const rofl::caddress_ll& value, const rofl::caddress_ll& mask) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint64_t), oxm_id, exp_id)
	{
		set_u48value(value);
		set_u48mask(mask);
	};

	/**
	 *
	 */
	coxmatch_48_exp_masked(
			const coxmatch_48_exp_masked& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_48_exp_masked&
	operator= (
			const coxmatch_48_exp_masked& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint64_t
	get_u48value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		return (((uint64_t)oxm->value[0] << 40) | ((uint64_t)oxm->value[1] << 32) | ((uint64_t)oxm->value[2] << 24) |
				((uint64_t)oxm->value[3] << 16) | ((uint64_t)oxm->value[4] <<  8) | ((uint64_t)oxm->value[5] <<  0));
	};

	/**
	 *
	 */
	void
	set_u48value(uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		oxm->value[5] = (uint8_t)((value >>  0) & 0x00000000000000ff);
		oxm->value[4] = (uint8_t)((value >>  8) & 0x00000000000000ff);
		oxm->value[3] = (uint8_t)((value >> 16) & 0x00000000000000ff);
		oxm->value[2] = (uint8_t)((value >> 24) & 0x00000000000000ff);
		oxm->value[1] = (uint8_t)((value >> 32) & 0x00000000000000ff);
		oxm->value[0] = (uint8_t)((value >> 40) & 0x00000000000000ff);
	};

	/**
	 *
	 */
	uint64_t
	get_u48mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		return (((uint64_t)oxm->mask[0] << 40) | ((uint64_t)oxm->mask[1] << 32) | ((uint64_t)oxm->mask[2] << 24) |
				((uint64_t)oxm->mask[3] << 16) | ((uint64_t)oxm->mask[4] <<  8) | ((uint64_t)oxm->mask[5] <<  0));
	};

	/**
	 *
	 */
	void
	set_u48mask(uint64_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		oxm->mask[5] = (uint8_t)((mask >>  0) & 0x00000000000000ff);
		oxm->mask[4] = (uint8_t)((mask >>  8) & 0x00000000000000ff);
		oxm->mask[3] = (uint8_t)((mask >> 16) & 0x00000000000000ff);
		oxm->mask[2] = (uint8_t)((mask >> 24) & 0x00000000000000ff);
		oxm->mask[1] = (uint8_t)((mask >> 32) & 0x00000000000000ff);
		oxm->mask[0] = (uint8_t)((mask >> 40) & 0x00000000000000ff);
	};

	/**
	 *
	 */
	uint64_t
	get_u48masked_value() const
	{
		return (get_u48value() & get_u48mask());
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48value_as_lladdr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		rofl::caddress_ll addr;
		memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u48value(const rofl::caddress_ll& value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		memcpy(oxm->value, value.somem(), OFP_ETH_ALEN);
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48mask_as_lladdr() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		rofl::caddress_ll addr;
		memcpy(addr.somem(), oxm->mask, OFP_ETH_ALEN);
		return addr;
	};

	/**
	 *
	 */
	void
	set_u48mask(const rofl::caddress_ll& value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint48_t*)somem();
		memcpy(oxm->mask, value.somem(), OFP_ETH_ALEN);
	};

	/**
	 *
	 */
	rofl::caddress_ll
	get_u48masked_value_as_lladdr() const
	{
		return (get_u48value_as_lladdr() & get_u48mask_as_lladdr());
	};
};



class coxmatch_64_exp :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_64_exp(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for single 64bit values
	 */
	coxmatch_64_exp(
			uint64_t oxm_id, uint64_t exp_id, uint64_t value) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 1*sizeof(uint64_t), oxm_id, exp_id)
	{ set_u64value(value); };

	/**
	 *
	 */
	coxmatch_64_exp(
			const coxmatch_64_exp& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_64_exp&
	operator= (
			const coxmatch_64_exp& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint64_t
	get_u64value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
		return be64toh(oxm->word);
	};

	/**
	 *
	 */
	void
	set_u64value(uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
		oxm->word = htobe64(value);
	};
};



class coxmatch_64_exp_masked :
		public coxmatch_exp
{
public:
	/**
	 *
	 */
	coxmatch_64_exp_masked(
			const coxmatch& oxm) :
				coxmatch_exp(oxm)
	{};

	/**
	 * @brief	coxmatch base class for masked 64bit values
	 */
	coxmatch_64_exp_masked(
			uint64_t oxm_id, uint64_t exp_id, uint64_t value, uint64_t mask) :
				coxmatch_exp(sizeof(struct ofp_oxm_experimenter_header) + 2*sizeof(uint64_t), oxm_id, exp_id)
	{
		set_u64value(value);
		set_u64mask(mask);
	};

	/**
	 *
	 */
	coxmatch_64_exp_masked(
			const coxmatch_64_exp_masked& oxm)
	{ *this = oxm; };

	/**
	 *
	 */
	coxmatch_64_exp_masked&
	operator= (
			const coxmatch_64_exp_masked& oxm)
	{
		if (this == &oxm)
			return *this;
		coxmatch_exp::operator= (oxm);
		return *this;
	};

public:

	/**
	 *
	 */
	uint64_t
	get_u64value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
		return be64toh(oxm->word);
	};

	/**
	 *
	 */
	void
	set_u64value(uint64_t value)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
		oxm->word = htobe64(value);
	};

	/**
	 *
	 */
	uint64_t
	get_u64mask() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
		return be64toh(oxm->mask);
	};

	/**
	 *
	 */
	void
	set_u64mask(uint64_t mask)
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
		oxm->mask = htobe64(mask);
	};

	/**
	 *
	 */
	uint64_t
	get_u64masked_value() const
	{
		struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t* oxm =
				(struct rofl::openflow::ofp_oxm_ofb_exp_uint64_t*)somem();
		return be64toh(oxm->word & oxm->mask);
	};
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
	coxmatch_ofb_in_port(
			uint32_t port_no) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IN_PORT, port_no)
	{};
	coxmatch_ofb_in_port(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_in_port()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_in_port& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<in-port: " << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IN_PHY_PORT
 */
class coxmatch_ofb_in_phy_port : public coxmatch_32 {
public:
	coxmatch_ofb_in_phy_port(
			uint32_t port_no) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT, port_no)
	{};
	coxmatch_ofb_in_phy_port(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_in_phy_port()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_in_phy_port& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<in-phy-port: " << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_METADATA
 */
class coxmatch_ofb_metadata : public coxmatch_64 {
public:
	coxmatch_ofb_metadata(
			uint64_t metadata) :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_METADATA, metadata)
	{};
	coxmatch_ofb_metadata(
			const coxmatch& oxm) :
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
		os << indent(2) << "<metadata: "
		<< (unsigned long long)oxm.get_u64value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_METADATA
 */
class coxmatch_ofb_metadata_masked : public coxmatch_64_masked {
public:
	coxmatch_ofb_metadata_masked(
			uint64_t metadata, uint64_t mask) :
				coxmatch_64_masked(rofl::openflow::OXM_TLV_BASIC_METADATA_MASK, metadata, mask)
	{};
	coxmatch_ofb_metadata_masked(
			const coxmatch& oxm) :
				coxmatch_64_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_metadata_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_metadata_masked& oxm) {
		os << dynamic_cast<const coxmatch_64_masked&>(oxm);
		os << std::hex;
		os << indent(2) << "<metadata: "
		<< (unsigned long long)oxm.get_u64value() << "/" << (unsigned long long)oxm.get_u64mask()
		<< std::dec
		<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ETH_DST
 */
class coxmatch_ofb_eth_dst : public coxmatch_48 {
public:
	coxmatch_ofb_eth_dst(
			const cmacaddr& maddr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_DST, maddr)
	{};
	coxmatch_ofb_eth_dst(
			const coxmatch& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_dst& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << indent(2) << "<eth-dst: "
				<< oxm.get_u48value_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ETH_DST
 */
class coxmatch_ofb_eth_dst_masked : public coxmatch_48_masked {
public:
	coxmatch_ofb_eth_dst_masked(
			const cmacaddr& maddr, const cmacaddr& mask) :
				coxmatch_48_masked(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK, maddr, mask)
	{};
	coxmatch_ofb_eth_dst_masked(
			const coxmatch& oxm) :
				coxmatch_48_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_dst_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_dst_masked& oxm) {
		os << dynamic_cast<const coxmatch_48_masked&>(oxm);
		os << indent(2) << "<eth-dst: "
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
	coxmatch_ofb_eth_src(
			const cmacaddr& maddr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ETH_SRC, maddr)
	{};
	coxmatch_ofb_eth_src(const coxmatch& oxm) :
		coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_src& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << indent(2) << "<eth-src: "
				<< oxm.get_u48value_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ETH_SRC
 */
class coxmatch_ofb_eth_src_masked : public coxmatch_48_masked {
public:
	coxmatch_ofb_eth_src_masked(
			const cmacaddr& maddr, const cmacaddr& mask) :
				coxmatch_48_masked(rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK, maddr, mask)
	{};
	coxmatch_ofb_eth_src_masked(const coxmatch& oxm) :
		coxmatch_48_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_src_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_src_masked& oxm) {
		os << dynamic_cast<const coxmatch_48_masked&>(oxm);
		os << indent(2) << "<eth-src: "
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
	coxmatch_ofb_eth_type(
			uint16_t dl_type) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE, dl_type)
	{};
	coxmatch_ofb_eth_type(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_type& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<eth-type: 0x" << std::hex << (int)oxm.get_u16value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_vid : public coxmatch_16 {
public:
	coxmatch_ofb_vlan_vid(
			uint16_t vid) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_VLAN_VID, vid)
	{};
	coxmatch_ofb_vlan_vid(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_vid()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_vid& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << rofl::indent(2) << "<coxmatch_ofb_vlan_vid >" << std::endl;
		os << rofl::indent(4) << "<vlan-vid: 0x" << std::hex << (int)oxm.get_u16value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_vid_masked : public coxmatch_16_masked {
public:
	coxmatch_ofb_vlan_vid_masked(
			uint16_t vid, uint16_t mask) :
				coxmatch_16_masked(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK, vid, mask)
	{};
	coxmatch_ofb_vlan_vid_masked(
			const coxmatch& oxm) :
				coxmatch_16_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_vid_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_vid_masked& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << rofl::indent(2) << "<coxmatch_ofb_vlan_vid_masked >" << std::endl;
		os << rofl::indent(4) << "<vlan-vid: 0x" << std::hex << (int)oxm.get_u16value()
				<< "/0x" << (int)oxm.get_u16mask() << std::dec << " >" << std::endl;
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
			const coxmatch& oxm) :
		coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_untagged()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_untagged& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os << rofl::indent(2) << "<vlan-untagged >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_present : public coxmatch_16_masked {
public:
	coxmatch_ofb_vlan_present() :
		coxmatch_16_masked(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK,
				(uint16_t)rofl::openflow::OFPVID_PRESENT,
				(uint16_t)rofl::openflow::OFPVID_PRESENT)
	{};
	coxmatch_ofb_vlan_present(const coxmatch& oxm) :
		coxmatch_16_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_present()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_present& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os << rofl::indent(2) << "<vlan-present >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_PCP
 */
class coxmatch_ofb_vlan_pcp : public coxmatch_8 {
public:
	coxmatch_ofb_vlan_pcp(
			uint8_t pcp) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP, pcp)
	{};
	coxmatch_ofb_vlan_pcp(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_pcp()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_pcp& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<vlan-pcp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_DSCP
 */
class coxmatch_ofb_ip_dscp : public coxmatch_8 {
public:
	coxmatch_ofb_ip_dscp(
			uint8_t dscp) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_DSCP, dscp)
	{};
	coxmatch_ofb_ip_dscp(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_dscp()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_dscp& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ip-dscp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_ECN
 */
class coxmatch_ofb_ip_ecn : public coxmatch_8 {
public:
	coxmatch_ofb_ip_ecn(
			uint8_t ecn) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_ECN, ecn)
	{};
	coxmatch_ofb_ip_ecn(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_ecn()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_ecn& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ip-ecn: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_PROTO
 */
class coxmatch_ofb_ip_proto : public coxmatch_8 {
public:
	coxmatch_ofb_ip_proto(
			uint8_t proto) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_IP_PROTO, proto)
	{};
	coxmatch_ofb_ip_proto(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_proto()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_proto& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ip-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV4_SRC
 */
class coxmatch_ofb_ipv4_src : public coxmatch_32 {
public:
	coxmatch_ofb_ipv4_src(
			uint32_t src) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src)
	{};
	coxmatch_ofb_ipv4_src(
			const rofl::caddress_in4& src) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src)
	{};
	coxmatch_ofb_ipv4_src(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_src& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<ipv4-src: " << oxm.get_u32value_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV4_SRC
 */
class coxmatch_ofb_ipv4_src_masked : public coxmatch_32_masked {
public:
	coxmatch_ofb_ipv4_src_masked(
			uint32_t src, uint32_t mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv4_src_masked(
			const rofl::caddress_in4& src, const rofl::caddress_in4& mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv4_src_masked(
			const coxmatch& oxm) :
				coxmatch_32_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_src_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_src_masked& oxm) {
		os << dynamic_cast<const coxmatch_32_masked&>(oxm);
		os << indent(2) << "<ipv4-src: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV4_DST
 */
class coxmatch_ofb_ipv4_dst : public coxmatch_32 {
public:
	coxmatch_ofb_ipv4_dst(
			uint32_t dst) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst)
	{};
	coxmatch_ofb_ipv4_dst(
			const caddress_in4& dst) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst)
	{};
	coxmatch_ofb_ipv4_dst(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_dst& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<ipv4-dst: " << oxm.get_u32value_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV4_DST
 */
class coxmatch_ofb_ipv4_dst_masked : public coxmatch_32_masked {
public:
	coxmatch_ofb_ipv4_dst_masked(
			uint32_t dst, uint32_t mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv4_dst_masked(
			const caddress_in4& dst, const caddress_in4& mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv4_dst_masked(
			const coxmatch& oxm) :
				coxmatch_32_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_dst_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_dst_masked& oxm) {
		os << dynamic_cast<const coxmatch_32_masked&>(oxm);
		os << indent(2) << "<ipv4-dst: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_SRC
 */
class coxmatch_ofb_ipv6_src : public coxmatch_128 {
public:
	coxmatch_ofb_ipv6_src(
			const rofl::caddress_in6& src) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC, src)
	{};
	coxmatch_ofb_ipv6_src(
			const coxmatch& oxm) :
				coxmatch_128(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_src& oxm) {
		os << dynamic_cast<const coxmatch_128&>(oxm);
		os << indent(2) << "<ipv6-src: " << oxm.get_u128value().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_SRC
 */
class coxmatch_ofb_ipv6_src_masked : public coxmatch_128_masked {
public:
	coxmatch_ofb_ipv6_src_masked(
			const rofl::caddress_in6& src, const rofl::caddress_in6& mask) :
				coxmatch_128_masked(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv6_src_masked(
			const coxmatch& oxm) :
				coxmatch_128_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_src_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_src_masked& oxm) {
		os << dynamic_cast<const coxmatch_128_masked&>(oxm);
		os << indent(2) << "<ipv6-src: " << oxm.get_u128value().str() << "/";
		os << oxm.get_u128mask().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_DST
 */
class coxmatch_ofb_ipv6_dst : public coxmatch_128 {
public:
	coxmatch_ofb_ipv6_dst(
			const rofl::caddress_in6& dst) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_DST, dst)
	{};
	coxmatch_ofb_ipv6_dst(
			const coxmatch& oxm) :
				coxmatch_128(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_dst& oxm) {
		os << dynamic_cast<const coxmatch_128&>(oxm);
		os << indent(2) << "<ipv6-dst: " << oxm.get_u128value().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_DST
 */
class coxmatch_ofb_ipv6_dst_masked : public coxmatch_128_masked {
public:
	coxmatch_ofb_ipv6_dst_masked(
			const rofl::caddress_in6& dst, const rofl::caddress_in6& mask) :
				coxmatch_128_masked(rofl::openflow::OXM_TLV_BASIC_IPV6_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv6_dst_masked(
			const coxmatch& oxm) :
				coxmatch_128_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_dst_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_dst_masked& oxm) {
		os << dynamic_cast<const coxmatch_128_masked&>(oxm);
		os << indent(2) << "<ipv6-dst: " << oxm.get_u128value().str() << "/";
		os << oxm.get_u128mask().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_ND_TARGET
 */
class coxmatch_ofb_ipv6_nd_target : public coxmatch_128 {
public:
	coxmatch_ofb_ipv6_nd_target(
			const rofl::caddress_in6& nd_target) :
				coxmatch_128(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET, nd_target)
	{};
	coxmatch_ofb_ipv6_nd_target(
			const coxmatch& oxm) :
				coxmatch_128(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_target()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_target& oxm) {
		os << dynamic_cast<const coxmatch_128&>(oxm);
		os << rofl::indent(2) << "<ipv6-nd-target: " << oxm.get_u128value().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TCP_SRC
 */
class coxmatch_ofb_tcp_src : public coxmatch_16 {
public:
	coxmatch_ofb_tcp_src(
			uint16_t src) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_TCP_SRC, src)
	{};
	coxmatch_ofb_tcp_src(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_tcp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tcp_src& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<tcp-src: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TCP_DST
 */
class coxmatch_ofb_tcp_dst : public coxmatch_16 {
public:
	coxmatch_ofb_tcp_dst(
			uint16_t dst) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_TCP_DST, dst)
	{};
	coxmatch_ofb_tcp_dst(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_tcp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tcp_dst& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<tcp-dst: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_UDP_SRC
 */
class coxmatch_ofb_udp_src : public coxmatch_16 {
public:
	coxmatch_ofb_udp_src(
			uint16_t src) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_UDP_SRC, src)
	{};
	coxmatch_ofb_udp_src(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_udp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_udp_src& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<udp-src: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_UDP_DST
 */
class coxmatch_ofb_udp_dst : public coxmatch_16 {
public:
	coxmatch_ofb_udp_dst(
			uint16_t dst) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_UDP_DST, dst)
	{};
	coxmatch_ofb_udp_dst(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_udp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_udp_dst& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<udp-dst: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_SCTP_SRC
 */
class coxmatch_ofb_sctp_src : public coxmatch_16 {
public:
	coxmatch_ofb_sctp_src(
			uint16_t src) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC, src)
	{};
	coxmatch_ofb_sctp_src(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_sctp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_sctp_src& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<sctp-src: " << (int)oxm.get_u16value()	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_SCTP_DST
 */
class coxmatch_ofb_sctp_dst : public coxmatch_16 {
public:
	coxmatch_ofb_sctp_dst(
			uint16_t dst) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_SCTP_DST, dst)
	{};
	coxmatch_ofb_sctp_dst(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_sctp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_sctp_dst& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<sctp-dst: " << (int)oxm.get_u16value()	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV4_TYPE
 */
class coxmatch_ofb_icmpv4_type : public coxmatch_8 {
public:
	coxmatch_ofb_icmpv4_type(
			uint8_t type) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE, type)
	{};
	coxmatch_ofb_icmpv4_type(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv4_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv4_type& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv4-type: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV4_CODE
 */
class coxmatch_ofb_icmpv4_code : public coxmatch_8 {
public:
	coxmatch_ofb_icmpv4_code(
			uint8_t code) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE, code)
	{};
	coxmatch_ofb_icmpv4_code(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv4_code()
	{};
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv4_code& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv4-code: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ARP_OP
 */
class coxmatch_ofb_arp_opcode : public coxmatch_16 {
public:
	coxmatch_ofb_arp_opcode(
			uint16_t opcode) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_ARP_OP, opcode)
	{};
	coxmatch_ofb_arp_opcode(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_opcode()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_opcode& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<arp-opcode: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_SPA
 */
class coxmatch_ofb_arp_spa : public coxmatch_32 {
public:
	coxmatch_ofb_arp_spa(
			uint32_t spa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa)
	{};
	coxmatch_ofb_arp_spa(
			const caddress_in4& spa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa)
	{};
	coxmatch_ofb_arp_spa(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_spa()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_spa& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<arp-spa: " << oxm.get_u32value_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_SPA
 */
class coxmatch_ofb_arp_spa_masked : public coxmatch_32_masked {
public:
	coxmatch_ofb_arp_spa_masked(
			uint32_t spa, uint32_t mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK, spa, mask)
	{};
	coxmatch_ofb_arp_spa_masked(
			const caddress_in4& spa, const caddress_in4& mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK, spa, mask)
	{};
	coxmatch_ofb_arp_spa_masked(
			const coxmatch& oxm) :
				coxmatch_32_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_spa_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_spa_masked& oxm) {
		os << dynamic_cast<const coxmatch_32_masked&>(oxm);
		os << indent(2) << "<arp-spa: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_TPA
 */
class coxmatch_ofb_arp_tpa : public coxmatch_32 {
public:
	coxmatch_ofb_arp_tpa(
			uint32_t tpa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa)
	{};
	coxmatch_ofb_arp_tpa(
			const caddress_in4& tpa) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa)
	{};
	coxmatch_ofb_arp_tpa(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tpa()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tpa& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<arp-tpa: " << oxm.get_u32value_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_TPA
 */
class coxmatch_ofb_arp_tpa_masked : public coxmatch_32_masked {
public:
	coxmatch_ofb_arp_tpa_masked(
			uint32_t tpa, uint32_t mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK, tpa, mask)
	{};
	coxmatch_ofb_arp_tpa_masked(
			const caddress_in4& tpa, const caddress_in4& mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK, tpa, mask)
	{};
	coxmatch_ofb_arp_tpa_masked(
			const coxmatch& oxm) :
				coxmatch_32_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tpa_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tpa_masked& oxm) {
		os << dynamic_cast<const coxmatch_32_masked&>(oxm);
		os << indent(2) << "<arp-tpa: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_SHA
 */
class coxmatch_ofb_arp_sha : public coxmatch_48 {
public:
	coxmatch_ofb_arp_sha(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, lladdr)
	{};
	coxmatch_ofb_arp_sha(
			const cmacaddr& lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, lladdr)
	{};
	coxmatch_ofb_arp_sha(
			const coxmatch& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_sha()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_sha& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << indent(2) << "<arp-sha: "
				<< oxm.get_u48value_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_SHA
 */
class coxmatch_ofb_arp_sha_masked : public coxmatch_48_masked {
public:
	coxmatch_ofb_arp_sha_masked(
			uint64_t lladdr, uint64_t llmask) :
				coxmatch_48_masked(rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_sha_masked(
			const cmacaddr& lladdr, const cmacaddr& llmask) :
				coxmatch_48_masked(rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_sha_masked(
			const coxmatch& oxm) :
				coxmatch_48_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_sha_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_sha_masked& oxm) {
		os << dynamic_cast<const coxmatch_48_masked&>(oxm);
		os << indent(2) << "<arp-sha: "
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
	coxmatch_ofb_arp_tha(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_THA, lladdr)
	{};
	coxmatch_ofb_arp_tha(
			const cmacaddr& lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_ARP_THA, lladdr)
	{};
	coxmatch_ofb_arp_tha(
			const coxmatch& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tha()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tha& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << indent(2) << "<arp-tha: "
				<< oxm.get_u48value_as_lladdr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_THA
 */
class coxmatch_ofb_arp_tha_masked : public coxmatch_48_masked {
public:
	coxmatch_ofb_arp_tha_masked(
			uint64_t lladdr, uint64_t llmask) :
				coxmatch_48_masked(rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_tha_masked(
			const rofl::caddress_ll& lladdr, const rofl::caddress_ll& llmask) :
				coxmatch_48_masked(rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_tha_masked(
			const coxmatch& oxm) :
				coxmatch_48_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tha_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tha_masked& oxm) {
		os << dynamic_cast<const coxmatch_48_masked&>(oxm);
		os << indent(2) << "<arp-tha: "
				<< oxm.get_u48value_as_lladdr().str() << "/"
				<< oxm.get_u48mask_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_FLABEL
 */
class coxmatch_ofb_ipv6_flabel : public coxmatch_32 {
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL, flow_label)
	{};
	coxmatch_ofb_ipv6_flabel(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_flabel()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_flabel& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<ipv6-flabel: 0x" << std::hex
						<< (int)oxm.get_u32value()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_FLABEL
 */
class coxmatch_ofb_ipv6_flabel_masked : public coxmatch_32_masked {
public:
	coxmatch_ofb_ipv6_flabel_masked(
			uint32_t flow_label, uint32_t mask) :
				coxmatch_32_masked(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL_MASK, flow_label, mask)
	{};
	coxmatch_ofb_ipv6_flabel_masked(
			const coxmatch& oxm) :
				coxmatch_32_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_flabel_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_flabel_masked& oxm) {
		os << dynamic_cast<const coxmatch_32_masked&>(oxm);
		os << indent(2) << "<ipv6-flabel: 0x" << std::hex
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
	coxmatch_ofb_icmpv6_type(
			uint8_t type) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE, type)
	{};
	coxmatch_ofb_icmpv6_type(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv6_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv6_type& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv6-type: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV6_CODE
 */
class coxmatch_ofb_icmpv6_code : public coxmatch_8 {
public:
	coxmatch_ofb_icmpv6_code(
			uint8_t code) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE, code)
	{};
	coxmatch_ofb_icmpv6_code(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv6_code()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv6_code& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv6-code: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_ND_SLL
 */
class coxmatch_ofb_ipv6_nd_sll : public coxmatch_48 {
public:
	coxmatch_ofb_ipv6_nd_sll(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL, lladdr)
	{};
	coxmatch_ofb_ipv6_nd_sll(
			const cmacaddr& addr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL, addr)
	{};
	coxmatch_ofb_ipv6_nd_sll(
			const coxmatch& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_sll()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_sll& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << indent(2) << "<ipv6-nd-sll: "
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
	coxmatch_ofb_ipv6_nd_tll(
			uint64_t lladdr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL, lladdr)
	{};
	coxmatch_ofb_ipv6_nd_tll(
			const cmacaddr& addr) :
				coxmatch_48(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL, addr)
	{};
	coxmatch_ofb_ipv6_nd_tll(
			const coxmatch& oxm) :
				coxmatch_48(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_tll()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_tll& oxm) {
		os << dynamic_cast<const coxmatch_48&>(oxm);
		os << indent(2) << "<ipv6-nd-tll: "
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
	coxmatch_ofb_mpls_label(
			uint32_t mpls_label) :
				coxmatch_32(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL, mpls_label)
	{};
	coxmatch_ofb_mpls_label(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_label()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_label& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<mpls-label: 0x" << std::hex << (int)oxm.get_u32value() << std::dec	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_TC
 */
class coxmatch_ofb_mpls_tc : public coxmatch_8 {
public:
	coxmatch_ofb_mpls_tc(
			uint8_t mpls_tc) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_MPLS_TC, mpls_tc)
	{};
	coxmatch_ofb_mpls_tc(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_tc()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_tc& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<mpls-tc: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_BOS
 */
class coxmatch_ofb_mpls_bos : public coxmatch_8 {
public:
	coxmatch_ofb_mpls_bos(
			uint8_t mpls_bos) :
				coxmatch_8(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS, mpls_bos)
	{};
	coxmatch_ofb_mpls_bos(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_bos()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_bos& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<mpls-bos: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_TUNNEL_ID
 */
class coxmatch_ofb_tunnel_id : public coxmatch_64 {
public:
	coxmatch_ofb_tunnel_id(
			uint64_t tunnel_id) :
				coxmatch_64(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID, tunnel_id)
	{};
	coxmatch_ofb_tunnel_id(
			const coxmatch& oxm) :
				coxmatch_64(oxm)
	{};
	virtual
	~coxmatch_ofb_tunnel_id()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tunnel_id& oxm) {
		os << dynamic_cast<const coxmatch_64&>(oxm);
		os << indent(2) << "<tunnel-id: "
						<< std::hex
						<< (unsigned long long)oxm.get_u64value()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_TUNNEL_ID
 */
class coxmatch_ofb_tunnel_id_masked : public coxmatch_64_masked {
public:
	coxmatch_ofb_tunnel_id_masked(
			uint64_t tunnel_id, uint64_t mask) :
				coxmatch_64_masked(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID_MASK, tunnel_id, mask)
	{};
	coxmatch_ofb_tunnel_id_masked(
			const coxmatch& oxm) :
				coxmatch_64_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_tunnel_id_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tunnel_id_masked& oxm) {
		os << dynamic_cast<const coxmatch_64_masked&>(oxm);
		os << indent(2) << "<tunnel-id: "
						<< std::hex
						<< (unsigned long long)oxm.get_u64value() << "/" << (unsigned long long)oxm.get_u64mask()
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
	coxmatch_ofb_pbb_isid(
			uint32_t pbb_isid) :
				coxmatch_24(rofl::openflow::OXM_TLV_BASIC_PBB_ISID, pbb_isid)
	{};
	coxmatch_ofb_pbb_isid(
			const coxmatch& oxm) :
				coxmatch_24(oxm)
	{};
	virtual
	~coxmatch_ofb_pbb_isid()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_pbb_isid& oxm) {
		os << dynamic_cast<const coxmatch_24&>(oxm);
		os << indent(2) << "<pbb-isid: 0x"
						<< std::hex
						<< (unsigned int)oxm.get_u24value()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_PBB_ISID
 */
class coxmatch_ofb_pbb_isid_masked : public coxmatch_24_masked {
public:
	coxmatch_ofb_pbb_isid_masked(
			uint32_t pbb_isid, uint32_t mask) :
				coxmatch_24_masked(rofl::openflow::OXM_TLV_BASIC_PBB_ISID_MASK, pbb_isid, mask)
	{};
	coxmatch_ofb_pbb_isid_masked(
			const coxmatch& oxm) :
				coxmatch_24_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_pbb_isid_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_pbb_isid_masked& oxm) {
		os << dynamic_cast<const coxmatch_24_masked&>(oxm);
		os << indent(2) << "<pbb-isid: 0x"
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
	coxmatch_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr) :
				coxmatch_16(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR, ipv6_exthdr)
	{};
	coxmatch_ofb_ipv6_exthdr(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_exthdr()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_exthdr& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<ipv6-exthdr: "
						<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_EXTHDR
 */
class coxmatch_ofb_ipv6_exthdr_masked : public coxmatch_16_masked {
public:
	coxmatch_ofb_ipv6_exthdr_masked(
			uint16_t ipv6_exthdr, uint16_t mask) :
				coxmatch_16_masked(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR_MASK, ipv6_exthdr, mask)
	{};
	coxmatch_ofb_ipv6_exthdr_masked(
			const coxmatch& oxm) :
				coxmatch_16_masked(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_exthdr_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_exthdr_masked& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<ipv6-exthdr: "
						<< (int)oxm.get_u16value() << "/" << oxm.get_u16mask()
						<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_NW_PROTO (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_proto : public coxmatch_8 {
public:
	coxmatch_ofx_nw_proto(
			uint8_t proto) :
				coxmatch_8(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO, proto)
	{};
	coxmatch_ofx_nw_proto(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_proto()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_proto& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<nw-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_NW_TOS (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_tos : public coxmatch_8 {
public:
	coxmatch_ofx_nw_tos(
			uint8_t tos) :
				coxmatch_8(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS, tos)
	{};
	coxmatch_ofx_nw_tos(
			const coxmatch& oxm) :
				coxmatch_8(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_tos()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_tos& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<nw-tos: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_NW_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_src : public coxmatch_32 {
public:
	coxmatch_ofx_nw_src(
			uint32_t src) :
				coxmatch_32(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, src)
	{};
	coxmatch_ofx_nw_src(
			const caddress_in4& src) :
				coxmatch_32(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, src)
	{};
	coxmatch_ofx_nw_src(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_src& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<nw-src: "
				<< oxm.get_u32value_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_NW_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_src_masked : public coxmatch_32_masked {
public:
	coxmatch_ofx_nw_src_masked(
			uint32_t src, uint32_t mask) :
				coxmatch_32_masked(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, src, mask)
	{};
	coxmatch_ofx_nw_src_masked(
			const caddress_in4& src, const caddress_in4& mask) :
				coxmatch_32_masked(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, src, mask)
	{};
	coxmatch_ofx_nw_src_masked(
			const coxmatch& oxm) :
				coxmatch_32_masked(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_src_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_src_masked& oxm) {
		os << dynamic_cast<const coxmatch_32_masked&>(oxm);
		os << indent(2) << "<nw-src: "
				<< oxm.get_u32value_as_addr().str() << "/"
				<< oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_NW_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_dst : public coxmatch_32 {
public:
	coxmatch_ofx_nw_dst(
			uint32_t dst) :
				coxmatch_32(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, dst)
	{};
	coxmatch_ofx_nw_dst(
			const caddress_in4& dst) :
				coxmatch_32(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, dst)
	{};
	coxmatch_ofx_nw_dst(
			const coxmatch& oxm) :
				coxmatch_32(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_dst& oxm) {
		os << dynamic_cast<const coxmatch_32&>(oxm);
		os << indent(2) << "<nw-dst: "
				<< oxm.get_u32value_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_NW_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_dst_masked : public coxmatch_32_masked {
public:
	coxmatch_ofx_nw_dst_masked(
			uint32_t dst, uint32_t mask) :
				coxmatch_32_masked(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, dst, mask)
	{};
	coxmatch_ofx_nw_dst_masked(
			const caddress_in4& dst, const caddress_in4& mask) :
				coxmatch_32_masked(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, dst, mask)
	{};
	coxmatch_ofx_nw_dst_masked(
			const coxmatch& oxm) :
				coxmatch_32_masked(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_dst_masked()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_dst_masked& oxm) {
		os << dynamic_cast<const coxmatch_32_masked&>(oxm);
		os << indent(2) << "<nw-dst: "
				<< oxm.get_u32value_as_addr().str() << "/"
				<< oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_TP_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_tp_src : public coxmatch_16 {
public:
	coxmatch_ofx_tp_src(
			uint16_t src) :
				coxmatch_16(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC, src)
	{};
	coxmatch_ofx_tp_src(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofx_tp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_tp_src& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<tp-src: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TP_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_tp_dst : public coxmatch_16 {
public:
	coxmatch_ofx_tp_dst(
			uint16_t dst) :
				coxmatch_16(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST, dst)
	{};
	coxmatch_ofx_tp_dst(
			const coxmatch& oxm) :
				coxmatch_16(oxm)
	{};
	virtual
	~coxmatch_ofx_tp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_tp_dst& oxm) {
		os << dynamic_cast<const coxmatch_16&>(oxm);
		os << indent(2) << "<tp-dst: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


}; // end of namespace openflow
}; // end of namespace rofl




#endif /* COXMATCH_H_ */
