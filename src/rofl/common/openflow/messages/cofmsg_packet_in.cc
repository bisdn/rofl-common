#include "rofl/common/openflow/messages/cofmsg_packet_in.h"

using namespace rofl::openflow;

/*static*/
const size_t rofl::openflow::cofmsg_packet_in::OFP10_PACKET_IN_STATIC_HDR_LEN =
		(sizeof(struct rofl::openflow10::ofp_packet_in) - 2); /* =18 */
/*static*/
const size_t rofl::openflow::cofmsg_packet_in::OFP12_PACKET_IN_STATIC_HDR_LEN =
		(sizeof(struct rofl::openflow12::ofp_packet_in) - sizeof(struct rofl::openflow12::ofp_match));	// length without struct openflow12::ofp_match
/*static*/
const size_t rofl::openflow::cofmsg_packet_in::OFP13_PACKET_IN_STATIC_HDR_LEN =
		(sizeof(struct rofl::openflow13::ofp_packet_in) - sizeof(struct rofl::openflow13::ofp_match));	// length without struct openflow13::ofp_match


size_t
cofmsg_packet_in::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (OFP10_PACKET_IN_STATIC_HDR_LEN
				+ packet.length());

	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2/* bytes padding */
				+ packet.length());
	} break;
	default: {
		return (OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2/* bytes padding */
				+ packet.length());
	};
	}
	return 0;
}



void
cofmsg_packet_in::pack(uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eInvalid("eInvalid").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_packet_in* hdr =
				(struct rofl::openflow10::ofp_packet_in*)buf;

		hdr->buffer_id = htobe32(buffer_id);
		hdr->total_len = htobe16(total_len);
		hdr->in_port   = htobe16(in_port);
		hdr->reason    = reason;

		if (not packet.empty()) {
			packet.pack(hdr->data, packet.length());
		}

	} break;
	case rofl::openflow12::OFP_VERSION: {

		struct rofl::openflow12::ofp_packet_in* hdr =
				(struct rofl::openflow12::ofp_packet_in*)buf;

		hdr->buffer_id = htobe32(buffer_id);
		hdr->total_len = htobe16(total_len);
		hdr->reason    = reason;
		hdr->table_id  = table_id;

		match.pack((buf + rofl::openflow12::OFP_PACKET_IN_STATIC_HDR_LEN), match.length());

		if (not packet.empty()) {
			packet.pack(buf + rofl::openflow12::OFP_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, packet.length());
		}

	} break;
	default: {

		struct rofl::openflow13::ofp_packet_in* hdr =
				(struct rofl::openflow13::ofp_packet_in*)buf;

		hdr->buffer_id = htobe32(buffer_id);
		hdr->total_len = htobe16(total_len);
		hdr->reason    = reason;
		hdr->table_id  = table_id;
		hdr->cookie    = htobe64(cookie);

		match.pack((buf + rofl::openflow13::OFP_PACKET_IN_STATIC_HDR_LEN), match.length());

		if (not packet.empty()) {
			packet.pack(buf + rofl::openflow13::OFP_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, packet.length());
		}

	};
	}
}



void
cofmsg_packet_in::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	match.clear();
	match.set_version(get_version());
	packet.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow10::ofp_packet_in))
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		if (get_type() != rofl::openflow10::OFPT_PACKET_IN)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_packet_in* hdr =
				(struct rofl::openflow10::ofp_packet_in*)buf;

		buffer_id = be32toh(hdr->buffer_id);
		total_len = be16toh(hdr->total_len);
		in_port   = be16toh(hdr->in_port);
		reason    = hdr->reason;
		table_id  = 0;
		cookie    = 0;

		match.set_in_port(in_port);

		packet.unpack(buf + OFP10_PACKET_IN_STATIC_HDR_LEN,
				buflen - OFP10_PACKET_IN_STATIC_HDR_LEN);

	} break;
	case rofl::openflow12::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow12::ofp_packet_in))
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		if (get_type() != rofl::openflow12::OFPT_PACKET_IN)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_packet_in* hdr =
				(struct rofl::openflow12::ofp_packet_in*)buf;

		buffer_id = be32toh(hdr->buffer_id);
		total_len = be16toh(hdr->total_len);
		reason    = hdr->reason;
		in_port   = 0;
		table_id  = hdr->table_id;
		cookie    = 0;

		/* get variable length struct ofp_match */
		if (rofl::openflow12::OFPMT_OXM != be16toh(hdr->match.type)) // must be extensible match
			throw eBadSyntax("cofmsg_packet_in::unpack() unexpected match type");

		/* fixed part outside of struct ofp_match is 16bytes */
		if (be16toh(hdr->match.length) > (buflen - OFP12_PACKET_IN_STATIC_HDR_LEN))
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		match.unpack((uint8_t*)&(hdr->match), be16toh(hdr->match.length));

		/* set data and datalen variables */
		uint16_t offset = OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2; // +2: magic :)

		if (offset > buflen)
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		packet.unpack((uint8_t*)(buf + offset), buflen - offset);

	} break;
	default: {

		if (get_length() < sizeof(struct rofl::openflow13::ofp_packet_in))
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		if (get_type() != rofl::openflow13::OFPT_PACKET_IN)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_packet_in* hdr =
				(struct rofl::openflow13::ofp_packet_in*)buf;

		buffer_id = be32toh(hdr->buffer_id);
		total_len = be16toh(hdr->total_len);
		reason    = hdr->reason;
		in_port   = 0;
		table_id  = hdr->table_id;
		cookie    = be64toh(hdr->cookie);

		/* get variable length struct ofp_match */
		if (rofl::openflow13::OFPMT_OXM != be16toh(hdr->match.type)) // must be extensible match
			throw eBadSyntax("cofmsg_packet_in::unpack() unexpected match type");

		/* fixed part outside of struct ofp_match is 16bytes */
		if (be16toh(hdr->match.length) > (buflen - OFP13_PACKET_IN_STATIC_HDR_LEN))
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		match.unpack((uint8_t*)&(hdr->match), be16toh(hdr->match.length));

		/* set data and datalen variables */
		uint16_t offset = OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2; // +2: magic :)

		if (offset > buflen)
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		packet.unpack((uint8_t*)(buf + offset), buflen - offset);

	};
	}
}


