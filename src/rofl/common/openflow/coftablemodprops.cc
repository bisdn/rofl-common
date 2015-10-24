/*
 * coftablemodprops.cc
 *
 *  Created on: 31.03.2015
 *      Author: andi
 */

#include "rofl/common/openflow/coftablemodprops.h"

using namespace rofl::openflow;


coftablemod_props&
coftablemod_props::operator= (
		const coftablemod_props& props)
{
	if (this == &props)
		return *this;

	coftablemod_props::clear();

	ofp_version = props.ofp_version;

	for (std::map<uint16_t, coftablemod_prop*>::const_iterator
			it = props.get_properties().begin(); it != props.get_properties().end(); ++it) {
		switch (it->second->get_type()) {
		case rofl::openflow14::OFPTMPT_EVICTION: {
			add_table_mod_eviction() = props.get_table_mod_eviction();
		} break;
		case rofl::openflow14::OFPTMPT_VACANCY: {
			add_table_mod_vacancy() = props.get_table_mod_vacancy();
		} break;
		case rofl::openflow14::OFPTMPT_EXPERIMENTER: {
			add_table_mod_experimenter() = props.get_table_mod_experimenter();
		} break;
		default:
			std::cerr << "[rofl][coftablemod_props] ignoring unknown port descrption property, property-type:"
						<< it->second->get_type() << std::endl;
		}
	}

	return *this;
}



size_t
coftablemod_props::length() const
{
	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {
		size_t len = 0;
		for (std::map<uint16_t, coftablemod_prop*>::const_iterator
				it = properties.begin(); it != properties.end(); ++it) {
			len += it->second->length();
		}
		return len;
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
coftablemod_props::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < coftablemod_props::length())
		throw eInval();

	switch (ofp_version) {
	case openflow14::OFP_VERSION: {
#if 0
		if (has_table_mod_eviction()) {
			set_table_mod_eviction().pack(buf, set_table_mod_eviction().length());
			buf += set_table_mod_eviction().length();
			buflen -= set_table_mod_eviction().length();
		}
		if (has_table_mod_vacancy()) {
			set_table_mod_vacancy().pack(buf, set_table_mod_vacancy().length());
			buf += set_table_mod_vacancy().length();
			buflen -= set_table_mod_vacancy().length();
		}
		if (has_table_mod_experimenter()) {
			set_table_mod_experimenter().pack(buf, set_table_mod_experimenter().length());
			buf += set_table_mod_experimenter().length();
			buflen -= set_table_mod_experimenter().length();
		}
#endif
		for (std::map<uint16_t, coftablemod_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			coftablemod_prop& prop = *(it->second);
			prop.pack(buf, prop.length());
			buf += prop.length();
			buflen -= prop.length();
		}
	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
coftablemod_props::unpack(
		uint8_t *buf, size_t buflen)
{
	clear();

	switch (ofp_version) {
	case openflow14::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow14::ofp_table_mod_prop_header)) {

			struct rofl::openflow14::ofp_table_mod_prop_header* hdr =
					(struct rofl::openflow14::ofp_table_mod_prop_header*)buf;

			uint16_t type 	= be16toh(hdr->type);
			uint16_t len	= be16toh(hdr->length);

			if ((buflen < len) || (0 == len))
				throw eInval();

			switch (type) {
			case rofl::openflow14::OFPTMPT_EVICTION: {
				add_table_mod_eviction().unpack(buf, len);
				buf += set_table_mod_eviction().length();
				buflen -= set_table_mod_eviction().length();
			} break;
			case rofl::openflow14::OFPTMPT_VACANCY: {
				add_table_mod_vacancy().unpack(buf, len);
				buf += set_table_mod_vacancy().length();
				buflen -= set_table_mod_vacancy().length();
			} break;
			case rofl::openflow14::OFPTMPT_EXPERIMENTER: {
				add_table_mod_experimenter().unpack(buf, len);
				buf += set_table_mod_experimenter().length();
				buflen -= set_table_mod_experimenter().length();
			} break;
			default: {
				std::cerr << "[rofl][coftablemod_props] ignoring unknown port description property, property-type:"
							<<(unsigned int)type << std::endl;
				buf += len;
				buflen -= len;
			};
			}
		}

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}





