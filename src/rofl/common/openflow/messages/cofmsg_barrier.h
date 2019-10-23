/*
 * cofmsg_barrier.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_BARRIER_H_
#define COFMSG_BARRIER_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_barrier_request : public cofmsg {
public:
  /**
   *
   */
  virtual ~cofmsg_barrier_request(){};

  /**
   *
   */
  cofmsg_barrier_request(uint8_t version = 0, uint32_t xid = 0,
                         uint8_t *data = 0, size_t datalen = 0)
      : cofmsg(version, datalen, xid), body(data, datalen) {
    switch (get_version()) {
    case rofl::openflow10::OFP_VERSION: {
      set_type(rofl::openflow10::OFPT_BARRIER_REQUEST /* =18 */);
    } break;
    default: {
      set_type(rofl::openflow12::OFPT_BARRIER_REQUEST /* =20 */);
    };
    }
  };

  /**
   *
   */
  cofmsg_barrier_request(const cofmsg_barrier_request &msg) { *this = msg; };

  /**
   *
   */
  cofmsg_barrier_request &operator=(const cofmsg_barrier_request &msg) {
    if (this == &msg)
      return *this;
    cofmsg::operator=(msg);
    body = msg.body;
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
  virtual void pack(uint8_t *buf = (uint8_t *)0, size_t buflen = 0);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  const rofl::cmemory &get_body() const { return body; };

  /**
   *
   */
  rofl::cmemory &set_body() { return body; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmsg_barrier_request const &msg) {
    os << dynamic_cast<cofmsg const &>(msg);
    os << "<cofmsg_barrier_request >" << std::endl;

    os << msg.body;
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    ss << cofmsg::str() << "-Barrier-Request- ";
    return ss.str();
  };

private:
  rofl::cmemory body;
};

/**
 *
 */
class cofmsg_barrier_reply : public cofmsg {
public:
  /**
   *
   */
  virtual ~cofmsg_barrier_reply(){};

  /**
   *
   */
  cofmsg_barrier_reply(uint8_t version = 0, uint32_t xid = 0, uint8_t *data = 0,
                       size_t datalen = 0)
      : cofmsg(version, datalen, xid), body(data, datalen) {
    switch (get_version()) {
    case rofl::openflow10::OFP_VERSION: {
      set_type(rofl::openflow10::OFPT_BARRIER_REPLY /* =19 */);
    } break;
    default: {
      set_type(rofl::openflow12::OFPT_BARRIER_REPLY /* =21 */);
    };
    }
  };

  /**
   *
   */
  cofmsg_barrier_reply(const cofmsg_barrier_reply &msg) { *this = msg; };

  /**
   *
   */
  cofmsg_barrier_reply &operator=(const cofmsg_barrier_reply &msg) {
    if (this == &msg)
      return *this;
    cofmsg::operator=(msg);
    body = msg.body;
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
  virtual void pack(uint8_t *buf = (uint8_t *)0, size_t buflen = 0);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  const cmemory &get_body() const { return body; };

  /**
   *
   */
  rofl::cmemory &set_body() { return body; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmsg_barrier_reply const &msg) {
    os << dynamic_cast<cofmsg const &>(msg);
    os << "<cofmsg_barrier_reply >" << std::endl;

    os << msg.body;
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    ss << cofmsg::str() << "-Barrier-Reply- ";
    return ss.str();
  };

private:
  rofl::cmemory body;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_BARRIER_H_ */
