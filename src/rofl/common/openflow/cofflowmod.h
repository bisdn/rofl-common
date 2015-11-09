/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ROFL_COMMON_OPENFLOW_COFFLOWMOD_H
#define ROFL_COMMON_OPENFLOW_COFFLOWMOD_H 1

#include <string>
#include <vector>
#include <algorithm>
#include <endian.h>
#include <strings.h>

#include "rofl/common/exception.hpp"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/openflow/cofinstruction.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/coxmatch.h"

namespace rofl {
namespace openflow {

class cofflowmod {
public:

	/**
	 *
	 */
	virtual
	~cofflowmod()
	{};

	/**
	 *
	 */
	cofflowmod(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				ofp_version(ofp_version),
				match(ofp_version),
				actions(ofp_version),
				instructions(ofp_version),
				command(0),
				table_id(0),
				idle_timeout(0),
				hard_timeout(0),
				cookie(0),
				cookie_mask(0),
				priority(0),
				buffer_id(OFP_NO_BUFFER),
				out_port(OFPP_ANY),
				out_group(OFPG_ANY),
				flags(0)
	{};

	/**
	 *
	 */
	cofflowmod(
			const cofflowmod& flowmod)
	{ *this = flowmod; };

	/**
	 *
	 */
	cofflowmod&
	operator= (
			const cofflowmod& flowmod) {
		if (this == &flowmod)
			return *this;

		ofp_version		= flowmod.ofp_version;

		match 			= flowmod.match;
		actions			= flowmod.actions;
		instructions 	= flowmod.instructions;

		command			= flowmod.command;
		table_id		= flowmod.table_id;
		idle_timeout	= flowmod.idle_timeout;
		hard_timeout	= flowmod.hard_timeout;
		cookie			= flowmod.cookie;
		cookie_mask		= flowmod.cookie_mask;
		priority		= flowmod.priority;
		buffer_id		= flowmod.buffer_id;
		out_port		= flowmod.out_port;
		out_group		= flowmod.out_group;
		flags			= flowmod.flags;

		return *this;
	};

public:

	/**
	 *
	 */
	cofflowmod&
	clear() {
		match.clear();
		actions.clear();
		instructions.clear();
		return *this;
	};

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	cofflowmod&
	set_version(
			uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		match.set_version(ofp_version);
		actions.set_version(ofp_version);
		instructions.set_version(ofp_version);
		return *this;
	};

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 *
	 */
	rofl::openflow::cofmatch&
	set_match()
	{ return match; };

	/**
	 *
	 */
	cofflowmod&
	set_match(
			const rofl::openflow::cofmatch& match)
	{ (this->match = match).set_version(ofp_version); return *this; };

	/**
	 *
	 */
	const rofl::openflow::cofmatch&
	get_match() const
	{ return match; };

	/**
	 *
	 */
	rofl::openflow::cofactions&
	set_actions()
	{ return actions; };

	/**
	 *
	 */
	cofflowmod&
	set_actions(
			const rofl::openflow::cofactions& actions)
	{ (this->actions = actions).set_version(ofp_version); return *this; };

	/**
	 *
	 */
	const rofl::openflow::cofactions&
	get_actions() const
	{ return actions; };

	/**
	 *
	 */
	rofl::openflow::cofinstructions&
	set_instructions()
	{ return instructions; };

	/**
	 *
	 */
	cofflowmod&
	set_instructions(
			const rofl::openflow::cofinstructions& instructions)
	{ (this->instructions = instructions).set_version(ofp_version); return *this; };

	/**
	 *
	 */
	const rofl::openflow::cofinstructions&
	get_instructions() const
	{ return instructions; };

	/**
	 *
	 */
	cofflowmod&
	set_command(
			uint8_t command)
	{ this->command = command; return *this; };

	/**
	 *
	 */
	uint8_t
	get_command() const
	{ return command; };

	/**
	 *
	 */
	cofflowmod&
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
	cofflowmod&
	set_idle_timeout(
			uint16_t idle_timeout)
	{ this->idle_timeout = idle_timeout; return *this; };

	/**
	 *
	 */
	uint16_t
	get_idle_timeout() const
	{ return idle_timeout; };

	/**
	 *
	 */
	cofflowmod&
	set_hard_timeout(
			uint16_t hard_timeout)
	{ this->hard_timeout = hard_timeout; return *this; };

	/**
	 */
	uint16_t
	get_hard_timeout() const
	{ return hard_timeout; };

	/**
	 *
	 */
	cofflowmod&
	set_cookie(
			uint64_t cookie)
	{ this->cookie = cookie; return *this; };

	/**
	 *
	 */
	uint64_t
	get_cookie() const
	{ return cookie; };

	/**
	 *
	 */
	cofflowmod&
	set_cookie_mask(
			uint64_t cookie_mask)
	{ this->cookie_mask = cookie_mask; return *this; };

	/**
	 *
	 */
	uint64_t
	get_cookie_mask() const
	{ return cookie_mask; };

	/**
	 *
	 */
	cofflowmod&
	set_priority(
			uint16_t priority)
	{ this->priority = priority; return *this; };

	/**
	 *
	 */
	uint16_t
	get_priority() const
	{ return priority; };

	/**
	 *
	 */
	cofflowmod&
	set_buffer_id(
			uint32_t buffer_id)
	{ this->buffer_id = buffer_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_buffer_id() const
	{ return buffer_id; };

	/**
	 *
	 */
	cofflowmod&
	set_out_port(
			uint32_t out_port)
	{ this->out_port = out_port; return *this; };

	/**
	 *
	 */
	uint32_t
	get_out_port() const
	{ return out_port; };

	/**
	 *
	 */
	cofflowmod&
	set_out_group(
			uint32_t out_group)
	{ this->out_group = out_group; return *this; };

	/**
	 *
	 */
	uint32_t
	get_out_group() const
	{ return out_group; };

	/**
	 *
	 */
	cofflowmod&
	set_flags(
			uint16_t flags)
	{ this->flags = flags; return *this; };

	/**
	 *
	 */
	uint16_t
	get_flags() const
	{ return flags; };

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
	operator<< (std::ostream& os, cofflowmod const& fe) {
		os  << "<cofflowmod ofp-version:" << (int)fe.get_version() << " >" << std::endl;

		switch (fe.get_command()) {
		case rofl::openflow::OFPFC_ADD:
			os  << "<command: ADD >" 			<< std::endl; break;
		case rofl::openflow::OFPFC_DELETE:
			os  << "<command: DELETE >" 		<< std::endl; break;
		case rofl::openflow::OFPFC_DELETE_STRICT:
			os  << "<command: DELETE-STRICT >"<< std::endl; break;
		case rofl::openflow::OFPFC_MODIFY:
			os  << "<command: MODIFY >" 		<< std::endl; break;
		case rofl::openflow::OFPFC_MODIFY_STRICT:
			os  << "<command: MODIFY-STRICT >"<< std::endl; break;
		default:
			os  << "<command: UNKNOWN >" 		<< std::endl; break;
		}

		os << std::hex;
		os  << "<cookie: 0x" 		<< (unsigned long long)fe.get_cookie() 		<< " >" << std::endl;
		os  << "<cookie-mask: 0x"	<< (unsigned long long)fe.get_cookie_mask()	<< " >" << std::endl;
		os << std::dec;
		os  << "<table-id: " 		<< (unsigned int)fe.get_table_id() 		<< " >" << std::endl;
		os  << "<idle-timeout: " 	<< (unsigned int)fe.get_idle_timeout() 	<< " >" << std::endl;
		os  << "<hard-timeout: " 	<< (unsigned int)fe.get_hard_timeout() 	<< " >" << std::endl;

		os << std::hex;
		os  << "<priority: 0x" 	<< (unsigned int)fe.get_priority() 		<< " >" << std::endl;
		os  << "<buffer-id: 0x"	<< (unsigned int)fe.get_buffer_id() 	<< " >" << std::endl;
		os  << "<out-port: 0x" 	<< (unsigned int)fe.get_out_port() 		<< " >" << std::endl;
		os  << "<out-group: 0x"	<< (unsigned int)fe.get_out_group() 	<< " >" << std::endl;
		os  << "<flags: 0x" 		<< (unsigned int)fe.get_flags() 		<< " >" << std::endl;
		os << std::dec;

		
		os << fe.get_match();

		switch (fe.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			os << fe.get_actions();
		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << fe.get_instructions();
		} break;
		}

		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "command: " << (int)command << ", ";
		ss << "table_id: " << (int)table_id << ", ";
		ss << "idle_timeout: " << (int)idle_timeout << ", ";
		ss << "hard_timeout: " << (int)hard_timeout << ", ";
		ss << "cookie: " << (unsigned long long)cookie << ", ";
		ss << "cookie_mask: " << (unsigned long long)cookie_mask << ", ";
		ss << "priority: " << (int)priority << ", ";
		ss << "buffer_id: " << (int)buffer_id << ", ";
		ss << "out_port: " << (int)out_port << ", ";
		ss << "out_group: " << (int)out_group << ", ";
		ss << "flags: " << (int)flags << ", ";
		ss << "match: " << match << ", ";
		ss << "instructions: " << instructions << ", ";
		return ss.str();
	};

private: // data structures

	uint8_t 			ofp_version;

	cofmatch 			match;
	cofactions			actions;	// for OFP 1.0 only
	cofinstructions 	instructions;

	uint8_t				command;
	uint8_t				table_id;
	uint16_t			idle_timeout;
	uint16_t			hard_timeout;
	uint64_t			cookie;
	uint64_t			cookie_mask;
	uint16_t 			priority;
	uint32_t			buffer_id;
	uint32_t			out_port;
	uint32_t			out_group;
	uint16_t			flags;

	struct ofp10_flow_mod {
		struct rofl::openflow10::ofp_match match;
		uint64_t cookie;
		uint16_t command;
		uint16_t idle_timeout;
		uint16_t hard_timeout;
		uint16_t priority;
		uint32_t buffer_id;
		uint16_t out_port;
		uint16_t flags;
		uint8_t actions[0];
	} __attribute__((packed));

	struct ofp13_flow_mod {
		uint64_t cookie;
		uint64_t cookie_mask;
		uint8_t table_id;
		uint8_t command;
		uint16_t idle_timeout;
		uint16_t hard_timeout;
		uint16_t priority;
		uint32_t buffer_id;
		uint32_t out_port;
		uint32_t out_group;
		uint16_t flags;
		uint8_t pad[2];
		uint8_t match[0];
		//uint8_t instructions[0];
	} __attribute__((packed));
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFFLOWMOD_H */
