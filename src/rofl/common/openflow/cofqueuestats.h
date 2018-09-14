/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofqueuestats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFQUEUESTATS_H
#define ROFL_COMMON_OPENFLOW_COFQUEUESTATS_H 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eQueueStatsBase : public exception {
public:
  eQueueStatsBase(const std::string &__arg = std::string(""))
      : exception(__arg) {
    set_exception("eQueueStatsBase");
  };
};
class eQueueStatsNotFound : public eQueueStatsBase {
public:
  eQueueStatsNotFound(const std::string &__arg = std::string(""))
      : eQueueStatsBase(__arg) {
    set_exception("eQueueStatsNotFound");
  };
};

class cofqueue_stats_request {
public:
  /**
   *
   */
  ~cofqueue_stats_request(){};

  /**
   *
   */
  cofqueue_stats_request(
      uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : of_version(of_version), port_no(0), queue_id(0){};

  /**
   *
   */
  cofqueue_stats_request(uint8_t of_version, uint32_t port_no,
                         uint32_t queue_id)
      : of_version(of_version), port_no(port_no), queue_id(queue_id){};

  /**
   *
   */
  cofqueue_stats_request(const cofqueue_stats_request &req) { *this = req; };

  /**
   *
   */
  cofqueue_stats_request &operator=(const cofqueue_stats_request &req) {
    if (this == &req)
      return *this;

    of_version = req.of_version;
    port_no = req.port_no;
    queue_id = req.queue_id;

    return *this;
  };

public:
  /**
   *
   */
  size_t length() const;

  /**
   *
   */
  void pack(uint8_t *buf, size_t buflen) const;

  /**
   *
   */
  void unpack(uint8_t *buf, size_t buflen);

public:
  /**
   *
   */
  cofqueue_stats_request &set_version(uint8_t of_version) {
    this->of_version = of_version;
    return *this;
  };

  /**
   *
   */
  uint8_t get_version() const { return of_version; };

  /**
   *
   */
  cofqueue_stats_request &set_port_no(uint32_t port_no) {
    this->port_no = port_no;
    return *this;
  };

  /**
   *
   */
  uint32_t get_port_no() const { return port_no; };

  /**
   *
   */
  cofqueue_stats_request &set_queue_id(uint32_t queue_id) {
    this->queue_id = queue_id;
    return *this;
  };

  /**
   *
   */
  uint32_t get_queue_id() const { return queue_id; };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofqueue_stats_request const &r) {
    os << "<cofqueue_stats_request >" << std::endl;
    os << "<port-no: 0x" << std::hex << (int)r.get_port_no() << std::dec << " >"
       << std::endl;
    os << "<queue-id: " << (int)r.get_queue_id() << " >" << std::endl;
    return os;
  };

private:
  uint8_t of_version;
  uint32_t port_no;
  uint32_t queue_id;
};

class cofqueue_stats_reply {
public:
  /**
   *
   */
  ~cofqueue_stats_reply(){};

  /**
   *
   */
  cofqueue_stats_reply(uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : of_version(of_version), port_no(0), queue_id(0), tx_bytes(0),
        tx_packets(0), tx_errors(0), duration_sec(0), duration_nsec(0){};

  /**
   *
   */
  cofqueue_stats_reply(uint8_t of_version, uint32_t port_no, uint32_t queue_id,
                       uint64_t tx_bytes, uint64_t tx_packets,
                       uint64_t tx_errors, uint32_t duration_sec,
                       uint32_t duration_nsec)
      : of_version(of_version), port_no(port_no), queue_id(queue_id),
        tx_bytes(tx_bytes), tx_packets(tx_packets), tx_errors(tx_errors),
        duration_sec(duration_sec), duration_nsec(duration_nsec){};

  /**
   *
   */
  cofqueue_stats_reply(const cofqueue_stats_reply &rep) { *this = rep; };

  /**
   *
   */
  cofqueue_stats_reply &operator=(const cofqueue_stats_reply &rep) {
    if (this == &rep)
      return *this;

    of_version = rep.of_version;
    port_no = rep.port_no;
    queue_id = rep.queue_id;
    tx_bytes = rep.tx_bytes;
    tx_packets = rep.tx_packets;
    tx_errors = rep.tx_errors;
    duration_sec = rep.duration_sec;
    duration_nsec = rep.duration_nsec;

    return *this;
  };

  /**
   *
   */
  bool operator==(const cofqueue_stats_reply &rep) {
    return ((of_version == rep.of_version) && (port_no == rep.port_no) &&
            (queue_id == rep.queue_id) && (tx_bytes == rep.tx_bytes) &&
            (tx_packets == rep.tx_packets) && (tx_errors == rep.tx_errors) &&
            (duration_sec == rep.duration_sec) &&
            (duration_nsec == rep.duration_nsec));
  };

public:
  /**
   *
   */
  cofqueue_stats_reply &set_version(uint8_t of_version) {
    this->of_version = of_version;
    return *this;
  };

  /**
   *
   */
  uint8_t get_version() const { return of_version; };

  /**
   *
   */
  cofqueue_stats_reply &set_port_no(uint32_t port_no) {
    this->port_no = port_no;
    return *this;
  };

  /**
   *
   */
  uint32_t get_port_no() const { return port_no; };

  /**
   *
   */
  cofqueue_stats_reply &set_queue_id(uint32_t queue_id) {
    this->queue_id = queue_id;
    return *this;
  };

  /**
   *
   */
  uint32_t get_queue_id() const { return queue_id; };

  /**
   *
   */
  cofqueue_stats_reply &set_tx_bytes(uint64_t tx_bytes) {
    this->tx_bytes = tx_bytes;
    return *this;
  };

  /**
   *
   */
  uint64_t get_tx_bytes() const { return tx_bytes; };

  /**
   *
   */
  cofqueue_stats_reply &set_tx_packets(uint64_t tx_packets) {
    this->tx_packets = tx_packets;
    return *this;
  };

  /**
   *
   */
  uint64_t get_tx_packets() const { return tx_packets; };

  /**
   *
   */
  cofqueue_stats_reply &set_tx_errors(uint64_t tx_errors) {
    this->tx_errors = tx_errors;
    return *this;
  };

  /**
   *
   */
  uint64_t get_tx_errors() const { return tx_errors; };

  /**
   *
   */
  cofqueue_stats_reply &set_duration_sec(uint32_t duration_sec) {
    this->duration_sec = duration_sec;
    return *this;
  };

  /**
   *
   */
  uint32_t get_duration_sec() const { return duration_sec; };

  /**
   *
   */
  cofqueue_stats_reply &set_duration_nsec(uint32_t duration_nsec) {
    this->duration_nsec = duration_nsec;
    return *this;
  };

  /**
   *
   */
  uint32_t get_duration_nsec() const { return duration_nsec; };

public:
  /**
   *
   */
  size_t length() const;

  /**
   *
   */
  void pack(uint8_t *buf, size_t buflen) const;

  /**
   *
   */
  void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofqueue_stats_reply const &r) {
    os << "<cofqueue_stats_reply >" << std::endl;
    os << std::hex;
    os << "<port-no: 0x" << (int)r.get_port_no() << " >" << std::endl;
    os << "<queue-id: 0x" << (int)r.get_queue_id() << " >" << std::endl;
    os << "<tx-packets: 0x" << (unsigned long long)r.get_tx_packets() << " >"
       << std::endl;
    os << "<tx-bytes: 0x" << (unsigned long long)r.get_tx_bytes() << " >"
       << std::endl;
    os << "<tx-errors: 0x" << (unsigned long long)r.get_tx_errors() << " >"
       << std::endl;
    os << std::dec;
    switch (r.get_version()) {
    case rofl::openflow13::OFP_VERSION: {
      os << "<duration-sec: " << (unsigned long long)r.get_duration_sec()
         << " >" << std::endl;
      os << "<duration-nsec: " << (unsigned long long)r.get_duration_nsec()
         << " >" << std::endl;
    } break;
    default: {};
    }
    return os;
  };

private:
  uint8_t of_version;
  uint32_t port_no;
  uint32_t queue_id;
  uint64_t tx_bytes;
  uint64_t tx_packets;
  uint64_t tx_errors;
  uint32_t duration_sec;
  uint32_t duration_nsec;
};
} // namespace openflow
} // namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFQUEUESTATS_H */
