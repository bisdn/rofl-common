/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_async_config.h
 *
 *  Created on: 15.06.2013
 *  Revised on: 02.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_ASYNC_CONFIG_H_
#define COFMSG_ASYNC_CONFIG_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofasyncconfig.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_get_async_config_request :
		public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_get_async_config_request();

	/**
	 *
	 */
	cofmsg_get_async_config_request(
			uint8_t version = 0,
			uint32_t xid = 0);

	/**
	 *
	 */
	cofmsg_get_async_config_request(
			const cofmsg_get_async_config_request& msg);

	/**
	 *
	 */
	cofmsg_get_async_config_request&
	operator= (
			const cofmsg_get_async_config_request& msg);

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

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_get_async_config_request& msg) {
		os << dynamic_cast<const cofmsg&>( msg );
		os << "<cofmsg_get_async_config_request >" << std::endl;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Get-Async-Config-Request- " << " ";
		return ss.str();
	};
};








/**
 *
 */
class cofmsg_get_async_config_reply :
		public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_get_async_config_reply();

	/**
	 *
	 */
	cofmsg_get_async_config_reply(
			uint8_t version = 0,
			uint32_t xid = 0,
			const rofl::openflow::cofasync_config& async_config = rofl::openflow::cofasync_config());

	/**
	 *
	 */
	cofmsg_get_async_config_reply(
			const cofmsg_get_async_config_reply& msg);

	/**
	 *
	 */
	cofmsg_get_async_config_reply&
	operator= (
			const cofmsg_get_async_config_reply& msg);

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
	const rofl::openflow::cofasync_config&
	get_async_config() const
	{ return async_config; };

	/**
	 *
	 */
	rofl::openflow::cofasync_config&
	set_async_config()
	{ return async_config; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_get_async_config_reply& msg) {
		os << dynamic_cast<const cofmsg&>( msg );
		os << "<cofmsg_get_async_config_reply >" << std::endl;
		
		os << msg.async_config;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Get-Async-Config-Reply- " << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofasync_config async_config;
};





/**
 *
 */
class cofmsg_set_async_config :
		public cofmsg
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_set_async_config();

	/**
	 *
	 */
	cofmsg_set_async_config(
			uint8_t version = 0,
			uint32_t xid = 0,
			const rofl::openflow::cofasync_config& async_config = rofl::openflow::cofasync_config());

	/**
	 *
	 */
	cofmsg_set_async_config(
			const cofmsg_set_async_config& msg);

	/**
	 *
	 */
	cofmsg_set_async_config&
	operator= (
			const cofmsg_set_async_config& msg);

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
	const rofl::openflow::cofasync_config&
	get_async_config() const
	{ return async_config; };

	/**
	 *
	 */
	rofl::openflow::cofasync_config&
	set_async_config()
	{ return async_config; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_set_async_config& msg) {
		os << dynamic_cast<const cofmsg&>( msg );
		os << "<cofmsg_set_async_config >" << std::endl;
		
		os << msg.async_config;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Set-Async-Config- " << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofasync_config async_config;
};




} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_ASYNC_CONFIG_H_ */
