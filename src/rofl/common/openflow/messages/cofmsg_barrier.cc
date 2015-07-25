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

	if ((0 == buf) || (0 == buflen))
		return;

	if (get_length() <= sizeof(struct rofl::openflow::ofp_header))
		return;

	body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
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

	body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}


