/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ROFL_COMMON_OPENFLOW_COFPORTS_H
#define ROFL_COMMON_OPENFLOW_COFPORTS_H 1

#include <algorithm>

#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"
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



class cofports {
public:

	/**
	 *
	 */
	~cofports()
	{ clear(); };

	/**
	 *
	 */
	cofports(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	cofports(
			uint8_t ofp_version, uint8_t *buf, size_t buflen)  :
				ofp_version(ofp_version)
	{
		if ((buf == nullptr) || (buflen == 0)) {
			return;
		}
		unpack(buf, buflen);
	};

	/**
	 *
	 */
	cofports(
			const cofports& ports)
	{ *this = ports; };

	/**
	 *
	 */
	cofports&
	operator= (
			const cofports& ports) {
		if (this == &ports)
			return *this;

		clear();

		ofp_version = ports.ofp_version;

		for (std::map<uint32_t, cofport*>::const_iterator
				it = ports.ports.begin(); it != ports.ports.end(); ++it) {
			this->ports[it->first] = new cofport(*it->second);
		}

		return *this;
	};

	/**
	 *
	 */
	cofports&
	operator+= (
			const cofports& ports) {
		/*
		 * this may replace existing ports in this cofports instance
		 */
		for (std::map<uint32_t, cofport*>::const_iterator
				it = ports.ports.begin(); it != ports.ports.end(); ++it) {
			if (this->ports.find(it->first) != this->ports.end()) {
				delete this->ports[it->first];
			}
			this->ports[it->first] = new cofport(*it->second);
		}

		return *this;
	};

public:

	/**
	 *
	 */
	cofports&
	set_version(
			uint8_t ofp_version)
	{ this->ofp_version = ofp_version; return *this; };

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

public:

	/**
	 *
	 */
	std::list<uint32_t>
	keys() const
	{
		AcquireReadLock rwlock(ports_lock);
		std::list<uint32_t> ids;
		for (auto it : ports) {
			ids.push_back(it.first);
		}
		return ids;
	};

	/**
	 *
	 */
	size_t
	size() const
	{
		AcquireReadLock lock(ports_lock);
		return ports.size();
	};

	/**
	 *
	 */
	cofports&
	clear()
	{
		AcquireReadWriteLock lock(ports_lock);
		for (auto it : ports) {
			delete it.second;
		}
		ports.clear();
		return *this;
	};

public:

	/**
	 *
	 */
	cofport&
	add_port(
			uint32_t portno) {
		AcquireReadWriteLock lock(ports_lock);
		if (ports.find(portno) != ports.end()) {
			ports.erase(portno);
		}
		ports[portno] = new cofport(ofp_version);
		ports[portno]->set_port_no(portno);
		return *(ports[portno]);
	};

	/**
	 *
	 */
	cofport&
	set_port(
			uint32_t portno) {
		AcquireReadWriteLock lock(ports_lock);
		if (ports.find(portno) == ports.end()) {
			ports[portno] = new cofport(ofp_version);
			ports[portno]->set_port_no(portno);
		}
		return *(ports[portno]);
	};

	/**
	 *
	 */
	const cofport&
	get_port(
			uint32_t portno) const {
		AcquireReadLock lock(ports_lock);
		if (ports.find(portno) == ports.end()) {
			throw ePortsNotFound("ePortsNotFound");
		}
		return *(ports.at(portno));
	};

	/**
	 *
	 */
	bool
	drop_port(
			uint32_t portno) {
		AcquireReadWriteLock lock(ports_lock);
		if (ports.find(portno) == ports.end()) {
			return false;
		}
		delete ports[portno];
		ports.erase(portno);
		return true;
	};

	/**
	 *
	 */
	bool
	has_port(
			uint32_t portno) const {
		AcquireReadLock lock(ports_lock);
		return (not (ports.find(portno) == ports.end()));
	};

public:

	/**
	 *
	 */
	cofport&
	set_port(
			const std::string& devname) {
		AcquireReadWriteLock lock(ports_lock);
		std::map<uint32_t, cofport*>::iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_port_name(devname))) == ports.end()) {
			throw ePortsNotFound("ePortsNotFound");
		}
		return *(it->second);
	};

	/**
	 *
	 */
	const cofport&
	get_port(
			const std::string& devname) const {
		AcquireReadLock lock(ports_lock);
		std::map<uint32_t, cofport*>::const_iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_port_name(devname))) == ports.end()) {
			throw ePortsNotFound("ePortsNotFound");
		}
		return *(it->second);
	};

	/**
	 *
	 */
	bool
	drop_port(
			const std::string& devname) {
		AcquireReadWriteLock lock(ports_lock);
		std::map<uint32_t, cofport*>::const_iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_port_name(devname))) == ports.end()) {
			return false;
		}
		delete it->second;
		ports.erase(it->first);
		return true;
	};

	/**
	 *
	 */
	bool
	has_port(
			const std::string& devname) const {
		AcquireReadLock lock(ports_lock);
		std::map<uint32_t, cofport*>::const_iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_port_name(devname))) == ports.end()) {
			return false;
		}
		return true;
	};

public:

	/**
	 *
	 */
	cofport&
	set_port(
			const rofl::caddress_ll& hwaddr) {
		AcquireReadWriteLock lock(ports_lock);
		std::map<uint32_t, cofport*>::iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_maddr(hwaddr))) == ports.end()) {
			throw ePortsNotFound("ePortsNotFound");
		}
		return *(it->second);
	};

	/**
	 *
	 */
	const cofport&
	get_port(
			const rofl::caddress_ll& hwaddr) const {
		AcquireReadLock lock(ports_lock);
		std::map<uint32_t, cofport*>::const_iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_maddr(hwaddr))) == ports.end()) {
			throw ePortsNotFound("ePortsNotFound");
		}
		return *(it->second);
	};

	/**
	 *
	 */
	bool
	drop_port(
			const rofl::caddress_ll& hwaddr) {
		AcquireReadWriteLock lock(ports_lock);
		std::map<uint32_t, cofport*>::const_iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_maddr(hwaddr))) == ports.end()) {
			return false;
		}
		delete it->second;
		ports.erase(it->first);
		return true;
	};

	/**
	 *
	 */
	bool
	has_port(
			const rofl::caddress_ll& hwaddr) const {
		AcquireReadLock lock(ports_lock);
		std::map<uint32_t, cofport*>::const_iterator it;
		if ((it = find_if(ports.begin(), ports.end(),
				cofport_find_by_maddr(hwaddr))) == ports.end()) {
			return false;
		}
		return true;
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
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofports const& ports) {
		os  << "<cofports #ports:" << (int)ports.size() << " >" << std::endl;;
		for (auto portno : ports.keys()) {
			os << ports.get_port(portno);
		}
		return os;
	};

private:

	uint8_t 						ofp_version;
	std::map<uint32_t, cofport*>	ports;
	mutable rofl::crwlock           ports_lock;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFPORTS_H */
