#include "cofmsg_flow_mod.h"

using namespace rofl::openflow;

size_t
cofmsg_flow_mod::length() const
{
	switch (get_version()) {
	default: {
		return cofmsg::length() + flowmod.length();
	};
	}
	return 0;
}



void
cofmsg_flow_mod::pack(uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_features_reply::pack()");

	switch (get_version()) {
	default: {

		struct rofl::openflow::ofp_header* hdr =
				(struct rofl::openflow::ofp_header*)buf;

		flowmod.pack(hdr->body, flowmod.length());
	};
	}
}



void
cofmsg_flow_mod::unpack(uint8_t *buf, size_t buflen)
{
	flowmod.clear();

	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	default: {

		if (buflen < get_length())
			throw eBadSyntaxTooShort("cofmsg_flow_mod::unpack()");

		struct rofl::openflow::ofp_header* hdr =
				(struct rofl::openflow::ofp_header*)buf;

		size_t flowmodlen = buflen - sizeof(struct rofl::openflow::ofp_header);

		if (flowmodlen > 0) {
			flowmod.unpack(hdr->body, flowmodlen);
		}
	};
	}
}



