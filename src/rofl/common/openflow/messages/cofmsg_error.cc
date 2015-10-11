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

	body.resize(0);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_error::length())
		throw eBadRequestBadLen("cofmsg_error::unpack() buf too short [1]");

	switch (get_version()) {
	default: {
		if (get_type() != rofl::openflow::OFPT_ERROR)
			throw eMsgInval("cofmsg_error::unpack() invalid message type");

		struct rofl::openflow::ofp_error_msg* hdr =
				(struct rofl::openflow::ofp_error_msg*)buf;

		err_type = be16toh(hdr->type);
		err_code = be16toh(hdr->code);

		if (buflen > sizeof(struct rofl::openflow::ofp_error_msg)) {
			body.unpack(buf + sizeof(struct rofl::openflow::ofp_error_msg),
					buflen - sizeof(struct rofl::openflow::ofp_error_msg));
		}
	};
	}

	if (get_length() < cofmsg_error::length())
		throw eBadRequestBadLen("cofmsg_error::unpack() buf too short [2]");
}


