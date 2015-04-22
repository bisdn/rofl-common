#include "rofl/common/openflow/messages/cofmsg_hello.h"

using namespace rofl::openflow;

size_t
cofmsg_hello::length() const
{
	return (cofmsg::length() + helloelems.length());
}



void
cofmsg_hello::pack(uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_hello::pack() buf too short");

	helloelems.pack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}



void
cofmsg_hello::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (get_length() <= sizeof(struct rofl::openflow::ofp_header))
		return;

	helloelems.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}


