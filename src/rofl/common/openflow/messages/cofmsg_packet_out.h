/*
 * cofmsg_packet_out.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_PACKET_OUT_H_
#define COFMSG_PACKET_OUT_H_ 1

#include "rofl/common/cpacket.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_packet_out : public cofmsg {
public:
  /**
   *
   */
  virtual ~cofmsg_packet_out(){};

  /**
   *
   */
  cofmsg_packet_out(
      uint8_t version = 0, uint32_t xid = 0, uint32_t buffer_id = 0,
      uint32_t in_port = 0,
      const rofl::openflow::cofactions &actions = rofl::openflow::cofactions(),
      uint8_t *data = nullptr, size_t datalen = 0)
      : cofmsg(version, rofl::openflow::OFPT_PACKET_OUT, xid),
        buffer_id(buffer_id), in_port(in_port), actions(actions),
        packet(data, datalen){};

  /**
   *
   */
  cofmsg_packet_out(const cofmsg_packet_out &msg) { *this = msg; };

  /**
   *
   */
  cofmsg_packet_out &operator=(const cofmsg_packet_out &msg) {
    if (this == &msg)
      return *this;
    cofmsg::operator=(msg);
    buffer_id = msg.buffer_id;
    in_port = msg.in_port;
    actions = msg.actions;
    packet = msg.packet;
    return *this;
  };

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
  uint32_t get_buffer_id() const { return buffer_id; };

  /**
   *
   */
  void set_buffer_id(uint32_t buffer_id) { this->buffer_id = buffer_id; };

  /**
   *
   */
  uint32_t get_in_port() const { return in_port; };

  /**
   *
   */
  void set_in_port(uint32_t in_port) { this->in_port = in_port; };

  /**
   *
   */
  const rofl::openflow::cofactions &get_actions() const { return actions; };

  /**
   *
   */
  rofl::openflow::cofactions &set_actions() { return actions; };

  /**
   *
   */
  const rofl::cpacket &get_packet() const { return packet; };

  /**
   *
   */
  rofl::cpacket &set_packet() { return packet; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmsg_packet_out const &msg) {
    os << dynamic_cast<cofmsg const &>(msg);
    os << "<cofmsg_packet_out >" << std::endl;
    os << std::hex;
    os << "<buffer-id: 0x" << (unsigned int)msg.get_buffer_id() << " >"
       << std::endl;
    os << "<in-port: 0x" << (unsigned int)msg.get_in_port() << " >"
       << std::endl;
    os << std::dec;
    os << msg.actions;
    os << msg.packet;
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    ss << cofmsg::str() << "-Packet-Out- "
       << "buffer_id: " << (unsigned int)get_buffer_id()
       << ", in_port: " << (unsigned int)get_in_port() << " ";
    return ss.str();
  };

private:
  uint32_t buffer_id;
  uint32_t in_port;
  rofl::openflow::cofactions actions;
  rofl::cpacket packet;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_PACKET_OUT_H_ */
