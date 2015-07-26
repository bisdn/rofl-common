/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_experimenter_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 26.07.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_EXPERIMENTER_STATS_H_
#define COFMSG_EXPERIMENTER_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/cmemory.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_experimenter_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_experimenter_stats_request();

	/**
	 *
	 */
	cofmsg_experimenter_stats_request(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			uint32_t exp_id = 0,
			uint32_t exp_type = 0,
			const rofl::cmemory& body = rofl::cmemory(0));

	/**
	 *
	 */
	cofmsg_experimenter_stats_request(
			const cofmsg_experimenter_stats_request& msg);

	/**
	 *
	 */
	cofmsg_experimenter_stats_request&
	operator= (
			const cofmsg_experimenter_stats_request& msg);

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
	get_exp_id() const
	{ return exp_id; };

	/**
	 *
	 */
	cofmsg_experimenter_stats_request&
	set_exp_id(
			uint32_t exp_id)
	{ this->exp_id = exp_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_exp_type() const
	{ return exp_type; };

	/**
	 *
	 */
	cofmsg_experimenter_stats_request&
	set_exp_type(
			uint32_t exp_type)
	{ this->exp_type = exp_type; return *this;  };

	/**
	 *
	 */
	const cmemory&
	get_body() const
	{ return body; };

	/**
	 *
	 */
	cmemory&
	set_body()
	{ return body; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_experimenter_stats_request& msg) {
		os << dynamic_cast<const cofmsg_stats_request&>( msg );
		os << indent(2) << "<cofmsg_experimenter_stats_request >" << std::endl;
		os << indent(4) << "<exp_id: 0x" << std::hex << (unsigned int)msg.get_exp_id() << std::dec << " >" << std::endl;
		os << indent(4) << "<exp_type: 0x" << std::hex << (unsigned int)msg.get_exp_type() << std::dec << " >" << std::endl;
		indent i(4);
		os << msg.body;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Experimenter-Stats-Request- ";
		ss << "exp_id: 0x" << std::hex << (unsigned int)get_exp_id() << std::dec << " ";
		ss << "exp_type: 0x" << std::hex << (unsigned int)get_exp_type() << std::dec << " ";
		return ss.str();
	};

private:

	uint32_t                exp_id;
	uint32_t                exp_type;
	rofl::cmemory			body;
};

/**
 *
 */
class cofmsg_experimenter_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_experimenter_stats_reply();

	/**
	 *
	 */
	cofmsg_experimenter_stats_reply(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			uint32_t exp_id = 0,
			uint32_t exp_type = 0,
			const rofl::cmemory& body = rofl::cmemory(0));

	/**
	 *
	 */
	cofmsg_experimenter_stats_reply(
			const cofmsg_experimenter_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_experimenter_stats_reply&
	operator= (
			const cofmsg_experimenter_stats_reply& msg);

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
	get_exp_id() const
	{ return exp_id; };

	/**
	 *
	 */
	cofmsg_experimenter_stats_reply&
	set_exp_id(
			uint32_t exp_id)
	{ this->exp_id = exp_id; return *this; };

	/**
	 *
	 */
	uint32_t
	get_exp_type() const
	{ return exp_type; };

	/**
	 *
	 */
	cofmsg_experimenter_stats_reply&
	set_exp_type(
			uint32_t exp_type)
	{ this->exp_type = exp_type; return *this;  };

	/**
	 *
	 */
	const cmemory&
	get_body() const
	{ return body; };

	/**
	 *
	 */
	cmemory&
	set_body()
	{ return body; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_experimenter_stats_reply& msg) {
		os << dynamic_cast<const cofmsg_stats_reply&>( msg );
		os << indent(2) << "<cofmsg_experimenter_stats_reply >" << std::endl;
		os << indent(4) << "<exp_id: 0x" << std::hex << (unsigned int)msg.get_exp_id() << std::dec << " >" << std::endl;
		os << indent(4) << "<exp_type: 0x" << std::hex << (unsigned int)msg.get_exp_type() << std::dec << " >" << std::endl;
		indent i(4);
		os << msg.body;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Experimenter-Stats-Reply- ";
		ss << "exp_id: 0x" << std::hex << (unsigned int)get_exp_id() << std::dec << " ";
		ss << "exp_type: 0x" << std::hex << (unsigned int)get_exp_type() << std::dec << " ";
		return ss.str();
	};

private:

	uint32_t                exp_id;
	uint32_t                exp_type;
	rofl::cmemory			body;
};


} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_GROUP_FEATURES_STATS_H_ */
