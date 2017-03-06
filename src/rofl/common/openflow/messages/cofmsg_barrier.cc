#include "rofl/common/openflow/messages/cofmsg_barrier.h"

using namespace rofl::openflow;

size_t cofmsg_barrier_request::length() const {
  return (cofmsg::length() + body.memlen());
}

void cofmsg_barrier_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg::pack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < get_length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  body.pack(buf + sizeof(struct rofl::openflow::ofp_header),
            buflen - sizeof(struct rofl::openflow::ofp_header));
}

void cofmsg_barrier_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg::unpack(buf, buflen);

  body.resize(0);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_barrier_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_type() != rofl::openflow10::OFPT_BARRIER_REQUEST)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);

    if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
      body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
                  buflen - sizeof(struct rofl::openflow::ofp_header));
    }

  } break;
  default: {
    if (get_type() != rofl::openflow13::OFPT_BARRIER_REQUEST)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);

    if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
      body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
                  buflen - sizeof(struct rofl::openflow::ofp_header));
    }
  };
  }

  if (get_length() < cofmsg_barrier_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);
}

size_t cofmsg_barrier_reply::length() const {
  return (cofmsg::length() + body.memlen());
}

void cofmsg_barrier_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg::pack(buf, buflen);

  body.resize(0);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < get_length())
    throw eInvalid("eInvalid", __FILE__, __PRETTY_FUNCTION__, __LINE__);

  body.pack(buf + sizeof(struct rofl::openflow::ofp_header),
            buflen - sizeof(struct rofl::openflow::ofp_header));
}

void cofmsg_barrier_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_barrier_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_type() != rofl::openflow10::OFPT_BARRIER_REPLY)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);

    if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
      body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
                  buflen - sizeof(struct rofl::openflow::ofp_header));
    }

  } break;
  default: {
    if (get_type() != rofl::openflow13::OFPT_BARRIER_REPLY)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__,
                               __PRETTY_FUNCTION__, __LINE__);

    if (buflen > sizeof(struct rofl::openflow::ofp_header)) {
      body.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
                  buflen - sizeof(struct rofl::openflow::ofp_header));
    }
  };
  }

  if (get_length() < cofmsg_barrier_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__,
                            __LINE__);
}
