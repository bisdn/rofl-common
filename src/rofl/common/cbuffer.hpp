/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CBUFFER_H
#define CBUFFER_H 1

#include <inttypes.h>
#include <pthread.h>
#include <stdlib.h>

#include <iomanip>
#include <set>
#include <sstream>
#include <string>

#include "rofl/common/cmemory.h"
#include "rofl/common/exception.hpp"

namespace rofl {

/* error classes */
class eBufBase : public exception {
public:
  eBufBase(const std::string &__arg) : exception(__arg){};
};
class eBufNotFound : public eBufBase {
public:
  eBufNotFound(const std::string &__arg) : eBufBase(__arg){};
};
class eBufInval : public eBufBase {
public:
  eBufInval(const std::string &__arg) : eBufBase(__arg){};
};
class eBufOutOfRange : public eBufBase {
public:
  eBufOutOfRange(const std::string &__arg) : eBufBase(__arg){};
};

/**
 * @class 	cbuffer
 * @brief	C++ abstraction for malloc'ed buffer areas.
 *
 * This class provides a set of convenience methods for dealing with
 * buffer areas managed by the C-functions malloc/realloc/free.
 * It is not hiding or preventing access from the buffer area.
 * The user can get access to the buffer addresses via somem()
 * and calculate offsets. However, when resizing the buffer area,
 * internal buffer addresses may have changed. cbuffer tracks
 * such changes and updates its internal variables appropriately.
 * Memory addresses kept outside of cbuffer must be updated by
 * the developer explicitly.
 */
class cbuffer : public cmemory {
private:
#define CBUFFER_DEFAULT_SIZE 0

public:
  /**
   * @brief	Constructor. Allocates a new buffer area with specified size
   * (default: 1024 bytes).
   *
   * @param len length of new buffer area to be allocated via malloc.
   */
  cbuffer(size_t len = CBUFFER_DEFAULT_SIZE);

  /**
   * @brief	Constructor. Allocates a new buffer area and clones specified
   * buffer.
   *
   * @param data pointer to start of buffer area to be cloned
   * @param datalen length of buffer area to be cloned
   */
  cbuffer(uint8_t *data, size_t datalen);

  /**
   * @brief	Copy constructor.
   *
   * @param b cbuffer instance
   */
  cbuffer(const cbuffer &b);

  /**
   * @brief	Destructor. Calls C-function free() for allocated buffer area.
   *
   */
  virtual ~cbuffer();

public:
  /**
   * @name Memory operations.
   */

  /**@{*/

  /**
   * @brief	Assignment operator.
   *
   * @param b cbuffer instance for operation
   */
  cbuffer &operator=(const cbuffer &b);

  /**@}*/

public:
  /**
   * @brief reset buffer
   */
  cbuffer &reset() {
    wseek(0, SEEK_SET);
    rseek(0, SEEK_SET);
    return *this;
  };

  /**
   * @brief	seek for writing data to buffer
   *
   * wseek(0, SEEK_SET) resets the write pointer
   */
  cbuffer &wseek(int offset = 0, int whence = SEEK_CUR) {
    if (offset < 0) {
      throw eBufInval("negative offset");
    }
    switch (whence) {
    case SEEK_SET: {
      if (offset > cmemory::length()) {
        cmemory::resize(offset);
      }
      wbytes = offset;
      rseek();
    } break;
    case SEEK_CUR: {
      if (((size_t)(wbytes + offset)) > cmemory::length()) {
        cmemory::resize(wbytes + offset);
      }
      wbytes += offset;
    } break;
    default: { throw eBufInval("invalid SEEK operation"); };
    }
    return *this;
  };

  /**
   * @brief	seek for reading data from buffer
   *
   * rseek(0, SEEK_SET) resets the read pointer
   */
  cbuffer &rseek(int offset = 0, int whence = SEEK_CUR) {
    if (offset < 0) {
      throw eBufInval("negative offset");
    }
    switch (whence) {
    case SEEK_SET: {
      if (offset < wbytes) {
        rbytes = offset;
      } else {
        rbytes = wbytes;
      }
    } break;
    case SEEK_CUR: {
      if ((rbytes + offset) < wbytes) {
        rbytes += offset;
      } else {
        rbytes = wbytes = 0; // FIXME: hmm ...
      }
    } break;
    default: { throw eBufInval("invalid SEEK operation"); };
    }
    return *this;
  };

  /**
   * @brief returns start of memory + wbytes
   */
  uint8_t *sowmem() const { return (cmemory::somem() + wbytes); };

  /**
   * @brief returns available space for writing data
   */
  size_t wmemlen() const { return (cmemory::length() - wbytes); };

  /**
   * @brief returns start of memory + rbytes
   */
  uint8_t *sormem() const { return (cmemory::somem() + rbytes); };

  /**
   * @brief returns available data for reading
   */
  size_t rmemlen() const { return (wbytes - rbytes); };

  /**
   * @brief	returns boolean flag true if no data is available
   */
  bool empty() const { return (wbytes == rbytes); };

public:
  friend std::ostream &operator<<(std::ostream &os, const cbuffer &b) {
    os << "<cbuffer:"
       << " length: " << b.memlen() << " wbytes: " << b.wbytes
       << " rbytes: " << b.rbytes << " >";
    return os;
  };

private:
  // bytes written to this buffer
  int wbytes;

  // bytes read from this buffer
  int rbytes;
};

}; // end of namespace

#endif
