/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CRANDOM_H
#define CRANDOM_H 1

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <limits>

#include "rofl/common/exception.hpp"

namespace rofl {

class crandom {
  static const std::string DEV_URANDOM;

public:
  /**
   *
   */
  virtual ~crandom();

  /**
   *
   */
  crandom();

  /**
   *
   */
  crandom(const crandom &r) { *this = r; };

  /**
   *
   */
  crandom &operator=(const crandom &r) {
    if (this == &r)
      return *this;
    seedp = r.seedp;
    return *this;
  };

public:
  /**
   * @brief	returns a random number between 0 and 1
   */
  double rand();

  /**
   * @brief	convenience method: return uint8_t
   */
  uint8_t uint8();

  /**
   * @brief	convenience method: return uint16_t
   */
  uint16_t uint16();

  /**
   * @brief	convenience method: return uint32_t
   */
  uint32_t uint32();

  /**
   * @brief	convenience method: return uint64_t
   */
  uint64_t uint64();

public:
  friend std::ostream &operator<<(std::ostream &os, crandom const &rand) {
    os << "<crandom seed=" << rand.seedp << " >";
    return os;
  };

private:
  unsigned int seedp;
};

}; // end of namespace

#endif
