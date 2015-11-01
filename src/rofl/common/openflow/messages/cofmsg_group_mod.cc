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

	if (buflen < cofmsg_group_mod::length())
		throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	default: {

		struct rofl::openflow::ofp_header* hdr =
				(struct rofl::openflow::ofp_header*)buf;

		groupmod.pack(hdr->body, groupmod.length());
	};
	}
}



void
cofmsg_group_mod::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	groupmod.set_version(get_version());
	groupmod.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_group_mod::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	default: {

		if (get_type() != rofl::openflow::OFPT_GROUP_MOD)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow::ofp_header* hdr =
				(struct rofl::openflow::ofp_header*)buf;

		if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
			groupmod.unpack(hdr->body, buflen - sizeof(struct rofl::openflow::ofp_header));
		}
	};
	}

	if (get_length() < cofmsg_group_mod::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}


