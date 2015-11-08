/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFFLOWSTATS_H
#define ROFL_COMMON_OPENFLOW_COFFLOWSTATS_H 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/exception.hpp"

namespace rofl {
namespace openflow {

class eFlowStatsBase		: public exception {
public:
	eFlowStatsBase(
			const std::string& __arg = std::string("")) :
				exception(__arg)
	{
		set_exception("eFlowStatsBase");
	};
};
class eFlowStatsNotFound	: public eFlowStatsBase {
public:
	eFlowStatsNotFound(
			const std::string& __arg = std::string("")) :
				eFlowStatsBase(__arg)
	{
		set_exception("eFlowStatsNotFound");
	};
};

class cofflow_stats_request {
public:

	/**
	 *
	 */
	virtual
	~cofflow_stats_request()
	{};

	/**
	 *
	 */
	cofflow_stats_request(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0) :
				of_version(of_version),
				match(of_version),
				table_id(0xff),
				out_port(rofl::openflow::OFPP_ANY),
				out_group(rofl::openflow::OFPG_ANY),
				cookie(0),
				cookie_mask(0)
	{
		if ((buflen == 0) || (nullptr == buf)) {
			return;
		}
		unpack(buf, buflen);
	};

	/**
	 *
	 */
	cofflow_stats_request(
			uint8_t of_version,
			const cofmatch& match,
			uint8_t table_id = 0xff,
			uint32_t out_port = rofl::openflow::OFPP_ANY,
			uint32_t out_group = rofl::openflow::OFPG_ANY,
			uint64_t cookie = 0,
			uint64_t cookie_mask = 0) :
				of_version(of_version),
				match(match),
				table_id(table_id),
				out_port(out_port),
				out_group(out_group),
				cookie(cookie),
				cookie_mask(cookie_mask)
	{ this->match.set_version(of_version); };

	/**
	 *
	 */
	cofflow_stats_request(
			const cofflow_stats_request& request)
	{ *this = request; };

	/**
	 *
	 */
	cofflow_stats_request&
	operator= (
			const cofflow_stats_request& request) {
		if (this == &request)
			return *this;

		of_version 	= request.of_version;
		match		= request.match;
		table_id	= request.table_id;
		out_port	= request.out_port;
		out_group	= request.out_group;
		cookie		= request.cookie;
		cookie_mask	= request.cookie_mask;

		return *this;
	};

public:

	/**
	 *
	 */
	cofflow_stats_request&
	set_version(
			uint8_t of_version)
	{
		this->of_version = of_version;
		match.set_version(of_version);
		return *this;
	};

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };

	/**
	 *
	 */
	cofflow_stats_request&
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
	cofflow_stats_request&
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
	cofflow_stats_request&
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
	cofflow_stats_request&
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
	cofflow_stats_request&
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
	cofflow_stats_request&
	set_match(
			const cofmatch& match)
	{ (this->match = match).set_version(of_version); return *this; };

	/**
	 *
	 */
	cofmatch&
	set_match()
	{ return match; };

	/**
	 *
	 */
	const cofmatch&
	get_match() const
	{ return match; };

public:

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
	size_t
	length() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflow_stats_request const& flow_stats_request) {
		switch (flow_stats_request.of_version) {
		case openflow10::OFP_VERSION: {
			os  << "<cofflow_stats_request >" << std::endl;
			os << "<table-id:" << (int)flow_stats_request.get_table_id() << " >" << std::endl;
			os << "<out-port:0x" << std::hex << (int)flow_stats_request.get_out_port() << std::dec << " >" << std::endl;
			
			os << flow_stats_request.match;

		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			os  << "<cofflow_stats_request >" << std::endl;
			os << "<table-id:" << (int)flow_stats_request.get_table_id() << " >" << std::endl;
			os << "<out-port:0x" << std::hex << (int)flow_stats_request.get_out_port() << std::dec << " >" << std::endl;
			os << "<out-group:0x" << std::hex << (int)flow_stats_request.get_out_group() << std::dec << " >" << std::endl;
			os << "<cookie:0x" << std::hex << (unsigned long long)flow_stats_request.get_cookie() << std::dec << " >" << std::endl;
			os << "<cookie-mask:0x" << std::hex << (unsigned long long)flow_stats_request.get_cookie_mask() << std::dec << " >" << std::endl;
			
			os << flow_stats_request.match;

		} break;
		default: {
			os << "<cofflow_stats_request unknown OFP version >" << std::endl;
		};
		}
		return os;
	};

private:

	uint8_t 	of_version;
	cofmatch 	match;
	uint8_t 	table_id;
	uint32_t	out_port;
	uint32_t	out_group;
	uint64_t	cookie;
	uint64_t	cookie_mask;
};



class cofflow_stats_reply {
public:

	/**
	 *
	 */
	virtual
	~cofflow_stats_reply()
	{};

	/**
	 *
	 */
	cofflow_stats_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0) :
				of_version(of_version),
				table_id(0),
				duration_sec(0),
				duration_nsec(0),
				priority(0),
				idle_timeout(0),
				hard_timeout(0),
				flags(0),
				cookie(0),
				packet_count(0),
				byte_count(0),
				match(of_version),
				actions(of_version),
				instructions(of_version)
	{
		if ((buflen == 0) || (nullptr == buf)) {
			return;
		}
		unpack(buf, buflen);
	};


	/**
	 *
	 */
	cofflow_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			uint32_t duration_sec,
			uint32_t duration_nsec,
			uint16_t priority,
			uint16_t idle_timeout,
			uint16_t hard_timeout,
			uint64_t cookie,
			uint64_t packet_count,
			uint64_t byte_count,
			const cofmatch& match,
			const cofactions& actions) :
				of_version(of_version),
				table_id(table_id),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec),
				priority(priority),
				idle_timeout(idle_timeout),
				hard_timeout(hard_timeout),
				flags(0),
				cookie(cookie),
				packet_count(packet_count),
				byte_count(byte_count),
				match(match),
				actions(actions)
	{};

	/**
	 *
	 */
	cofflow_stats_reply(
			uint8_t of_version,
			uint8_t table_id,
			uint32_t duration_sec,
			uint32_t duration_nsec,
			uint16_t priority,
			uint16_t idle_timeout,
			uint16_t hard_timeout,
			uint16_t flags,
			uint64_t cookie,
			uint64_t packet_count,
			uint64_t byte_count,
			const cofmatch& match,
			const cofinstructions& instructions) :
				of_version(of_version),
				table_id(table_id),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec),
				priority(priority),
				idle_timeout(idle_timeout),
				hard_timeout(hard_timeout),
				flags(flags),
				cookie(cookie),
				packet_count(packet_count),
				byte_count(byte_count),
				match(match),
				instructions(instructions)
	{};

	/**
	 *
	 */
	cofflow_stats_reply(
			const cofflow_stats_reply& reply)
	{ *this = reply; };

	/**
	 *
	 */
	cofflow_stats_reply&
	operator= (
			const cofflow_stats_reply& reply) {
		if (this == &reply)
			return *this;

		of_version 		= reply.of_version;
		table_id 		= reply.table_id;
		duration_sec 	= reply.duration_sec;
		duration_nsec 	= reply.duration_nsec;
		priority 		= reply.priority;
		idle_timeout 	= reply.idle_timeout;
		hard_timeout 	= reply.hard_timeout;
		flags			= reply.flags;
		cookie 			= reply.cookie;
		packet_count 	= reply.packet_count;
		byte_count 		= reply.byte_count;
		match 			= reply.match;
		actions 		= reply.actions;
		instructions 	= reply.instructions;

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (
			const cofflow_stats_reply& reply) const {
		return ((of_version 	== reply.of_version) &&
				(table_id 		== reply.table_id) &&
				(duration_sec 	== reply.duration_sec) &&
				(duration_nsec 	== reply.duration_nsec) &&
				(priority 		== reply.priority) &&
				(idle_timeout 	== reply.idle_timeout) &&
				(hard_timeout 	== reply.hard_timeout) &&
				(flags			== reply.flags) &&
				(cookie 		== reply.cookie) &&
				(packet_count 	== reply.packet_count) &&
				(byte_count 	== reply.byte_count) &&
				(match 			== reply.match)	&&
				(actions 		== reply.actions) &&
				(instructions 	== reply.instructions));
	};

public:

	/**
	 *
	 */
	cofflow_stats_reply&
	set_version(
			uint8_t of_version)
	{
		this->actions.set_version(of_version);
		this->instructions.set_version(of_version);
		this->match.set_version(of_version);
		this->of_version = of_version;
		return *this;
	};

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };

	/**
	 *
	 */
	cofflow_stats_reply&
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
	cofflow_stats_reply&
	set_duration_sec(
			uint32_t duration_sec)
	{ this->duration_sec = duration_sec; return *this; };

	/**
	 *
	 */
	uint32_t
	get_duration_sec() const
	{ return duration_sec; };

	/**
	 *
	 */
	cofflow_stats_reply&
	set_duration_nsec(
			uint32_t duration_nsec)
	{ this->duration_nsec = duration_nsec; return *this; };

	/**
	 *
	 */
	uint32_t
	get_duration_nsec() const
	{ return duration_nsec; };

	/**
	 *
	 */
	cofflow_stats_reply&
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
	cofflow_stats_reply&
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
	cofflow_stats_reply&
	set_hard_timeout(
			uint16_t hard_timeout)
	{ this->hard_timeout = hard_timeout; return *this; };

	/**
	 *
	 */
	uint16_t
	get_hard_timeout() const
	{ return hard_timeout; };

	/**
	 *
	 */
	cofflow_stats_reply&
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
	cofflow_stats_reply&
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
	cofflow_stats_reply&
	set_packet_count(
			uint64_t packet_count)
	{ this->packet_count = packet_count; return *this; };

	/**
	 *
	 */
	uint64_t
	get_packet_count() const
	{ return packet_count; };

	/**
	 *
	 */
	cofflow_stats_reply&
	set_byte_count(
			uint64_t byte_count)
	{ this->byte_count = byte_count; return *this; };

	/**
	 *
	 */
	uint64_t
	get_byte_count() const
	{ return byte_count; };

	/**
	 *
	 */
	cofflow_stats_reply&
	set_match(
			const cofmatch& match)
	{ (this->match = match).set_version(of_version); return *this; };

	/**
	 *
	 */
	cofmatch&
	set_match()
	{ return match; };

	/**
	 *
	 */
	const cofmatch&
	get_match() const
	{ return match; };

	/**
	 *
	 */
	cofflow_stats_reply&
	set_actions(
			const cofactions& actions)
	{ (this->actions = actions).set_version(of_version); return *this; };

	/**
	 *
	 */
	cofactions&
	set_actions()
	{ return actions; };

	/**
	 *
	 */
	const cofactions&
	get_actions() const
	{ return actions; };

	/**
	 *
	 */
	cofflow_stats_reply&
	set_instructions(
			const cofinstructions& instructions)
	{ (this->instructions = instructions).set_version(of_version); return *this; };

	/**
	 *
	 */
	cofinstructions&
	set_instructions()
	{ return instructions; };

	/**
	 *
	 */
	const cofinstructions&
	get_instructions() const
	{ return instructions; };

public:

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
	size_t
	length() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofflow_stats_reply const& flow_stats_reply) {
		switch (flow_stats_reply.of_version) {
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os  << "<cofflow_stats_reply >" << std::endl;
			os << "<table-id: " << (int)flow_stats_reply.get_table_id() << " >" << std::endl;
			os << "<duration-sec: " << (int)flow_stats_reply.get_duration_sec() << " >" << std::endl;
			os << "<duration-nsec: " << (int)flow_stats_reply.get_duration_nsec() << " >" << std::endl;
			os << "<priority: 0x" << std::hex << (int)flow_stats_reply.get_priority() << std::dec << " >" << std::endl;
			os << "<idle-timeout: " << (int)flow_stats_reply.get_idle_timeout() << " >" << std::endl;
			os << "<hard-timeout: " << (int)flow_stats_reply.get_hard_timeout() << " >" << std::endl;
			os << "<cookie: 0x" << std::hex << (unsigned long long)flow_stats_reply.get_cookie() << std::dec << " >" << std::endl;
			os << "<packet-count: " << (int)flow_stats_reply.get_packet_count() << " >" << std::endl;
			os << "<byte-count: " << (int)flow_stats_reply.get_byte_count() << " >" << std::endl;
			os << "<flags: " << (int)flow_stats_reply.get_flags() << " >" << std::endl;
			
			os << flow_stats_reply.match;
		} break;
		default: {

		};
		}
		
		switch (flow_stats_reply.of_version) {
		case openflow10::OFP_VERSION: os << flow_stats_reply.actions; break;
		case openflow12::OFP_VERSION: os << flow_stats_reply.instructions; break;
		case openflow13::OFP_VERSION: os << flow_stats_reply.instructions; break;
		default: os << "<unknown OFP version >" << std::endl;
		}
		return os;
	};

private:

	uint8_t         of_version;
	uint8_t         table_id;
	uint32_t        duration_sec;
	uint32_t        duration_nsec;
	uint16_t        priority;
	uint16_t        idle_timeout;
	uint16_t        hard_timeout;
	uint16_t        flags; 			// since OF1.3
	uint64_t        cookie;
	uint64_t        packet_count;
	uint64_t        byte_count;
	cofmatch        match;
	cofactions      actions;		// OF1.0 only
	cofinstructions	instructions;	// since OF1.2
};

} /* end of namespace */
} /* end of namespace */

#endif /* ROFL_COMMON_OPENFLOW_COFFLOWSTATS_H */
