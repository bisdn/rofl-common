/*
 * cofmsg_packet_in.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PACKET_IN_H_
#define COFMSG_PACKET_IN_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/cpacket.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_packet_in : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_packet_in()
	{};

	/**
	 *
	 */
	cofmsg_packet_in(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint32_t buffer_id = 0,
			uint16_t total_len = 0,
			uint8_t  reason = 0,
			uint8_t  table_id = 0,
			uint64_t cookie = 0,
			uint16_t in_port = 0,
			const rofl::openflow::cofmatch& match = rofl::openflow::cofmatch(),
			uint8_t *data = (uint8_t*)0,
			size_t datalen = 0) :
				cofmsg(version, rofl::openflow::OFPT_PACKET_IN, xid),
				buffer_id(buffer_id),
				total_len(total_len),
				in_port(in_port),
				reason(reason),
				table_id(table_id),
				cookie(cookie),
				match(match),
				packet(data, datalen)
	{ this->match.set_version(version); };

	/**
	 *
	 */
	cofmsg_packet_in(
			const cofmsg_packet_in& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_packet_in&
	operator= (
			const cofmsg_packet_in& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		buffer_id = msg.buffer_id;
		total_len = msg.total_len;
		in_port   = msg.in_port;
		reason    = msg.reason;
		table_id  = msg.table_id;
		cookie    = msg.cookie;
		match     = msg.match;
		packet    = msg.packet;
		return *this;
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
	pack(
			uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	uint32_t
	get_buffer_id() const
	{ return buffer_id; };

	/**
	 *
	 */
	void
	set_buffer_id(
			uint32_t buffer_id)
	{ this->buffer_id = buffer_id; };

	/**
	 *
	 */
	uint16_t
	get_total_len() const
	{ return total_len; };

	/**
	 *
	 */
	void
	set_total_len(
			uint16_t total_len)
	{ this->total_len = total_len; };

	/**
	 *
	 */
	uint16_t
	get_in_port() const
	{ return in_port; };

	/**
	 *
	 */
	void
	set_in_port(
			uint16_t in_port)
	{ this->in_port = in_port; };

	/**
	 *
	 */
	uint8_t
	get_reason() const
	{ return reason; };

	/**
	 *
	 */
	void
	set_reason(
			uint8_t reason)
	{ this->reason = reason; };

	/**
	 *
	 */
	uint8_t
	get_table_id() const
	{ return table_id; };

	/**
	 *
	 */
	void
	set_table_id(
			uint8_t table_id)
	{ this->table_id = table_id; };

	/**
	 *
	 */
	uint64_t
	get_cookie() const
	{ return cookie; };

	/**
	 *
	 */
	void
	set_cookie(
			uint64_t cookie)
	{ this->cookie = cookie; };

	/**
	 *
	 */
	const rofl::openflow::cofmatch&
	get_match() const
	{ return match; };

	/**
	 *
	 */
	rofl::openflow::cofmatch&
	set_match()
	{ return match; };

	/**
	 *
	 */
	const rofl::cpacket&
	get_packet() const
	{ return packet; };

	/**
	 *
	 */
	rofl::cpacket&
	set_packet()
	{ return packet; };

private:

	/**
	 *
	 */
	std::string
	s_reason() const {
		std::string ss;
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			if (rofl::openflow10::OFPR_NO_MATCH == get_reason())
				ss.append("no match ");
			if (rofl::openflow10::OFPR_ACTION == get_reason())
				ss.append("action ");
		} break;
		case rofl::openflow12::OFP_VERSION: {
			if (rofl::openflow12::OFPR_NO_MATCH == get_reason())
				ss.append("no match ");
			if (rofl::openflow12::OFPR_ACTION == get_reason())
				ss.append("action ");
			if (rofl::openflow12::OFPR_INVALID_TTL == get_reason())
				ss.append("invalid ttl ");
		} break;
		case rofl::openflow13::OFP_VERSION: {
			if (rofl::openflow13::OFPR_NO_MATCH == get_reason())
				ss.append("no match ");
			if (rofl::openflow13::OFPR_ACTION == get_reason())
				ss.append("action ");
			if (rofl::openflow13::OFPR_INVALID_TTL == get_reason())
				ss.append("invalid ttl ");
		} break;
		default: {
			if (rofl::openflow14::OFPR_TABLE_MISS == get_reason())
				ss.append("table miss ");
			if (rofl::openflow14::OFPR_APPLY_ACTION == get_reason())
				ss.append("apply action ");
			if (rofl::openflow14::OFPR_INVALID_TTL == get_reason())
				ss.append("invalid ttl ");
			if (rofl::openflow14::OFPR_ACTION_SET == get_reason())
				ss.append("action set ");
			if (rofl::openflow14::OFPR_GROUP == get_reason())
				ss.append("group ");
			if (rofl::openflow14::OFPR_PACKET_OUT == get_reason())
				ss.append("packet out ");
		};
		}
		return ss;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_packet_in const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << "<cofmsg_packet_in >" << std::endl;
		switch (msg.get_version()) {
		case openflow10::OFP_VERSION: {
			os << "<buffer-id:" 	<< (int)msg.get_buffer_id() << " >" << std::endl;
			os << "<total-len:" 	<< (int)msg.get_total_len() << " >" << std::endl;
			os << "<reason:" 		<< msg.s_reason() 	<< " >" << std::endl;
			os << "<in-port:" 		<< (int)msg.get_in_port() 	<< " >" << std::endl;
		} break;
		case openflow12::OFP_VERSION: {
			os << "<buffer-id:"	<< (int)msg.get_buffer_id() << " >" << std::endl;
			os << "<total-len:" 	<< (int)msg.get_total_len() << " >" << std::endl;
			os << "<reason:" 		<< msg.s_reason() 	<< " >" << std::endl;
			os << "<table-id:" 	<< (int)msg.get_table_id() 	<< " >" << std::endl;

		} break;
		case openflow13::OFP_VERSION: {
			os << "<buffer-id:" 	<< (int)msg.get_buffer_id() << " >" << std::endl;
			os << "<total-len:" 	<< (int)msg.get_total_len() << " >" << std::endl;
			os << "<reason:" 		<< msg.s_reason() 	<< " >" << std::endl;
			os << "<table-id: " 	<< (int)msg.get_table_id() 	<< " >" << std::endl;
			os << "<cookie: 0x" 	<< std::hex << (unsigned long long)msg.get_cookie() << std::dec << " >" << std::endl;

		} break;
		default: {
			os << "<unsupported OF version: " 	<< (int)msg.get_version() 	<< " >" << std::endl;
		} break;
		}
		os << msg.match;
		os << msg.packet;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Packet-In- " << " ";
		ss << "buffer_id: " << (unsigned int)buffer_id << ", ";
		ss << "total_len: " << (unsigned int)total_len << ", ";
		ss << "reason: " << s_reason() << ", ";
		if (get_version() == rofl::openflow10::OFP_VERSION) {
			ss << "in_port: " << (unsigned int)in_port << ", ";
		}
		ss << "table_id: " << (unsigned int)table_id << ", ";
		ss << "cookie: " << (unsigned long long)cookie << ", ";
		return ss.str();
	};

private:

	uint32_t                    buffer_id;
	uint16_t                    total_len;
	uint16_t                    in_port;   // OFP 1.0 only
	uint8_t                     reason;
	uint8_t						table_id;  // since OFP 1.2
	uint64_t					cookie;    // since OFP 1.3
	rofl::openflow::cofmatch    match;
	rofl::cpacket               packet;

    static const size_t         OFP10_PACKET_IN_STATIC_HDR_LEN;
    static const size_t         OFP12_PACKET_IN_STATIC_HDR_LEN;
    static const size_t         OFP13_PACKET_IN_STATIC_HDR_LEN;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PACKET_IN_H_ */
