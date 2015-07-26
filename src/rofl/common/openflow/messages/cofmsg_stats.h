/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 25.07.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_STATS_H_
#define COFMSG_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {


class cofmsg_stats_request : public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_stats_request();

	/**
	 *
	 */
	cofmsg_stats_request(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t stats_type = 0,
			uint16_t stats_flags = 0);

	/**
	 *
	 */
	cofmsg_stats_request(
			const cofmsg_stats_request& msg);

	/**
	 *
	 */
	cofmsg_stats_request&
	operator= (
			const cofmsg_stats_request& msg);

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
	uint16_t
	get_stats_type() const
	{ return stats_type; };

	/**
	 *
	 */
	void
	set_stats_type(
			uint16_t type)
	{ this->stats_type = type; };

	/**
	 *
	 */
	uint16_t
	get_stats_flags() const
	{ return stats_flags; };

	/**
	 *
	 */
	void
	set_stats_flags(
			uint16_t flags)
	{ this->stats_flags = flags; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_stats_request const& msg) {
		std::string s_flags;

		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_stats_request ";
		os << "type:" << (int)msg.get_stats_type() << " ";
		os << "flags:" << msg.s_flags() << " ";
		os << ">" << std::endl;;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Stats-Request- " << " ";
		ss << "type: " << (unsigned int)get_stats_type() << ", ";
		ss << "flags: " << s_flags() << " ";
		return ss.str();
	};

private:

	std::string
	s_flags() const {
		std::string str;
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			if (get_stats_flags() & rofl::openflow10::OFPSF_REPLY_MORE)
				str.append("MORE ");
		} break;
		default: {
			if (get_stats_flags() & rofl::openflow13::OFPMPF_REPLY_MORE)
				str.append("MORE ");
		};
		}
		return str;
	};

private:

	uint16_t stats_type;
	uint16_t stats_flags;
};



class cofmsg_stats_reply : public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_stats_reply();

	/**
	 *
	 */
	cofmsg_stats_reply(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t stats_type = 0,
			uint16_t stats_flags = 0);

	/**
	 *
	 */
	cofmsg_stats_reply(
			const cofmsg_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_stats_reply&
	operator= (
			const cofmsg_stats_reply& msg);

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
	uint16_t
	get_stats_type() const
	{ return stats_type; };

	/**
	 *
	 */
	void
	set_stats_type(
			uint16_t type)
	{ this->stats_type = type; };

	/**
	 *
	 */
	uint16_t
	get_stats_flags() const
	{ return stats_flags; };

	/**
	 *
	 */
	void
	set_stats_flags(
			uint16_t flags)
	{ this->stats_flags = flags; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_stats_reply const& msg) {
		std::string s_flags;

		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_stats_reply ";
		os << "type:" << (int)msg.get_stats_type() << " ";
		os << "flags:" << msg.s_flags() << " ";
		os << ">" << std::endl;;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Stats-Reply- " << " ";
		ss << "type: " << (unsigned int)get_stats_type() << ", ";
		ss << "flags: " << s_flags() << " ";
		return ss.str();
	};

private:

	std::string
	s_flags() const {
		std::string str;
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			if (get_stats_flags() & rofl::openflow10::OFPSF_REPLY_MORE)
				str.append("MORE ");
		} break;
		default: {
			if (get_stats_flags() & rofl::openflow13::OFPMPF_REPLY_MORE)
				str.append("MORE ");
		};
		}
		return str;
	};

private:

	uint16_t stats_type;
	uint16_t stats_flags;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFMSG_STATS_H_ */
