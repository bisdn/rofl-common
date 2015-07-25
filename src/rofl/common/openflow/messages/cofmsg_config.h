/*
 * cofmsg_config.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_CONFIG_H_
#define COFMSG_CONFIG_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_get_config_request : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_get_config_request()
	{};

	/**
	 *
	 */
	cofmsg_get_config_request(
			uint8_t version = 0,
			uint32_t xid = 0) :
				cofmsg(version, rofl::openflow::OFPT_GET_CONFIG_REQUEST, xid)
	{};

	/**
	 *
	 */
	cofmsg_get_config_request(
			const cofmsg_get_config_request& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_get_config_request&
	operator= (
			const cofmsg_get_config_request& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		return *this;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_get_config_request const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_get_config_request >" << std::endl;;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Get-Config-Request- " << " ";
		return ss.str();
	};
};


/**
 *
 */
class cofmsg_get_config_reply : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_get_config_reply()
	{};

	/**
	 *
	 */
	cofmsg_get_config_reply(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			uint16_t miss_send_len = 0) :
				cofmsg(version, rofl::openflow::OFPT_GET_CONFIG_REPLY, xid),
				flags(flags),
				miss_send_len(miss_send_len)
	{};

	/**
	 *
	 */
	cofmsg_get_config_reply(
			const cofmsg_get_config_reply& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_get_config_reply&
	operator= (
			const cofmsg_get_config_reply& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		flags = msg.flags;
		miss_send_len = msg.miss_send_len;
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
	uint16_t
	get_flags() const
	{ return flags; };

	/**
	 *
	 */
	void
	set_flags(
			uint16_t flags)
	{ this->flags = flags; };

	/**
	 *
	 */
	uint16_t
	get_miss_send_len() const
	{ return miss_send_len; };

	/**
	 *
	 */
	void
	set_miss_send_len(
			uint16_t miss_send_len)
	{ this->miss_send_len = miss_send_len; };

private:

	/**
	 *
	 */
	std::string
	s_flags() const {
		std::string ss;
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			if (get_flags() == rofl::openflow10::OFPC_FRAG_NORMAL)
				ss.append("FRAG-NORMAL "); return ss;
			if (get_flags() & rofl::openflow10::OFPC_FRAG_DROP)
				ss.append("FRAG-DROP ");
			if (get_flags() & rofl::openflow10::OFPC_FRAG_REASM)
				ss.append("FRAG-REASM ");
		} break;
		case rofl::openflow12::OFP_VERSION: {
			if (get_flags() == rofl::openflow12::OFPC_FRAG_NORMAL)
				ss.append("FRAG-NORMAL "); return ss;
			if (get_flags() & rofl::openflow12::OFPC_FRAG_DROP)
				ss.append("FRAG-DROP ");
			if (get_flags() & rofl::openflow12::OFPC_FRAG_REASM)
				ss.append("FRAG-REASM ");
			if (get_flags() & rofl::openflow12::OFPC_INVALID_TTL_TO_CONTROLLER)
				ss.append("INVALID-TTL-TO-CONTROLLER ");
		} break;
		default: {
			if (get_flags() == rofl::openflow13::OFPC_FRAG_NORMAL)
				ss.append("FRAG-NORMAL "); return ss;
			if (get_flags() & rofl::openflow13::OFPC_FRAG_DROP)
				ss.append("FRAG-DROP ");
			if (get_flags() & rofl::openflow13::OFPC_FRAG_REASM)
				ss.append("FRAG-REASM ");
		};
		}
		return ss;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_get_config_reply& msg) {
		os << indent(0) << dynamic_cast<const cofmsg&>( msg );
		os << indent(2) << "<cofmsg_get_config_reply ";
		os << "flags:" << msg.s_flags() << ", ";
		os << "miss-send-len:" << (int)msg.get_miss_send_len() << " ";
		os << " >" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Get-Config-Reply- " << " ";
		ss << "flags: " << s_flags() << ", ";
		ss << "miss_send_len: " << (unsigned int)get_miss_send_len() << " ";
		return ss.str();
	};

private:

	uint16_t flags;
	uint16_t miss_send_len;
};



/**
 *
 */
class cofmsg_set_config : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_set_config()
	{};

	/**
	 *
	 */
	cofmsg_set_config(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint16_t flags = 0,
			uint16_t miss_send_len = 0) :
				cofmsg(version, rofl::openflow::OFPT_SET_CONFIG, xid),
				flags(flags),
				miss_send_len(miss_send_len)
	{};

	/**
	 *
	 */
	cofmsg_set_config(
			const cofmsg_set_config& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_set_config&
	operator= (
			const cofmsg_set_config& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		flags         = msg.flags;
		miss_send_len = msg.miss_send_len;
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
	uint16_t
	get_flags() const
	{ return flags; };

	/**
	 *
	 */
	void
	set_flags(
			uint16_t flags)
	{ this->flags = flags; };

	/**
	 *
	 */
	uint16_t
	get_miss_send_len() const
	{ return miss_send_len; };

	/**
	 *
	 */
	void
	set_miss_send_len(
			uint16_t miss_send_len)
	{ this->miss_send_len = miss_send_len; };

private:

	/**
	 *
	 */
	std::string
	s_flags() const {
		std::string ss;
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			if (get_flags() == rofl::openflow10::OFPC_FRAG_NORMAL)
				ss.append("FRAG-NORMAL "); return ss;
			if (get_flags() & rofl::openflow10::OFPC_FRAG_DROP)
				ss.append("FRAG-DROP ");
			if (get_flags() & rofl::openflow10::OFPC_FRAG_REASM)
				ss.append("FRAG-REASM ");
		} break;
		case rofl::openflow12::OFP_VERSION: {
			if (get_flags() == rofl::openflow12::OFPC_FRAG_NORMAL)
				ss.append("FRAG-NORMAL "); return ss;
			if (get_flags() & rofl::openflow12::OFPC_FRAG_DROP)
				ss.append("FRAG-DROP ");
			if (get_flags() & rofl::openflow12::OFPC_FRAG_REASM)
				ss.append("FRAG-REASM ");
			if (get_flags() & rofl::openflow12::OFPC_INVALID_TTL_TO_CONTROLLER)
				ss.append("INVALID-TTL-TO-CONTROLLER ");
		} break;
		default: {
			if (get_flags() == rofl::openflow13::OFPC_FRAG_NORMAL)
				ss.append("FRAG-NORMAL "); return ss;
			if (get_flags() & rofl::openflow13::OFPC_FRAG_DROP)
				ss.append("FRAG-DROP ");
			if (get_flags() & rofl::openflow13::OFPC_FRAG_REASM)
				ss.append("FRAG-REASM ");
		};
		}
		return ss;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_set_config& msg) {
		os << indent(0) << dynamic_cast<const cofmsg&>( msg );
		os << indent(2) << "<cofmsg_set_config ";
		os << "flags:" << msg.s_flags() << ", ";
		os << "miss-send-len:" << (int)msg.get_miss_send_len() << " ";
		os << " >" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Set-Config- " << " ";
		ss << "flags: " << s_flags() << ", ";
		ss << "miss_send_len: " << (unsigned int)get_miss_send_len() << " ";
		return ss.str();
	};

private:

	uint16_t flags;
	uint16_t miss_send_len;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_CONFIG_H_ */
