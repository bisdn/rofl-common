/*
 * cofportdescprops.h
 *
 *  Created on: 29.03.2015
 *      Author: andi
 */

#ifndef COFPORTDESCPROPS_H_
#define COFPORTDESCPROPS_H_ 1

#include <map>

#include "rofl/common/locking.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofportdescprop.h"

namespace rofl {
namespace openflow {

class cofportdesc_props {
public:

	/**
	 *
	 */
	cofportdesc_props(
		uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
			ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	virtual
	~cofportdesc_props()
	{ clear(); };

	/**
	 *
	 */
	cofportdesc_props(
		const cofportdesc_props& props)
	{ *this = props; };

	/**
	 */
	cofportdesc_props&
	operator= (
		const cofportdesc_props& props);

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
	set_version(uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		for (std::map<uint16_t, cofportdesc_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			it->second->set_version(ofp_version);
		}
	};

	/**
	 *
	 */
	const std::map<uint16_t, cofportdesc_prop*>&
	get_properties() const
	{ return properties; };

	/**
	 *
	 */
	std::map<uint16_t, cofportdesc_prop*>&
	set_properties()
	{ return properties; };

public:

	/**
	 *
	 */
	void
	clear() {
		AcquireReadWriteLock lock(rwlock_props);
		for (std::map<uint16_t, cofportdesc_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			delete it->second;
		}
		properties.clear();
	};

public:

	/**
	 * @brief	Add a new port description of type ethernet for specified portno
	 *
	 * Creates a new port description of type ethernet. Side effect: destroys any
	 * existing ethernetport description first.
	 *
	 * @return reference to new created cofportdesc_prop_ethernet instance
	 */
	cofportdesc_prop_ethernet&
	add_port_desc_ethernet() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_ETHERNET) != properties.end()) {
			delete properties[rofl::openflow14::OFPPDPT_ETHERNET];
		}
		properties[rofl::openflow14::OFPPDPT_ETHERNET] = new cofportdesc_prop_ethernet(get_version());
		return dynamic_cast<cofportdesc_prop_ethernet&>(*(properties[rofl::openflow14::OFPPDPT_ETHERNET]));
	};

	/**
	 * @brief	Return existing or create new port description of type ethernet
	 *
	 * Returns an existing or if none exists, creates a new port description
	 * of type ethernet.
	 *
	 * @return reference to existing or new cofportdesc_prop_ethernet instance
	 */
	cofportdesc_prop_ethernet&
	set_port_desc_ethernet() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_ETHERNET) == properties.end()) {
			properties[rofl::openflow14::OFPPDPT_ETHERNET] = new cofportdesc_prop_ethernet(get_version());
		}
		return dynamic_cast<cofportdesc_prop_ethernet&>(*(properties[rofl::openflow14::OFPPDPT_ETHERNET]));
	};

	/**
	 * @brief	Return existing port description of type ethernet
	 *
	 * Returns a const reference to an existing port description of type ethernet.
	 * Throws an exception when port description has no ethernet port description.
	 *
	 * @return const reference to existing cofportdesc_prop_ethernet instance
	 */
	const cofportdesc_prop_ethernet&
	get_port_desc_ethernet() const {
		AcquireReadLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_ETHERNET) == properties.end()) {
			throw ePortDescPropNotFound("cofportdescprops::get_port_desc_ethernet() not found");
		}
		return dynamic_cast<const cofportdesc_prop_ethernet&>(*(properties.at(rofl::openflow14::OFPPDPT_ETHERNET)));
	};

	/**
	 * @brief	Drop an existing port description of type ethernet
	 */
	void
	drop_port_desc_ethernet() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_ETHERNET) == properties.end()) {
			return;
		}
		delete properties[rofl::openflow14::OFPPDPT_ETHERNET];
		properties.erase(rofl::openflow14::OFPPDPT_ETHERNET);
	};

	/**
	 * @brief	Returns true when an ethernet port description exists in this property list.
	 */
	bool
	has_port_desc_ethernet() const {
		return (not (properties.find(rofl::openflow14::OFPPDPT_ETHERNET) == properties.end()));
	};

public:

	/**
	 * @brief	Add a new port description of type optical for specified portno
	 *
	 * Creates a new port description of type optical. Side effect: destroys any
	 * existing opticalport description first.
	 *
	 * @return reference to new created cofportdesc_prop_optical instance
	 */
	cofportdesc_prop_optical&
	add_port_desc_optical() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_OPTICAL) != properties.end()) {
			delete properties[rofl::openflow14::OFPPDPT_OPTICAL];
		}
		properties[rofl::openflow14::OFPPDPT_OPTICAL] = new cofportdesc_prop_optical(get_version());
		return dynamic_cast<cofportdesc_prop_optical&>(*(properties[rofl::openflow14::OFPPDPT_OPTICAL]));
	};

	/**
	 * @brief	Return existing or create new port description of type optical
	 *
	 * Returns an existing or if none exists, creates a new port description
	 * of type optical.
	 *
	 * @return reference to existing or new cofportdesc_prop_optical instance
	 */
	cofportdesc_prop_optical&
	set_port_desc_optical() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_OPTICAL) == properties.end()) {
			properties[rofl::openflow14::OFPPDPT_OPTICAL] = new cofportdesc_prop_optical(get_version());
		}
		return dynamic_cast<cofportdesc_prop_optical&>(*(properties[rofl::openflow14::OFPPDPT_OPTICAL]));
	};

	/**
	 * @brief	Return existing port description of type optical
	 *
	 * Returns a const reference to an existing port description of type optical.
	 * Throws an exception when port description has no optical port description.
	 *
	 * @return const reference to existing cofportdesc_prop_optical instance
	 */
	const cofportdesc_prop_optical&
	get_port_desc_optical() const {
		AcquireReadLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_OPTICAL) == properties.end()) {
			throw ePortDescPropNotFound("cofportdescprops::get_port_desc_optical() not found");
		}
		return dynamic_cast<const cofportdesc_prop_optical&>(*(properties.at(rofl::openflow14::OFPPDPT_OPTICAL)));
	};

	/**
	 * @brief	Drop an existing port description of type optical
	 */
	void
	drop_port_desc_optical() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_OPTICAL) == properties.end()) {
			return;
		}
		delete properties[rofl::openflow14::OFPPDPT_OPTICAL];
		properties.erase(rofl::openflow14::OFPPDPT_OPTICAL);
	};

	/**
	 * @brief	Returns true when an optical port description exists in this property list.
	 */
	bool
	has_port_desc_optical() const {
		return (not (properties.find(rofl::openflow14::OFPPDPT_OPTICAL) == properties.end()));
	};

public:

	/**
	 * @brief	Add a new port description of type experimenter for specified portno
	 *
	 * Creates a new port description of type experimenter. Side effect: destroys any
	 * existing experimenterport description first.
	 *
	 * @return reference to new created cofportdesc_prop_experimenter instance
	 */
	cofportdesc_prop_experimenter&
	add_port_desc_experimenter() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_EXPERIMENTER) != properties.end()) {
			delete properties[rofl::openflow14::OFPPDPT_EXPERIMENTER];
		}
		properties[rofl::openflow14::OFPPDPT_EXPERIMENTER] = new cofportdesc_prop_experimenter(get_version());
		return dynamic_cast<cofportdesc_prop_experimenter&>(*(properties[rofl::openflow14::OFPPDPT_EXPERIMENTER]));
	};

	/**
	 * @brief	Return existing or create new port description of type experimenter
	 *
	 * Returns an existing or if none exists, creates a new port description
	 * of type experimenter.
	 *
	 * @return reference to existing or new cofportdesc_prop_experimenter instance
	 */
	cofportdesc_prop_experimenter&
	set_port_desc_experimenter() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_EXPERIMENTER) == properties.end()) {
			properties[rofl::openflow14::OFPPDPT_EXPERIMENTER] = new cofportdesc_prop_experimenter(get_version());
		}
		return dynamic_cast<cofportdesc_prop_experimenter&>(*(properties[rofl::openflow14::OFPPDPT_EXPERIMENTER]));
	};

	/**
	 * @brief	Return existing port description of type experimenter
	 *
	 * Returns a const reference to an existing port description of type experimenter.
	 * Throws an exception when port description has no experimenter port description.
	 *
	 * @return const reference to existing cofportdesc_prop_experimenter instance
	 */
	const cofportdesc_prop_experimenter&
	get_port_desc_experimenter() const {
		AcquireReadLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_EXPERIMENTER) == properties.end()) {
			throw ePortDescPropNotFound("cofportdescprops::get_port_desc_experimenter() not found");
		}
		return dynamic_cast<const cofportdesc_prop_experimenter&>(*(properties.at(rofl::openflow14::OFPPDPT_EXPERIMENTER)));
	};

	/**
	 * @brief	Drop an existing port description of type experimenter
	 */
	void
	drop_port_desc_experimenter() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPPDPT_EXPERIMENTER) == properties.end()) {
			return;
		}
		delete properties[rofl::openflow14::OFPPDPT_EXPERIMENTER];
		properties.erase(rofl::openflow14::OFPPDPT_EXPERIMENTER);
	};

	/**
	 * @brief	Returns true when an experimenter port description exists in this property list.
	 */
	bool
	has_port_desc_experimenter() const {
		return (not (properties.find(rofl::openflow14::OFPPDPT_EXPERIMENTER) == properties.end()));
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const;



	/**
	 *
	 */
	virtual void
	unpack(
		uint8_t *buf, size_t buflen);



	/**
	 *
	 */
	virtual void
	pack(
		uint8_t *buf,
		size_t buflen);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofportdesc_props const& props) {
		os  << "<cofportdesc_props version: " << (unsigned int)props.get_version()
				<< " #props: " << props.get_properties().size() << " >" << std::endl;
		
		for (std::map<uint16_t, cofportdesc_prop*>::const_iterator
				it = props.properties.begin(); it != props.properties.end(); ++it) {
			switch (it->second->get_type()) {
			case rofl::openflow14::OFPPDPT_ETHERNET: {
				os << dynamic_cast<const cofportdesc_prop_ethernet&>( *(it->second) );
			} break;
			case rofl::openflow14::OFPPDPT_OPTICAL: {
				os << dynamic_cast<const cofportdesc_prop_optical&>( *(it->second) );
			} break;
			case rofl::openflow14::OFPPDPT_EXPERIMENTER: {
				os << dynamic_cast<const cofportdesc_prop_experimenter&>( *(it->second) );
			} break;
			default: {
				os << (*(it->second));
			}
			}
		}
		return os;
	};

private:

	uint8_t                                 ofp_version;
	mutable rofl::crwlock                   rwlock_props;
	std::map<uint16_t, cofportdesc_prop*>   properties;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFQUEUEPROPLIST_H_ */
