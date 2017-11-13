/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cbuffer.hpp"

using namespace rofl;

cbuffer::cbuffer(size_t len) : cmemory(len), wbytes(0), rbytes(0) {}

cbuffer::cbuffer(uint8_t *data, size_t datalen)
    : cmemory(data, datalen), wbytes(datalen), rbytes(0) {}

cbuffer::cbuffer(const cbuffer &b) { *this = b; }

cbuffer::~cbuffer() {}

cbuffer &cbuffer::operator=(const cbuffer &b) {
  if (this == &b)
    return *this;

  cmemory::operator=(b);
  this->wbytes = b.wbytes;
  this->rbytes = b.rbytes;

  return *this;
}
