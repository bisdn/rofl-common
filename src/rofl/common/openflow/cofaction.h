/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACTION_H
#define COFACTION_H 1

#include <algorithm>
#include <endian.h>
#include <stdexcept>
#include <string>
#include <vector>
#ifndef htobe16
#include "../endian_conversion.h"
#endif

#include "rofl/common/caddress.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

#include "rofl/common/cindex.h"

namespace rofl {
namespace openflow {

/* error classes */
class eActionBase : public exception {
public:
  eActionBase() : exception("eActionBase"){};
};
class eActionInval : public eActionBase {};     // invalid parameter
class eActionInvalType : public eActionBase {}; // invalid action type
class eActionNotFound : public eActionBase {};

class cofaction {
public:
  /**
   *
   */
  static std::ostream &dump(std::ostream &os, const cofaction &action);

public:
  /**
   *
   */
  cofaction(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
            uint16_t type = 0)
      : ofp_version(ofp_version), type(type),
        len(sizeof(struct rofl::openflow::ofp_action)){};

  /**
   *
   */
  virtual ~cofaction(){};

  /**
   *
   */
  cofaction(const cofaction &action) { *this = action; };

  /**
   *
   */
  cofaction &operator=(const cofaction &action) {
    if (this == &action)
      return *this;
    ofp_version = action.ofp_version;
    type = action.type;
    len = action.len;
    return *this;
  };

  /**
   *
   */
  virtual void check_prerequisites() const {};

public:
  /**
   *
   */
  uint8_t get_version() const { return ofp_version; };

  /**
   *
   */
  void set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

  /**
   *
   */
  uint16_t get_type() const { return type; };

  /**
   *
   */
  void set_type(uint16_t type) { this->type = type; };

  /**
   *
   */
  uint16_t get_length() const { return len; };

  /**
   *
   */
  void set_length(uint16_t len) { this->len = len; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os, cofaction const &action) {
    os << "<cofaction ofp-version:" << (int)action.get_version() << " type:0x"
       << std::hex << (int)action.get_type() << std::dec
       << " length:" << (int)action.get_length() << " >" << std::endl;
    return os;
  }

  class cofaction_find_by_type {
    uint16_t type;

  public:
    cofaction_find_by_type(uint16_t type) : type(type){};
    bool operator()(const cofaction &action) {
      return (action.get_type() == type);
    };
    bool operator()(const cofaction *action) {
      return (action->get_type() == type);
    };
    bool operator()(const std::pair<cindex, cofaction *> &p) {
      return (p.second->get_type() == type);
    };
  };

private:
  uint8_t ofp_version;
  uint16_t type;
  uint16_t len;
};

class cofaction_output : public cofaction {
public:
  /**
   *
   */
  cofaction_output(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                   uint32_t port_no = 0, uint16_t max_len = 128)
      : cofaction(ofp_version, rofl::openflow::OFPAT_OUTPUT), port_no(port_no),
        max_len(max_len){};

  /**
   *
   */
  virtual ~cofaction_output(){};

  /**
   *
   */
  cofaction_output(const cofaction_output &action)
      : cofaction(action), port_no(action.port_no), max_len(action.max_len) {}

  /**
   *
   */
  cofaction_output &operator=(const cofaction_output &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    port_no = action.port_no;
    max_len = action.max_len;
    return *this;
  };

  /**
   *
   */
  virtual void check_prerequisites() const;

public:
  /**
   *
   */
  void set_port_no(uint32_t port_no) { this->port_no = port_no; };

  /**
   *
   */
  uint32_t get_port_no() const { return port_no; };

  /**
   *
   */
  void set_max_len(uint16_t max_len) { this->max_len = max_len; };

  /**
   *
   */
  uint16_t get_max_len() const { return max_len; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_output const &action) {
    os << "<cofaction_output" << std::hex << " port-no: 0x"
       << (unsigned int)action.get_port_no() << " max-len: 0x"
       << (unsigned int)action.get_max_len() << std::dec << ">" << std::endl
       << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint32_t port_no;
  uint16_t max_len;
};

/*
 * old OF1.0 actions
 */

class cofaction_set_vlan_vid : public cofaction {
public:
  /**
   *
   */
  cofaction_set_vlan_vid(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint16_t vlan_vid = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_VLAN_VID),
        vlan_vid(vlan_vid){};

  /**
   *
   */
  virtual ~cofaction_set_vlan_vid(){};

  /**
   *
   */
  cofaction_set_vlan_vid(const cofaction_set_vlan_vid &action)
      : cofaction(action), vlan_vid(action.vlan_vid) {}

  /**
   *
   */
  cofaction_set_vlan_vid &operator=(const cofaction_set_vlan_vid &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    vlan_vid = action.vlan_vid;
    return *this;
  };

public:
  /**
   *
   */
  void set_vlan_vid(uint16_t vlan_vid) { this->vlan_vid = vlan_vid; };

  /**
   *
   */
  uint16_t get_vlan_vid() const { return vlan_vid; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_set_vlan_vid const &action) {
    os << "<cofaction_set_vlan_vid ";
    os << "vid:0x" << std::hex << (unsigned int)action.get_vlan_vid()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t vlan_vid;
};

class cofaction_set_vlan_pcp : public cofaction {
public:
  /**
   *
   */
  cofaction_set_vlan_pcp(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint8_t vlan_pcp = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_VLAN_PCP),
        vlan_pcp(vlan_pcp){};

  /**
   *
   */
  virtual ~cofaction_set_vlan_pcp(){};

  /**
   *
   */
  cofaction_set_vlan_pcp(const cofaction_set_vlan_pcp &action)
      : cofaction(action), vlan_pcp(action.vlan_pcp) {}

  /**
   *
   */
  cofaction_set_vlan_pcp &operator=(const cofaction_set_vlan_pcp &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    vlan_pcp = action.vlan_pcp;
    return *this;
  };

public:
  /**
   *
   */
  void set_vlan_pcp(uint8_t vlan_pcp) { this->vlan_pcp = vlan_pcp; };

  /**
   *
   */
  uint8_t get_vlan_pcp() const { return vlan_pcp; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_set_vlan_pcp const &action) {
    os << "<cofaction_set_vlan_pcp ";
    os << "pcp:ox" << std::hex << (unsigned int)action.get_vlan_pcp()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint8_t vlan_pcp;
};

class cofaction_strip_vlan : public cofaction {
public:
  /**
   *
   */
  cofaction_strip_vlan(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_STRIP_VLAN){};

  /**
   *
   */
  virtual ~cofaction_strip_vlan(){};

  /**
   *
   */
  cofaction_strip_vlan(const cofaction_strip_vlan &action)
      : cofaction(action) {}

  /**
   *
   */
  cofaction_strip_vlan &operator=(const cofaction_strip_vlan &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_strip_vlan &action) {
    os << "<cofaction_strip_vlan >" << std::endl;

    os << dynamic_cast<const cofaction &>(action);
    return os;
  };
};

class cofaction_set_dl_src : public cofaction {
public:
  /**
   *
   */
  cofaction_set_dl_src(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      const rofl::cmacaddr &macaddr = rofl::cmacaddr("00:00:00:00:00:00"))
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_DL_SRC),
        macaddr(macaddr){};

  /**
   *
   */
  virtual ~cofaction_set_dl_src(){};

  /**
   *
   */
  cofaction_set_dl_src(const cofaction_set_dl_src &action)
      : cofaction(action), macaddr(action.macaddr) {}

  /**
   *
   */
  cofaction_set_dl_src &operator=(const cofaction_set_dl_src &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    macaddr = action.macaddr;
    return *this;
  };

public:
  /**
   *
   */
  const cmacaddr &get_dl_src() const { return macaddr; };

  /**
   *
   */
  cmacaddr &set_dl_src() { return macaddr; };

  /**
   *
   */
  void set_dl_src(const cmacaddr &macaddr) { this->macaddr = macaddr; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_dl_src &action) {
    os << "<cofaction_set_dl_src macaddr:";
    os << action.get_dl_src() << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  rofl::cmacaddr macaddr;
};

class cofaction_set_dl_dst : public cofaction {
public:
  /**
   *
   */
  cofaction_set_dl_dst(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      const rofl::cmacaddr &macaddr = rofl::cmacaddr("00:00:00:00:00:00"))
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_DL_DST),
        macaddr(macaddr){};

  /**
   *
   */
  virtual ~cofaction_set_dl_dst(){};

  /**
   *
   */
  cofaction_set_dl_dst(const cofaction_set_dl_dst &action)
      : cofaction(action), macaddr(action.macaddr) {}

  /**
   *
   */
  cofaction_set_dl_dst &operator=(const cofaction_set_dl_dst &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    macaddr = action.macaddr;
    return *this;
  };

public:
  /**
   *
   */
  const cmacaddr &get_dl_dst() const { return macaddr; };

  /**
   *
   */
  cmacaddr &set_dl_dst() { return macaddr; };

  /**
   *
   */
  void set_dl_dst(const cmacaddr &macaddr) { this->macaddr = macaddr; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_dl_dst &action) {
    os << "<cofaction_set_dl_dst macaddr:";
    os << action.get_dl_dst() << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  rofl::cmacaddr macaddr;
};

class cofaction_set_nw_src : public cofaction {
public:
  /**
   *
   */
  cofaction_set_nw_src(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      const rofl::caddress_in4 &addr = rofl::caddress_in4("0.0.0.0"))
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_SRC), addr(addr){};

  /**
   *
   */
  virtual ~cofaction_set_nw_src(){};

  /**
   *
   */
  cofaction_set_nw_src(const cofaction_set_nw_src &action)
      : cofaction(action), addr(action.addr) {}

  /**
   *
   */
  cofaction_set_nw_src &operator=(const cofaction_set_nw_src &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    addr = action.addr;
    return *this;
  };

public:
  /**
   *
   */
  const caddress_in4 &get_nw_src() const { return addr; };

  /**
   *
   */
  caddress_in4 &set_nw_src() { return addr; };

  /**
   *
   */
  void set_nw_src(const caddress_in4 &addr) { this->addr = addr; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_nw_src &action) {
    os << "<cofaction_set_nw_src ";
    os << "nw-src:" << action.get_nw_src() << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  rofl::caddress_in4 addr;
};

class cofaction_set_nw_dst : public cofaction {
public:
  /**
   *
   */
  cofaction_set_nw_dst(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      const rofl::caddress_in4 &addr = rofl::caddress_in4("0.0.0.0"))
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_DST), addr(addr){};

  /**
   *
   */
  virtual ~cofaction_set_nw_dst(){};

  /**
   *
   */
  cofaction_set_nw_dst(const cofaction_set_nw_dst &action)
      : cofaction(action), addr(action.addr) {}

  /**
   *
   */
  cofaction_set_nw_dst &operator=(const cofaction_set_nw_dst &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    addr = action.addr;
    return *this;
  };

public:
  /**
   *
   */
  const caddress_in4 &get_nw_dst() const { return addr; };

  /**
   *
   */
  caddress_in4 &set_nw_dst() { return addr; };

  /**
   *
   */
  void set_nw_dst(const caddress_in4 &addr) { this->addr = addr; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_nw_dst &action) {
    os << "<cofaction_set_nw_dst ";
    os << "nw-dst:" << action.get_nw_dst() << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  rofl::caddress_in4 addr;
};

class cofaction_set_nw_tos : public cofaction {
public:
  /**
   *
   */
  cofaction_set_nw_tos(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint8_t nw_tos = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_TOS),
        nw_tos(nw_tos){};

  /**
   *
   */
  virtual ~cofaction_set_nw_tos(){};

  /**
   *
   */
  cofaction_set_nw_tos(const cofaction_set_nw_tos &action)
      : cofaction(action), nw_tos(action.nw_tos) {}

  /**
   *
   */
  cofaction_set_nw_tos &operator=(const cofaction_set_nw_tos &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    nw_tos = action.nw_tos;
    return *this;
  }

public:
  /**
   *
   */
  void set_nw_tos(uint8_t nw_tos) { this->nw_tos = nw_tos; };

  /**
   *
   */
  uint8_t get_nw_tos() const { return nw_tos; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_set_nw_tos const &action) {
    os << "<cofaction_set_nw_tos ";
    os << "nw-tos:0x" << std::hex << (unsigned int)action.get_nw_tos()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint8_t nw_tos;
};

class cofaction_set_tp_src : public cofaction {
public:
  /**
   *
   */
  cofaction_set_tp_src(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint16_t tp_src = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_TP_SRC),
        tp_src(tp_src){};

  /**
   *
   */
  virtual ~cofaction_set_tp_src(){};

  /**
   *
   */
  cofaction_set_tp_src(const cofaction_set_tp_src &action)
      : cofaction(action), tp_src(action.tp_src) {}

  /**
   *
   */
  cofaction_set_tp_src &operator=(const cofaction_set_tp_src &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    tp_src = action.tp_src;
    return *this;
  };

public:
  /**
   *
   */
  void set_tp_src(uint16_t tp_src) { this->tp_src = tp_src; };

  /**
   *
   */
  uint16_t get_tp_src() const { return tp_src; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_tp_src &action) {
    os << "<cofaction_set_tp_src ";
    os << "tp-src:" << (unsigned int)action.get_tp_src() << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t tp_src;
};

class cofaction_set_tp_dst : public cofaction {
public:
  /**
   *
   */
  cofaction_set_tp_dst(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint16_t tp_dst = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_TP_DST),
        tp_dst(tp_dst){};

  /**
   *
   */
  virtual ~cofaction_set_tp_dst(){};

  /**
   *
   */
  cofaction_set_tp_dst(const cofaction_set_tp_dst &action)
      : cofaction(action), tp_dst(action.tp_dst) {}

  /**
   *
   */
  cofaction_set_tp_dst &operator=(const cofaction_set_tp_dst &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    tp_dst = action.tp_dst;
    return *this;
  };

public:
  /**
   *
   */
  void set_tp_dst(uint16_t tp_dst) { this->tp_dst = tp_dst; };

  /**
   *
   */
  uint16_t get_tp_dst() const { return tp_dst; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_tp_dst &action) {
    os << "<cofaction_set_tp_dst ";
    os << "tp-dst:" << (unsigned int)action.get_tp_dst() << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t tp_dst;
};

class cofaction_enqueue : public cofaction {
public:
  /**
   *
   */
  cofaction_enqueue(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                    uint16_t port_no = 0, uint32_t queue_id = 0)
      : cofaction(ofp_version, rofl::openflow10::OFPAT_ENQUEUE),
        port_no(port_no), queue_id(queue_id){};

  /**
   *
   */
  virtual ~cofaction_enqueue(){};

  /**
   *
   */
  cofaction_enqueue(const cofaction_enqueue &action)
      : cofaction(action), port_no(action.port_no), queue_id(action.queue_id) {}

  /**
   *
   */
  cofaction_enqueue &operator=(const cofaction_enqueue &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    port_no = action.port_no;
    queue_id = action.queue_id;
    return *this;
  };

public:
  /**
   *
   */
  void set_port_no(uint16_t port_no) { this->port_no = port_no; };

  /**
   *
   */
  uint16_t get_port_no() const { return port_no; };

  /**
   *
   */
  void set_queue_id(uint32_t queue_id) { this->queue_id = queue_id; };

  /**
   *
   */
  uint32_t get_queue_id() const { return queue_id; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_enqueue &action) {
    os << "<cofaction_enqueue" << std::hex << " port-no:0x"
       << (unsigned int)action.get_port_no() << " queue-id:0x"
       << (unsigned int)action.get_queue_id() << " >" << std::endl
       << std::dec << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t port_no;
  uint32_t queue_id;
};

class cofaction_vendor : public cofaction {
public:
  /**
   *
   */
  cofaction_vendor(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                   uint32_t exp_id = 0,
                   const rofl::cmemory &exp_body = rofl::cmemory((size_t)0))
      : cofaction(ofp_version, rofl::openflow::OFPAT_EXPERIMENTER),
        exp_id(exp_id), exp_body(exp_body){};

  /**
   *
   */
  virtual ~cofaction_vendor(){};

  /**
   *
   */
  cofaction_vendor(const cofaction_vendor &action)
      : cofaction(action), exp_id(action.exp_id), exp_body(action.exp_body) {}

  /**
   *
   */
  cofaction_vendor &operator=(const cofaction_vendor &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    exp_id = action.exp_id;
    exp_body = action.exp_body;
    return *this;
  };

public:
  /**
   *
   */
  void set_exp_id(uint32_t exp_id) { this->exp_id = exp_id; };

  /**
   *
   */
  uint32_t get_exp_id() const { return exp_id; };

  /**
   *
   */
  const rofl::cmemory &get_exp_body() const { return exp_body; };

  /**
   *
   */
  rofl::cmemory &set_exp_body() { return exp_body; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_vendor &action) {
    os << "<cofaction_vendor ";
    os << "exp-id:0x" << std::hex << (unsigned int)action.get_exp_id()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    os << action.get_exp_body();
    return os;
  };

private:
  uint32_t exp_id;
  rofl::cmemory exp_body;
};

class cofaction_copy_ttl_out : public cofaction {
public:
  /**
   *
   */
  cofaction_copy_ttl_out(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_COPY_TTL_OUT){};

  /**
   *
   */
  virtual ~cofaction_copy_ttl_out(){};

  /**
   *
   */
  cofaction_copy_ttl_out(const cofaction_copy_ttl_out &action)
      : cofaction(action) {}

  /**
   *
   */
  cofaction_copy_ttl_out &operator=(const cofaction_copy_ttl_out &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_copy_ttl_out const &action) {
    os << "<cofaction_copy_ttl_out >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };
};

class cofaction_copy_ttl_in : public cofaction {
public:
  /**
   *
   */
  cofaction_copy_ttl_in(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_COPY_TTL_IN){};

  /**
   *
   */
  virtual ~cofaction_copy_ttl_in(){};

  /**
   *
   */
  cofaction_copy_ttl_in(const cofaction_copy_ttl_in &action)
      : cofaction(action) {}

  /**
   *
   */
  cofaction_copy_ttl_in &operator=(const cofaction_copy_ttl_in &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_copy_ttl_in const &action) {
    os << "<cofaction_copy_ttl_in >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };
};

class cofaction_set_mpls_ttl : public cofaction {
public:
  /**
   *
   */
  cofaction_set_mpls_ttl(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint8_t mpls_ttl = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_MPLS_TTL),
        mpls_ttl(mpls_ttl){};

  /**
   *
   */
  virtual ~cofaction_set_mpls_ttl(){};

  /**
   *
   */
  cofaction_set_mpls_ttl(const cofaction_set_mpls_ttl &action)
      : cofaction(action), mpls_ttl(action.mpls_ttl) {}

  /**
   *
   */
  cofaction_set_mpls_ttl &operator=(const cofaction_set_mpls_ttl &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    mpls_ttl = action.mpls_ttl;
    return *this;
  }

public:
  /**
   *
   */
  void set_mpls_ttl(uint8_t mpls_ttl) { this->mpls_ttl = mpls_ttl; };

  /**
   *
   */
  uint8_t get_mpls_ttl() const { return mpls_ttl; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_mpls_ttl &action) {
    os << "<cofaction_set_mpls_ttl ";
    os << "mpls-ttl:0x" << std::hex << (unsigned int)action.get_mpls_ttl()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint8_t mpls_ttl;
};

class cofaction_dec_mpls_ttl : public cofaction {
public:
  /**
   *
   */
  cofaction_dec_mpls_ttl(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_DEC_MPLS_TTL){};

  /**
   *
   */
  virtual ~cofaction_dec_mpls_ttl(){};

  /**
   *
   */
  cofaction_dec_mpls_ttl(const cofaction_dec_mpls_ttl &action)
      : cofaction(action) {}

  /**
   *
   */
  cofaction_dec_mpls_ttl &operator=(const cofaction_dec_mpls_ttl &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_dec_mpls_ttl const &action) {
    os << "<cofaction_dec_mpls_ttl >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };
};

class cofaction_push_vlan : public cofaction {
public:
  /**
   *
   */
  cofaction_push_vlan(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                      uint16_t eth_type = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_PUSH_VLAN),
        eth_type(eth_type){};

  /**
   *
   */
  virtual ~cofaction_push_vlan(){};

  /**
   *
   */
  cofaction_push_vlan(const cofaction_push_vlan &action)
      : cofaction(action), eth_type(action.eth_type) {}

  /**
   *
   */
  cofaction_push_vlan &operator=(const cofaction_push_vlan &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    eth_type = action.eth_type;
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  void set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

  /**
   *
   */
  uint16_t get_eth_type() const { return eth_type; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_push_vlan &action) {
    os << "<cofaction_push_vlan ";
    os << "eth-type:0x" << std::hex << (unsigned int)action.get_eth_type()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t eth_type;
};

class cofaction_pop_vlan : public cofaction {
public:
  /**
   *
   */
  cofaction_pop_vlan(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_POP_VLAN){};

  /**
   *
   */
  virtual ~cofaction_pop_vlan(){};

  /**
   *
   */
  cofaction_pop_vlan(const cofaction_pop_vlan &action) : cofaction(action) {}

  /**
   *
   */
  cofaction_pop_vlan &operator=(const cofaction_pop_vlan &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_pop_vlan &action) {
    os << "<cofaction_pop_vlan >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };
};

class cofaction_push_mpls : public cofaction {
public:
  /**
   *
   */
  cofaction_push_mpls(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                      uint16_t eth_type = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_PUSH_MPLS),
        eth_type(eth_type){};

  /**
   *
   */
  virtual ~cofaction_push_mpls(){};

  /**
   *
   */
  cofaction_push_mpls(const cofaction_push_mpls &action)
      : cofaction(action), eth_type(action.eth_type) {}

  /**
   *
   */
  cofaction_push_mpls &operator=(const cofaction_push_mpls &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    eth_type = action.eth_type;
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  void set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

  /**
   *
   */
  uint16_t get_eth_type() const { return eth_type; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_push_mpls &action) {
    os << "<cofaction_push_mpls ";
    os << "eth-type:0x" << std::hex << (unsigned int)action.get_eth_type()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t eth_type;
};

class cofaction_pop_mpls : public cofaction {
public:
  /**
   *
   */
  cofaction_pop_mpls(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                     uint16_t eth_type = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_POP_MPLS),
        eth_type(eth_type){};

  /**
   *
   */
  virtual ~cofaction_pop_mpls(){};

  /**
   *
   */
  cofaction_pop_mpls(const cofaction_pop_mpls &action)
      : cofaction(action), eth_type(action.eth_type) {}

  /**
   *
   */
  cofaction_pop_mpls &operator=(const cofaction_pop_mpls &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    eth_type = action.eth_type;
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  void set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

  /**
   *
   */
  uint16_t get_eth_type() const { return eth_type; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_pop_mpls &action) {
    os << "<cofaction_pop_mpls ";
    os << "eth-type:0x" << std::hex << (unsigned int)action.get_eth_type()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t eth_type;
};

class cofaction_group : public cofaction {
public:
  /**
   *
   */
  cofaction_group(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                  uint32_t group_id = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_GROUP),
        group_id(group_id){};

  /**
   *
   */
  virtual ~cofaction_group(){};

  /**
   *
   */
  cofaction_group(const cofaction_group &action)
      : cofaction(action), group_id(action.group_id) {}

  /**
   *
   */
  cofaction_group &operator=(const cofaction_group &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    group_id = action.group_id;
    return *this;
  };

public:
  /**
   *
   */
  void set_group_id(uint32_t group_id) { this->group_id = group_id; };

  /**
   *
   */
  uint32_t get_group_id() const { return group_id; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_group &action) {
    os << "<cofaction_group ";
    os << "group-id:0x" << std::hex << (int)action.get_group_id() << std::dec
       << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint32_t group_id;
};

class cofaction_set_nw_ttl : public cofaction {
public:
  /**
   *
   */
  cofaction_set_nw_ttl(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint8_t nw_ttl = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_TTL),
        nw_ttl(nw_ttl){};

  /**
   *
   */
  virtual ~cofaction_set_nw_ttl(){};

  /**
   *
   */
  cofaction_set_nw_ttl(const cofaction_set_nw_ttl &action)
      : cofaction(action), nw_ttl(action.nw_ttl) {}

  /**
   *
   */
  cofaction_set_nw_ttl &operator=(const cofaction_set_nw_ttl &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    nw_ttl = action.nw_ttl;
    return *this;
  };

public:
  /**
   *
   */
  void set_nw_ttl(uint8_t nw_ttl) { this->nw_ttl = nw_ttl; };

  /**
   *
   */
  uint8_t get_nw_ttl() const { return nw_ttl; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_nw_ttl &action) {
    os << "<cofaction_set_nw_ttl ";
    os << "nw-ttl:" << (unsigned int)action.get_nw_ttl() << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint8_t nw_ttl;
};

class cofaction_dec_nw_ttl : public cofaction {
public:
  /**
   *
   */
  cofaction_dec_nw_ttl(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_DEC_NW_TTL){};

  /**
   *
   */
  virtual ~cofaction_dec_nw_ttl(){};

  /**
   *
   */
  cofaction_dec_nw_ttl(const cofaction_dec_nw_ttl &action)
      : cofaction(action) {}

  /**
   *
   */
  cofaction_dec_nw_ttl &operator=(const cofaction_dec_nw_ttl &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_dec_nw_ttl const &action) {
    os << "<cofaction_dec_nw_ttl >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };
};

class cofaction_set_queue : public cofaction {
public:
  /**
   *
   */
  cofaction_set_queue(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                      uint32_t queue_id = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_QUEUE),
        queue_id(queue_id){};

  /**
   *
   */
  virtual ~cofaction_set_queue(){};

  /**
   *
   */
  cofaction_set_queue(const cofaction_set_queue &action)
      : cofaction(action), queue_id(action.queue_id) {}

  /**
   *
   */
  cofaction_set_queue &operator=(const cofaction_set_queue &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    queue_id = action.queue_id;
    return *this;
  };

public:
  /**
   *
   */
  void set_queue_id(uint32_t queue_id) { this->queue_id = queue_id; };

  /**
   *
   */
  uint32_t get_queue_id() const { return queue_id; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_queue &action) {
    os << "<cofaction_set_queue ";
    os << "queue-id:" << (unsigned int)action.get_queue_id() << " >"
       << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint32_t queue_id;
};

class cofaction_set_field : public cofaction {
public:
  enum oxm_set_field_type_t {
    OXM_TYPE_UNKNOWN = 0,
    OXM_TYPE_8 = 1,
    OXM_TYPE_16 = 2,
    OXM_TYPE_24 = 3,
    OXM_TYPE_32 = 4,
    OXM_TYPE_48 = 5,
    OXM_TYPE_64 = 6,
    OXM_TYPE_128 = 7,
    OXM_TYPE_EXP = 8,
  };

public:
  /**
   *
   */
  cofaction_set_field(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_SET_FIELD),
        oxm_set_field_type(OXM_TYPE_UNKNOWN), oxm_8(0), oxm_16(0), oxm_24(0),
        oxm_32(0), oxm_48(0), oxm_64(0), oxm_128(0){};

  /**
   *
   */
  virtual ~cofaction_set_field(){};

  /**
   *
   */
  cofaction_set_field(const cofaction_set_field &action)
      : cofaction(action), oxm_set_field_type(action.oxm_set_field_type),
        oxm_8(action.oxm_8), oxm_16(action.oxm_16), oxm_24(action.oxm_24),
        oxm_32(action.oxm_32), oxm_48(action.oxm_48), oxm_64(action.oxm_64),
        oxm_128(action.oxm_128), oxm_exp(action.oxm_exp) {}

  /**
   *
   */
  cofaction_set_field &operator=(const cofaction_set_field &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    oxm_set_field_type = action.oxm_set_field_type;
    oxm_8 = action.oxm_8;
    oxm_16 = action.oxm_16;
    oxm_24 = action.oxm_24;
    oxm_32 = action.oxm_32;
    oxm_48 = action.oxm_48;
    oxm_64 = action.oxm_64;
    oxm_128 = action.oxm_128;
    oxm_exp = action.oxm_exp;
    return *this;
  };

public:
  /**
   *
   */
  enum oxm_set_field_type_t get_oxm_set_field_type() const {
    return oxm_set_field_type;
  }

  /**
   *
   */
  uint16_t get_oxm_class() const {
    switch (oxm_set_field_type) {
    case OXM_TYPE_8: {
      return oxm_8.get_oxm_class();
    } break;
    case OXM_TYPE_16: {
      return oxm_16.get_oxm_class();
    } break;
    case OXM_TYPE_24: {
      return oxm_24.get_oxm_class();
    } break;
    case OXM_TYPE_32: {
      return oxm_32.get_oxm_class();
    } break;
    case OXM_TYPE_48: {
      return oxm_48.get_oxm_class();
    } break;
    case OXM_TYPE_64: {
      return oxm_64.get_oxm_class();
    } break;
    case OXM_TYPE_128: {
      return oxm_128.get_oxm_class();
    } break;
    case OXM_TYPE_EXP: {
      return oxm_exp.get_oxm_class();
    } break;
    default: { throw eActionInval(); };
    }
  };

  /**
   *
   */
  uint8_t get_oxm_field() const {
    switch (oxm_set_field_type) {
    case OXM_TYPE_8: {
      return oxm_8.get_oxm_field();
    } break;
    case OXM_TYPE_16: {
      return oxm_16.get_oxm_field();
    } break;
    case OXM_TYPE_24: {
      return oxm_24.get_oxm_field();
    } break;
    case OXM_TYPE_32: {
      return oxm_32.get_oxm_field();
    } break;
    case OXM_TYPE_48: {
      return oxm_48.get_oxm_field();
    } break;
    case OXM_TYPE_64: {
      return oxm_64.get_oxm_field();
    } break;
    case OXM_TYPE_128: {
      return oxm_128.get_oxm_field();
    } break;
    case OXM_TYPE_EXP: {
      return oxm_exp.get_oxm_field();
    } break;
    default: { throw eActionInval(); };
    }
  };

  /**
   *
   */
  uint32_t get_oxm_type() const {
    switch (oxm_set_field_type) {
    case OXM_TYPE_8: {
      return oxm_8.get_oxm_type();
    } break;
    case OXM_TYPE_16: {
      return oxm_16.get_oxm_type();
    } break;
    case OXM_TYPE_24: {
      return oxm_24.get_oxm_type();
    } break;
    case OXM_TYPE_32: {
      return oxm_32.get_oxm_type();
    } break;
    case OXM_TYPE_48: {
      return oxm_48.get_oxm_type();
    } break;
    case OXM_TYPE_64: {
      return oxm_64.get_oxm_type();
    } break;
    case OXM_TYPE_128: {
      return oxm_128.get_oxm_type();
    } break;
    case OXM_TYPE_EXP: {
      return oxm_exp.get_oxm_type();
    } break;
    default: { throw eActionInval(); };
    }
  };

  /**
   *
   */
  uint32_t get_oxm_id() const {
    switch (oxm_set_field_type) {
    case OXM_TYPE_8: {
      return oxm_8.get_oxm_id();
    } break;
    case OXM_TYPE_16: {
      return oxm_16.get_oxm_id();
    } break;
    case OXM_TYPE_24: {
      return oxm_24.get_oxm_id();
    } break;
    case OXM_TYPE_32: {
      return oxm_32.get_oxm_id();
    } break;
    case OXM_TYPE_48: {
      return oxm_48.get_oxm_id();
    } break;
    case OXM_TYPE_64: {
      return oxm_64.get_oxm_id();
    } break;
    case OXM_TYPE_128: {
      return oxm_128.get_oxm_id();
    } break;
    case OXM_TYPE_EXP: {
      return oxm_exp.get_oxm_id();
    } break;
    default: { throw eActionInval(); };
    }
  };

  /**
   *
   */
  uint32_t get_oxm_exp_id() const {
    switch (oxm_set_field_type) {
    case OXM_TYPE_EXP: {
      return oxm_exp.get_oxm_exp_id();
    } break;
    default: { return 0; };
    }
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_8 &get_oxm_8() const { return oxm_8; };

  /**
   *
   */
  rofl::openflow::coxmatch_8 &set_oxm_8() {
    oxm_set_field_type = OXM_TYPE_8;
    return oxm_8;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_8 &oxm_8) {
    oxm_set_field_type = OXM_TYPE_8;
    this->oxm_8 = oxm_8;
    return *this;
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_16 &get_oxm_16() const { return oxm_16; };

  /**
   *
   */
  rofl::openflow::coxmatch_16 &set_oxm_16() {
    oxm_set_field_type = OXM_TYPE_16;
    return oxm_16;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_16 &oxm_16) {
    oxm_set_field_type = OXM_TYPE_16;
    this->oxm_16 = oxm_16;
    return *this;
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_24 &get_oxm_24() const { return oxm_24; };

  /**
   *
   */
  rofl::openflow::coxmatch_24 &set_oxm_24() {
    oxm_set_field_type = OXM_TYPE_24;
    return oxm_24;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_24 &oxm_24) {
    oxm_set_field_type = OXM_TYPE_24;
    this->oxm_24 = oxm_24;
    return *this;
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_32 &get_oxm_32() const { return oxm_32; };

  /**
   *
   */
  rofl::openflow::coxmatch_32 &set_oxm_32() {
    oxm_set_field_type = OXM_TYPE_32;
    return oxm_32;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_32 &oxm_32) {
    oxm_set_field_type = OXM_TYPE_32;
    this->oxm_32 = oxm_32;
    return *this;
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_48 &get_oxm_48() const { return oxm_48; };

  /**
   *
   */
  rofl::openflow::coxmatch_48 &set_oxm_48() {
    oxm_set_field_type = OXM_TYPE_48;
    return oxm_48;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_48 &oxm_48) {
    oxm_set_field_type = OXM_TYPE_48;
    this->oxm_48 = oxm_48;
    return *this;
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_64 &get_oxm_64() const { return oxm_64; };

  /**
   *
   */
  rofl::openflow::coxmatch_64 &set_oxm_64() {
    oxm_set_field_type = OXM_TYPE_64;
    return oxm_64;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_64 &oxm_64) {
    oxm_set_field_type = OXM_TYPE_64;
    this->oxm_64 = oxm_64;
    return *this;
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_128 &get_oxm_128() const { return oxm_128; };

  /**
   *
   */
  rofl::openflow::coxmatch_128 &set_oxm_128() {
    oxm_set_field_type = OXM_TYPE_128;
    return oxm_128;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_128 &oxm_128) {
    oxm_set_field_type = OXM_TYPE_128;
    this->oxm_128 = oxm_128;
    return *this;
  };

public:
  /**
   *
   */
  const rofl::openflow::coxmatch_exp &get_oxm_exp() const { return oxm_exp; };

  /**
   *
   */
  rofl::openflow::coxmatch_exp &set_oxm_exp() {
    oxm_set_field_type = OXM_TYPE_EXP;
    return oxm_exp;
  };

  /**
   *
   */
  cofaction_set_field &set_oxm(const rofl::openflow::coxmatch_exp &oxm_exp) {
    oxm_set_field_type = OXM_TYPE_EXP;
    this->oxm_exp = oxm_exp;
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_set_field &action) {
    os << "<cofaction_set_field >" << std::endl;

    os << dynamic_cast<const cofaction &>(action);
    switch (action.get_oxm_set_field_type()) {
    case OXM_TYPE_8: {
      os << action.get_oxm_8();
    } break;
    case OXM_TYPE_16: {
      os << action.get_oxm_16();
    } break;
    case OXM_TYPE_24: {
      os << action.get_oxm_24();
    } break;
    case OXM_TYPE_32: {
      os << action.get_oxm_32();
    } break;
    case OXM_TYPE_48: {
      os << action.get_oxm_48();
    } break;
    case OXM_TYPE_64: {
      os << action.get_oxm_64();
    } break;
    case OXM_TYPE_128: {
      os << action.get_oxm_128();
    } break;
    case OXM_TYPE_EXP: {
      os << action.get_oxm_exp();
    } break;
    default: {};
    }
    return os;
  };

private:
  enum oxm_set_field_type_t oxm_set_field_type;

  rofl::openflow::coxmatch_8 oxm_8;
  rofl::openflow::coxmatch_16 oxm_16;
  rofl::openflow::coxmatch_24 oxm_24;
  rofl::openflow::coxmatch_32 oxm_32;
  rofl::openflow::coxmatch_48 oxm_48;
  rofl::openflow::coxmatch_64 oxm_64;
  rofl::openflow::coxmatch_128 oxm_128;
  rofl::openflow::coxmatch_exp oxm_exp;
};

class cofaction_experimenter : public cofaction {
public:
  /**
   *
   */
  cofaction_experimenter(
      uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint32_t exp_id = 0,
      const rofl::cmemory &exp_body = rofl::cmemory((size_t)0))
      : cofaction(ofp_version, rofl::openflow::OFPAT_EXPERIMENTER),
        exp_id(exp_id), exp_body(exp_body){};

  /**
   *
   */
  virtual ~cofaction_experimenter(){};

  /**
   *
   */
  cofaction_experimenter(const cofaction_experimenter &action)
      : cofaction(action), exp_id(action.exp_id), exp_body(action.exp_body) {}

  /**
   *
   */
  cofaction_experimenter &operator=(const cofaction_experimenter &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    exp_id = action.exp_id;
    exp_body = action.exp_body;
    return *this;
  };

public:
  /**
   *
   */
  void set_exp_id(uint32_t exp_id) { this->exp_id = exp_id; };

  /**
   *
   */
  uint32_t get_exp_id() const { return exp_id; };

  /**
   *
   */
  rofl::cmemory &set_exp_body() { return exp_body; };

  /**
   *
   */
  void set_exp_body(const rofl::cmemory &exp_body) {
    this->exp_body = exp_body;
  };

  /**
   *
   */
  const rofl::cmemory &get_exp_body() const { return exp_body; };

public:
  /**
   *
   */
  virtual size_t length() const;

  /**
   *
   */
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofaction_experimenter const &action) {
    os << "<cofaction_experimenter" << std::hex << " exp-id:0x"
       << (unsigned int)action.get_exp_id() << std::dec << ">" << std::endl
       << dynamic_cast<cofaction const &>(action) << action.get_exp_body();
    return os;
  };

private:
  uint32_t exp_id;
  rofl::cmemory exp_body;
};

class cofaction_push_pbb : public cofaction {
public:
  /**
   *
   */
  cofaction_push_pbb(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                     uint16_t eth_type = 0)
      : cofaction(ofp_version, rofl::openflow::OFPAT_PUSH_PBB),
        eth_type(eth_type){};

  /**
   *
   */
  virtual ~cofaction_push_pbb(){};

  /**
   *
   */
  cofaction_push_pbb(const cofaction_push_pbb &action)
      : cofaction(action), eth_type(action.eth_type) {}

  /**
   *
   */
  cofaction_push_pbb &operator=(const cofaction_push_pbb &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
    eth_type = action.eth_type;
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  void set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

  /**
   *
   */
  uint16_t get_eth_type() const { return eth_type; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_push_pbb &action) {
    os << "<cofaction_push_pbb ";
    os << "eth-type:0x" << std::hex << (unsigned int)action.get_eth_type()
       << std::dec << " >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };

private:
  uint16_t eth_type;
};

class cofaction_pop_pbb : public cofaction {
public:
  /**
   *
   */
  cofaction_pop_pbb(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : cofaction(ofp_version, rofl::openflow::OFPAT_POP_PBB){};

  /**
   *
   */
  virtual ~cofaction_pop_pbb(){};

  /**
   *
   */
  cofaction_pop_pbb(const cofaction_pop_pbb &action) : cofaction(action) {}

  /**
   *
   */
  cofaction_pop_pbb &operator=(const cofaction_pop_pbb &action) {
    if (this == &action)
      return *this;
    cofaction::operator=(action);
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
  virtual void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  virtual void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  const cofaction_pop_pbb &action) {
    os << "<cofaction_pop_pbb >" << std::endl;

    os << dynamic_cast<cofaction const &>(action);
    return os;
  };
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif // COFACTION_H
