/*
 * cofmsg_features.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FEATURES_H_
#define COFMSG_FEATURES_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofports.h"
#include "rofl/common/cauxid.h"
#include "rofl/common/cdpid.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_features_request : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_features_request()
	{};

	/**
	 *
	 */
	cofmsg_features_request(
			uint8_t version = 0,
			uint32_t xid = 0) :
				cofmsg(version, rofl::openflow::OFPT_FEATURES_REQUEST, 0, xid)
	{};

	/**
	 *
	 */
	cofmsg_features_request(
			const cofmsg_features_request& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_features_request&
	operator= (
			const cofmsg_features_request& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		return *this;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_features_request const& msg) {
		os << indent(0) << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_features_request >" << std::endl;
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Features-Request- ";
		return ss.str();
	};
};


/**
 *
 */
class cofmsg_features_reply : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_features_reply()
	{};

	/**
	 *
	 */
	cofmsg_features_reply(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint64_t dpid = 0,
			uint32_t n_buffers = 0,
			uint8_t  n_tables = 0,
			uint8_t  auxid = 0, // since OFP 1.3
			uint32_t capabilities = 0,
			uint32_t actions_bitmap = 0, // OFP 1.0 only
			const rofl::openflow::cofports& ports = rofl::openflow::cofports()) :
				cofmsg(version, rofl::openflow::OFPT_FEATURES_REPLY, 0, xid),
				dpid(dpid),
				n_buffers(n_buffers),
				n_tables(n_tables),
				auxid(auxid),
				capabilities(capabilities),
				actions(actions_bitmap),
				ports(ports)
	{
		switch (get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			set_length(sizeof(struct rofl::openflow10::ofp_switch_features)/* =32 */ + ports.length());
		} break;
		case rofl::openflow12::OFP_VERSION: {
			set_length(sizeof(struct rofl::openflow12::ofp_switch_features)/* =32 */ + ports.length());
		} break;
		default: {
			set_length(sizeof(struct rofl::openflow13::ofp_switch_features)/* =32 */);
		};
		}
	};

	/**
	 *
	 */
	cofmsg_features_reply(
			const cofmsg_features_reply& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_features_reply&
	operator= (
			const cofmsg_features_reply& msg) {
		if (this == &msg)
			return *this;
		dpid         = msg.dpid;
		n_buffers    = msg.n_buffers;
		n_tables     = msg.n_tables;
		auxid        = msg.auxid;
		capabilities = msg.capabilities;
		actions      = msg.actions;
		ports        = msg.ports;
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
	uint64_t
	get_dpid() const
	{ return dpid; };

	/**
	 *
	 */
	void
	set_dpid(
			uint64_t dpid)
	{ this->dpid = dpid; };

	/**
	 *
	 */
	uint32_t
	get_n_buffers() const
	{ return n_buffers; };

	/**
	 *
	 */
	void
	set_n_buffers(
			uint32_t n_buffers)
	{ this->n_buffers = n_buffers; };

	/**
	 *
	 */
	uint8_t
	get_n_tables() const
	{ return n_tables; };

	/**
	 *
	 */
	void
	set_n_tables(
			uint8_t n_tables)
	{ this->n_tables = n_tables; };

	/**
	 *
	 */
	uint8_t
	get_auxid() const
	{ return auxid; };

	/**
	 *
	 */
	void
	set_auxid(
			uint8_t auxid)
	{ this->auxid = auxid; };

	/**
	 *
	 */
	uint32_t
	get_capabilities() const
	{ return capabilities; };

	/**
	 *
	 */
	void
	set_capabilities(
			uint32_t capabilities)
	{ this->capabilities = capabilities; };

	/**
	 *
	 */
	uint32_t
	get_actions_bitmap() const
	{ return actions; };

	/**
	 *
	 */
	void
	set_actions_bitmap(
			uint32_t actions)
	{ this->actions = actions; };

	/**
	 *
	 */
	const rofl::openflow::cofports&
	get_ports() const
	{ return ports; };

	/**
	 *
	 */
	rofl::openflow::cofports&
	set_ports()
	{ return ports; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmsg_features_reply& msg) {
		os << dynamic_cast<const cofmsg&>( msg );
		os << indent(2) << "<cofmsg_features_reply ";
		switch (msg.get_version()) {
		case openflow10::OFP_VERSION: {
			os << "dpid:" 			<< msg.get_dpid() 				<< " ";
			os << "#buffers:" 		<< (int)msg.get_n_buffers() 	<< " ";
			os << "#tables:" 		<< (int)msg.get_n_tables() 		<< " ";
			os << "capabilities:" 	<< std::hex << (int)msg.get_capabilities() << std::dec << " ";
			os << "actions:" 		<< (int)msg.get_actions_bitmap() << " ";
			os << " >" << std::endl;
			indent i(4);
			os << msg.ports;
		} break;
		case openflow12::OFP_VERSION: {
			os << "dpid:" 			<< msg.get_dpid() 				<< " ";
			os << "#buffers:" 		<< (int)msg.get_n_buffers() 	<< " ";
			os << "#tables:" 		<< (int)msg.get_n_tables() 		<< " ";
			os << "capabilities:" 	<< std::hex << (int)msg.get_capabilities() << std::dec << " ";
			os << " >" << std::endl;
			indent i(4);
			os << msg.ports;
		} break;
		case openflow13::OFP_VERSION: {
			os << "dpid:" 			<< msg.get_dpid() 				<< " ";
			os << "#buffers:" 		<< (int)msg.get_n_buffers() 	<< " ";
			os << "#tables:" 		<< (int)msg.get_n_tables() 		<< " ";
			os << "capabilities:" 	<< std::hex << (int)msg.get_capabilities() << std::dec << " ";
			os << " >" << std::endl;
		} break;
		default: {

		} break;
		}
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Features-Reply- " << " ";
		ss << "dpid: " << cdpid(get_dpid()).str() << ", ";
		if (get_version() >= rofl::openflow13::OFP_VERSION) {
			ss << "auxid: " << cauxid(get_auxid()).str() << ", ";
		}
		ss << "#buffers: " << (int)get_n_buffers() << ", ";
		ss << "#tables: " << (int)get_n_tables() << " ";
		return ss.str();
	};

private:

	uint64_t                 dpid;
	uint32_t                 n_buffers;
	uint8_t                  n_tables;
	uint8_t                  auxid;
	uint32_t                 capabilities;
	uint32_t                 actions;
	rofl::openflow::cofports ports;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_FEATURES_H_ */
