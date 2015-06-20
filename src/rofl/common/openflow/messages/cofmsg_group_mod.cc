#include "rofl/common/openflow/messages/cofmsg_group_mod.h"

using namespace rofl::openflow;

cofmsg_group_mod::cofmsg_group_mod(
		uint8_t ofp_version, uint32_t xid, const cofgroupmod& groupmod) :
				cofmsg(ofp_version, xid, rofl::openflow::OFPT_FLOW_MOD),
				groupmod(groupmod)
{
	this->groupmod.set_version(ofp_version);
}



cofmsg_group_mod::cofmsg_group_mod(
		cmemory *memarea) :
	cofmsg(memarea),
	groupmod(get_version())
{

}



cofmsg_group_mod::cofmsg_group_mod(
		const cofmsg_group_mod& groupmod)
{
	*this = groupmod;
}



cofmsg_group_mod&
cofmsg_group_mod::operator= (
		const cofmsg_group_mod& msg)
{
	if (this == &msg)
		return *this;

	cofmsg::operator= (msg);

	groupmod	= msg.groupmod;

	return *this;
}



cofmsg_group_mod::~cofmsg_group_mod()
{

}



void
cofmsg_group_mod::reset()
{
	cofmsg::reset();
	groupmod.clear();
}



size_t
cofmsg_group_mod::length() const
{
	switch (ofh_header->version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		return sizeof(struct rofl::openflow::ofp_header) + groupmod.length();
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_mod::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {

		cofmsg::pack(buf, buflen);

		struct rofl::openflow::ofp_header* hdr = (struct rofl::openflow::ofp_header*)buf;

		groupmod.pack(hdr->body, groupmod.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_mod::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_mod::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	groupmod.clear();

	switch (get_version()) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {

		if (cofmsg::framelen() < length())
			throw eInval("cofmsg_group_mod::validate() framelen too short");

		struct rofl::openflow::ofp_header* hdr = (struct rofl::openflow::ofp_header*)(cofmsg::soframe());

		size_t groupmodlen = cofmsg::framelen() - sizeof(struct rofl::openflow::ofp_header);

		if (groupmodlen > 0) {
			groupmod.unpack(hdr->body, groupmodlen);
		}

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



void
cofmsg_group_mod::check_prerequisites() const
{

}





