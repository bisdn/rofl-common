#include "rofl/common/openflow/messages/cofmsg_port_status.h"

using namespace rofl::openflow;

size_t
cofmsg_port_status::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return sizeof(struct rofl::openflow10::ofp_port_status);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return sizeof(struct rofl::openflow12::ofp_port_status);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow13::ofp_port_status);
	} break;
	case rofl::openflow14::OFP_VERSION:
	default: {
		return sizeof(struct rofl::openflow14::ofp_port_status) -
				sizeof(struct rofl::openflow14::ofp_port) + port.length();
	};
	}
	return 0;
}



void
cofmsg_port_status::pack(uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_port_status::pack()");

	switch (get_version()) {
	default: {

		struct rofl::openflow10::ofp_port_status* hdr =
				(struct rofl::openflow10::ofp_port_status*)buf;

		hdr->reason = reason;

		port.pack((uint8_t*)&(hdr->desc), port.length());

	};
	}
}



void
cofmsg_port_status::unpack(uint8_t *buf, size_t buflen)
{
	port.clear();

	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow10::ofp_port_status))
			throw eBadSyntaxTooShort("cofmsg_port_status::unpack()");

		struct rofl::openflow10::ofp_port_status* hdr =
				(struct rofl::openflow10::ofp_port_status*)buf;

		reason = hdr->reason;

		port.unpack((uint8_t*)&(hdr->desc), sizeof(struct rofl::openflow10::ofp_port));

	} break;
	case rofl::openflow14::OFP_VERSION:
	default: {

		if (get_length() < sizeof(struct rofl::openflow14::ofp_port_status))
			throw eBadSyntaxTooShort("cofmsg_port_status::unpack()");

		struct rofl::openflow14::ofp_port_status* hdr =
				(struct rofl::openflow14::ofp_port_status*)buf;

		reason = hdr->reason;

		size_t portlen = get_length() -
				(sizeof(struct rofl::openflow14::ofp_port_status) -
						sizeof(struct rofl::openflow14::ofp_port));

		port.unpack((uint8_t*)&(hdr->desc), portlen);

	};
	}
}


