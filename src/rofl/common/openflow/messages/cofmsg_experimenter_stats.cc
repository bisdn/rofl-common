#include "cofmsg_experimenter_stats.h"

using namespace rofl::openflow;

cofmsg_experimenter_stats_request::~cofmsg_experimenter_stats_request()
{}



cofmsg_experimenter_stats_request::cofmsg_experimenter_stats_request(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		uint32_t exp_id,
		uint32_t exp_type,
		const rofl::cmemory& body) :
				cofmsg_stats_request(version, xid, rofl::openflow::OFPMP_EXPERIMENTER, stats_flags),
				exp_id(exp_id),
				exp_type(exp_type),
				body(body)
{
	switch (version) {
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_VENDOR);
	} break;
	default: {
		set_stats_type(rofl::openflow13::OFPMP_EXPERIMENTER);
	};
	}
}



cofmsg_experimenter_stats_request::cofmsg_experimenter_stats_request(
		const cofmsg_experimenter_stats_request& msg)
{
	*this = msg;
}



cofmsg_experimenter_stats_request&
cofmsg_experimenter_stats_request::operator= (
		const cofmsg_experimenter_stats_request& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_request::operator= (msg);
	exp_id = msg.exp_id;
	exp_type = msg.exp_type;
	body = msg.body;
	return *this;
}



size_t
cofmsg_experimenter_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request) +
				sizeof(struct rofl::openflow10::ofp_vendor_stats_header) + body.memlen());
	} break;
	default: {
		return (sizeof(struct rofl::openflow13::ofp_experimenter_header) + body.memlen());
	};
	}
	return 0;
}



void
cofmsg_experimenter_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_experimenter_stats_request::length())
		throw eMsgInval("cofmsg_experimenter_stats_request::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_request* hdr =
				(struct rofl::openflow10::ofp_stats_request*)buf;
		struct rofl::openflow10::ofp_vendor_stats_header* exp_hdr =
				(struct rofl::openflow10::ofp_vendor_stats_header*)(buf + sizeof(struct rofl::openflow10::ofp_stats_request));
		exp_hdr->vendor = htobe32(exp_id);
		body.pack(hdr->body + sizeof(struct rofl::openflow10::ofp_vendor_stats_header), body.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_experimenter_header* hdr =
				(struct rofl::openflow13::ofp_experimenter_header*)buf;
		hdr->experimenter = htobe32(exp_id);
		hdr->exp_type = htobe32(exp_type);
		body.pack(hdr->body, body.length());
	};
	}
}



void
cofmsg_experimenter_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats_request::unpack(buf, buflen);

	body.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_experimenter_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_experimenter_stats_request::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		struct rofl::openflow10::ofp_vendor_stats_header* exp_hdr =
				(struct rofl::openflow10::ofp_vendor_stats_header*)(buf + sizeof(struct rofl::openflow10::ofp_stats_request));
		exp_id = be32toh(exp_hdr->vendor);
		exp_type = 0;
		if (buflen > (sizeof(struct rofl::openflow10::ofp_stats_reply) +
						sizeof(struct rofl::openflow10::ofp_vendor_stats_header))) {
			body.unpack(hdr->body + sizeof(struct rofl::openflow10::ofp_vendor_stats_header),
									buflen - (sizeof(struct rofl::openflow10::ofp_stats_reply) +
												sizeof(struct rofl::openflow10::ofp_vendor_stats_header)));
		}
	} break;
	default: {
		struct rofl::openflow13::ofp_experimenter_header* hdr =
				(struct rofl::openflow13::ofp_experimenter_header*)buf;
		exp_id = be32toh(hdr->experimenter);
		exp_type = be32toh(hdr->exp_type);
		if (buflen > sizeof(struct rofl::openflow13::ofp_experimenter_header)) {
			body.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_experimenter_header));
		}
	};
	}

	if (get_length() < cofmsg_experimenter_stats_request::length())
		throw eBadSyntaxTooShort("cofmsg_experimenter_stats_request::unpack() buf too short");
}




cofmsg_experimenter_stats_reply::~cofmsg_experimenter_stats_reply()
{}



cofmsg_experimenter_stats_reply::cofmsg_experimenter_stats_reply(
		uint8_t version,
		uint32_t xid,
		uint16_t stats_flags,
		uint32_t exp_id,
		uint32_t exp_type,
		const rofl::cmemory& body) :
				cofmsg_stats_reply(version, xid, rofl::openflow::OFPMP_EXPERIMENTER, stats_flags),
				exp_id(exp_id),
				exp_type(exp_type),
				body(body)
{
	switch (version) {
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_VENDOR);
	} break;
	default: {
		set_stats_type(rofl::openflow13::OFPMP_EXPERIMENTER);
	};
	}
}



cofmsg_experimenter_stats_reply::cofmsg_experimenter_stats_reply(
		const cofmsg_experimenter_stats_reply& msg)
{
	*this = msg;
}



cofmsg_experimenter_stats_reply&
cofmsg_experimenter_stats_reply::operator= (
		const cofmsg_experimenter_stats_reply& msg)
{
	if (this == &msg)
		return *this;
	cofmsg_stats_reply::operator= (msg);
	exp_id = msg.exp_id;
	exp_type = msg.exp_type;
	body = msg.body;
	return *this;
}



size_t
cofmsg_experimenter_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) +
				sizeof(struct rofl::openflow10::ofp_vendor_stats_header) + body.memlen());
	} break;
	default: {
		return (sizeof(struct rofl::openflow13::ofp_experimenter_header) + body.memlen());
	};
	}
	return 0;
}



void
cofmsg_experimenter_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_experimenter_stats_reply::length())
		throw eMsgInval("cofmsg_experimenter_stats_reply::pack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		struct rofl::openflow10::ofp_vendor_stats_header* exp_hdr =
				(struct rofl::openflow10::ofp_vendor_stats_header*)(buf + sizeof(struct rofl::openflow10::ofp_stats_reply));
		exp_hdr->vendor = htobe32(exp_id);
		body.pack(hdr->body + sizeof(struct rofl::openflow10::ofp_vendor_stats_header), body.length());
	} break;
	default: {
		struct rofl::openflow13::ofp_experimenter_header* hdr =
				(struct rofl::openflow13::ofp_experimenter_header*)buf;
		hdr->experimenter = htobe32(exp_id);
		hdr->exp_type = htobe32(exp_type);
		body.pack(hdr->body, body.length());
	};
	}
}



void
cofmsg_experimenter_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats_reply::unpack(buf, buflen);

	body.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_experimenter_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_experimenter_stats_reply::unpack() buf too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		struct rofl::openflow10::ofp_stats_reply* hdr =
				(struct rofl::openflow10::ofp_stats_reply*)buf;
		struct rofl::openflow10::ofp_vendor_stats_header* exp_hdr =
				(struct rofl::openflow10::ofp_vendor_stats_header*)(buf + sizeof(struct rofl::openflow10::ofp_stats_reply));
		exp_id = be32toh(exp_hdr->vendor);
		exp_type = 0;
		if (buflen > (sizeof(struct rofl::openflow10::ofp_stats_reply) +
						sizeof(struct rofl::openflow10::ofp_vendor_stats_header))) {
			body.unpack(hdr->body + sizeof(struct rofl::openflow10::ofp_vendor_stats_header),
									buflen - (sizeof(struct rofl::openflow10::ofp_stats_reply) +
												sizeof(struct rofl::openflow10::ofp_vendor_stats_header)));
		}
	} break;
	default: {
		struct rofl::openflow13::ofp_experimenter_header* hdr =
				(struct rofl::openflow13::ofp_experimenter_header*)buf;
		exp_id = be32toh(hdr->experimenter);
		exp_type = be32toh(hdr->exp_type);
		if (buflen > sizeof(struct rofl::openflow13::ofp_experimenter_header)) {
			body.unpack(hdr->body, buflen - sizeof(struct rofl::openflow13::ofp_experimenter_header));
		}
	};
	}

	if (get_length() < cofmsg_experimenter_stats_reply::length())
		throw eBadSyntaxTooShort("cofmsg_experimenter_stats_reply::unpack() buf too short");
}



