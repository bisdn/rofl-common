/*
 * cofmsg_port_status.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PORT_STATUS_H_
#define COFMSG_PORT_STATUS_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofport.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_port_status : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_port_status()
	{};

	/**
	 *
	 */
	cofmsg_port_status(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint8_t reason = 0,
			const rofl::openflow::cofport& port = rofl::openflow::cofport()) :
				cofmsg(version, rofl::openflow::OFPT_PORT_STATUS, xid),
				reason(reason),
				port(port)
	{};

	/**
	 *
	 */
	cofmsg_port_status(
			const cofmsg_port_status& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_port_status&
	operator= (
			const cofmsg_port_status& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		reason = msg.reason;
		port   = msg.port;
		return *this;
	};

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
	uint8_t
	get_reason() const
	{ return reason; };

	/**
	 *
	 */
	void
	set_reason(
			uint8_t reason)
	{ this->reason = reason; };

	/**
	 *
	 */
	const rofl::openflow::cofport&
	get_port() const
	{ return port; };

	/**
	 *
	 */
	rofl::openflow::cofport&
	set_port()
	{ return port; };

private:

	/**
	 *
	 */
	std::string
	s_reason() const {
		std::string ss;
		switch (get_version()) {
		default: {
			if (rofl::openflow10::OFPPR_ADD    == get_reason())
				ss.append("add ");
			if (rofl::openflow10::OFPPR_DELETE == get_reason())
				ss.append("delete ");
			if (rofl::openflow10::OFPPR_MODIFY == get_reason())
				ss.append("modify ");
		};
		}
		return ss;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_port_status& msg) {
		os << dynamic_cast<const cofmsg&>( msg );
		os << indent(2) << "<cofmsg_port_status >" << std::endl;
			switch (msg.get_reason()) {
			case openflow::OFPPR_ADD: {
				os << indent(4) << "<reason: -ADD- >" << std::endl;
			} break;
			case openflow::OFPPR_DELETE: {
				os << indent(4) << "<reason: -DELETE- >" << std::endl;
			} break;
			case openflow::OFPPR_MODIFY: {
				os << indent(4) << "<reason: -MODIFY- >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<reason: -UNKNOWN- >" << std::endl;
			} break;
			}
			indent i(4);
			os << msg.port;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Port-Status- " << " ";
		ss << "reason: " << (unsigned int)get_reason() << ", ";
		//ss << "port: " << get_port().str() << " ";
		return ss.str();
	};

private:

	uint8_t reason;
	rofl::openflow::cofport port;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PORT_STATUS_H_ */
