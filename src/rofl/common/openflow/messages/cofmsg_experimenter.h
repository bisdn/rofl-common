/*
 * cofmsg_experimenter.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_EXPERIMENTER_H_
#define COFMSG_EXPERIMENTER_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/cmemory.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_experimenter : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_experimenter()
	{};

	/**
	 *
	 */
	cofmsg_experimenter(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint32_t exp_id = 0,
			uint32_t exp_type = 0,
			uint8_t *data = (uint8_t*)0,
			size_t datalen = 0) :
				cofmsg(version, rofl::openflow::OFPT_EXPERIMENTER, xid),
				exp_id(exp_id),
				exp_type(exp_type),
				body(data, datalen)
	{};

	/**
	 *
	 */
	cofmsg_experimenter(
			const cofmsg_experimenter& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_experimenter&
	operator= (
			const cofmsg_experimenter& msg) {
		if (this == &msg)
			return *this;
		exp_id   = msg.exp_id;
		exp_type = msg.exp_type;
		body     = msg.body;
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
	uint32_t
	get_exp_id() const
	{ return exp_id; };

	/**
	 *
	 */
	void
	set_exp_id(
			uint32_t exp_id)
	{ this->exp_id = exp_id; };

	/**
	 *
	 */
	uint32_t
	get_exp_type() const
	{ return exp_type; };

	/**
	 *
	 */
	void
	set_exp_type(
			uint32_t exp_type)
	{ this->exp_type = exp_type; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_body() const
	{ return body; };

	/**
	 *
	 */
	rofl::cmemory&
	set_body()
	{ return body; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_experimenter const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << "<cofmsg_experimenter >" << std::endl;
		os << "<exp-id:" << (int)msg.get_exp_id() << " >" << std::endl;
		switch (msg.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			/* field does not exist in OFP 1.0 */
		} break;
		default: {
			os << "<exp-type:" << (int)msg.get_exp_type() << " >" << std::endl;
		};
		}
		
		os << msg.body;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Experimenter- ";
		ss << "exp_id: " << (int)get_exp_id() << " ";
		ss << "exp_type: " << (int)get_exp_type() << " ";
		return ss.str();
	};

private:

	uint32_t      exp_id;
	uint32_t      exp_type;
	rofl::cmemory body;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_EXPERIMENTER_H_ */
