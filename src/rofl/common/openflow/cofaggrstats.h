/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFAGGRSTATS_H
#define ROFL_COMMON_OPENFLOW_COFAGGRSTATS_H 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class cofaggr_stats_request {
public:

	/**
	 *
	 */
	virtual
	~cofaggr_stats_request()
	{};

	/**
	 *
	 */
	cofaggr_stats_request(
			uint8_t of_version = openflow::OFP_VERSION_UNKNOWN,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0) :
				of_version(of_version),
				match(of_version),
				table_id(0),
				out_port(0),
				out_group(0),
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
	cofaggr_stats_request(
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
	cofaggr_stats_request(
			const cofaggr_stats_request& req)
	{ *this = req; };

	/**
	 *
	 */
	cofaggr_stats_request&
	operator= (
			const cofaggr_stats_request& req) {
		if (this == &req)
			return *this;

		of_version 	= req.of_version;
		match		= req.match;
		table_id	= req.table_id;
		out_port	= req.out_port;
		out_group	= req.out_group;
		cookie		= req.cookie;
		cookie_mask	= req.cookie_mask;

		return *this;
	};

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

	/**
	 *
	 */
	cofaggr_stats_request&
	set_version(
			uint8_t of_version)
	{ this->of_version = of_version; match.set_version(of_version); return *this; };

	/**
	 *
	 */
	uint8_t
	get_version() const
	{ return of_version; };

	/**
	 *
	 */
	cofaggr_stats_request&
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
	cofaggr_stats_request&
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
	cofaggr_stats_request&
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
	cofaggr_stats_request&
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
	cofaggr_stats_request&
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
	cofaggr_stats_request&
	set_match(
			const cofmatch& match)
	{ this->match = match; this->match.set_version(of_version); return *this; };

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

	friend std::ostream&
	operator<< (std::ostream& os, cofaggr_stats_request const& aggr_stats_request) {
		switch (aggr_stats_request.get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			os  << "<cofaggr_stats_request >" << std::endl;
		} break;
		case rofl::openflow10::OFP_VERSION: {
			os  << "<cofaggr_stats_request >" << std::endl;
			os << "<table-id:" << (int)aggr_stats_request.get_table_id() << " >" << std::endl;
			os << "<out-port:0x" << std::hex << (int)aggr_stats_request.get_out_port() << std::dec << " >" << std::endl;
			
			os << aggr_stats_request.match;

		} break;
		case rofl::openflow12::OFP_VERSION: {
			os  << "<cofaggr_stats_request >" << std::endl;
			os << "<table-id:" << (int)aggr_stats_request.get_table_id() << " >" << std::endl;
			os << "<out-port:0x" << std::hex << (int)aggr_stats_request.get_out_port() << std::dec << " >" << std::endl;
			os << "<out-group:0x" << std::hex << (int)aggr_stats_request.get_out_group() << std::dec << " >" << std::endl;
			os << "<cookie:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie() << std::dec << " >" << std::endl;
			os << "<cookie-mask:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie_mask() << std::dec << " >" << std::endl;
			
			os << aggr_stats_request.match;

		} break;
		case rofl::openflow13::OFP_VERSION: {
			os  << "<cofaggr_stats_request >" << std::endl;
			os << "<table-id:" << (int)aggr_stats_request.get_table_id() << " >" << std::endl;
			os << "<out-port:0x" << std::hex << (int)aggr_stats_request.get_out_port() << std::dec << " >" << std::endl;
			os << "<out-group:0x" << std::hex << (int)aggr_stats_request.get_out_group() << std::dec << " >" << std::endl;
			os << "<cookie:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie() << std::dec << " >" << std::endl;
			os << "<cookie-mask:0x" << std::hex << (unsigned long long)aggr_stats_request.get_cookie_mask() << std::dec << " >" << std::endl;
			
			os << aggr_stats_request.match;

		} break;
		default: {
			throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
		return os;
	};

private: // data structures

	uint8_t 	of_version;
	cofmatch 	match;
	uint8_t 	table_id;
	uint32_t	out_port;
	uint32_t	out_group;
	uint64_t	cookie;
	uint64_t	cookie_mask;
};



class cofaggr_stats_reply {
public:

	/**
	 *
	 */
	virtual
	~cofaggr_stats_reply()
	{};

	/**
	 *
	 */
	cofaggr_stats_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0) :
				of_version(of_version),
				packet_count(0),
				byte_count(0),
				flow_count(0)
	{
		if ((buflen == 0) || (nullptr == buf)) {
			return;
		}
		unpack(buf, buflen);
	};

	/**
	 *
	 */
	cofaggr_stats_reply(
			uint8_t of_version,
			uint64_t packet_count,
			uint64_t byte_count,
			uint32_t flow_count) :
				of_version(of_version),
				packet_count(packet_count),
				byte_count(byte_count),
				flow_count(flow_count)
	{};

	/**
	 *
	 */
	cofaggr_stats_reply(
			const cofaggr_stats_reply& rep)
	{ *this = rep; };

	/**
	 *
	 */
	cofaggr_stats_reply&
	operator= (
			const cofaggr_stats_reply& rep) {
		if (this == &rep)
			return *this;

		of_version 		= rep.of_version;
		packet_count	= rep.packet_count;
		byte_count		= rep.byte_count;
		flow_count		= rep.flow_count;

		return *this;
	};

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

	/**
	 *
	 */
	cofaggr_stats_reply&
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
	cofaggr_stats_reply&
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
	cofaggr_stats_reply&
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
	cofaggr_stats_reply&
	set_flow_count(
			uint32_t flow_count)
	{ this->flow_count = flow_count; return *this; };

	/**
	 *
	 */
	uint32_t
	get_flow_count() const
	{ return flow_count; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaggr_stats_reply const& reply) {
		switch (reply.get_version()) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {
			os  << "<cofaggr_stats_reply >" << std::endl;
		} break;
		case rofl::openflow10::OFP_VERSION:
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os  << "<cofaggr_stats_reply >" << std::endl;
			os << "<packet-count:" << (int)reply.get_packet_count() << " >" << std::endl;
			os << "<byte-count:" << (int)reply.get_byte_count() << " >" << std::endl;
			os << "<flow-count:" << (int)reply.get_flow_count() << " >" << std::endl;
		} break;
		default: {
			throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
		return os;
	};

private:

	uint8_t 	of_version;
	uint64_t	packet_count;
	uint64_t	byte_count;
	uint32_t	flow_count;
};


} /* end of namespace openflow */
} /* end of namespace rofl */

#endif /* ROFL_COMMON_OPENFLOW_COFAGGRSTATS_H */
