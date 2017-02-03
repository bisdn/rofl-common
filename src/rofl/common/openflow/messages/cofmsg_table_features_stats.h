/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_table_stats.h
 *
 *  Created on: 18.03.2013
 *  Created on: 02.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_TABLE_FEATURES_H_
#define COFMSG_TABLE_FEATURES_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/coftables.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_table_features_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_table_features_stats_request();

	/**
	 *
	 */
	cofmsg_table_features_stats_request(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const rofl::openflow::coftables& tables = rofl::openflow::coftables());

	/**
	 *
	 */
	cofmsg_table_features_stats_request(
			const cofmsg_table_features_stats_request& msg);

	/**
	 *
	 */
	cofmsg_table_features_stats_request&
	operator= (
			const cofmsg_table_features_stats_request& msg);

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
	rofl::openflow::coftables&
	set_tables()
	{ return tables; };

	/**
	 *
	 */
	const rofl::openflow::coftables&
	get_tables() const
	{ return tables; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_table_features_stats_request& msg) {
		os << dynamic_cast<const cofmsg_stats_request&>( msg );
		os << "<cofmsg_table_features_request >" << std::endl;
		 os << msg.tables;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_request::str() << "-Table-Features-Stats-Request- ";
		return ss.str();
	};

private:

	rofl::openflow::coftables   tables;
};








/**
 *
 */
class cofmsg_table_features_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_table_features_stats_reply();

	/**
	 *
	 */
	cofmsg_table_features_stats_reply(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0,
			const rofl::openflow::coftables& tables = rofl::openflow::coftables());

	/**
	 *
	 */
	cofmsg_table_features_stats_reply(
			const cofmsg_table_features_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_table_features_stats_reply&
	operator= (
			const cofmsg_table_features_stats_reply& msg);

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
	const rofl::openflow::coftables&
	get_tables() const
	{ return tables; };

	/**
	 *
	 */
	rofl::openflow::coftables&
	set_tables()
	{ return tables; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_table_features_stats_reply& msg) {
		os << dynamic_cast<const cofmsg_stats_reply&>( msg );
		os << "<cofmsg_table_features_reply >" << std::endl;
		 os << msg.tables;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg_stats_reply::str() << "-Table-Features-Stats-Reply- ";
		return ss.str();
	};

private:

	rofl::openflow::coftables   tables;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_TABLE_STATS_H_ */
