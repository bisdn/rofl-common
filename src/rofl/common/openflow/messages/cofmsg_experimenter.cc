#include "rofl/common/openflow/messages/cofmsg_experimenter.h"

using namespace rofl::openflow;

size_t
cofmsg_experimenter::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return sizeof(struct rofl::openflow10::ofp_vendor_header) + body.memlen();
	} break;
	default: {
		return sizeof(struct rofl::openflow12::ofp_experimenter_header) + body.memlen();
	};
	}
	return 0;
}



void
cofmsg_experimenter::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_experimenter::length())
		throw eInvalid("eInvalid").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_vendor_header* hdr =
				(struct rofl::openflow10::ofp_vendor_header*)buf;

		hdr->vendor = htobe32(exp_id);

		body.pack(buf + sizeof(struct rofl::openflow10::ofp_vendor_header),
				buflen - sizeof(struct rofl::openflow10::ofp_vendor_header));

	} break;
	default: {

		struct rofl::openflow12::ofp_experimenter_header* hdr =
				(struct rofl::openflow12::ofp_experimenter_header*)buf;

		hdr->experimenter = htobe32(exp_id);
		hdr->exp_type = htobe32(exp_type);

		body.pack(buf + sizeof(struct rofl::openflow12::ofp_experimenter_header),
				buflen - sizeof(struct rofl::openflow12::ofp_experimenter_header));

	};
	}
}



void
cofmsg_experimenter::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	body.resize(0);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_experimenter::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_type() != rofl::openflow10::OFPT_VENDOR)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_vendor_header* hdr =
				(struct rofl::openflow10::ofp_vendor_header*)buf;

		exp_id = be32toh(hdr->vendor);
		exp_type = 0;

		if (buflen > sizeof(struct rofl::openflow10::ofp_vendor_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow10::ofp_vendor_header),
					buflen - sizeof(struct rofl::openflow10::ofp_vendor_header));
		}

	} break;
	default: {
		if (get_type() != rofl::openflow13::OFPT_EXPERIMENTER)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_experimenter_header* hdr =
				(struct rofl::openflow13::ofp_experimenter_header*)buf;

		exp_id   = be32toh(hdr->experimenter);
		exp_type = be32toh(hdr->exp_type);

		if (buflen > sizeof(struct rofl::openflow13::ofp_experimenter_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow13::ofp_experimenter_header),
					buflen - sizeof(struct rofl::openflow13::ofp_experimenter_header));
		}

	};
	}

	if (get_length() < cofmsg_experimenter::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}


