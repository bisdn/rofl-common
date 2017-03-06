/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmsg_table_mod.h
 *
 *  Created on: 18.03.2013
 *  Revised on: 02.08.2015
 *      Author: Andreas Koepsel
 */

#ifndef COFMSG_TABLE_MOD_H_
#define COFMSG_TABLE_MOD_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_table_mod : public cofmsg {
public:
  /**
   *
   */
  virtual ~cofmsg_table_mod();

  /**
   *
   */
  cofmsg_table_mod(uint8_t version = 0, uint32_t xid = 0, uint8_t table_id = 0,
                   uint32_t config = 0);

  /**
   *
   */
  cofmsg_table_mod(const cofmsg_table_mod &msg);

  /**
   *
   */
  cofmsg_table_mod &operator=(const cofmsg_table_mod &msg);

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
  uint8_t get_table_id() const { return table_id; };

  /**
   *
   */
  void set_table_id(uint8_t table_id) { this->table_id = table_id; };

  /**
   *
   */
  uint32_t get_config() const { return config; };

  /**
   *
   */
  void set_config(uint32_t config) { this->config = config; };

  /**
   *
   */
  uint32_t &set_config() { return config; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmsg_table_mod const &msg) {
    os << dynamic_cast<cofmsg const &>(msg);
    os << "<cofmsg_table_mod >" << std::endl;
    os << "<table_id: " << (int)msg.get_table_id() << " >" << std::endl;
    os << "<config: 0x" << std::hex << (unsigned int)msg.get_config()
       << std::dec << " >" << std::endl;
    return os;
  };

  virtual std::string str() const {
    std::stringstream ss;
    ss << cofmsg::str() << "-Table-Mod- ";
    return ss.str();
  };

private:
  uint8_t table_id;
  uint32_t config;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_TABLE_MOD_H_ */
