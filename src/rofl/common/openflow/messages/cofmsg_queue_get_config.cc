/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg_queue_get_config.h"

using namespace rofl::openflow;

cofmsg_queue_get_config_request::~cofmsg_queue_get_config_request() {}

cofmsg_queue_get_config_request::cofmsg_queue_get_config_request(
    uint8_t version, uint32_t xid, uint32_t portno)
    : cofmsg(version, rofl::openflow::OFPT_QUEUE_GET_CONFIG_REQUEST /*=22*/,
             xid),
      portno(portno) {
  switch (version) {
  case rofl::openflow10::OFP_VERSION: {
    set_type(rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST); /*=20*/
  } break;
  default: { /* do nothing */
  };
  }
}

cofmsg_queue_get_config_request::cofmsg_queue_get_config_request(
    const cofmsg_queue_get_config_request &msg)
    : cofmsg(msg), portno(msg.portno) {}

cofmsg_queue_get_config_request &cofmsg_queue_get_config_request::
operator=(const cofmsg_queue_get_config_request &msg) {
  if (this == &msg)
    return *this;
  cofmsg::operator=(msg);
  portno = msg.portno;
  return *this;
}

size_t cofmsg_queue_get_config_request::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct rofl::openflow10::ofp_queue_get_config_request));
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_queue_get_config_request));
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_queue_get_config_request));
  } break;
  default:
    throw eBadVersion("eBadVersion").set_func(__FUNCTION__).set_line(__LINE__);
  }
  return 0;
}

void cofmsg_queue_get_config_request::pack(uint8_t *buf, size_t buflen) {
  cofmsg::pack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_queue_get_config_request::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    struct rofl::openflow10::ofp_queue_get_config_request *hdr =
        (struct rofl::openflow10::ofp_queue_get_config_request *)buf;
    hdr->port = htobe16((uint16_t)(portno & 0x0000ffff));
  } break;
  default: {
    struct rofl::openflow13::ofp_queue_get_config_request *hdr =
        (struct rofl::openflow13::ofp_queue_get_config_request *)buf;
    hdr->port = htobe32(portno);
  };
  }
}

void cofmsg_queue_get_config_request::unpack(uint8_t *buf, size_t buflen) {
  cofmsg::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_queue_get_config_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_type() != rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow10::ofp_queue_get_config_request *hdr =
        (struct rofl::openflow10::ofp_queue_get_config_request *)buf;
    portno = be16toh(hdr->port);
  } break;
  default: {
    if (get_type() != rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_queue_get_config_request *hdr =
        (struct rofl::openflow13::ofp_queue_get_config_request *)buf;
    portno = be32toh(hdr->port);
  };
  }

  if (get_length() < cofmsg_queue_get_config_request::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}

cofmsg_queue_get_config_reply::~cofmsg_queue_get_config_reply() {}

cofmsg_queue_get_config_reply::cofmsg_queue_get_config_reply(
    uint8_t version, uint32_t xid, uint32_t portno,
    const rofl::openflow::cofpacket_queues &queues)
    : cofmsg(version, rofl::openflow::OFPT_QUEUE_GET_CONFIG_REPLY /*=23*/, xid),
      portno(portno), queues(queues) {
  switch (version) {
  case rofl::openflow10::OFP_VERSION: {
    set_type(rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY); /*=21*/
  } break;
  default: { /* do nothing */
  };
  }
}

cofmsg_queue_get_config_reply::cofmsg_queue_get_config_reply(
    const cofmsg_queue_get_config_reply &msg)
    : cofmsg(msg), portno(msg.portno), queues(msg.queues) {}

cofmsg_queue_get_config_reply &cofmsg_queue_get_config_reply::
operator=(const cofmsg_queue_get_config_reply &msg) {
  if (this == &msg)
    return *this;
  cofmsg::operator=(msg);
  portno = msg.portno;
  queues = msg.queues;
  return *this;
}

size_t cofmsg_queue_get_config_reply::length() const {
  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    return (sizeof(struct rofl::openflow10::ofp_queue_get_config_reply) +
            queues.length());
  } break;
  case rofl::openflow12::OFP_VERSION: {
    return (sizeof(struct rofl::openflow12::ofp_queue_get_config_reply) +
            queues.length());
  } break;
  case rofl::openflow13::OFP_VERSION: {
    return (sizeof(struct rofl::openflow13::ofp_queue_get_config_reply) +
            queues.length());
  } break;
  default:
    throw eBadVersion("eBadVersion").set_func(__FUNCTION__).set_line(__LINE__);
  }
  return 0;
}

void cofmsg_queue_get_config_reply::pack(uint8_t *buf, size_t buflen) {
  cofmsg::pack(buf, buflen); // copies common statistics header

  if ((0 == buf) || (0 == buflen))
    return;

  if (buflen < cofmsg_queue_get_config_reply::length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    struct rofl::openflow10::ofp_queue_get_config_reply *hdr =
        (struct rofl::openflow10::ofp_queue_get_config_reply *)buf;
    hdr->port = htobe16((uint16_t)(portno & 0x0000ffff));
    queues.pack((uint8_t *)(hdr->queues), queues.length());
  } break;
  default: {
    struct rofl::openflow13::ofp_queue_get_config_reply *hdr =
        (struct rofl::openflow13::ofp_queue_get_config_reply *)buf;
    hdr->port = htobe32(portno);
    queues.pack((uint8_t *)(hdr->queues), queues.length());
  };
  }
}

void cofmsg_queue_get_config_reply::unpack(uint8_t *buf, size_t buflen) {
  cofmsg::unpack(buf, buflen);

  if ((0 == buf) || (0 == buflen))
    return;

  queues.clear();
  queues.set_version(get_version());

  if (buflen < cofmsg_queue_get_config_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {
    if (get_type() != rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow10::ofp_queue_get_config_reply *hdr =
        (struct rofl::openflow10::ofp_queue_get_config_reply *)buf;
    portno = be16toh(hdr->port);
    if (buflen > sizeof(struct rofl::openflow10::ofp_queue_get_config_reply)) {
      queues.unpack(
          (uint8_t *)(hdr->queues),
          buflen - sizeof(struct rofl::openflow10::ofp_queue_get_config_reply));
    }
  } break;
  default: {
    if (get_type() != rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY)
      throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                               __LINE__);

    struct rofl::openflow13::ofp_queue_get_config_reply *hdr =
        (struct rofl::openflow13::ofp_queue_get_config_reply *)buf;
    portno = be32toh(hdr->port);
    if (buflen > sizeof(struct rofl::openflow13::ofp_queue_get_config_reply)) {
      queues.unpack(
          (uint8_t *)(hdr->queues),
          buflen - sizeof(struct rofl::openflow13::ofp_queue_get_config_reply));
    }
  };
  }

  if (get_length() < cofmsg_queue_get_config_reply::length())
    throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                            __LINE__);
}
