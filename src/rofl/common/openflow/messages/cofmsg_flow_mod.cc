#include "cofmsg_flow_mod.h"

using namespace rofl::openflow;

cofmsg_flow_mod::~cofmsg_flow_mod() {}

cofmsg_flow_mod::cofmsg_flow_mod(uint8_t version, uint32_t xid,
                                 const rofl::openflow::cofflowmod &flowmod)
    : cofmsg(version, rofl::openflow::OFPT_FLOW_MOD, xid), flowmod(flowmod) {
  this->flowmod.set_version(version);
}

cofmsg_flow_mod::cofmsg_flow_mod(const cofmsg_flow_mod &msg) { *this = msg; }

cofmsg_flow_mod &cofmsg_flow_mod::operator=(const cofmsg_flow_mod &msg) {
  if (this == &msg)
    return *this;
  cofmsg::operator=(msg);
  flowmod = msg.flowmod;
  return *this;
}

void cofmsg_flow_mod::check_prerequisites() const {
  flowmod.check_prerequisites();
}

size_t cofmsg_flow_mod::length() const {
  switch (get_version()) {
  default: { return cofmsg::length() + flowmod.length(); };
  }
  return 0;
}

void cofmsg_flow_mod::pack(uint8_t *buf, size_t buflen) {
  cofmsg::pack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < get_length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  default: {

    struct rofl::openflow::ofp_header *hdr =
        (struct rofl::openflow::ofp_header *)buf;

    flowmod.pack(hdr->body, flowmod.length());
  };
  }
}

void cofmsg_flow_mod::unpack(uint8_t *buf, size_t buflen) {
  cofmsg::unpack(buf, buflen);

  flowmod.set_version(get_version());
  flowmod.clear();

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_flow_mod::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  default: {

    if (get_type() != rofl::openflow::OFPT_FLOW_MOD)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow::ofp_header *hdr =
        (struct rofl::openflow::ofp_header *)buf;

    if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
      flowmod.unpack(hdr->body,
                     buflen - sizeof(struct rofl::openflow::ofp_header));
    }
  };
  }

  if (get_length() < cofmsg_flow_mod::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}
