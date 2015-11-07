/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coftables.h
 *
 *  Created on: 16.02.2014
 *      Author: andreas
 */

#ifndef ROFL_COMMON_OPENFLOW_COFTABLES_H
#define ROFL_COMMON_OPENFLOW_COFTABLES_H 1

#include <inttypes.h>
#include <map>

#include "rofl/common/openflow/coftablestatsarray.h"
#include "rofl/common/openflow/coftablefeatures.h"
#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"

namespace rofl {
namespace openflow {

class eTablesBase 		: public exception {
public:
	eTablesBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eOFTablesBase");
	};
};
class eTablesNotFound		: public eTablesBase {
public:
	eTablesNotFound(
			const std::string& __arg = std::string("")) :
				eTablesBase(__arg)
	{
		set_exception("eOFTablesNotFound");
	};
};


class coftables {
public:

	/**
	 *
	 */
	virtual
	~coftables()
	{};

	/**
	 *
	 */
	coftables(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version)
	{};

	/**
	 *
	 */
	coftables(
			const coftables& tables)
	{ *this = tables; };

	/**
	 *
	 */
	coftables&
	operator= (
			const coftables& tables) {
		if (this == &tables)
			return *this;

		ofp_version = tables.ofp_version;

		this->tables.clear();
		for (std::map<uint8_t, coftable_features>::const_iterator
				it = tables.tables.begin(); it != tables.tables.end(); ++it) {
			this->tables[it->first] = it->second;
		}

		return *this;
	};

	/**
	 *
	 */
	coftables&
	operator+= (
			const coftables& tables) {
		/*
		 * this operation may replace tables, if they use the same table-id
		 */
		for (std::map<uint8_t, coftable_features>::const_iterator
				it = tables.tables.begin(); it != tables.tables.end(); ++it) {
			this->tables[it->first] = it->second;
		}

		return *this;
	};

public:

	/**
	 *
	 */
	coftables&
	set_version(
			uint8_t ofp_version)
	{
		this->ofp_version = ofp_version;
		AcquireReadWriteLock lock(tables_lock);
		for (auto it : tables) {
			it.second.set_version(ofp_version);
		}
		return *this;
	};

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

	/**
	 *
	 */
	std::list<uint8_t>
	keys() const {
		AcquireReadLock lock(tables_lock);
		std::list<uint8_t> ids;
		for (auto it : tables) {
			ids.push_back(it.first);
		}
		return ids;
	};

	/**
	 *
	 */
	virtual void
	clear() {
		AcquireReadWriteLock lock(tables_lock);
		tables.clear();
	};

	/**
	 *
	 */
	coftable_features&
	add_table(
			uint8_t table_id) {
		AcquireReadWriteLock lock(tables_lock);
		if (tables.find(table_id) != tables.end()) {
			tables.erase(table_id);
		}
		tables[table_id] = coftable_features(ofp_version);
		tables[table_id].set_table_id(table_id);
		return tables[table_id];
	};

	/**
	 *
	 */
	coftable_features&
	set_table(
			uint8_t table_id) {
		AcquireReadWriteLock lock(tables_lock);
		if (tables.find(table_id) == tables.end()) {
			tables[table_id] = coftable_features(ofp_version);
			tables[table_id].set_table_id(table_id);
		}
		return tables[table_id];
	};

	/**
	 *
	 */
	const coftable_features&
	get_table(
			uint8_t table_id) const {
		AcquireReadLock lock(tables_lock);
		if (tables.find(table_id) == tables.end()) {
			throw eTablesNotFound();
		}
		return tables.at(table_id);
	};

	/**
	 *
	 */
	bool
	drop_table(
			uint8_t table_id) {
		AcquireReadWriteLock lock(tables_lock);
		if (tables.find(table_id) == tables.end()) {
			return false;
		}
		tables.erase(table_id);
		return true;
	};

	/**
	 *
	 */
	bool
	has_table(
			uint8_t table_id) const {
		AcquireReadLock lock(tables_lock);
		return (tables.find(table_id) != tables.end());
	};

public:

	/**
	 *
	 */
	static void
	map_tablestatsarray_to_tables(
			rofl::openflow::coftablestatsarray& tablestatsarray, rofl::openflow::coftables& tables);

	/**
	 *
	 */
	static void
	map_tables_to_tablestatsarray(
			rofl::openflow::coftables& tables, rofl::openflow::coftablestatsarray& tablestatsarray);

	/**
	 *
	 */
	static void
	map_match_to_prop_oxm(
			uint64_t match, rofl::openflow::coftable_feature_prop_oxm& prop_oxm);

	/**
	 *
	 */
	static void
	map_prop_oxm_to_match(
			const rofl::openflow::coftable_feature_prop_oxm& prop_oxm, uint64_t& match);

	/**
	 *
	 */
	static void
	map_actions_to_prop_actions(
			uint32_t actions, rofl::openflow::coftable_feature_prop_actions& prop_actions);

	/**
	 *
	 */
	static void
	map_prop_actions_to_actions(
		const rofl::openflow::coftable_feature_prop_actions& prop_actions, uint32_t& actions);

	/**
	 *
	 */
	static void
	map_instructions_to_prop_instructions(
			uint32_t instructions, rofl::openflow::coftable_feature_prop_instructions& prop_instructions);

	/**
	 *
	 */
	static void
	map_prop_instructions_to_instructions(
			const rofl::openflow::coftable_feature_prop_instructions& prop_instructions, uint32_t& instructions);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftables const& tables) {
		os  << "<coftables #tables:" << tables.tables.size() << " >" << std::endl;
		
		for (std::map<uint8_t, coftable_features>::const_iterator
				it = tables.tables.begin(); it != tables.tables.end(); ++it) {
			os << it->second;
		}
		return os;
	};

private:

	uint8_t 									ofp_version;
	std::map<uint8_t, coftable_features>		tables;
	rofl::crwlock                               tables_lock;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFTABLES_H */


