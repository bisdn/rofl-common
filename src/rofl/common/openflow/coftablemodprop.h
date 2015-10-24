/*
 * coftablemodprop.h
 *
 *  Created on: 31.03.2015
 *      Author: andi
 */

#ifndef COFTABLEMODPROP_H_
#define COFTABLEMODPROP_H_ 1

#include <assert.h>
#include <ostream>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {
namespace openflow {

class eTableModPropBase 		: public RoflException {
public:
	eTableModPropBase(const std::string& __arg) : RoflException(__arg) {};
};
class eTableModPropNotFound 	: public eTableModPropBase {
public:
	eTableModPropNotFound(const std::string& __arg) : eTableModPropBase(__arg) {};
};


class coftablemod_prop {
public:

	/**
	 *
	 */
	coftablemod_prop(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t type = 0,
			uint16_t len = sizeof(struct rofl::openflow14::ofp_table_mod_prop_header)) :
				ofp_version(of_version),
				type(type),
				len(len)
	{};

	/**
	 *
	 */
	virtual
	~coftablemod_prop()
	{};


	/**
	 *
	 */
	coftablemod_prop(
			const coftablemod_prop& prop)
	{ *this = prop; };


	/**
	 *
	 */
	coftablemod_prop&
	operator= (
			const coftablemod_prop& pdp) {
		if (this == &pdp)
			return *this;
		ofp_version = pdp.ofp_version;
		type		= pdp.type;
		len			= pdp.len;
		return *this;
	};


public:


	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (ofp_version) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_table_mod_prop_header));
		} break;
		default:
			throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
		}
	};


	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


public:

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 *
	 */
	void
	set_version(
			uint8_t ofp_version)
	{ this->ofp_version = ofp_version; };

	/**
	 *
	 */
	uint16_t
	get_type() const
	{ return type; };


	/**
	 *
	 */
	void
	set_type(
			uint16_t type)
	{ this->type = type; };


	/**
	 *
	 */
	uint16_t
	get_length() const
	{ return len; };

	/**
	 *
	 */
	void
	set_length(
			uint16_t len)
	{ this->len = len; };

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coftablemod_prop const& prop) {
		os << rofl::indent(0) << "<coftablemod_prop "
				<< "version: " << (int)prop.get_version() << " "
				<< std::hex
				<< "type: 0x" << (int)prop.get_type() << " "
				<< std::dec
				<< ">" << std::endl;
		return os;
	};


private:

	uint8_t 			ofp_version;
	uint16_t			type;
	mutable uint16_t 	len;
};



class coftablemod_prop_eviction : public coftablemod_prop {
public:

	/**
	 *
	 */
	coftablemod_prop_eviction(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				coftablemod_prop(ofp_version,
						rofl::openflow14::OFPTMPT_EVICTION,
						sizeof(struct rofl::openflow14::ofp_table_mod_prop_eviction)),
				flags(0)
	{};

	/**
	 *
	 */
	virtual
	~coftablemod_prop_eviction()
	{};

	/**
	 *
	 */
	coftablemod_prop_eviction(
			const coftablemod_prop_eviction& prop)
	{ *this = prop; };

	/**
	 *
	 */
	coftablemod_prop_eviction&
	operator= (
			const coftablemod_prop_eviction& prop) {
		if (this == &prop)
			return *this;
		coftablemod_prop::operator= (prop);
		flags = prop.flags;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (get_version()) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_table_mod_prop_eviction));
		} break;
		default:
			throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
		}
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	uint32_t
	get_flags() const
	{ return flags; };

	/**
	 *
	 */
	void
	set_flags(uint32_t flags)
	{ this->flags = flags; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const coftablemod_prop_eviction& prop) {
		os << rofl::indent(0) << "<coftablemod_prop_eviction version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const coftablemod_prop&>( prop ); }
		os << rofl::indent(4) << "<flags: 0x" << std::hex << (unsigned int)prop.get_flags() << std::dec << " >" << std::endl;
		return os;
	};

private:

	uint32_t flags;
};

class coftablemod_prop_vacancy : public coftablemod_prop {
public:

	/**
	 *
	 */
	coftablemod_prop_vacancy(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				coftablemod_prop(ofp_version,
						rofl::openflow14::OFPTMPT_VACANCY,
						sizeof(struct rofl::openflow14::ofp_table_mod_prop_vacancy)),
				vacancy_down(0),
				vacancy_up(0),
				vacancy(0)
	{};

	/**
	 *
	 */
	virtual
	~coftablemod_prop_vacancy()
	{};

	/**
	 *
	 */
	coftablemod_prop_vacancy(
			const coftablemod_prop_vacancy& prop)
	{ *this = prop; };

	/**
	 *
	 */
	coftablemod_prop_vacancy&
	operator= (
			const coftablemod_prop_vacancy& prop) {
		if (this == &prop)
			return *this;
		coftablemod_prop::operator= (prop);
		vacancy_down = prop.vacancy_down;
		vacancy_up   = prop.vacancy_up;
		vacancy      = prop.vacancy;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (get_version()) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_table_mod_prop_vacancy));
		} break;
		default:
			throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
		}
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	uint8_t
	get_vacancy_down() const
	{ return vacancy_down; };

	/**
	 *
	 */
	void
	set_vacancy_down(uint8_t vacancy_down)
	{ this->vacancy_down = vacancy_down; };

	/**
	 *
	 */
	uint8_t
	get_vacancy_up() const
	{ return vacancy_up; };

	/**
	 *
	 */
	void
	set_vacancy_up(uint8_t vacancy_up)
	{ this->vacancy_up = vacancy_up; };

	/**
	 *
	 */
	uint8_t
	get_vacancy() const
	{ return vacancy; };

	/**
	 *
	 */
	void
	set_vacancy(uint8_t vacancy)
	{ this->vacancy = vacancy; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const coftablemod_prop_vacancy& prop) {
		os << rofl::indent(0) << "<coftablemod_prop_vacancy version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const coftablemod_prop&>( prop ); }
		os << rofl::indent(4) << "<vacancy_down: 0x" << std::hex << (unsigned int)prop.get_vacancy_down() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<vacancy_up: 0x" << std::hex << (unsigned int)prop.get_vacancy_up() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<vacancy: 0x" << std::hex << (unsigned int)prop.get_vacancy() << std::dec << " >" << std::endl;
		return os;
	};

private:

	uint8_t vacancy_down;
	uint8_t vacancy_up;
	uint8_t vacancy;
};

class coftablemod_prop_experimenter : public coftablemod_prop {
public:

	/**
	 *
	 */
	coftablemod_prop_experimenter(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				coftablemod_prop(ofp_version,
						rofl::openflow14::OFPPDPT_EXPERIMENTER,
						sizeof(struct rofl::openflow14::ofp_table_mod_prop_experimenter)),
				exp_id(0),
				exp_type(0)
	{};

	/**
	 *
	 */
	virtual
	~coftablemod_prop_experimenter()
	{};

	/**
	 *
	 */
	coftablemod_prop_experimenter(
			const coftablemod_prop_experimenter& prop)
	{ *this = prop; };

	/**
	 *
	 */
	coftablemod_prop_experimenter&
	operator= (
			const coftablemod_prop_experimenter& prop) {
		if (this == &prop)
			return *this;
		coftablemod_prop::operator= (prop);
		exp_id		= prop.exp_id;
		exp_type	= prop.exp_type;
		exp_body 	= prop.exp_body;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (get_version()) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_table_mod_prop_experimenter) + exp_body.memlen());
		} break;
		default:
			throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
		}
	};

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	uint32_t
	get_exp_id() const
	{ return exp_id; };

	/**
	 *
	 */
	void
	set_exp_id(uint32_t exp_id)
	{ this->exp_id = exp_id; };

	/**
	 *
	 */
	uint32_t
	get_exp_type() const
	{ return exp_type; };

	/**
	 *
	 */
	void
	set_exp_type(uint32_t exp_type)
	{ this->exp_type = exp_type; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_exp_body() const
	{ return exp_body; };

	/**
	 *
	 */
	rofl::cmemory&
	set_exp_body()
	{ return exp_body; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const coftablemod_prop_experimenter& prop) {
		os << rofl::indent(0) << "<coftablemod_prop_experimenter version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const coftablemod_prop&>( prop ); }
		os << rofl::indent(4) << "<exp-id: 0x" << std::hex << (unsigned int)prop.get_exp_id() << " >" << std::dec << std::endl;
		os << rofl::indent(4) << "<exp-type: 0x" << std::hex << (unsigned int)prop.get_exp_type() << " >" << std::dec << std::endl;
		{ rofl::indent i(4); os << prop.get_exp_body(); }
		return os;
	};

private:

	uint32_t 		exp_id;
	uint32_t		exp_type;
	rofl::cmemory 	exp_body;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFQUEUEPROP_H_ */
