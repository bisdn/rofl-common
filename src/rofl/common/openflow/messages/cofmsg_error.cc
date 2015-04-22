#include "cofmsg_error.h"

using namespace rofl::openflow;

size_t
cofmsg_error::length() const
{
	return (sizeof(struct rofl::openflow::ofp_error_msg) + body.memlen());
}



void
cofmsg_error::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_error::pack() buf too short");

	struct rofl::openflow::ofp_error_msg* hdr =
			(struct rofl::openflow::ofp_error_msg*)buf;

	hdr->type = htobe16(err_type);
	hdr->code = htobe16(err_code);

	body.pack(buf + sizeof(struct rofl::openflow::ofp_error_msg),
			buflen - sizeof(struct rofl::openflow::ofp_error_msg));
}



void
cofmsg_error::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (get_length() <= sizeof(struct rofl::openflow::ofp_error_msg))
		return;

	struct rofl::openflow::ofp_error_msg* hdr =
			(struct rofl::openflow::ofp_error_msg*)buf;

	err_type = be16toh(hdr->type);
	err_code = be16toh(hdr->code);

	body.unpack(buf + sizeof(struct rofl::openflow::ofp_error_msg),
			buflen - sizeof(struct rofl::openflow::ofp_error_msg));
}


