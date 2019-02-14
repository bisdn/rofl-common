/*
 * csockaddr.h
 *
 *  Created on: 10.06.2014
 *      Author: andreas
 */

#ifndef CSOCKADDR_H_
#define CSOCKADDR_H_

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <iostream>

#include "rofl/common/caddress.h"
#include "rofl/common/cmemory.h"

#include "rofl/common/exception.hpp"

namespace rofl {

class eSockAddrBase : public exception {
public:
  eSockAddrBase(const std::string &__arg) : exception(__arg){};
};

class eSockAddrInval : public eSockAddrBase {
public:
  eSockAddrInval(const std::string &__arg) : eSockAddrBase(__arg){};
};

class csockaddr : public rofl::cmemory {
public:
  union {
    uint8_t *addru_mem;
    struct sockaddr *addru_saddr;
    struct sockaddr_in *addru_s4addr;
    struct sockaddr_in6 *addru_s6addr;
  } addr_addru;

#define ca_mem addr_addru.addru_mem
#define ca_saddr addr_addru.addru_saddr   // generic sockaddr
#define ca_s4addr addr_addru.addru_s4addr // sockaddr_in
#define ca_s6addr addr_addru.addru_s6addr // sockaddr_in6

  socklen_t salen;

public:
  /**
   *
   */
  csockaddr(int family, const std::string &addr, uint16_t port_no);

  /**
   *
   */
  csockaddr(const caddress_in4 &addr, uint16_t port_no);

  /**
   *
   */
  csockaddr(const caddress_in6 &addr, uint16_t port_no);

  /**
   *
   */
  csockaddr()
      : rofl::cmemory(sizeof(struct sockaddr_un)),
        salen(sizeof(struct sockaddr_un)) {
    ca_mem = cmemory::somem();
  };

  /**
   *
   */
  virtual ~csockaddr(){};

  /**
   *
   */
  csockaddr(const csockaddr &sockaddr)
      : cmemory(sockaddr), salen(sockaddr.salen) {
    ca_mem = cmemory::somem();
  }

  /**
   *
   */
  csockaddr &operator=(const csockaddr &sockaddr) {
    if (this == &sockaddr)
      return *this;
    cmemory::operator=(sockaddr);
    ca_mem = cmemory::somem();
    salen = sockaddr.salen;
    return *this;
  };

public:
  /**
   *
   */
  int get_family() const;

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *sa, size_t salen);

  /**
   *
   */
  virtual void unpack(uint8_t *sa, size_t salen);

public:
  friend std::ostream &operator<<(std::ostream &os, const csockaddr &sockaddr) {
    os << "<csockaddr >" << std::endl;
    switch (sockaddr.ca_saddr->sa_family) {
    case AF_INET: {
      caddress_in4 addr;
      addr.set_addr_nbo(sockaddr.ca_s4addr->sin_addr.s_addr);

      os << addr;
      os << "<portno: " << (unsigned int)be16toh(sockaddr.ca_s4addr->sin_port)
         << " >" << std::endl;
    } break;
    case AF_INET6: {
      caddress_in6 addr;
      addr.unpack(sockaddr.ca_s6addr->sin6_addr.s6_addr, 16);

      os << addr;
      os << "<portno: " << (unsigned int)be16toh(sockaddr.ca_s6addr->sin6_port)
         << " >" << std::endl;
    } break;
    }
    return os;
  };

  std::string str() const {
    std::stringstream sstr;
    switch (ca_saddr->sa_family) {
    case AF_INET: {
      caddress_in4 addr;
      addr.set_addr_nbo(ca_s4addr->sin_addr.s_addr);
      sstr << "inet4:" << addr.str() << ":"
           << (unsigned int)be16toh(ca_s4addr->sin_port);
    } break;
    case AF_INET6: {
      caddress_in6 addr;
      addr.unpack(ca_s6addr->sin6_addr.s6_addr, 16);
      sstr << "inet6:" << addr.str() << ":"
           << (unsigned int)be16toh(ca_s6addr->sin6_port);
    } break;
    }
    return sstr.str();
  };
};

}; // end of namespace rofl

#endif /* CSOCKADDR_H_ */
