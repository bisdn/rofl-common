#include "rofl/common/openflow/messages/cofmsg_features.h"

using namespace rofl::openflow;

size_t
cofmsg_features_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_switch_features) + ports.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_switch_features) + ports.length());
	} break;
	default: {
		return (sizeof(struct rofl::openflow13::ofp_switch_features));
	};
	}
}



void
cofmsg_features_reply::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eInvalid("eInvalid").set_func(__PRETTY_FUNCTION__).set_line(__LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_switch_features* hdr =
				(struct rofl::openflow10::ofp_switch_features*)buf;

		hdr->datapath_id  = htobe64(dpid);
		hdr->n_buffers    = htobe32(n_buffers);
		hdr->n_tables     = n_tables;
		hdr->capabilities = htobe32(capabilities);
		hdr->actions      = htobe32(actions);

		ports.pack(buf + sizeof(struct rofl::openflow10::ofp_switch_features),
				buflen - sizeof(struct rofl::openflow10::ofp_switch_features));

	} break;
	case rofl::openflow12::OFP_VERSION: {

		struct rofl::openflow12::ofp_switch_features* hdr =
				(struct rofl::openflow12::ofp_switch_features*)buf;

		hdr->datapath_id  = htobe64(dpid);
		hdr->n_buffers    = htobe32(n_buffers);
		hdr->n_tables     = n_tables;
		hdr->capabilities = htobe32(capabilities);

		ports.pack(buf + sizeof(struct rofl::openflow12::ofp_switch_features),
				buflen - sizeof(struct rofl::openflow12::ofp_switch_features));

	} break;
	default: {

		struct rofl::openflow13::ofp_switch_features* hdr =
				(struct rofl::openflow13::ofp_switch_features*)buf;

		hdr->datapath_id  = htobe64(dpid);
		hdr->n_buffers    = htobe32(n_buffers);
		hdr->n_tables     = n_tables;
		hdr->capabilities = htobe32(capabilities);
		hdr->auxiliary_id = auxid;

	};
	}
}



void
cofmsg_features_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	this->ports.clear();
	this->ports.set_version(get_version());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofmsg_features_reply::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		if (get_type() != rofl::openflow10::OFPT_FEATURES_REPLY)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow10::ofp_switch_features* hdr =
				(struct rofl::openflow10::ofp_switch_features*)buf;

		dpid         = be64toh(hdr->datapath_id);
		n_buffers    = be32toh(hdr->n_buffers);
		n_tables     = hdr->n_tables;
		capabilities = be32toh(hdr->capabilities);
		actions      = be32toh(hdr->actions);

		if (buflen > sizeof(struct rofl::openflow10::ofp_switch_features)) {
			ports.unpack(buf + sizeof(struct rofl::openflow10::ofp_switch_features),
					buflen - sizeof(struct rofl::openflow10::ofp_switch_features));
		}

	} break;
	case rofl::openflow12::OFP_VERSION: {

		if (get_type() != rofl::openflow12::OFPT_FEATURES_REPLY)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow12::ofp_switch_features* hdr =
				(struct rofl::openflow12::ofp_switch_features*)buf;

		dpid         = be64toh(hdr->datapath_id);
		n_buffers    = be32toh(hdr->n_buffers);
		n_tables     = hdr->n_tables;
		capabilities = be32toh(hdr->capabilities);

		if (buflen > sizeof(struct rofl::openflow12::ofp_switch_features)) {
			ports.unpack(buf + sizeof(struct rofl::openflow12::ofp_switch_features),
					buflen - sizeof(struct rofl::openflow12::ofp_switch_features));
		}

	} break;
	default: {

		if (get_type() != rofl::openflow13::OFPT_FEATURES_REPLY)
			throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);

		struct rofl::openflow13::ofp_switch_features* hdr =
				(struct rofl::openflow13::ofp_switch_features*)buf;

		dpid         = be64toh(hdr->datapath_id);
		n_buffers    = be32toh(hdr->n_buffers);
		n_tables     = hdr->n_tables;
		auxid        = hdr->auxiliary_id;
		capabilities = be32toh(hdr->capabilities);

	};
	}

	if (get_length() < cofmsg_features_reply::length())
		throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
}


