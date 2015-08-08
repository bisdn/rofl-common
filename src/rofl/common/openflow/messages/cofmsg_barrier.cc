#include "rofl/common/openflow/messages/cofmsg_barrier.h"

using namespace rofl::openflow;

size_t
cofmsg_barrier_request::length() const
{
	return (cofmsg::length() + body.memlen());
}



void
cofmsg_barrier_request::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_barrier_request::pack()");

	body.pack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}



void
cofmsg_barrier_request::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	body.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	if (get_length() <= sizeof(struct rofl::openflow::ofp_header))
		return;

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_type() != rofl::openflow10::OFPT_BARRIER_REQUEST)
			throw eMsgInval("cofmsg_barrier_request::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
					buflen - sizeof(struct rofl::openflow::ofp_header));
		}

	} break;
	default: {
		if (get_type() != rofl::openflow13::OFPT_BARRIER_REQUEST)
			throw eMsgInval("cofmsg_barrier_request::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
					buflen - sizeof(struct rofl::openflow::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_barrier_request::length())
		throw eBadSyntaxTooShort("cofmsg_barrier_request::unpack() buf too short");
}



size_t
cofmsg_barrier_reply::length() const
{
	return (cofmsg::length() + body.memlen());
}



void
cofmsg_barrier_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	body.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_barrier_reply::pack() buf too short");

	body.pack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}



void
cofmsg_barrier_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (get_length() <= sizeof(struct rofl::openflow::ofp_header))
		return;

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_type() != rofl::openflow10::OFPT_BARRIER_REPLY)
			throw eMsgInval("cofmsg_barrier_reply::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
					buflen - sizeof(struct rofl::openflow::ofp_header));
		}

	} break;
	default: {
		if (get_type() != rofl::openflow13::OFPT_BARRIER_REPLY)
			throw eMsgInval("cofmsg_barrier_reply::unpack() invalid message type");

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
					buflen - sizeof(struct rofl::openflow::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_barrier_reply::length())
		throw eBadSyntaxTooShort("cofmsg_barrier_reply::unpack() buf too short");
}


