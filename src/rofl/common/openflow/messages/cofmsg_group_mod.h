/*
 * cofmsg_group_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_GROUP_MOD_H_
#define COFMSG_GROUP_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofgroupmod.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_group_mod :
		public cofmsg
{
public:

	/**
	 *
	 */
	cofmsg_group_mod(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			const cofgroupmod& groupmod = cofgroupmod());

	/**
	 *
	 */
	cofmsg_group_mod(
			const cofmsg_group_mod& msg);

	/**
	 *
	 */
	cofmsg_group_mod&
	operator= (
			const cofmsg_group_mod& msg);

	/**
	 *
	 */
	virtual
	~cofmsg_group_mod();

	/**
	 *
	 */
	cofmsg_group_mod(
			cmemory *memarea);

public:

	/** reset packet content
	 *
	 */
	virtual void
	reset();

	/** parse packet and validate it
	 */
	virtual void
	validate();

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	virtual void
	set_version(uint8_t ofp_version)
	{
		cofmsg::set_version(ofp_version);
		groupmod.set_version(ofp_version);
	};

	/**
	 *
	 */
	rofl::openflow::cofgroupmod&
	set_groupmod()
	{ return groupmod; };

	/**
	 *
	 */
	const rofl::openflow::cofgroupmod&
	get_groupmod() const
	{ return groupmod; };

public:

	/** returns length of packet in packed state
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
	operator<< (std::ostream& os, const cofmsg_group_mod& msg) {
		os << rofl::indent(0) << "<cofmsg_group_mod >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofmsg&>( msg ); };
		{ rofl::indent i(4); os << msg.get_groupmod(); };
		return os;
	};

private:

	rofl::openflow::cofgroupmod	groupmod;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFMSG_GROUP_MOD_H_ */
