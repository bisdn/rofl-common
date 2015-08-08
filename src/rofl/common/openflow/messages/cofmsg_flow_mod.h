/*
 * cofmsg_flow_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_MOD_H_
#define COFMSG_FLOW_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofflowmod.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_flow_mod : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_flow_mod()
	{};

	/**
	 *
	 */
	cofmsg_flow_mod(
			uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			const rofl::openflow::cofflowmod& flowmod = rofl::openflow::cofflowmod()) :
				cofmsg(version, rofl::openflow::OFPT_FLOW_MOD, xid),
				flowmod(flowmod)
	{
		this->flowmod.set_version(version);
	};

	/**
	 *
	 */
	cofmsg_flow_mod(
			const cofmsg_flow_mod& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_flow_mod&
	operator= (
			const cofmsg_flow_mod& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		flowmod	= msg.flowmod;
		return *this;
	};

	/**
	 *
	 */
	void
	check_prerequisites() const
	{ flowmod.check_prerequisites(); };

public:

	/**
	 *
	 */
	const rofl::openflow::cofflowmod&
	get_flowmod() const
	{ return flowmod; };

	/**
	 *
	 */
	rofl::openflow::cofflowmod&
	set_flowmod()
	{ return flowmod; };

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
	operator<< (std::ostream& os, const cofmsg_flow_mod& msg) {
		os << rofl::indent(0) << "<cofmsg_flow_mod >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofmsg&>( msg ); };
		{ rofl::indent i(4); os << msg.get_flowmod(); };
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Flow-Mod- " << " ";
		ss << "flowmod: " << flowmod.str() << " ";
		return ss.str();
	};

private:

	rofl::openflow::cofflowmod flowmod;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_FLOW_MOD_H_ */
