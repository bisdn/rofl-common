/*
 * cofmsg_flow_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_MOD_H_
#define COFMSG_FLOW_MOD_H_ 1

#include "rofl/common/openflow/cofflowmod.h"
#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_flow_mod : public cofmsg {
public:
  /**
   *
   */
  virtual ~cofmsg_flow_mod();

  /**
   *
   */
  cofmsg_flow_mod(
      uint8_t version = rofl::openflow::OFP_VERSION_UNKNOWN, uint32_t xid = 0,
      const rofl::openflow::cofflowmod &flowmod = rofl::openflow::cofflowmod());

  /**
   *
   */
  cofmsg_flow_mod(const cofmsg_flow_mod &msg);

  /**
   *
   */
  cofmsg_flow_mod &operator=(const cofmsg_flow_mod &msg);

  /**
   *
   */
  void check_prerequisites() const;

public:
  /** returns length of packet in packed state
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
  const rofl::openflow::cofflowmod &get_flowmod() const { return flowmod; };

  /**
   *
   */
  rofl::openflow::cofflowmod &set_flowmod() { return flowmod; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofmsg_flow_mod &msg) {
    os << "<cofmsg_flow_mod >" << std::endl;
    { os << dynamic_cast<const cofmsg &>(msg); };
    { os << msg.get_flowmod(); };
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    ss << cofmsg::str() << "-Flow-Mod- flowmod: " << flowmod.str() << " ";
    return ss.str();
  };

private:
  rofl::openflow::cofflowmod flowmod;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_FLOW_MOD_H_ */
