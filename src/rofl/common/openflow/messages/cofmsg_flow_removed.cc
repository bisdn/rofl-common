#include "rofl/common/openflow/messages/cofmsg_flow_removed.h"

using namespace rofl::openflow;

/*static*/const size_t cofmsg_flow_removed::OFP10_FLOW_REMOVED_STATIC_HDR_LEN =
		sizeof(struct rofl::openflow10::ofp_flow_removed);
/*static*/const size_t cofmsg_flow_removed::OFP12_FLOW_REMOVED_STATIC_HDR_LEN =
		sizeof(struct rofl::openflow12::ofp_flow_removed) - sizeof(struct rofl::openflow::ofp_match);

size_t
cofmsg_flow_removed::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (OFP10_FLOW_REMOVED_STATIC_HDR_LEN);
	} break;
	default: {
		return (OFP12_FLOW_REMOVED_STATIC_HDR_LEN + match.length());
	};
	}
	return 0;
}



void
cofmsg_flow_removed::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_flow_removed::pack()");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_flow_removed* hdr =
				(struct rofl::openflow10::ofp_flow_removed*)buf;

		hdr->cookie        = htobe64(cookie);
		hdr->priority      = htobe16(priority);
		hdr->reason        = reason;
		hdr->duration_sec  = htobe32(duration_sec);
		hdr->duration_nsec = htobe32(duration_nsec);
		hdr->idle_timeout  = htobe16(idle_timeout);
		hdr->packet_count  = htobe64(packet_count);
		hdr->byte_count    = htobe64(byte_count);

		match.pack((uint8_t*)&(hdr->match),
				sizeof(struct rofl::openflow10::ofp_match));

	} break;
	default: {

		struct rofl::openflow12::ofp_flow_removed* hdr =
				(struct rofl::openflow12::ofp_flow_removed*)buf;

		hdr->cookie        = htobe64(cookie);
		hdr->priority      = htobe16(priority);
		hdr->reason        = reason;
		hdr->table_id      = table_id;
		hdr->duration_sec  = htobe32(duration_sec);
		hdr->duration_nsec = htobe32(duration_nsec);
		hdr->idle_timeout  = htobe16(idle_timeout);
		hdr->hard_timeout  = htobe16(hard_timeout);
		hdr->packet_count  = htobe64(packet_count);
		hdr->byte_count    = htobe64(byte_count);

		match.pack(buf + OFP12_FLOW_REMOVED_STATIC_HDR_LEN,
				buflen - OFP12_FLOW_REMOVED_STATIC_HDR_LEN);

	};
	}
}



void
cofmsg_flow_removed::unpack(
		uint8_t *buf, size_t buflen)
{
	match.clear();

	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow10::ofp_flow_removed))
			throw eBadSyntaxTooShort("cofmsg_flow_removed::unpack()");

		struct rofl::openflow10::ofp_flow_removed* hdr =
				(struct rofl::openflow10::ofp_flow_removed*)buf;

		cookie        = be64toh(hdr->cookie);
		priority      = be16toh(hdr->priority);
		reason        = hdr->reason;
		table_id      = 0;
		duration_sec  = be32toh(hdr->duration_sec);
		duration_nsec = be32toh(hdr->duration_nsec);
		idle_timeout  = be16toh(hdr->idle_timeout);
		hard_timeout  = 0;
		packet_count  = be64toh(hdr->packet_count);
		byte_count    = be64toh(hdr->byte_count);

		match.unpack((uint8_t*)&(hdr->match),
				sizeof(struct rofl::openflow10::ofp_match));

	} break;
	default: {

		if (get_length() < sizeof(struct rofl::openflow12::ofp_flow_removed))
			throw eBadSyntaxTooShort("cofmsg_flow_removed::unpack()");

		struct rofl::openflow12::ofp_flow_removed* hdr =
				(struct rofl::openflow12::ofp_flow_removed*)buf;

		cookie        = be64toh(hdr->cookie);
		priority      = be16toh(hdr->priority);
		reason        = hdr->reason;
		table_id      = hdr->table_id;
		duration_sec  = be32toh(hdr->duration_sec);
		duration_nsec = be32toh(hdr->duration_nsec);
		idle_timeout  = be16toh(hdr->idle_timeout);
		hard_timeout  = be16toh(hdr->hard_timeout);
		packet_count  = be64toh(hdr->packet_count);
		byte_count    = be64toh(hdr->byte_count);

		size_t matchlen = get_length() - OFP12_FLOW_REMOVED_STATIC_HDR_LEN;

		if (matchlen < 2*sizeof(uint16_t))
			throw eBadSyntaxTooShort("cofmsg_flow_removed::unpack()");

		match.unpack((uint8_t*)&(hdr->match), matchlen);

	};
	}
}


