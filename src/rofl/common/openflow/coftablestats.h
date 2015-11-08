/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coftablestats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFTABLESTATS_H
#define ROFL_COMMON_OPENFLOW_COFTABLESTATS_H 1

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eTableStatsBase			: public exception {
public:
	eTableStatsBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eTableStatsBase");
	};
};
class eTableStatsNotFound		: public eTableStatsBase {
public:
	eTableStatsNotFound(
			const std::string& __arg = std::string("")) :
				eTableStatsBase(__arg)
	{
		set_exception("eTableStatsNotFound");
	};
};

class coftable_stats_reply {
public:

	/**
	 *
	 */
	~coftable_stats_reply()
	{};

	/**
	 *
	 */
	coftable_stats_reply(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN)  :
				of_version(of_version),
				table_id(0xff),
				match(0),
				wildcards(0),
				write_actions(0),
				apply_actions(0),
				write_setfields(0),
				apply_setfields(0),
				metadata_match(0),
				metadata_write(0),
				instructions(0),
				config(0),
				max_entries(0),
				active_count(0),
				lookup_count(0),
				matched_count(0)
	{};

	/** OF1.0
	 *
	 */
	coftable_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			std::string name,
			uint32_t wildcards,
			uint32_t max_entries,
			uint32_t active_count,
			uint64_t lookup_count,
			uint64_t matched_count)  :
				of_version(of_version),
				table_id(table_id),
				name(name),
				// not used by OF1.0
				match(0),
				wildcards(wildcards),
				write_actions(0),
				apply_actions(0),
				write_setfields(0),
				apply_setfields(0),
				metadata_match(0),
				metadata_write(0),
				instructions(0),
				config(0),
				// used by OF1.0
				max_entries(max_entries),
				active_count(active_count),
				lookup_count(lookup_count),
				matched_count(matched_count)
	{};

	/** OF1.2
	 *
	 */
	coftable_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			std::string name,
			uint64_t match,
			uint64_t wildcards,
			uint32_t write_actions,
			uint32_t apply_actions,
			uint64_t write_setfields,
			uint64_t apply_setfields,
			uint64_t metadata_match,
			uint64_t metadata_write,
			uint32_t instructions,
			uint32_t config,
			uint32_t max_entries,
			uint32_t active_count,
			uint64_t lookup_count,
			uint64_t matched_count)  :
				of_version(of_version),
				table_id(table_id),
				name(name),
				match(match),
				wildcards(wildcards),
				write_actions(write_actions),
				apply_actions(apply_actions),
				write_setfields(write_setfields),
				apply_setfields(apply_setfields),
				metadata_match(metadata_match),
				metadata_write(metadata_write),
				instructions(instructions),
				config(config),
				max_entries(max_entries),
				active_count(active_count),
				lookup_count(lookup_count),
				matched_count(matched_count)
	{};

	/** OF1.3
	 *
	 */
	coftable_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			uint32_t active_count,
			uint64_t lookup_count,
			uint64_t matched_count) :
				of_version(of_version),
				table_id(table_id),
				// not used by OF1.3
				match(0),
				wildcards(0),
				write_actions(0),
				apply_actions(0),
				write_setfields(0),
				apply_setfields(0),
				metadata_match(0),
				metadata_write(0),
				instructions(0),
				config(0),
				max_entries(0),
				// used by OF1.3
				active_count(active_count),
				lookup_count(lookup_count),
				matched_count(matched_count)
	{};

	/**
	 *
	 */
	coftable_stats_reply(
			const coftable_stats_reply& rep)
	{ *this = rep; };

	/**
	 *
	 */
	coftable_stats_reply&
	operator= (
			const coftable_stats_reply& rep) {
		if (this == &rep)
			return *this;

		of_version 		= rep.of_version;
		table_id		= rep.table_id;
		name			= rep.name;
		match			= rep.match;
		wildcards		= rep.wildcards;
		write_actions	= rep.write_actions;
		apply_actions	= rep.apply_actions;
		write_setfields	= rep.write_setfields;
		apply_setfields	= rep.apply_setfields;
		metadata_match	= rep.metadata_match;
		metadata_write	= rep.metadata_write;
		instructions	= rep.instructions;
		config			= rep.config;
		max_entries		= rep.max_entries;
		active_count	= rep.active_count;
		lookup_count	= rep.lookup_count;
		matched_count	= rep.matched_count;

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			coftable_stats_reply const& rep) const {
		return ((of_version 		== rep.of_version) &&
				(table_id 			== rep.table_id) &&
				(name 				== rep.name) &&
				(match 				== rep.match) &&
				(wildcards 			== rep.wildcards) &&
				(write_actions 		== rep.write_actions) &&
				(apply_actions 		== rep.apply_actions) &&
				(write_setfields 	== rep.write_setfields) &&
				(apply_setfields 	== rep.apply_setfields) &&
				(metadata_match 	== rep.metadata_match) &&
				(metadata_write 	== rep.metadata_write) &&
				(instructions 		== rep.instructions) &&
				(config 			== rep.config) &&
				(max_entries 		== rep.max_entries) &&
				(active_count 		== rep.active_count) &&
				(lookup_count 		== rep.lookup_count) &&
				(matched_count 		== rep.matched_count));
	};

public:

	/**
	 *
	 */
	coftable_stats_reply&
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
	coftable_stats_reply&
	set_table_id(
			uint8_t table_id)
	{ this->table_id = table_id; return *this; };

	/**
	 *
	 */
	uint8_t
	get_table_id() const
	{ return table_id; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_name(
			const std::string& name)
	{ this->name = name; return *this; };

	/**
	 *
	 */
	std::string const&
	get_name() const
	{ return name; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_match(
			uint64_t match)
	{ this->match = match; return *this; };

	/**
	 *
	 */
	uint64_t
	get_match() const
	{ return match; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_wildcards(
			uint64_t wildcards)
	{ this->wildcards = wildcards; return *this; };

	/**
	 *
	 */
	uint64_t
	get_wildcards() const
	{ return wildcards; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_write_actions(
			uint32_t write_actions)
	{ this->write_actions = write_actions; return *this; };

	/**
	 *
	 */
	uint32_t
	get_write_actions() const
	{ return write_actions; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_apply_actions(
			uint32_t apply_actions)
	{ this->apply_actions = apply_actions; return *this; };

	/**
	 *
	 */
	uint32_t
	get_apply_actions() const
	{ return apply_actions; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_write_setfields(
			uint64_t write_setfields)
	{ this->write_setfields = write_setfields; return *this; };

	/**
	 *
	 */
	uint64_t
	get_write_setfields() const
	{ return write_setfields; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_apply_setfields(
			uint64_t apply_setfields)
	{ this->apply_setfields = apply_setfields; return *this; };

	/**
	 *
	 */
	uint64_t
	get_apply_setfields() const
	{ return apply_setfields; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_metadata_match(
			uint64_t metadata_match)
	{ this->metadata_match = metadata_match; return *this; };

	/**
	 *
	 */
	uint64_t
	get_metadata_match() const
	{ return metadata_match; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_metadata_write(
			uint64_t metadata_write)
	{ this->metadata_write = metadata_write; return *this; };

	/**
	 *
	 */
	uint64_t
	get_metadata_write() const
	{ return metadata_write; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_instructions(
			uint32_t instructions)
	{ this->instructions = instructions; return *this; };

	/**
	 *
	 */
	uint32_t
	get_instructions() const
	{ return instructions; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_config(
			uint32_t config)
	{ this->config = config; return *this; };

	/**
	 *
	 */
	uint32_t
	get_config() const
	{ return config; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_max_entries(
			uint32_t max_entries)
	{ this->max_entries = max_entries; return *this; };

	/**
	 *
	 */
	uint32_t
	get_max_entries() const
	{ return max_entries; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_active_count(
			uint32_t active_count)
	{ this->active_count = active_count; return *this; };

	/**
	 *
	 */
	uint32_t
	get_active_count() const
	{ return active_count; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_lookup_count(
			uint64_t lookup_count)
	{ this->lookup_count = lookup_count; return *this; };

	/**
	 *
	 */
	uint64_t
	get_lookup_count() const
	{ return lookup_count; };

	/**
	 *
	 */
	coftable_stats_reply&
	set_matched_count(
			uint64_t matched_count)
	{ this->matched_count = matched_count; return *this; };

	/**
	 *
	 */
	uint64_t
	get_matched_count() const
	{ return matched_count; };

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
			uint8_t *buf, size_t buflen) const;

	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_stats_reply const& tsr) {
		switch (tsr.of_version) {
		case rofl::openflow10::OFP_VERSION: {
			os  << "<coftable_stats_reply ";
					os << "table-id:" << (int)(tsr.table_id) << " ";
					os << "name:" << tsr.name << " ";
					os << std::hex;
					os << "match: 0x" << (unsigned long long)tsr.match << " ";
					os << "wildcards: 0x" << (unsigned long long)tsr.wildcards << " >" << std::endl;

					os << "<max_entries: 0x" << (unsigned int)tsr.max_entries << " ";
					os << "active-count: 0x" << (unsigned int)tsr.active_count << " ";
					os << "lookup-count: 0x" << (unsigned long long)tsr.lookup_count << " ";
					os << "matched-count: 0x" << (unsigned long long)tsr.matched_count << " ";
					os << std::dec;
					os << ">" << std::endl;
		} break;
		case rofl::openflow12::OFP_VERSION: {
			os  << "<coftable_stats_reply ";
					os << "table-id:" << (int)(tsr.table_id) << " ";
					os << "name:" << tsr.name << " ";
					os << std::hex;
					os << "match: 0x" << (unsigned long long)tsr.match << " ";
					os << "wildcards: 0x" << (unsigned long long)tsr.wildcards << " >" << std::endl;
					os << "<write-actions: 0x" << (unsigned int)tsr.write_actions << " ";
					os << "apply-actions: 0x" << (unsigned int)tsr.apply_actions << " ";
					os << "write-setfields: 0x" << (unsigned long long)tsr.write_setfields << " ";
					os << "apply-setfields: 0x" << (unsigned long long)tsr.apply_setfields << " >" << std::endl;;
					os << "<metadata-match: 0x" << (unsigned long long)tsr.metadata_match << " ";
					os << "metadata-write: 0x" << (unsigned long long)tsr.metadata_write << " ";
					os << "instructions: 0x" << (unsigned int)tsr.instructions << " ";
					os << std::dec;
					os << "config:" << (unsigned int)tsr.config << " >" << std::endl;;
					os << "<max_entries:" << (unsigned int)tsr.max_entries << " ";
					os << "active-count:" << (unsigned int)tsr.active_count << " ";
					os << "lookup-count:" << (unsigned long long)tsr.lookup_count << " ";
					os << "matched-count:" << (unsigned long long)tsr.matched_count << " >" << std::endl;
		} break;
		case rofl::openflow13::OFP_VERSION: {
			os  << "<coftable_stats_reply ofp-version:" << (int)tsr.of_version << " >" << std::endl;
			os << std::hex;
			os << "<table-id: 0x" << (int)(tsr.table_id) << " >" << std::endl;
			os << "<active-count: 0x" << (unsigned int)tsr.active_count << " >" << std::endl;
			os << "<lookup-count: 0x" << (unsigned long long)tsr.lookup_count << " >" << std::endl;
			os << "<matched-count: 0x" << (unsigned long long)tsr.matched_count << " >" << std::endl;
			os << std::dec;
		} break;
		default: {
			os  << "<coftable_stats_reply >";
		};
		}
		return os;
	};

private:

	uint8_t 		of_version;
	uint8_t			table_id;
	std::string		name;
	uint64_t 		match;
	uint64_t		wildcards;
	uint32_t		write_actions;
	uint32_t 		apply_actions;
	uint64_t		write_setfields;
	uint64_t		apply_setfields;
	uint64_t		metadata_match;
	uint64_t		metadata_write;
	uint32_t		instructions;
	uint32_t		config;
	uint32_t		max_entries;
	uint32_t		active_count;
	uint64_t		lookup_count;
	uint64_t		matched_count;
};

}
}

#endif /* ROFL_COMMON_OPENFLOW_COFTABLESTATS_H */
