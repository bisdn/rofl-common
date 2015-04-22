#include "rofl/common/openflow/messages/cofmsg_config.h"

using namespace rofl::openflow;

size_t
cofmsg_get_config_reply::length() const
{
	return (sizeof(struct rofl::openflow10::ofp_switch_config));
}



void
cofmsg_get_config_reply::pack(
		uint8_t* buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_get_config_reply::pack()");

	struct rofl::openflow10::ofp_switch_config* hdr =
			(struct rofl::openflow10::ofp_switch_config*)buf;

	hdr->flags         = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);
}



void
cofmsg_get_config_reply::unpack(
		uint8_t* buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (get_length() <= sizeof(struct rofl::openflow10::ofp_switch_config))
		throw eSwitchConfigBadLen();

	struct rofl::openflow10::ofp_switch_config* hdr =
			(struct rofl::openflow10::ofp_switch_config*)buf;

	flags         = be16toh(hdr->flags);
	miss_send_len = be16toh(hdr->miss_send_len);
}



size_t
cofmsg_set_config::length() const
{
	return (sizeof(struct rofl::openflow10::ofp_switch_config));
}



void
cofmsg_set_config::pack(
		uint8_t* buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_set_config::pack()");

	struct rofl::openflow10::ofp_switch_config* hdr =
			(struct rofl::openflow10::ofp_switch_config*)buf;

	hdr->flags         = htobe16(flags);
	hdr->miss_send_len = htobe16(miss_send_len);
}



void
cofmsg_set_config::unpack(
		uint8_t* buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (get_length() <= sizeof(struct rofl::openflow10::ofp_switch_config))
		throw eSwitchConfigBadLen();

	struct rofl::openflow10::ofp_switch_config* hdr =
			(struct rofl::openflow10::ofp_switch_config*)buf;

	flags         = be16toh(hdr->flags);
	miss_send_len = be16toh(hdr->miss_send_len);
}



