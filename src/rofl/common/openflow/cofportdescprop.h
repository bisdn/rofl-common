/*
 * cofportdescprop.h
 *
 *  Created on: 29.03.2015
 *      Author: andi
 */

#ifndef COFPORTDESCPROP_H_
#define COFPORTDESCPROP_H_ 1

#include <assert.h>
#include <ostream>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {
namespace openflow {

class ePortDescPropBase 		: public RoflException {
public:
	ePortDescPropBase(const std::string& __arg) : RoflException(__arg) {};
};
class ePortDescPropNotFound 	: public ePortDescPropBase {
public:
	ePortDescPropNotFound(const std::string& __arg) : ePortDescPropBase(__arg) {};
};


class cofportdesc_prop {
public:

	/**
	 *
	 */
	cofportdesc_prop(
			uint8_t of_version = openflow::OFP_VERSION_UNKNOWN,
			uint16_t type = 0) :
				ofp_version(of_version),
				type(type),
				len(sizeof(struct rofl::openflow14::ofp_port_desc_prop_header))
	{};


	/**
	 *
	 */
	virtual
	~cofportdesc_prop()
	{};


	/**
	 *
	 */
	cofportdesc_prop(
			const cofportdesc_prop& prop)
	{ *this = prop; };


	/**
	 *
	 */
	cofportdesc_prop&
	operator= (
			const cofportdesc_prop& pdp) {
		if (this == &pdp)
			return *this;
		ofp_version = pdp.ofp_version;
		type		= pdp.type;
		len			= pdp.len;
		return *this;
	};


public:


	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (ofp_version) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_port_desc_prop_header));
		} break;
		default:
			throw eBadVersion();
		}
	};


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
	uint8_t
	get_version() const
	{ return ofp_version; };

	/**
	 *
	 */
	void
	set_version(
			uint8_t ofp_version)
	{ this->ofp_version = ofp_version; };

	/**
	 *
	 */
	uint16_t
	get_type() const
	{ return type; };


	/**
	 *
	 */
	void
	set_type(
			uint16_t type)
	{ this->type = type; };


	/**
	 *
	 */
	uint16_t
	get_length() const
	{ return len; };

	/**
	 *
	 */
	void
	set_length(
			uint16_t len)
	{ this->len = len; };

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofportdesc_prop const& prop) {
		os << rofl::indent(0) << "<cofportdesc_prop "
				<< "version: " << (int)prop.get_version() << " "
				<< std::hex
				<< "type: 0x" << (int)prop.get_type() << " "
				<< std::dec
				<< ">" << std::endl;
		return os;
	};


private:

	uint8_t 			ofp_version;
	uint16_t			type;
	mutable uint16_t 	len;
};



class cofportdesc_prop_ethernet : public cofportdesc_prop {
public:

	/**
	 *
	 */
	cofportdesc_prop_ethernet(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				cofportdesc_prop(ofp_version, rofl::openflow14::OFPPDPT_ETHERNET),
				curr(0),
				advertised(0),
				supported(0),
				peer(0),
				curr_speed(0),
				max_speed(0)
	{};

	/**
	 *
	 */
	virtual
	~cofportdesc_prop_ethernet()
	{};

	/**
	 *
	 */
	cofportdesc_prop_ethernet(
			const cofportdesc_prop_ethernet& prop)
	{ *this = prop; };

	/**
	 *
	 */
	cofportdesc_prop_ethernet&
	operator= (
			const cofportdesc_prop_ethernet& prop) {
		if (this == &prop)
			return *this;
		cofportdesc_prop::operator= (prop);
		curr       = prop.curr;
		advertised = prop.advertised;
		supported  = prop.supported;
		peer       = prop.peer;
		curr_speed = prop.curr_speed;
		max_speed  = prop.max_speed;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (get_version()) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_port_desc_prop_ethernet));
		} break;
		default:
			throw eBadVersion();
		}
	};

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
	uint32_t
	get_curr() const
	{ return curr; };

	/**
	 *
	 */
	void
	set_curr(uint32_t curr)
	{ this->curr = curr; };

	/**
	 *
	 */
	uint32_t
	get_advertised() const
	{ return advertised; };

	/**
	 *
	 */
	void
	set_advertised(uint32_t advertised)
	{ this->advertised = advertised; };

	/**
	 *
	 */
	uint32_t
	get_supported() const
	{ return supported; };

	/**
	 *
	 */
	void
	set_supported(uint32_t supported)
	{ this->supported = supported; };

	/**
	 *
	 */
	uint32_t
	get_peer() const
	{ return peer; };

	/**
	 *
	 */
	void
	set_peer(uint32_t peer)
	{ this->peer = peer; };

	/**
	 *
	 */
	uint32_t
	get_curr_speed() const
	{ return curr_speed; };

	/**
	 *
	 */
	void
	set_curr_speed(uint32_t curr_speed)
	{ this->curr_speed = curr_speed; };

	/**
	 *
	 */
	uint32_t
	get_max_speed() const
	{ return max_speed; };

	/**
	 *
	 */
	void
	set_max_speed(uint32_t max_speed)
	{ this->max_speed = max_speed; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofportdesc_prop_ethernet& prop) {
		os << rofl::indent(0) << "<cofportdesc_prop_ethernet version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofportdesc_prop&>( prop ); }
		os << rofl::indent(4) << "<curr: 0x" << std::hex << (unsigned int)prop.get_curr() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<advertised: 0x" << std::hex << (unsigned int)prop.get_advertised() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<supported: 0x" << std::hex << (unsigned int)prop.get_supported() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<peer: 0x" << std::hex << (unsigned int)prop.get_peer() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<curr_speed: 0x" << std::hex << (unsigned int)prop.get_curr_speed() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<max_speed: 0x" << std::hex << (unsigned int)prop.get_max_speed() << std::dec << " >" << std::endl;
		return os;
	};

private:

	uint32_t curr;
	uint32_t advertised;
	uint32_t supported;
	uint32_t peer;
	uint32_t curr_speed;
	uint32_t max_speed;
};

class cofportdesc_prop_optical : public cofportdesc_prop {
public:

	/**
	 *
	 */
	cofportdesc_prop_optical(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				cofportdesc_prop(ofp_version, rofl::openflow14::OFPPDPT_OPTICAL),
				supported(0),
				tx_min_freq_lmda(0),
				tx_max_freq_lmda(0),
				tx_grid_freq_lmda(0),
				rx_min_freq_lmda(0),
				rx_max_freq_lmda(0),
				rx_grid_freq_lmda(0),
				tx_pwr_min(0),
				tx_pwr_max(0)
	{};

	/**
	 *
	 */
	virtual
	~cofportdesc_prop_optical()
	{};

	/**
	 *
	 */
	cofportdesc_prop_optical(
			const cofportdesc_prop_optical& prop)
	{ *this = prop; };

	/**
	 *
	 */
	cofportdesc_prop_optical&
	operator= (
			const cofportdesc_prop_optical& prop) {
		if (this == &prop)
			return *this;
		cofportdesc_prop::operator= (prop);
		supported         = prop.supported;
		tx_min_freq_lmda  = prop.tx_min_freq_lmda;
		tx_max_freq_lmda  = prop.tx_max_freq_lmda;
		tx_grid_freq_lmda = prop.tx_grid_freq_lmda;
		rx_min_freq_lmda  = prop.rx_min_freq_lmda;
		rx_max_freq_lmda  = prop.rx_max_freq_lmda;
		rx_grid_freq_lmda = prop.rx_grid_freq_lmda;
		tx_pwr_min        = prop.tx_pwr_min;
		tx_pwr_max        = prop.tx_pwr_max;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (get_version()) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_port_desc_prop_optical));
		} break;
		default:
			throw eBadVersion();
		}
	};

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
	uint32_t
	get_supported() const
	{ return supported; };

	/**
	 *
	 */
	void
	set_supported(uint32_t supported)
	{ this->supported = supported; };

	/**
	 *
	 */
	uint32_t
	get_tx_min_freq_lmda() const
	{ return tx_min_freq_lmda; };

	/**
	 *
	 */
	void
	set_tx_min_freq_lmda(uint32_t tx_min_freq_lmda)
	{ this->tx_min_freq_lmda = tx_min_freq_lmda; };

	/**
	 *
	 */
	uint32_t
	get_tx_max_freq_lmda() const
	{ return tx_max_freq_lmda; };

	/**
	 *
	 */
	void
	set_tx_max_freq_lmda(uint32_t tx_max_freq_lmda)
	{ this->tx_max_freq_lmda = tx_max_freq_lmda; };

	/**
	 *
	 */
	uint32_t
	get_tx_grid_freq_lmda() const
	{ return tx_grid_freq_lmda; };

	/**
	 *
	 */
	void
	set_tx_grid_freq_lmda(uint32_t tx_grid_freq_lmda)
	{ this->tx_grid_freq_lmda = tx_grid_freq_lmda; };

	/**
	 *
	 */
	uint32_t
	get_rx_min_freq_lmda() const
	{ return rx_min_freq_lmda; };

	/**
	 *
	 */
	void
	set_rx_min_freq_lmda(uint32_t rx_min_freq_lmda)
	{ this->rx_min_freq_lmda = rx_min_freq_lmda; };

	/**
	 *
	 */
	uint32_t
	get_rx_max_freq_lmda() const
	{ return rx_max_freq_lmda; };

	/**
	 *
	 */
	void
	set_rx_max_freq_lmda(uint32_t rx_max_freq_lmda)
	{ this->rx_max_freq_lmda = rx_max_freq_lmda; };

	/**
	 *
	 */
	uint32_t
	get_rx_grid_freq_lmda() const
	{ return rx_grid_freq_lmda; };

	/**
	 *
	 */
	void
	set_rx_grid_freq_lmda(uint32_t rx_grid_freq_lmda)
	{ this->rx_grid_freq_lmda = rx_grid_freq_lmda; };

	/**
	 *
	 */
	uint32_t
	get_tx_pwr_min() const
	{ return tx_pwr_min; };

	/**
	 *
	 */
	void
	set_tx_pwr_min(uint32_t tx_pwr_min)
	{ this->tx_pwr_min = tx_pwr_min; };

	/**
	 *
	 */
	uint32_t
	get_tx_pwr_max() const
	{ return tx_pwr_max; };

	/**
	 *
	 */
	void
	set_tx_pwr_max(uint32_t tx_pwr_max)
	{ this->tx_pwr_max = tx_pwr_max; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofportdesc_prop_optical& prop) {
		os << rofl::indent(0) << "<cofportdesc_prop_optical version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofportdesc_prop&>( prop ); }
		os << rofl::indent(4) << "<supported: 0x" << std::hex << (unsigned int)prop.get_supported() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<tx_min_freq_lmda: 0x" << std::hex << (unsigned int)prop.get_tx_min_freq_lmda() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<tx_max_freq_lmda: 0x" << std::hex << (unsigned int)prop.get_tx_max_freq_lmda() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<tx_grid_freq_lmda: 0x" << std::hex << (unsigned int)prop.get_tx_grid_freq_lmda() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<rx_min_freq_lmda: 0x" << std::hex << (unsigned int)prop.get_rx_min_freq_lmda() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<rx_max_freq_lmda: 0x" << std::hex << (unsigned int)prop.get_rx_max_freq_lmda() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<rx_grid_freq_lmda: 0x" << std::hex << (unsigned int)prop.get_rx_grid_freq_lmda() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<tx_pwr_min: 0x" << std::hex << (unsigned int)prop.get_tx_pwr_min() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<tx_pwr_max: 0x" << std::hex << (unsigned int)prop.get_tx_pwr_max() << std::dec << " >" << std::endl;
		return os;
	};

private:

	uint32_t supported;
	uint32_t tx_min_freq_lmda;
	uint32_t tx_max_freq_lmda;
	uint32_t tx_grid_freq_lmda;
	uint32_t rx_min_freq_lmda;
	uint32_t rx_max_freq_lmda;
	uint32_t rx_grid_freq_lmda;
	uint32_t tx_pwr_min;
	uint32_t tx_pwr_max;
};

class cofportdesc_prop_experimenter : public cofportdesc_prop {
public:

	/**
	 *
	 */
	cofportdesc_prop_experimenter(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN) :
				cofportdesc_prop(ofp_version, rofl::openflow14::OFPPDPT_EXPERIMENTER),
				exp_id(0),
				exp_type(0)
	{};

	/**
	 *
	 */
	virtual
	~cofportdesc_prop_experimenter()
	{};

	/**
	 *
	 */
	cofportdesc_prop_experimenter(
			const cofportdesc_prop_experimenter& prop)
	{ *this = prop; };

	/**
	 *
	 */
	cofportdesc_prop_experimenter&
	operator= (
			const cofportdesc_prop_experimenter& prop) {
		if (this == &prop)
			return *this;
		cofportdesc_prop::operator= (prop);
		exp_id		= prop.exp_id;
		exp_type	= prop.exp_type;
		exp_body 	= prop.exp_body;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const {
		switch (get_version()) {
		case rofl::openflow14::OFP_VERSION: {
			return (sizeof(struct rofl::openflow14::ofp_port_desc_prop_experimenter) + exp_body.memlen());
		} break;
		default:
			throw eBadVersion();
		}
	};

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
	uint32_t
	get_exp_id() const
	{ return exp_id; };

	/**
	 *
	 */
	void
	set_exp_id(uint32_t exp_id)
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
	set_exp_type(uint32_t exp_type)
	{ this->exp_type = exp_type; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_exp_body() const
	{ return exp_body; };

	/**
	 *
	 */
	rofl::cmemory&
	set_exp_body()
	{ return exp_body; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofportdesc_prop_experimenter& prop) {
		os << rofl::indent(0) << "<cofportdesc_prop_experimenter version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofportdesc_prop&>( prop ); }
		os << rofl::indent(4) << "<exp-id: 0x" << std::hex << (unsigned int)prop.get_exp_id() << " >" << std::dec << std::endl;
		os << rofl::indent(4) << "<exp-type: 0x" << std::hex << (unsigned int)prop.get_exp_type() << " >" << std::dec << std::endl;
		{ rofl::indent i(4); os << prop.get_exp_body(); }
		return os;
	};

private:

	uint32_t 		exp_id;
	uint32_t		exp_type;
	rofl::cmemory 	exp_body;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFQUEUEPROP_H_ */
