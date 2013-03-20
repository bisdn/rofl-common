/*
 * cofmsg_group_features_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_GROUP_FEATURES_STATS_H_
#define COFMSG_GROUP_FEATURES_STATS_H_ 1

#include "cofmsg_stats.h"
#include "cofgroupfeaturesstats.h"

namespace rofl
{

/**
 *
 */
class cofmsg_group_features_stats :
	public cofmsg_stats
{
private:

	cofgroup_features_stats_reply 		group_features_stats;

	union {
		uint8_t*						ofhu_group_features_stats;
		struct ofp10_group_features_stats*		ofhu10_group_features_stats;
		struct ofp12_group_features_stats*		ofhu12_group_features_stats;
		// TODO: OF1.3
	} ofhu;

#define ofh_group_features_stats   			ofhu.ofhu_group_features_stats
#define ofh10_group_features_stats 			ofhu.ofhu10_group_features_stats
#define ofh12_group_features_stats 			ofhu.ofhu12_group_features_stats
// TODO OF1.3

public:


	/** constructor
	 *
	 */
	cofmsg_group_features_stats(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			cofgroup_features_stats_reply const& group_features_stats = cofgroup_features_stats_reply());


	/**
	 *
	 */
	cofmsg_group_features_stats(
			cofmsg_group_features_stats const& stats);


	/**
	 *
	 */
	cofmsg_group_features_stats&
	operator= (
			cofmsg_group_features_stats const& stats);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_group_features_stats() {};


	/**
	 *
	 */
	cofmsg_group_features_stats(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual void
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


public:


	/**
	 *
	 */
	cofgroup_features_stats_reply&
	get_group_features_stats();
};

} // end of namespace rofl

#endif /* COFMSG_GROUP_FEATURES_STATS_H_ */
