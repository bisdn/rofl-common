/*
 * cofhelloelems.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "cofhelloelems.h"

using namespace rofl::openflow;

cofhelloelems::~cofhelloelems()
{
	clear();
}



cofhelloelems::cofhelloelems()
{}



cofhelloelems::cofhelloelems(
		const cofhelloelems& elems)
{
	*this = elems;
}



cofhelloelems&
cofhelloelems::operator= (
		const cofhelloelems& elems)
{
	if (this == &elems)
		return *this;

	clear();

	for (std::map<uint8_t, cofhello_elem*>::const_iterator
			it = elems.elems.begin(); it != elems.elems.end(); ++it) {
		switch (it->first) {
		case rofl::openflow13::OFPHET_VERSIONBITMAP: {
			add_hello_elem_versionbitmap() = elems.get_hello_elem_versionbitmap();
		} break;
		default: {

		};
		}
	}

	return *this;
}



void
cofhelloelems::clear()
{
	for (std::map<uint8_t, cofhello_elem*>::const_iterator
			it = elems.begin(); it != elems.end(); ++it) {
		delete it->second;
	}
	elems.clear();
}



size_t
cofhelloelems::length() const
{
	size_t len = 0;
	for (std::map<uint8_t, cofhello_elem*>::const_iterator
			it = elems.begin(); it != elems.end(); ++it) {
		len += it->second->length();
	}
	return len;
}



void
cofhelloelems::pack(
		uint8_t *buf, size_t buflen)
{
	if (buflen < cofhelloelems::length())
		throw eHelloElemsInval();

	for (std::map<uint8_t, cofhello_elem*>::const_iterator
			it = elems.begin(); it != elems.end(); ++it) {
		cofhello_elem& elem = *(it->second);
		elem.pack(buf, elem.length());
		buf += elem.length();
	}
}



void
cofhelloelems::unpack(
		uint8_t *buf, size_t buflen)
{
	clear();

	while (buflen > sizeof(struct rofl::openflow13::ofp_hello_elem_header)) {

		struct rofl::openflow13::ofp_hello_elem_header* hello =
				(struct rofl::openflow13::ofp_hello_elem_header*)buf;

		if ((be16toh(hello->length) > buflen) || (be16toh(hello->length) == 0))
			break;

		switch (be16toh(hello->type)) {
		case rofl::openflow13::OFPHET_VERSIONBITMAP: {
			add_hello_elem_versionbitmap().unpack(buf, be16toh(hello->length));
		} break;
		default: {

		};
		}

		/* calculate padded length */
		size_t total_length = be16toh(hello->length);
		size_t pad = (0x7 & total_length);
		/* append padding if not a multiple of 8 */
		if (pad) {
			total_length += 8 - pad;
		}

		/* adjust buf and buflen */
		buf += total_length;
		buflen -= total_length;
	}
}



cofhello_elem_versionbitmap&
cofhelloelems::add_hello_elem_versionbitmap()
{
	if (elems.find(rofl::openflow13::OFPHET_VERSIONBITMAP) != elems.end()) {
		delete elems[rofl::openflow13::OFPHET_VERSIONBITMAP];
	}
	elems[rofl::openflow13::OFPHET_VERSIONBITMAP] = new cofhello_elem_versionbitmap();
	return *dynamic_cast<cofhello_elem_versionbitmap*>(elems[rofl::openflow13::OFPHET_VERSIONBITMAP]);
}



cofhello_elem_versionbitmap&
cofhelloelems::set_hello_elem_versionbitmap()
{
	if (elems.find(rofl::openflow13::OFPHET_VERSIONBITMAP) == elems.end()) {
		elems[rofl::openflow13::OFPHET_VERSIONBITMAP] = new cofhello_elem_versionbitmap();
	}
	return *dynamic_cast<cofhello_elem_versionbitmap*>(elems[rofl::openflow13::OFPHET_VERSIONBITMAP]);
}



const cofhello_elem_versionbitmap&
cofhelloelems::get_hello_elem_versionbitmap() const
{
	if (elems.find(rofl::openflow13::OFPHET_VERSIONBITMAP) == elems.end()) {
		throw eHelloElemsNotFound();
	}
	return *dynamic_cast<cofhello_elem_versionbitmap*>(elems.at(rofl::openflow13::OFPHET_VERSIONBITMAP));
}



void
cofhelloelems::drop_hello_elem_versionbitmap()
{
	if (elems.find(rofl::openflow13::OFPHET_VERSIONBITMAP) == elems.end()) {
		return;
	}
	delete elems[rofl::openflow13::OFPHET_VERSIONBITMAP];
	elems.erase(rofl::openflow13::OFPHET_VERSIONBITMAP);
}



bool
cofhelloelems::has_hello_elem_versionbitmap() const
{
	return (elems.find(rofl::openflow13::OFPHET_VERSIONBITMAP) != elems.end());
}



