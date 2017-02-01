/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_role.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 02.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_ROLE_H
#define COFMSG_ROLE_H 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofrole.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_role_request :
		public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_role_request();

	/**
	 *
	 */
	cofmsg_role_request(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			const rofl::openflow::cofrole& role = rofl::openflow::cofrole());

	/**
	 *
	 */
	cofmsg_role_request(
			const cofmsg_role_request& msg);

	/**
	 *
	 */
	cofmsg_role_request&
	operator= (
			const cofmsg_role_request& msg);

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
	const rofl::openflow::cofrole&
	get_role() const
	{ return role; };

	/**
	 *
	 */
	rofl::openflow::cofrole&
	set_role()
	{ return role; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_role_request& msg) {
		os << dynamic_cast<const cofmsg&>( msg );
		os  << "<cofmsg_role_request >" << std::endl;
		
		os << msg.role;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Role-Request- ";
		return ss.str();
	};

private:

	rofl::openflow::cofrole role;
};



/**
 *
 */
class cofmsg_role_reply :
		public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_role_reply();

	/**
	 *
	 */
	cofmsg_role_reply(
			uint8_t version = 0,
			uint32_t xid = 0,
			const rofl::openflow::cofrole& role = rofl::openflow::cofrole());

	/**
	 *
	 */
	cofmsg_role_reply(
			const cofmsg_role_reply& msg);

	/**
	 *
	 */
	cofmsg_role_reply&
	operator= (
			const cofmsg_role_reply& msg);

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
	const rofl::openflow::cofrole&
	get_role() const
	{ return role; };

	/**
	 *
	 */
	rofl::openflow::cofrole&
	set_role()
	{ return role; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_role_reply& msg) {
		os << dynamic_cast<const cofmsg&>( msg );
		os  << "<cofmsg_role_reply >" << std::endl;
		
		os << msg.role;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Role-Reply- ";
		return ss.str();
	};

private:

	rofl::openflow::cofrole role;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_ROLE_H_ */
