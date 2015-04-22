#include "rofl/common/openflow/messages/cofmsg_group_mod.h"

using namespace rofl::openflow;

size_t
cofmsg_group_mod::length() const
{
	return cofmsg::length() + groupmod.length();
}



void
cofmsg_group_mod::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_group_mod::pack()");

	switch (get_version()) {
	default: {

		struct rofl::openflow::ofp_header* hdr =
				(struct rofl::openflow::ofp_header*)buf;

		groupmod.pack(hdr->body, groupmod.length());
	};
	}
}



void
cofmsg_group_mod::unpack(uint8_t *buf, size_t buflen)
{
	groupmod.clear();

	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	default: {

		if (buflen < get_length())
			throw eBadSyntaxTooShort("cofmsg_group_mod::unpack()");

		struct rofl::openflow::ofp_header* hdr =
				(struct rofl::openflow::ofp_header*)buf;

		size_t groupmodlen = buflen - sizeof(struct rofl::openflow::ofp_header);

		if (groupmodlen > 0) {
			groupmod.unpack(hdr->body, groupmodlen);
		}
	};
	}
}


