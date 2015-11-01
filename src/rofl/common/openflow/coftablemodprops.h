/*
 * coftablemodprops.h
 *
 *  Created on: 31.03.2015
 *      Author: andi
 */

#ifndef COFTABLEMODPROPS_H_
#define COFTABLEMODPROPS_H_ 1

#include <map>

#include "rofl/common/locking.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/coftablemodprop.h"

namespace rofl {
namespace openflow {

class coftablemod_props {
public:

	/**
	 *
	 */
	coftablemod_props(
		uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
			ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	virtual
	~coftablemod_props()
	{ clear(); };

	/**
	 *
	 */
	coftablemod_props(
		const coftablemod_props& props)
	{ *this = props; };

	/**
	 */
	coftablemod_props&
	operator= (
		const coftablemod_props& props);

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
		for (std::map<uint16_t, coftablemod_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			it->second->set_version(ofp_version);
		}
	};

	/**
	 *
	 */
	const std::map<uint16_t, coftablemod_prop*>&
	get_properties() const
	{ return properties; };

	/**
	 *
	 */
	std::map<uint16_t, coftablemod_prop*>&
	set_properties()
	{ return properties; };

public:

	/**
	 *
	 */
	void
	clear() {
		AcquireReadWriteLock lock(rwlock_props);
		for (std::map<uint16_t, coftablemod_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			delete it->second;
		}
		properties.clear();
	};

public:

	/**
	 * @brief	Add a new tablemod property of type eviction for specified portno
	 *
	 * Creates a new tablemod property of type eviction. Side effect: destroys any
	 * existing evictiontablemod property first.
	 *
	 * @return reference to new created coftablemod_prop_eviction instance
	 */
	coftablemod_prop_eviction&
	add_table_mod_eviction() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EVICTION) != properties.end()) {
			delete properties[rofl::openflow14::OFPTMPT_EVICTION];
		}
		properties[rofl::openflow14::OFPTMPT_EVICTION] = new coftablemod_prop_eviction(get_version());
		return dynamic_cast<coftablemod_prop_eviction&>(*(properties[rofl::openflow14::OFPTMPT_EVICTION]));
	};

	/**
	 * @brief	Return existing or create new tablemod property of type eviction
	 *
	 * Returns an existing or if none exists, creates a new tablemod property
	 * of type eviction.
	 *
	 * @return reference to existing or new coftablemod_prop_eviction instance
	 */
	coftablemod_prop_eviction&
	set_table_mod_eviction() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EVICTION) == properties.end()) {
			properties[rofl::openflow14::OFPTMPT_EVICTION] = new coftablemod_prop_eviction(get_version());
		}
		return dynamic_cast<coftablemod_prop_eviction&>(*(properties[rofl::openflow14::OFPTMPT_EVICTION]));
	};

	/**
	 * @brief	Return existing tablemod property of type eviction
	 *
	 * Returns a const reference to an existing tablemod property of type eviction.
	 * Throws an exception when tablemod property has no eviction tablemod property.
	 *
	 * @return const reference to existing coftablemod_prop_eviction instance
	 */
	const coftablemod_prop_eviction&
	get_table_mod_eviction() const {
		AcquireReadLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EVICTION) == properties.end()) {
			throw eTableModPropNotFound("coftablemodprops::get_table_mod_eviction() not found");
		}
		return dynamic_cast<const coftablemod_prop_eviction&>(*(properties.at(rofl::openflow14::OFPTMPT_EVICTION)));
	};

	/**
	 * @brief	Drop an existing tablemod property of type eviction
	 */
	void
	drop_table_mod_eviction() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EVICTION) == properties.end()) {
			return;
		}
		delete properties[rofl::openflow14::OFPTMPT_EVICTION];
		properties.erase(rofl::openflow14::OFPTMPT_EVICTION);
	};

	/**
	 * @brief	Returns true when an eviction tablemod property exists in this property list.
	 */
	bool
	has_table_mod_eviction() const {
		return (not (properties.find(rofl::openflow14::OFPTMPT_EVICTION) == properties.end()));
	};

public:

	/**
	 * @brief	Add a new tablemod property of type vacancy for specified portno
	 *
	 * Creates a new tablemod property of type vacancy. Side effect: destroys any
	 * existing vacancytablemod property first.
	 *
	 * @return reference to new created coftablemod_prop_vacancy instance
	 */
	coftablemod_prop_vacancy&
	add_table_mod_vacancy() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_VACANCY) != properties.end()) {
			delete properties[rofl::openflow14::OFPTMPT_VACANCY];
		}
		properties[rofl::openflow14::OFPTMPT_VACANCY] = new coftablemod_prop_vacancy(get_version());
		return dynamic_cast<coftablemod_prop_vacancy&>(*(properties[rofl::openflow14::OFPTMPT_VACANCY]));
	};

	/**
	 * @brief	Return existing or create new tablemod property of type vacancy
	 *
	 * Returns an existing or if none exists, creates a new tablemod property
	 * of type vacancy.
	 *
	 * @return reference to existing or new coftablemod_prop_vacancy instance
	 */
	coftablemod_prop_vacancy&
	set_table_mod_vacancy() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_VACANCY) == properties.end()) {
			properties[rofl::openflow14::OFPTMPT_VACANCY] = new coftablemod_prop_vacancy(get_version());
		}
		return dynamic_cast<coftablemod_prop_vacancy&>(*(properties[rofl::openflow14::OFPTMPT_VACANCY]));
	};

	/**
	 * @brief	Return existing tablemod property of type vacancy
	 *
	 * Returns a const reference to an existing tablemod property of type vacancy.
	 * Throws an exception when tablemod property has no vacancy tablemod property.
	 *
	 * @return const reference to existing coftablemod_prop_vacancy instance
	 */
	const coftablemod_prop_vacancy&
	get_table_mod_vacancy() const {
		AcquireReadLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_VACANCY) == properties.end()) {
			throw eTableModPropNotFound("coftablemodprops::get_table_mod_vacancy() not found");
		}
		return dynamic_cast<const coftablemod_prop_vacancy&>(*(properties.at(rofl::openflow14::OFPTMPT_VACANCY)));
	};

	/**
	 * @brief	Drop an existing tablemod property of type vacancy
	 */
	void
	drop_table_mod_vacancy() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_VACANCY) == properties.end()) {
			return;
		}
		delete properties[rofl::openflow14::OFPTMPT_VACANCY];
		properties.erase(rofl::openflow14::OFPTMPT_VACANCY);
	};

	/**
	 * @brief	Returns true when an vacancy tablemod property exists in this property list.
	 */
	bool
	has_table_mod_vacancy() const {
		return (not (properties.find(rofl::openflow14::OFPTMPT_VACANCY) == properties.end()));
	};

public:

	/**
	 * @brief	Add a new tablemod property of type experimenter for specified portno
	 *
	 * Creates a new tablemod property of type experimenter. Side effect: destroys any
	 * existing experimentertablemod property first.
	 *
	 * @return reference to new created coftablemod_prop_experimenter instance
	 */
	coftablemod_prop_experimenter&
	add_table_mod_experimenter() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EXPERIMENTER) != properties.end()) {
			delete properties[rofl::openflow14::OFPTMPT_EXPERIMENTER];
		}
		properties[rofl::openflow14::OFPTMPT_EXPERIMENTER] = new coftablemod_prop_experimenter(get_version());
		return dynamic_cast<coftablemod_prop_experimenter&>(*(properties[rofl::openflow14::OFPTMPT_EXPERIMENTER]));
	};

	/**
	 * @brief	Return existing or create new tablemod property of type experimenter
	 *
	 * Returns an existing or if none exists, creates a new tablemod property
	 * of type experimenter.
	 *
	 * @return reference to existing or new coftablemod_prop_experimenter instance
	 */
	coftablemod_prop_experimenter&
	set_table_mod_experimenter() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EXPERIMENTER) == properties.end()) {
			properties[rofl::openflow14::OFPTMPT_EXPERIMENTER] = new coftablemod_prop_experimenter(get_version());
		}
		return dynamic_cast<coftablemod_prop_experimenter&>(*(properties[rofl::openflow14::OFPTMPT_EXPERIMENTER]));
	};

	/**
	 * @brief	Return existing tablemod property of type experimenter
	 *
	 * Returns a const reference to an existing tablemod property of type experimenter.
	 * Throws an exception when tablemod property has no experimenter tablemod property.
	 *
	 * @return const reference to existing coftablemod_prop_experimenter instance
	 */
	const coftablemod_prop_experimenter&
	get_table_mod_experimenter() const {
		AcquireReadLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EXPERIMENTER) == properties.end()) {
			throw eTableModPropNotFound("coftablemodprops::get_table_mod_experimenter() not found");
		}
		return dynamic_cast<const coftablemod_prop_experimenter&>(*(properties.at(rofl::openflow14::OFPTMPT_EXPERIMENTER)));
	};

	/**
	 * @brief	Drop an existing tablemod property of type experimenter
	 */
	void
	drop_table_mod_experimenter() {
		AcquireReadWriteLock lock(rwlock_props);
		if (properties.find(rofl::openflow14::OFPTMPT_EXPERIMENTER) == properties.end()) {
			return;
		}
		delete properties[rofl::openflow14::OFPTMPT_EXPERIMENTER];
		properties.erase(rofl::openflow14::OFPTMPT_EXPERIMENTER);
	};

	/**
	 * @brief	Returns true when an experimenter tablemod property exists in this property list.
	 */
	bool
	has_table_mod_experimenter() const {
		return (not (properties.find(rofl::openflow14::OFPTMPT_EXPERIMENTER) == properties.end()));
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
	operator<< (std::ostream& os, coftablemod_props const& props) {
		os  << "<coftablemod_props version: " << (unsigned int)props.get_version()
				<< " #props: " << props.get_properties().size() << " >" << std::endl;
		
		for (std::map<uint16_t, coftablemod_prop*>::const_iterator
				it = props.properties.begin(); it != props.properties.end(); ++it) {
			switch (it->second->get_type()) {
			case rofl::openflow14::OFPTMPT_EVICTION: {
				os << dynamic_cast<const coftablemod_prop_eviction&>( *(it->second) );
			} break;
			case rofl::openflow14::OFPTMPT_VACANCY: {
				os << dynamic_cast<const coftablemod_prop_vacancy&>( *(it->second) );
			} break;
			case rofl::openflow14::OFPTMPT_EXPERIMENTER: {
				os << dynamic_cast<const coftablemod_prop_experimenter&>( *(it->second) );
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

	std::map<uint16_t, coftablemod_prop*>   properties;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFQUEUEPROPLIST_H_ */
