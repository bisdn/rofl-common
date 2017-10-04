/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_queue_get_config.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_QUEUE_GET_CONFIG_H_
#define COFMSG_QUEUE_GET_CONFIG_H_ 1

#include <ostream>

#include "rofl/common/openflow/cofpacketqueues.h"
#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_queue_get_config_request : public cofmsg {
public:
  /**
   *
   */
  virtual ~cofmsg_queue_get_config_request();

  /**
   *
   */
  cofmsg_queue_get_config_request(
      uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN, uint32_t xid = 0,
      uint32_t portno = 0);

  /**
   *
   */
  cofmsg_queue_get_config_request(const cofmsg_queue_get_config_request &msg);

  /**
   *
   */
  cofmsg_queue_get_config_request &
  operator=(const cofmsg_queue_get_config_request &msg);

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf = nullptr, size_t buflen = 0);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  uint32_t get_portno() const { return portno; };

  /**
   *
   */
  void set_portno(uint32_t portno) { this->portno = portno; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofmsg_queue_get_config_request &msg) {
    os << dynamic_cast<const cofmsg &>(msg);
    os << "<cofmsg_queue_get_config_request >" << std::endl;
    os << "<portno: 0x" << std::hex << (unsigned int)msg.get_portno()
       << std::dec << " >" << std::endl;
    return os;
  };

  virtual std::string str() const {
    std::stringstream ss;
    ss << cofmsg::str() << "-Queue-Get-Config-Request- ";
    return ss.str();
  };

private:
  uint32_t portno;
};

/**
 *
 */
class cofmsg_queue_get_config_reply : public cofmsg {
public:
  /**
   *
   */
  virtual ~cofmsg_queue_get_config_reply();

  /**
   *
   */
  cofmsg_queue_get_config_reply(uint8_t of_version = 0, uint32_t xid = 0,
                                uint32_t port_no = 0,
                                const rofl::openflow::cofpacket_queues &pql =
                                    rofl::openflow::cofpacket_queues());

  /**
   *
   */
  cofmsg_queue_get_config_reply(const cofmsg_queue_get_config_reply &msg);

  /**
   *
   */
  cofmsg_queue_get_config_reply &
  operator=(const cofmsg_queue_get_config_reply &msg);

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf = nullptr, size_t buflen = 0);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  uint32_t get_portno() const { return portno; };

  /**
   *
   */
  void set_portno(uint32_t portno) { this->portno = portno; };

  /**
   *
   */
  const rofl::openflow::cofpacket_queues &get_queues() const { return queues; };

  /**
   *
   */
  rofl::openflow::cofpacket_queues &set_queues() { return queues; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofmsg_queue_get_config_reply &msg) {
    os << dynamic_cast<const cofmsg &>(msg);
    os << "<cofmsg_queue_get_config_request >" << std::endl;
    os << "<portno: 0x" << std::hex << (unsigned int)msg.get_portno()
       << std::dec << " >" << std::endl;

    os << msg.queues;
    return os;
  };

  virtual std::string str() const {
    std::stringstream ss;
    ss << cofmsg::str() << "-Queue-Get-Config-Reply- ";
    return ss.str();
  };

private:
  uint32_t portno;
  cofpacket_queues queues;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_QUEUE_GET_CONFIG_H_ */
