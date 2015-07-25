/*
 * cofmsg_echo.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_ECHO_H_
#define COFMSG_ECHO_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/cmemory.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_echo_request : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_echo_request()
	{};

	/**
	 *
	 */
	cofmsg_echo_request(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint8_t* data = 0,
			size_t datalen = 0) :
				cofmsg(version, rofl::openflow::OFPT_ECHO_REQUEST, xid),
				body(data, datalen)
	{};

	/**
	 *
	 */
	cofmsg_echo_request(
			const cofmsg_echo_request& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_echo_request&
	operator= (
			const cofmsg_echo_request& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		body = msg.body;
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
	operator<< (std::ostream& os, const cofmsg_echo_request& msg) {
		os << indent(0) << dynamic_cast<const cofmsg&>( msg );
		os << indent(2) << "<cofmsg_echo_request >" << std::endl;
		os << indent(2) << msg.body;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Echo-Request- " << " ";
		return ss.str();
	};

private:

	rofl::cmemory body;
};



/**
 *
 */
class cofmsg_echo_reply : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_echo_reply()
	{};

	/**
	 *
	 */
	cofmsg_echo_reply(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint8_t* data = 0,
			size_t datalen = 0) :
				cofmsg(version, rofl::openflow::OFPT_ECHO_REPLY, xid),
				body(data, datalen)
	{};

	/**
	 *
	 */
	cofmsg_echo_reply(
			const cofmsg_echo_reply& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_echo_reply&
	operator= (
			const cofmsg_echo_reply& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		body = msg.body;
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
	operator<< (std::ostream& os, cofmsg_echo_reply const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_echo_reply >" << std::endl;
		os << indent(2) << msg.body;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Echo-Reply- " << " ";
		return ss.str();
	};

private:

	rofl::cmemory body;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_ECHO_H_ */
