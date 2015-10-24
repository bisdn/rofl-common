/*
 * cofportdescprop.cc
 *
 *  Created on: 29.03.2015
 *      Author: andi
 */

#include "rofl/common/openflow/cofportdescprop.h"

using namespace rofl::openflow;

void
cofportdesc_prop::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofportdesc_prop::length()) // take length() method from this base class
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow14::OFP_VERSION: {

		struct rofl::openflow14::ofp_port_desc_prop_header* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_header*)buf;

		len = length(); // take virtual length() method

		hdr->type	= htobe16(type);
		hdr->length	= htobe16(len);

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
cofportdesc_prop::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofportdesc_prop::length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow14::OFP_VERSION: {

		struct rofl::openflow14::ofp_port_desc_prop_header* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_header*)buf;

		type	= be16toh(hdr->type);
		len		= be16toh(hdr->length);

		if (len < sizeof(struct rofl::openflow14::ofp_port_desc_prop_header))
			throw eInval();

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
cofportdesc_prop_ethernet::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofportdesc_prop_ethernet::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		cofportdesc_prop::pack(buf, buflen);

		struct rofl::openflow14::ofp_port_desc_prop_ethernet* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_ethernet*)buf;

		hdr->curr	    = htobe32(curr);
		hdr->advertised = htobe32(advertised);
		hdr->supported  = htobe32(supported);
		hdr->peer       = htobe32(peer);
		hdr->curr_speed = htobe32(curr_speed);
		hdr->max_speed  = htobe32(max_speed);

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
cofportdesc_prop_ethernet::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow14::ofp_port_desc_prop_ethernet))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		cofportdesc_prop::unpack(buf, buflen);

		if (get_length() < sizeof(struct rofl::openflow14::ofp_port_desc_prop_ethernet))
			throw eInval();

		struct rofl::openflow14::ofp_port_desc_prop_ethernet* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_ethernet*)buf;

		curr       = be32toh(hdr->curr);
		advertised = be32toh(hdr->advertised);
		supported  = be32toh(hdr->supported);
		peer       = be32toh(hdr->peer);
		curr_speed = be32toh(hdr->curr_speed);
		max_speed  = be32toh(hdr->max_speed);

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
cofportdesc_prop_optical::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofportdesc_prop_optical::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		cofportdesc_prop::pack(buf, buflen);

		struct rofl::openflow14::ofp_port_desc_prop_optical* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_optical*)buf;

		hdr->supported         = htobe32(supported);
		hdr->tx_min_freq_lmda  = htobe32(tx_min_freq_lmda);
		hdr->tx_max_freq_lmda  = htobe32(tx_max_freq_lmda);
		hdr->tx_grid_freq_lmda = htobe32(tx_grid_freq_lmda);
		hdr->rx_min_freq_lmda  = htobe32(rx_min_freq_lmda);
		hdr->rx_max_freq_lmda  = htobe32(rx_max_freq_lmda);
		hdr->rx_grid_freq_lmda = htobe32(rx_grid_freq_lmda);
		hdr->tx_pwr_min        = htobe32(tx_pwr_min);
		hdr->tx_pwr_max        = htobe32(tx_pwr_max);

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
cofportdesc_prop_optical::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow14::ofp_port_desc_prop_optical))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		cofportdesc_prop::unpack(buf, buflen);

		if (get_length() < sizeof(struct rofl::openflow14::ofp_port_desc_prop_optical))
			throw eInval();

		struct rofl::openflow14::ofp_port_desc_prop_optical* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_optical*)buf;

		supported           = be32toh(hdr->supported);
		tx_min_freq_lmda    = be32toh(hdr->tx_min_freq_lmda);
		tx_max_freq_lmda    = be32toh(hdr->tx_max_freq_lmda);
		tx_grid_freq_lmda	= be32toh(hdr->tx_grid_freq_lmda);
		rx_min_freq_lmda    = be32toh(hdr->rx_min_freq_lmda);
		rx_max_freq_lmda    = be32toh(hdr->rx_max_freq_lmda);
		rx_grid_freq_lmda	= be32toh(hdr->rx_grid_freq_lmda);
		tx_pwr_min			= be32toh(hdr->tx_pwr_min);
		tx_pwr_max			= be32toh(hdr->tx_pwr_max);

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
cofportdesc_prop_experimenter::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofportdesc_prop_experimenter::length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		cofportdesc_prop::pack(buf, buflen);

		struct rofl::openflow14::ofp_port_desc_prop_experimenter* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_experimenter*)buf;

		hdr->experimenter	= htobe32(exp_id);
		hdr->exp_type		= htobe32(exp_type);
		exp_body.pack((uint8_t*)hdr->experimenter_data, exp_body.memlen());

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}



void
cofportdesc_prop_experimenter::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < sizeof(struct rofl::openflow14::ofp_port_desc_prop_experimenter))
		throw eInval();

	switch (get_version()) {
	case rofl::openflow14::OFP_VERSION: {

		cofportdesc_prop::unpack(buf, buflen);

		if (buflen < get_length())
			throw eInval();

		struct rofl::openflow14::ofp_port_desc_prop_experimenter* hdr =
				(struct rofl::openflow14::ofp_port_desc_prop_experimenter*)buf;

		exp_id	= be32toh(hdr->experimenter);
		exp_type = be32toh(hdr->exp_type);
		exp_body.unpack((uint8_t*)hdr->experimenter_data, get_length() - sizeof(struct rofl::openflow14::ofp_port_desc_prop_experimenter));

	} break;
	default:
		throw eBadVersion("eBadVersion").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);
	}
}


