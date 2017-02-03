/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_group_features_stats.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 26.07.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_GROUP_FEATURES_STATS_H_
#define COFMSG_GROUP_FEATURES_STATS_H_ 1

#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/cofgroupfeaturesstats.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_group_features_stats_request :
		public cofmsg_stats_request
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_group_features_stats_request();

	/**
	 *
	 */
	cofmsg_group_features_stats_request(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t flags = 0);

	/**
	 *
	 */
	cofmsg_group_features_stats_request(
			const cofmsg_group_features_stats_request& msg);

	/**
	 *
	 */
	cofmsg_group_features_stats_request&
	operator= (
			const cofmsg_group_features_stats_request& msg);

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
	operator<< (std::ostream& os, const cofmsg_group_features_stats_request& msg) {
		os << dynamic_cast<const cofmsg_stats_request&>( msg );
		os << "<cofmsg_group_features_stats_request >" << std::endl;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Group-Features-Stats-Request- ";
		return ss.str();
	};
};



/**
 *
 */
class cofmsg_group_features_stats_reply :
		public cofmsg_stats_reply
{
public:

	/**
	 *
	 */
	virtual
	~cofmsg_group_features_stats_reply();

	/**
	 *
	 */
	cofmsg_group_features_stats_reply(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			const rofl::openflow::cofgroup_features_stats_reply& group_features_stats = rofl::openflow::cofgroup_features_stats_reply());

	/**
	 *
	 */
	cofmsg_group_features_stats_reply(
			const cofmsg_group_features_stats_reply& msg);

	/**
	 *
	 */
	cofmsg_group_features_stats_reply&
	operator= (
			const cofmsg_group_features_stats_reply& msg);

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
	const cofgroup_features_stats_reply&
	get_group_features_stats() const
	{ return group_features_stats; };

	/**
	 *
	 */
	cofgroup_features_stats_reply&
	set_group_features_stats()
	{ return group_features_stats; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_group_features_stats_reply& msg) {
		os << dynamic_cast<const cofmsg_stats_reply&>( msg );
		os << "<cofmsg_group_features_stats_reply >" << std::endl;
		
		os << msg.group_features_stats;
		return os;
	};

	virtual std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Group-Features-Stats-Reply- ";
		return ss.str();
	};

private:

	cofgroup_features_stats_reply 		group_features_stats;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_GROUP_FEATURES_STATS_H_ */
