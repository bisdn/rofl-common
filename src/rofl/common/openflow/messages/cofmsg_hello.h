/*
 * cofmsg_hello.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_HELLO_H_
#define COFMSG_HELLO_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofhelloelems.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_hello : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_hello()
	{};

	/**
	 *
	 */
	cofmsg_hello(
			uint8_t version = 0,
			uint32_t xid = 0,
			const rofl::openflow::cofhelloelems& helloelems =
					rofl::openflow::cofhelloelems()) :
				cofmsg(version, rofl::openflow::OFPT_HELLO, helloelems.length(), xid),
				helloelems(helloelems)
	{};

	/**
	 *
	 */
	cofmsg_hello(
			const cofmsg_hello& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_hello&
	operator= (
			const cofmsg_hello& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		helloelems = msg.helloelems;
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
	const rofl::openflow::cofhelloelems&
	get_helloelems() const
	{ return helloelems; };

	/**
	 *
	 */
	rofl::openflow::cofhelloelems&
	set_helloelems()
	{ return helloelems; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_hello const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_hello >" << std::endl;
		os << indent(2) << msg.helloelems;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Hello- ";
		return ss.str();
	};

private:

	rofl::openflow::cofhelloelems helloelems;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_HELLO_H_ */
