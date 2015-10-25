/*
 * cofportdescprops.cc
 *
 *  Created on: 29.03.2015
 *      Author: andi
 */

#include "rofl/common/openflow/cofportdescprops.h"

using namespace rofl::openflow;


cofportdesc_props&
cofportdesc_props::operator= (
		const cofportdesc_props& props)
{
	if (this == &props)
		return *this;

	cofportdesc_props::clear();

	ofp_version = props.ofp_version;

	for (std::map<uint16_t, cofportdesc_prop*>::const_iterator
			it = props.get_properties().begin(); it != props.get_properties().end(); ++it) {
		switch (it->second->get_type()) {
		case rofl::openflow14::OFPPDPT_ETHERNET: {
			add_port_desc_ethernet() = props.get_port_desc_ethernet();
		} break;
		case rofl::openflow14::OFPPDPT_OPTICAL: {
			add_port_desc_optical() = props.get_port_desc_optical();
		} break;
		case rofl::openflow14::OFPPDPT_EXPERIMENTER: {
			add_port_desc_experimenter() = props.get_port_desc_experimenter();
		} break;
		default:
			std::cerr << "[rofl][cofportdesc_props] ignoring unknown port descrption property, property-type:"
						<< it->second->get_type() << std::endl;
		}
	}

	return *this;
}



size_t
cofportdesc_props::length() const
{
	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {
		size_t len = 0;
		for (std::map<uint16_t, cofportdesc_prop*>::const_iterator
				it = properties.begin(); it != properties.end(); ++it) {
			len += it->second->length();
		}
		return len;
	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofportdesc_props::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofportdesc_props::length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (ofp_version) {
	case openflow14::OFP_VERSION: {
#if 0
		if (has_port_desc_ethernet()) {
			set_port_desc_ethernet().pack(buf, set_port_desc_ethernet().length());
			buf += set_port_desc_ethernet().length();
			buflen -= set_port_desc_ethernet().length();
		}
		if (has_port_desc_optical()) {
			set_port_desc_optical().pack(buf, set_port_desc_optical().length());
			buf += set_port_desc_optical().length();
			buflen -= set_port_desc_optical().length();
		}
		if (has_port_desc_experimenter()) {
			set_port_desc_experimenter().pack(buf, set_port_desc_experimenter().length());
			buf += set_port_desc_experimenter().length();
			buflen -= set_port_desc_experimenter().length();
		}
#endif
		for (std::map<uint16_t, cofportdesc_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			cofportdesc_prop& prop = *(it->second);
			prop.pack(buf, prop.length());
			buf += prop.length();
			buflen -= prop.length();
		}
	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}



void
cofportdesc_props::unpack(
		uint8_t *buf, size_t buflen)
{
	clear();

	switch (ofp_version) {
	case openflow14::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow14::ofp_port_desc_prop_header)) {

			struct rofl::openflow14::ofp_port_desc_prop_header* hdr =
					(struct rofl::openflow14::ofp_port_desc_prop_header*)buf;

			uint16_t type 	= be16toh(hdr->type);
			uint16_t len	= be16toh(hdr->length);

			if ((buflen < len) || (0 == len))
				throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

			switch (type) {
			case rofl::openflow14::OFPPDPT_ETHERNET: {
				add_port_desc_ethernet().unpack(buf, len);
				buf += set_port_desc_ethernet().length();
				buflen -= set_port_desc_ethernet().length();
			} break;
			case rofl::openflow14::OFPPDPT_OPTICAL: {
				add_port_desc_optical().unpack(buf, len);
				buf += set_port_desc_optical().length();
				buflen -= set_port_desc_optical().length();
			} break;
			case rofl::openflow14::OFPPDPT_EXPERIMENTER: {
				add_port_desc_experimenter().unpack(buf, len);
				buf += set_port_desc_experimenter().length();
				buflen -= set_port_desc_experimenter().length();
			} break;
			default: {
				std::cerr << "[rofl][cofportdesc_props] ignoring unknown port description property, property-type:"
							<<(unsigned int)type << std::endl;
				buf += len;
				buflen -= len;
			};
			}
		}

	} break;
	default:
		throw eBadVersion("eBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
}





