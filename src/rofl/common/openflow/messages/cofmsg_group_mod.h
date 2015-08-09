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
	virtual
	~cofmsg_group_mod()
	{};

	/**
	 *
	 */
	cofmsg_group_mod(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			const rofl::openflow::cofgroupmod& groupmod = rofl::openflow::cofgroupmod()) :
				cofmsg(version, rofl::openflow12::OFPT_GROUP_MOD, xid),
				groupmod(groupmod)
	{
		this->groupmod.set_version(version);
	};

	/**
	 *
	 */
	cofmsg_group_mod(
			const cofmsg_group_mod& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_group_mod&
	operator= (
			const cofmsg_group_mod& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		groupmod = msg.groupmod;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	check_prerequisites() const
	{ groupmod.check_prerequisites(); };

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
			uint8_t *buf, size_t buflen);

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

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_group_mod& msg) {
		os << rofl::indent(0) << "<cofmsg_group_mod >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofmsg&>( msg ); };
		{ rofl::indent i(4); os << msg.get_groupmod(); };
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Group-Mod- " << " ";
		ss << "groupmod: " << groupmod.str() << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofgroupmod groupmod;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFMSG_GROUP_MOD_H_ */
