/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofports.h"

using namespace rofl::openflow;

size_t cofports::length() const {
  size_t len = 0;
  AcquireReadLock lock(ports_lock);
  for (auto it : ports) {
    len += (*(it.second)).length();
  }
  return len;
}

void cofports::pack(uint8_t *buf, size_t buflen) {
  if (buflen < length()) {
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);
  }

  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION: {
    for (std::map<uint32_t, cofport *>::iterator it = ports.begin();
         it != ports.end(); ++it) {
      cofport &port = *(it->second);
      port.pack(buf, sizeof(struct rofl::openflow10::ofp_port));
      buf += sizeof(struct rofl::openflow10::ofp_port);
    }
  } break;
  case rofl::openflow12::OFP_VERSION: {
    for (std::map<uint32_t, cofport *>::iterator it = ports.begin();
         it != ports.end(); ++it) {
      cofport &port = *(it->second);
      port.pack(buf, port.length());
      buf += port.length();
    }
  } break;
  case rofl::openflow13::OFP_VERSION: {
    for (std::map<uint32_t, cofport *>::iterator it = ports.begin();
         it != ports.end(); ++it) {
      cofport &port = *(it->second);
      port.pack(buf, port.length());
      buf += port.length();
    }
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofports::unpack(uint8_t *buf, size_t buflen) {
  clear();

  while (buflen > 0) {

    switch (ofp_version) {
    case rofl::openflow10::OFP_VERSION: {
      if (buflen < (int)sizeof(struct rofl::openflow10::ofp_port))
        return;

      cofport port(ofp_version, buf, sizeof(struct rofl::openflow10::ofp_port));
      if (ports.find(port.get_port_no()) != ports.end()) {
        delete ports[port.get_port_no()];
      }
      ports[port.get_port_no()] = new cofport(port);

      buf += sizeof(struct rofl::openflow10::ofp_port);
      buflen -= sizeof(struct rofl::openflow10::ofp_port);

    } break;
    case rofl::openflow12::OFP_VERSION: {
      if (buflen < (int)sizeof(struct rofl::openflow12::ofp_port))
        return;

      cofport port(ofp_version, buf, sizeof(struct rofl::openflow12::ofp_port));
      if (ports.find(port.get_port_no()) != ports.end()) {
        delete ports[port.get_port_no()];
      }
      ports[port.get_port_no()] = new cofport(port);

      buf += sizeof(struct rofl::openflow12::ofp_port);
      buflen -= sizeof(struct rofl::openflow12::ofp_port);

    } break;
    case rofl::openflow13::OFP_VERSION: {
      if (buflen < (int)sizeof(struct rofl::openflow13::ofp_port))
        return;

      cofport port(ofp_version, buf, sizeof(struct rofl::openflow13::ofp_port));
      if (ports.find(port.get_port_no()) != ports.end()) {
        delete ports[port.get_port_no()];
      }
      ports[port.get_port_no()] = new cofport(port);

      buf += sizeof(struct rofl::openflow13::ofp_port);
      buflen -= sizeof(struct rofl::openflow13::ofp_port);

    } break;
    default:
      throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
    }
  }
}
