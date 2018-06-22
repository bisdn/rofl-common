/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofport.h"

using namespace rofl::openflow;

size_t cofport::length() const {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
    return sizeof(struct openflow10::ofp_port);
  case rofl::openflow12::OFP_VERSION:
    return sizeof(struct openflow12::ofp_port);
  case rofl::openflow13::OFP_VERSION:
    return sizeof(struct openflow13::ofp_port);
  default:
    return sizeof(struct openflow14::ofp_port) + properties.length();
  }
}

void cofport::pack(uint8_t *buf, size_t buflen) {
  if (buflen < length()) {
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);
  }

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_port *hdr =
        (struct rofl::openflow10::ofp_port *)buf;

    hdr->port_no = htobe16(portno & 0x0000ffff);
    memcpy(hdr->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
    static_assert(sizeof(hdr->name) == OFP_MAX_PORT_NAME_LEN, "invalid length of hdr->name");
    // this is somewhat awkward
#if __GNUC__ >= 8
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
    strncpy(hdr->name, name.c_str(), OFP_MAX_PORT_NAME_LEN - 1);
#if __GNUC__ >= 8
#pragma GCC diagnostic pop
#endif
    if (name.length() >= OFP_MAX_PORT_NAME_LEN) {
      hdr->name[OFP_MAX_PORT_NAME_LEN - 1] = '\0';
    }
    hdr->config = htobe32(config);
    hdr->state = htobe32(state);
    hdr->curr = htobe32(get_ethernet().get_curr());
    hdr->advertised = htobe32(get_ethernet().get_advertised());
    hdr->supported = htobe32(get_ethernet().get_supported());
    hdr->peer = htobe32(get_ethernet().get_peer());

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow12::ofp_port *hdr =
        (struct rofl::openflow12::ofp_port *)buf;

    hdr->port_no = htobe32(portno);
    memcpy(hdr->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
    strncpy(hdr->name, name.c_str(), OFP_MAX_PORT_NAME_LEN - 1);
    if (name.length() >= OFP_MAX_PORT_NAME_LEN) {
      hdr->name[OFP_MAX_PORT_NAME_LEN - 1] = '\0';
    }
    hdr->config = htobe32(config);
    hdr->state = htobe32(state);
    hdr->curr = htobe32(get_ethernet().get_curr());
    hdr->advertised = htobe32(get_ethernet().get_advertised());
    hdr->supported = htobe32(get_ethernet().get_supported());
    hdr->peer = htobe32(get_ethernet().get_peer());
    hdr->curr_speed = htobe32(get_ethernet().get_curr_speed());
    hdr->max_speed = htobe32(get_ethernet().get_max_speed());

  } break;
  default: {

    struct rofl::openflow14::ofp_port *hdr =
        (struct rofl::openflow14::ofp_port *)buf;

    hdr->port_no = htobe32(portno);
    memcpy(hdr->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
    strncpy(hdr->name, name.c_str(), OFP_MAX_PORT_NAME_LEN - 1);
    if (name.length() >= OFP_MAX_PORT_NAME_LEN) {
      hdr->name[OFP_MAX_PORT_NAME_LEN - 1] = '\0';
    }
    hdr->config = htobe32(config);
    hdr->state = htobe32(state);

    properties.pack((uint8_t *)hdr->properties,
                    buflen - sizeof(struct rofl::openflow14::ofp_port));
  };
  }
}

void cofport::unpack(uint8_t *buf, size_t buflen) {
  if (buflen < length())
    throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);

  switch (get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    struct rofl::openflow10::ofp_port *hdr =
        (struct rofl::openflow10::ofp_port *)buf;

    portno = be16toh(hdr->port_no);
    memcpy(hwaddr.somem(), hdr->hw_addr, OFP_ETH_ALEN);
    name.assign(hdr->name, strnlen(hdr->name, OFP_MAX_PORT_NAME_LEN));

    config = be32toh(hdr->config);
    state = be32toh(hdr->state);
    set_ethernet().set_curr(be32toh(hdr->curr));
    set_ethernet().set_advertised(be32toh(hdr->advertised));
    set_ethernet().set_supported(be32toh(hdr->supported));
    set_ethernet().set_peer(be32toh(hdr->peer));

  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {

    struct rofl::openflow12::ofp_port *hdr =
        (struct rofl::openflow12::ofp_port *)buf;

    portno = be32toh(hdr->port_no);
    memcpy(hwaddr.somem(), hdr->hw_addr, OFP_ETH_ALEN);
    name.assign(hdr->name, strnlen(hdr->name, OFP_MAX_PORT_NAME_LEN));

    config = be32toh(hdr->config);
    state = be32toh(hdr->state);
    set_ethernet().set_curr(be32toh(hdr->curr));
    set_ethernet().set_advertised(be32toh(hdr->advertised));
    set_ethernet().set_supported(be32toh(hdr->supported));
    set_ethernet().set_peer(be32toh(hdr->peer));
    set_ethernet().set_curr_speed(be32toh(hdr->curr_speed));
    set_ethernet().set_max_speed(be32toh(hdr->max_speed));

  } break;
  default: {

    struct rofl::openflow14::ofp_port *hdr =
        (struct rofl::openflow14::ofp_port *)buf;

    portno = be32toh(hdr->port_no);
    memcpy(hwaddr.somem(), hdr->hw_addr, OFP_ETH_ALEN);
    name.assign(hdr->name, strnlen(hdr->name, OFP_MAX_PORT_NAME_LEN));

    config = be32toh(hdr->config);
    state = be32toh(hdr->state);

    properties.unpack((uint8_t *)hdr->properties,
                      buflen - sizeof(struct rofl::openflow14::ofp_port));
  };
  }
}

void cofport::link_state_set_blocked() {
  switch (ofp_version) {
  // non-existing for OF 1.0
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() | openflow12::OFPPS_BLOCKED);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::link_state_clr_blocked() {
  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() & ~openflow12::OFPPS_BLOCKED);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

bool cofport::link_state_is_blocked() const {
  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (get_state() & openflow12::OFPPS_BLOCKED);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::link_state_set_live() {
  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() | openflow12::OFPPS_LIVE);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::link_state_clr_live() {
  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() & ~openflow12::OFPPS_LIVE);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

bool cofport::link_state_is_live() const {
  switch (ofp_version) {
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (get_state() & openflow12::OFPPS_LIVE);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::link_state_set_link_down() {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() | openflow10::OFPPS_LINK_DOWN);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::link_state_clr_link_down() {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() & ~openflow10::OFPPS_LINK_DOWN);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

bool cofport::link_state_is_link_down() const {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
    return (get_state() & openflow10::OFPPS_LINK_DOWN);
    break;
  case rofl::openflow12::OFP_VERSION:
    return (get_state() & openflow12::OFPPS_LINK_DOWN);
    break;
  case rofl::openflow13::OFP_VERSION:
    return (get_state() & openflow12::OFPPS_LINK_DOWN);
    break; // FIXME: openflow13::OFPPS_LINK_DOWN, once it's been defined
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::link_state_phy_down() {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() | openflow10::OFPPS_LINK_DOWN);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::link_state_phy_up() {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    set_state(get_state() & ~openflow10::OFPPS_LINK_DOWN);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

bool cofport::link_state_phy_is_up() const {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return not(get_state() & openflow10::OFPPS_LINK_DOWN);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

bool cofport::config_is_port_down() const {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION:
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION: {
    return (get_config() & openflow10::OFPPS_LINK_DOWN);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::recv_port_mod(uint32_t config, uint32_t mask,
                            uint32_t advertise) {
  switch (ofp_version) {
  case rofl::openflow10::OFP_VERSION: {
    recv_port_mod_of10(config, mask, advertise);
  } break;
  case rofl::openflow12::OFP_VERSION:
  case rofl::openflow13::OFP_VERSION:
  case rofl::openflow14::OFP_VERSION: {
    recv_port_mod_of12(config, mask, advertise);
  } break;
  default:
    throw eBadVersion("eBadVersion", __FILE__, __FUNCTION__, __LINE__);
  }
}

void cofport::recv_port_mod_of10(uint32_t config, uint32_t mask,
                                 uint32_t advertise) {
  if (mask & openflow10::OFPPC_PORT_DOWN) {
    if (config & openflow10::OFPPC_PORT_DOWN) {
      set_config(get_config() | openflow10::OFPPC_PORT_DOWN);
    } else {
      set_config(get_config() & ~openflow10::OFPPC_PORT_DOWN);
    }
  }

  if (mask & openflow10::OFPPC_NO_STP) {
    if (config & openflow10::OFPPC_NO_STP) {
      set_config(get_config() | openflow10::OFPPC_NO_STP);
    } else {
      set_config(get_config() & ~openflow10::OFPPC_NO_STP);
    }
  }

  if (mask & openflow10::OFPPC_NO_RECV) {
    if (config & openflow10::OFPPC_NO_RECV) {
      set_config(get_config() | openflow10::OFPPC_NO_RECV);
    } else {
      set_config(get_config() & ~openflow10::OFPPC_NO_RECV);
    }
  }

  if (mask & openflow10::OFPPC_NO_RECV_STP) {
    if (config & openflow10::OFPPC_NO_RECV_STP) {
      set_config(get_config() | openflow10::OFPPC_NO_RECV_STP);
    } else {
      set_config(get_config() & ~openflow10::OFPPC_NO_RECV_STP);
    }
  }

  if (mask & openflow10::OFPPC_NO_FLOOD) {
    if (config & openflow10::OFPPC_NO_FLOOD) {
      set_config(get_config() | openflow10::OFPPC_NO_FLOOD);
    } else {
      set_config(get_config() & ~openflow10::OFPPC_NO_FLOOD);
    }
  }

  if (mask & openflow10::OFPPC_NO_PACKET_IN) {
    if (config & openflow10::OFPPC_NO_PACKET_IN) {
      set_config(get_config() | openflow10::OFPPC_NO_PACKET_IN);
    } else {
      set_config(get_config() & ~openflow10::OFPPC_NO_PACKET_IN);
    }
  }

  if (mask & openflow10::OFPPC_NO_FWD) {
    if (config & openflow10::OFPPC_NO_FWD) {
      set_config(get_config() | openflow10::OFPPC_NO_FWD);
    } else {
      set_config(get_config() & ~openflow10::OFPPC_NO_FWD);
    }
  }

  if (0 != advertise) {
    set_ethernet().set_advertised(advertise);
  }
}

void cofport::recv_port_mod_of12(uint32_t config, uint32_t mask,
                                 uint32_t advertise) {
  if (mask & openflow12::OFPPC_PORT_DOWN) {
    if (config & openflow12::OFPPC_PORT_DOWN) {
      set_config(get_config() | openflow12::OFPPC_PORT_DOWN);
    } else {
      set_config(get_config() & ~openflow12::OFPPC_PORT_DOWN);
    }
  }

  if (mask & openflow12::OFPPC_NO_RECV) {
    if (config & openflow12::OFPPC_NO_RECV) {
      set_config(get_config() | openflow12::OFPPC_NO_RECV);
    } else {
      set_config(get_config() & ~openflow12::OFPPC_NO_RECV);
    }
  }

  if (mask & openflow12::OFPPC_NO_PACKET_IN) {
    if (config & openflow12::OFPPC_NO_PACKET_IN) {
      set_config(get_config() | openflow12::OFPPC_NO_PACKET_IN);
    } else {
      set_config(get_config() & ~openflow12::OFPPC_NO_PACKET_IN);
    }
  }

  if (mask & openflow12::OFPPC_NO_FWD) {
    if (config & openflow12::OFPPC_NO_FWD) {
      set_config(get_config() | openflow12::OFPPC_NO_FWD);
    } else {
      set_config(get_config() & ~openflow12::OFPPC_NO_FWD);
    }
  }

  if (0 != advertise) {
    set_ethernet().set_advertised(advertise);
  }
}

uint8_t cofport::get_version() const { return ofp_version; }
