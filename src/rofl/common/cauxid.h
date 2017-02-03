/*
 * cauxid.h
 *
 *  Created on: 20.05.2014
 *      Author: andreas
 */

#ifndef CAUXID_H_
#define CAUXID_H_

#include <inttypes.h>
#include <iostream>
#include <sstream>

#include "rofl/common/exception.hpp"

namespace rofl {

class eAuxIdBase : public exception {
public:
  eAuxIdBase(const std::string __arg) : exception(__arg){};
};

class eAuxIdNotFound : public eAuxIdBase {
public:
  eAuxIdNotFound(const std::string __arg) : eAuxIdBase(__arg){};
};

class cauxid {

  uint8_t id;

public:
  /**
   *
   */
  cauxid(uint8_t id = 0) : id(id){};

  /**
   *
   */
  ~cauxid(){};

  /**
   *
   */
  cauxid(const cauxid &auxid) { *this = auxid; };

  /**
   *
   */
  cauxid &operator=(const cauxid &auxid) {
    if (this == &auxid)
      return *this;
    id = auxid.id;
    return *this;
  };

  /**
   *
   */
  bool operator<(const cauxid &auxid) const { return (id < auxid.id); };

  /**
   *
   */
  bool operator==(const cauxid &auxid) const { return (id == auxid.id); };

  /**
   *
   */
  uint8_t get_id() const { return id; }

public:
  friend std::ostream &operator<<(std::ostream &os, const cauxid &auxid) {
    os << "<cauxid id:" << (int)auxid.id << " >" << std::endl;
    return os;
  };

  /**
   *
   */
  std::string str() const {
    std::stringstream ss;
    ss << std::hex << (int)get_id() << std::dec;
    return ss.str();
  };
};

}; // end of namespace rofl

#endif /* CAUXID_H_ */
