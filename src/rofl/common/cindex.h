/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cindex.h
 *
 *  Created on: 17.07.2014
 *      Author: andreas
 */

#ifndef CINDEX_H_
#define CINDEX_H_

#include <iostream>
#include <sstream>

namespace rofl {

class cindex {
public:
  /**
   *
   */
  ~cindex(){};

  /**
   *
   */
  cindex() : index(0){};

  /**
   *
   */
  explicit cindex(unsigned int index) : index(index){};

  /**
   *
   */
  cindex(const cindex &idx) { *this = idx; };

  /**
   *
   */
  cindex &operator=(const cindex &idx) {
    if (this == &idx)
      return *this;
    index = idx.index;
    return *this;
  };

  /**
   *
   */
  bool operator<(const cindex &idx) const { return (index < idx.index); };

  /**
   *
   */
  bool operator==(const cindex &idx) const { return (index == idx.index); };

  /**
   *
   */
  bool operator!=(const cindex &idx) const { return (index != idx.index); };

  /**
   *
   */
  cindex &operator++() { // prefix
    ++index;
    return *this;
  };

  /**
   *
   */
  cindex operator++(__attribute__((unused)) int unused) { // postfix
    cindex result = *this;
    ++index;
    return result;
  };

public:
  /**
   *
   */
  void set_index(unsigned int index) { this->index = index; };

  /**
   *
   */
  unsigned int get_index() const { return index; };

public:
  friend std::ostream &operator<<(std::ostream &os, const cindex &index) {
    os << "<cindex idx:" << index.get_index() << " >";
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    ss << "index: " << index << " ";
    return ss.str();
  };

private:
  unsigned int index;
};

}; // end of namespace rofl

#endif /* CINDEX_H_ */
