/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFPORTS_H_
#define COFPORTS_H_ 1

#include <algorithm>

#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofport.h"

namespace rofl {
namespace openflow {

class ePortsBase 		: public exception {
public:
	ePortsBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("ePortsBase");
	};
}; // base error class cofinlist
class ePortsNotFound 	: public ePortsBase {
public:
	ePortsNotFound(
			const std::string& __arg = std::string("")) :
				ePortsBase(__arg)
	{
		set_exception("ePortsNotFound");
	};
}; // element not found

class cofports
{
	uint8_t 						ofp_version;
	std::map<uint32_t, cofport*>	ports;

public: // methods

	/**
	 *
	 */
	cofports(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofports();

	/**
	 *
	 */
	cofports(
			uint8_t ofp_version, uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	cofports(
			cofports const& ports);

	/**
	 *
	 */
	cofports&
	operator= (
			cofports const& ports);

	/**
	 *
	 */
	cofports&
	operator+= (
			cofports const& ports);

public:

	/**
	 *
	 */
	void
	clear();


	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	void
	pack(uint8_t* buf, size_t buflen);


	/** returns required length for array of struct ofp_instruction
	 * for all instructions defined in this->invec
	 */
	size_t
	length() const;

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/**
	 *
	 */
	std::map<uint32_t, cofport*>&
	set_ports() { return ports; };

	/**
	 *
	 */
	std::map<uint32_t, cofport*> const&
	get_ports() const { return ports; };

public:

	/**
	 *
	 */
	cofport&
	add_port(uint32_t portno);

	/**
	 *
	 */
	cofport&
	set_port(uint32_t portno);

	/**
	 *
	 */
	cofport&
	set_port(const std::string& devname);

	/**
	 *
	 */
	cofport&
	set_port(const rofl::caddress_ll& hwaddr);

	/**
	 *
	 */
	cofport const&
	get_port(uint32_t portno) const;

	/**
	 *
	 */
	cofport const&
	get_port(const std::string& devname) const;

	/**
	 *
	 */
	cofport const&
	get_port(const rofl::caddress_ll& hwaddr) const;

	/**
	 *
	 */
	void
	drop_port(uint32_t portno);

	/**
	 *
	 */
	void
	drop_port(const std::string& devname);

	/**
	 *
	 */
	void
	drop_port(const rofl::caddress_ll& hwaddr);

	/**
	 *
	 */
	bool
	has_port(uint32_t portno) const;

	/**
	 *
	 */
	bool
	has_port(const std::string& devname) const;

	/**
	 *
	 */
	bool
	has_port(const rofl::caddress_ll& hwaddr) const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofports const& ports) {
		os  << "<cofports #ports:" << (int)ports.get_ports().size() << " >" << std::endl;;
		
		for (std::map<uint32_t, cofport*>::const_iterator
				it = ports.get_ports().begin(); it != ports.get_ports().end(); ++it) {
			os << *(it->second);
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
