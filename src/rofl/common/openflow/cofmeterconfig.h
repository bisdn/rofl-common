/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterconfig.h
 *
 *  Created on: 26.05.2014
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFMETERCONFIG_H
#define ROFL_COMMON_OPENFLOW_COFMETERCONFIG_H 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofmeterbands.h"

namespace rofl {
namespace openflow {

class eRofMeterConfigBase		: public exception {
public:
	eRofMeterConfigBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eRofMeterConfigBase");
	};
};
class eRofMeterConfigNotFound	: public eRofMeterConfigBase {
public:
	eRofMeterConfigNotFound(
			const std::string& __arg = std::string("")) :
				eRofMeterConfigBase(__arg)
	{
		set_exception("eRofMeterConfigNotFound");
	};
};


class cofmeter_config_request {
public:

	/**
	 *
	 */
	~cofmeter_config_request()
	{};

	/**
	 *
	 */
	cofmeter_config_request(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0)  :
				of_version(of_version),
				meter_id(0)
	{
		if ((buflen == 0) || (nullptr == buf)) {
			return;
		}
		unpack(buf, buflen);
	};

	/**
	 *
	 */
	cofmeter_config_request(
			uint8_t of_version,
			uint32_t meter_id) :
				of_version(of_version),
				meter_id(meter_id)
	{};

	/**
	 *
	 */
	cofmeter_config_request(
			const cofmeter_config_request& req)
	{ *this = req; };

	/**
	 *
	 */
	cofmeter_config_request&
	operator= (
			const cofmeter_config_request& req) {
		if (this == &req)
			return *this;

		of_version 	= req.of_version;
		meter_id	= req.meter_id;

		return *this;
	};

	/**
	 *
	 */
	const bool
	operator== (
			const cofmeter_config_request& req) const
	{
		return (meter_id == req.meter_id);
	};

public:

	/**
	 *
	 */
	cofmeter_config_request&
	set_version(
			uint8_t of_version)
	{ this->of_version = of_version; return *this; };


	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };


	/**
	 *
	 */
	cofmeter_config_request&
	set_meter_id(
			uint32_t meter_id)
	{ this->meter_id = meter_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_meter_id() const
	{ return meter_id; };

public:

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_config_request const& r) {
		os  << "<cofmeter_config_request >" << std::endl;
		os << "<meter-id: 0x" << std::hex << (int)r.get_meter_id() << std::dec << " >" << std::endl;
		return os;
	};

private:

	uint8_t 	of_version;
	uint32_t	meter_id;
};



class cofmeter_config_reply {
public:

	/**
	 *
	 */
	~cofmeter_config_reply()
	{};

	/**
	 *
	 */
	cofmeter_config_reply(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0) :
				of_version(of_version),
				flags(0),
				meter_id(0),
				mbands(of_version)
	{
		if ((buflen == 0) || (nullptr == buf)) {
			return;
		}
		unpack(buf, buflen);
	};

	/**
	 *
	 */
	cofmeter_config_reply(
			uint8_t of_version,
			uint16_t flags,
			uint32_t meter_id,
			const rofl::openflow::cofmeter_bands& mbands)  :
				of_version(of_version),
				flags(flags),
				meter_id(meter_id),
				mbands(mbands)
	{ this->mbands.set_version(of_version); };

	/**
	 *
	 */
	cofmeter_config_reply(
			const cofmeter_config_reply& rep)
	{ *this = rep; };

	/**
	 *
	 */
	cofmeter_config_reply&
	operator= (
			const cofmeter_config_reply& rep) {
		if (this == &rep)
			return *this;

		of_version 		= rep.of_version;
		flags			= rep.flags;
		meter_id		= rep.meter_id;
		mbands			= rep.mbands;

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofmeter_config_reply& rep) {
		return ((of_version	== rep.of_version) &&
				(flags		== rep.flags) &&
				(meter_id	== rep.meter_id) &&
				(mbands		== rep.mbands));
	};

public:

	/**
	 *
	 */
	cofmeter_config_reply&
	set_version(
			uint8_t of_version)
	{ this->of_version = of_version; return *this; };

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };

	/**
	 *
	 */
	cofmeter_config_reply&
	set_flags(
			uint16_t flags)
	{ this->flags = flags; return *this; };

	/**
	 *
	 */
	uint16_t
	get_flags() const
	{ return flags; };

	/**
	 *
	 */
	cofmeter_config_reply&
	set_meter_id(
			uint32_t meter_id)
	{ this->meter_id = meter_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_meter_id() const
	{ return meter_id; };

	/**
	 *
	 */
	rofl::openflow::cofmeter_bands&
	set_meter_bands()
	{ return mbands; };

	/**
	 *
	 */
	cofmeter_config_reply&
	set_meter_bands(
			const rofl::openflow::cofmeter_bands& mbands)
	{ (this->mbands = mbands).set_version(of_version); return *this; };

	/**
	 *
	 */
	const rofl::openflow::cofmeter_bands&
	get_meter_bands() const
	{ return mbands; };

public:

	/**
	 *
	 */
	cofmeter_config_reply&
	clear() {
		flags = 0;
		meter_id = 0;
		mbands.clear();
		return *this;
	};

public:

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_config_reply const& r) {
		os  << "<cofmeter_config_reply >" << std::endl;
		os << std::hex;
		os << "<flags: 0x"		 		<< (unsigned int)r.get_flags() << " >" << std::endl;
		os << "<meter-id: 0x" 			<< (unsigned int)r.get_meter_id() << " >" << std::endl;
		os << std::dec;
		{  os << r.get_meter_bands(); }
		return os;
	};

private: // data structures

	uint8_t 						of_version;
	uint16_t						flags;
	uint32_t 						meter_id;
	rofl::openflow::cofmeter_bands 	mbands;
};

typedef cofmeter_config_reply cofmeter_config;

} /* end of namespace openflow */
} /* end of namespace rofl */

#endif /* ROFL_COMMON_OPENFLOW_COFMETERCONFIG_H */
