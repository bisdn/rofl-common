/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFPORT_H
#define COFPORT_H 1

#include <map>
#include <limits>

#include <string.h>
#include <endian.h>
#include <inttypes.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow_common.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/caddress.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofportstats.h"
#include "rofl/common/openflow/cofportdescprops.h"



#ifndef OFPP_UNSPECIFIED
#define OFPP_UNSPECIFIED 0
#endif



namespace rofl {
namespace openflow {

class ePortBase 			: public RoflException {
public:
	ePortBase(const std::string& __arg) : RoflException(__arg) {};
};
class ePortInval	 		: public ePortBase {
public:
	ePortInval(const std::string& __arg) : ePortBase(__arg) {};
};
class ePortNotFound 		: public ePortBase {
public:
	ePortNotFound(const std::string& __arg) : ePortBase(__arg) {};
};



class cofport {
public:

	/**
	 *
	 */
	~cofport()
	{};

	/**
	 *
	 */
	cofport(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version),
				portno(0),
				config(0),
				state(0),
				properties(ofp_version),
				port_stats(ofp_version)
	{
		if (rofl::openflow14::OFP_VERSION > ofp_version)
			add_ethernet();
	};

	/**
	 *
	 */
	cofport(
			uint8_t ofp_version,
			uint8_t *buf,
			size_t buflen) :
					ofp_version(ofp_version),
					portno(0),
					config(0),
					state(0),
					properties(ofp_version),
					port_stats(ofp_version)
	{ unpack(buf, buflen); };

	/**
	 *
	 */
	cofport(
			const cofport& port)
	{ *this = port; };

	/**
	 *
	 */
	cofport&
	operator= (
			const cofport& port) {
		if (this == &port)
			return *this;
		ofp_version	= port.ofp_version;
		portno		= port.portno;
		hwaddr		= port.hwaddr;
		name		= port.name;
		config		= port.config;
		state		= port.state;
		properties  = port.properties;
		port_stats 	= port.port_stats;
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

	/**
	 *
	 */
	uint8_t
	get_version() const;

public:

	/**
	 *
	 */
	void
	clear()
	{ properties.clear(); };

	/**
	 *
	 */
	const rofl::openflow::cofport_stats_reply&
	get_port_stats() const
	{ return port_stats; };

	/**
	 *
	 */
	rofl::openflow::cofport_stats_reply&
	set_port_stats()
	{ return port_stats; };

	/**
	 *
	 */
	const rofl::openflow::cofportdesc_props&
	get_properties() const
	{ return properties; };

	/**
	 *
	 */
	rofl::openflow::cofportdesc_props&
	set_properties()
	{ return properties; };

public:

	uint32_t
	get_port_no() const
	{ return portno; };

	void
	set_port_no(
			uint32_t portno)
	{ this->portno = portno; };

	const rofl::caddress_ll&
	get_hwaddr() const
	{ return hwaddr; };

	void
	set_hwaddr(
			const rofl::caddress_ll& hwaddr)
	{ this->hwaddr = hwaddr; };

	const std::string&
	get_name() const
	{ return name; };

	void
	set_name(
			const std::string& name)
	{ this->name = name; };

	uint32_t
	get_config() const
	{ return config; };

	void
	set_config(
			uint32_t config)
	{ this->config = config; };

	uint32_t
	get_state() const
	{ return state; };

	void
	set_state(
			uint32_t state)
	{ this->state = state; };

public:

	cofportdesc_prop_ethernet&
	add_ethernet()
	{ return properties.add_port_desc_ethernet(); };

	cofportdesc_prop_ethernet&
	set_ethernet()
	{ return properties.set_port_desc_ethernet(); };

	const cofportdesc_prop_ethernet&
	get_ethernet() const
	{ return properties.get_port_desc_ethernet(); };

	void
	drop_ethernet()
	{ properties.drop_port_desc_ethernet(); };

	bool
	has_ethernet() const
	{ return properties.has_port_desc_ethernet(); };

public:

	cofportdesc_prop_optical&
	add_optical()
	{ return properties.add_port_desc_optical(); };

	cofportdesc_prop_optical&
	set_optical()
	{ return properties.set_port_desc_optical(); };

	const cofportdesc_prop_optical&
	get_optical() const
	{ return properties.get_port_desc_optical(); };

	void
	drop_optical()
	{ properties.drop_port_desc_optical(); };

	bool
	has_optical() const
	{ return properties.has_port_desc_optical(); };

public:

	cofportdesc_prop_experimenter&
	add_experimenter()
	{ return properties.add_port_desc_experimenter(); };

	cofportdesc_prop_experimenter&
	set_experimenter()
	{ return properties.set_port_desc_experimenter(); };

	const cofportdesc_prop_experimenter&
	get_experimenter() const
	{ return properties.get_port_desc_experimenter(); };

	void
	drop_experimenter()
	{ properties.drop_port_desc_experimenter(); };

	bool
	has_experimenter() const
	{ return properties.has_port_desc_experimenter(); };

public:

	/**
	 *
	 */
	void
	link_state_set_blocked();

	/**
	 *
	 */
	void
	link_state_clr_blocked();

	/**
	 *
	 */
	bool
	link_state_is_blocked() const;

	/**
	 *
	 */
	void
	link_state_set_live();

	/**
	 *
	 */
	void
	link_state_clr_live();

	/**
	 *
	 */
	bool
	link_state_is_live() const;

	/**
	 *
	 */
	void
	link_state_set_link_down();

	/**
	 *
	 */
	void
	link_state_clr_link_down();

	/**
	 *
	 */
	bool
	link_state_is_link_down() const;

	/**
	 *
	 */
	void
	link_state_phy_down();

	/**
	 *
	 */
	void
	link_state_phy_up();

	/**
	 *
	 */
	bool
	link_state_phy_is_up() const;

	/**
	 *
	 */
	bool
	config_is_port_down() const;

	/**
	 *
	 */
	void
	recv_port_mod(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);

private:

	/**
	 *
	 */
	void
	recv_port_mod_of10(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);

	/**
	 *
	 */
	void
	recv_port_mod_of12(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);

public:


	friend std::ostream&
	operator<< (std::ostream& os, cofport const& port) {
		os << indent(0) << "<cofport >" << std::endl;
		os << indent(2) << "<portno: " << (int)port.get_port_no() << " >" << std::endl;
		os << indent(2) << "<hwaddr: " << port.get_hwaddr().str() << " >"	<< std::endl;
		os << indent(2) << "<name: " << port.get_name() << " >" << std::endl;
		os << indent(2) << "<config: " << (int)port.get_config()	<< " >" << std::endl;
		os << indent(2) << "<state: " << (int)port.get_state() << " >" << std::endl;
		if (port.has_ethernet()) {
			rofl::indent i(2); os << port.get_ethernet();
		}
		if (port.has_optical()) {
			rofl::indent i(2); os << port.get_optical();
		}
		if (port.has_experimenter()) {
			rofl::indent i(2); os << port.get_experimenter();
		}
		return os;
	};

private:

	uint8_t			    ofp_version;
	uint32_t			portno;
	rofl::caddress_ll	hwaddr;
	std::string			name;
	uint32_t			config;
	uint32_t			state;
	cofportdesc_props	properties;
	cofport_stats_reply	port_stats;
};


class cofport_find_by_port_no {
	uint32_t port_no;
public:
	cofport_find_by_port_no(uint32_t _port_no) :
		port_no(_port_no) {};
	bool operator() (cofport *ofport) {
		return (ofport->get_port_no() == port_no);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		return (p.second->get_port_no() == port_no);
	};
};


class cofport_find_by_port_name {
	std::string port_name;
public:
	cofport_find_by_port_name(std::string _port_name) :
		port_name(_port_name) {};
	bool operator() (cofport *ofport) {
		std::string name(ofport->get_name());
		return (name == port_name);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		std::string name(p.second->get_name());
		return (name == port_name);
	};
};



class cofport_find_by_maddr {
	cmacaddr maddr;
public:
	cofport_find_by_maddr(cmacaddr const& maddr) :
		maddr(maddr) {};
	bool operator() (cofport *ofport) {
		cmacaddr hwaddr(ofport->get_hwaddr());
		return (maddr == hwaddr);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		cmacaddr hwaddr(p.second->get_hwaddr());
		return (maddr == hwaddr);
	};
};


class cofport_find_port_no {
public:
	cofport_find_port_no(uint32_t port_no) :
		port_no(port_no) {};
	bool operator() (cofport const& ofport) {
		return (ofport.get_port_no() == port_no);
	};
	uint32_t port_no;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
