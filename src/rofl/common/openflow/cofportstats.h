/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofportstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFPORTSTATS_H
#define ROFL_COMMON_OPENFLOW_COFPORTSTATS_H 1

#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class ePortStatsBase : public exception {
public:
  ePortStatsBase(const std::string &__arg = std::string(""))
      : exception(__arg) {
    set_exception("ePortStatsBase");
  };
};
class ePortStatsNotFound : public ePortStatsBase {
public:
  ePortStatsNotFound(const std::string &__arg = std::string(""))
      : ePortStatsBase(__arg) {
    set_exception("ePortStatsNotFound");
  };
};

class cofport_stats_request {
public:
  /**
   *
   */
  ~cofport_stats_request(){};

  /**
   *
   */
  cofport_stats_request(
      uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint8_t *buf = nullptr, size_t buflen = 0)
      : of_version(of_version), port_no(rofl::openflow13::OFPP_ALL) {
    if ((buflen == 0) || (nullptr == buf)) {
      return;
    }
    unpack(buf, buflen);
  };

  /**
   *
   */
  cofport_stats_request(uint8_t of_version, uint32_t port_no)
      : of_version(of_version), port_no(port_no){};

  /**
   *
   */
  cofport_stats_request(const cofport_stats_request &req) { *this = req; };

  /**
   *
   */
  cofport_stats_request &operator=(const cofport_stats_request &req) {
    if (this == &req)
      return *this;

    of_version = req.of_version;
    port_no = req.port_no;

    return *this;
  };

public:
  /**
   *
   */
  cofport_stats_request &set_version(uint8_t of_version) {
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
  cofport_stats_request &set_port_no(uint32_t port_no) {
    this->port_no = port_no;
    return *this;
  };

  /**
   *
   */
  uint32_t get_port_no() const { return port_no; }

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
                                  cofport_stats_request const &r) {
    os << "<cofport_stats_request >" << std::endl;
    os << "<port-no: 0x" << std::hex << (int)r.get_port_no() << std::dec << " >"
       << std::endl;
    return os;
  };

private:
  uint8_t of_version;
  uint32_t port_no;
};

class cofport_stats_reply {
public:
  /**
   *
   */
  ~cofport_stats_reply(){};

  /**
   *
   */
  cofport_stats_reply(uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                      uint8_t *buf = nullptr, size_t buflen = 0)
      : of_version(of_version), port_no(0), rx_packets(0), tx_packets(0),
        rx_bytes(0), tx_bytes(0), rx_dropped(0), tx_dropped(0), rx_errors(0),
        tx_errors(0), rx_frame_err(0), rx_over_err(0), rx_crc_err(0),
        collisions(0), duration_sec(0), duration_nsec(0) {
    if ((buflen == 0) || (nullptr == buf)) {
      return;
    }
    unpack(buf, buflen);
  };

  /**
   *
   */
  cofport_stats_reply(uint8_t of_version, uint32_t port_no, uint64_t rx_packets,
                      uint64_t tx_packets, uint64_t rx_bytes, uint64_t tx_bytes,
                      uint64_t rx_dropped, uint64_t tx_dropped,
                      uint64_t rx_errors, uint64_t tx_errors,
                      uint64_t rx_frame_err, uint64_t rx_over_err,
                      uint64_t rx_crc_err, uint64_t collisions,
                      uint32_t duration_sec, uint32_t duration_nsec)
      : of_version(of_version), port_no(port_no), rx_packets(rx_packets),
        tx_packets(tx_packets), rx_bytes(rx_bytes), tx_bytes(tx_bytes),
        rx_dropped(rx_dropped), tx_dropped(tx_dropped), rx_errors(rx_errors),
        tx_errors(tx_errors), rx_frame_err(rx_frame_err),
        rx_over_err(rx_over_err), rx_crc_err(rx_crc_err),
        collisions(collisions), duration_sec(duration_sec),
        duration_nsec(duration_nsec){};

  /**
   *
   */
  cofport_stats_reply(const cofport_stats_reply &rep) { *this = rep; };

  /**
   *
   */
  cofport_stats_reply &operator=(const cofport_stats_reply &rep) {
    if (this == &rep)
      return *this;

    of_version = rep.of_version;
    port_no = rep.port_no;
    rx_packets = rep.rx_packets;
    tx_packets = rep.tx_packets;
    rx_bytes = rep.rx_bytes;
    tx_bytes = rep.tx_bytes;
    rx_dropped = rep.rx_dropped;
    tx_dropped = rep.tx_dropped;
    rx_errors = rep.rx_errors;
    tx_errors = rep.tx_errors;
    rx_frame_err = rep.rx_frame_err;
    rx_over_err = rep.rx_over_err;
    rx_crc_err = rep.rx_crc_err;
    collisions = rep.collisions;
    duration_sec = rep.duration_sec;
    duration_nsec = rep.duration_nsec;

    return *this;
  };

  /**
   *
   */
  bool operator==(const cofport_stats_reply &rep) const {
    return ((of_version == rep.of_version) && (port_no == rep.port_no) &&
            (rx_packets == rep.rx_packets) && (tx_packets == rep.tx_packets) &&
            (rx_bytes == rep.rx_bytes) && (tx_bytes == rep.tx_bytes) &&
            (rx_dropped == rep.rx_dropped) && (tx_dropped == rep.tx_dropped) &&
            (rx_errors == rep.rx_errors) && (tx_errors == rep.tx_errors) &&
            (rx_frame_err == rep.rx_frame_err) &&
            (rx_over_err == rep.rx_over_err) &&
            (rx_crc_err == rep.rx_crc_err) && (collisions == rep.collisions) &&
            (duration_sec == rep.duration_sec) &&
            (duration_nsec == rep.duration_nsec));
  };

public:
  /**
   *
   */
  cofport_stats_reply &set_version(uint8_t of_version) {
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
  cofport_stats_reply &set_port_no(uint32_t port_no) {
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
  cofport_stats_reply &set_rx_packets(uint64_t rx_packets) {
    this->rx_packets = rx_packets;
    return *this;
  };

  /**
   *
   */
  uint64_t get_rx_packets() const { return rx_packets; };

  /**
   *
   */
  cofport_stats_reply &set_tx_packets(uint64_t tx_packets) {
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
  cofport_stats_reply &set_rx_bytes(uint64_t rx_bytes) {
    this->rx_bytes = rx_bytes;
    return *this;
  };

  /**
   *
   */
  uint64_t get_rx_bytes() const { return rx_bytes; };

  /**
   *
   */
  cofport_stats_reply &set_tx_bytes(uint64_t tx_bytes) {
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
  cofport_stats_reply &set_rx_dropped(uint64_t rx_dropped) {
    this->rx_dropped = rx_dropped;
    return *this;
  };

  /**
   *
   */
  uint64_t get_rx_dropped() const { return rx_dropped; };

  /**
   *
   */
  cofport_stats_reply &set_tx_dropped(uint64_t tx_dropped) {
    this->tx_dropped = tx_dropped;
    return *this;
  };

  /**
   *
   */
  uint64_t get_tx_dropped() const { return tx_dropped; };

  /**
   *
   */
  cofport_stats_reply &set_rx_errors(uint64_t rx_errors) {
    this->rx_errors = rx_errors;
    return *this;
  };

  /**
   *
   */
  uint64_t get_rx_errors() const { return rx_errors; };

  /**
   *
   */
  cofport_stats_reply &set_tx_errors(uint64_t tx_errors) {
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
  cofport_stats_reply &set_rx_frame_err(uint64_t rx_frame_err) {
    this->rx_frame_err = rx_frame_err;
    return *this;
  };

  /**
   *
   */
  uint64_t get_rx_frame_err() const { return rx_frame_err; };

  /**
   *
   */
  cofport_stats_reply &set_rx_over_err(uint64_t rx_over_err) {
    this->rx_over_err = rx_over_err;
    return *this;
  };

  /**
   *
   */
  uint64_t get_rx_over_err() const { return rx_over_err; };

  /**
   *
   */
  cofport_stats_reply &set_rx_crc_err(uint64_t rx_crc_err) {
    this->rx_crc_err = rx_crc_err;
    return *this;
  };

  /**
   *
   */
  uint64_t get_rx_crc_err() const { return rx_crc_err; };

  /**
   *
   */
  cofport_stats_reply &set_collisions(uint64_t collisions) {
    this->collisions = collisions;
    return *this;
  };

  /**
   *
   */
  uint64_t get_collisions() const { return collisions; };

  /**
   *
   */
  cofport_stats_reply &set_duration_sec(uint32_t duration_sec) {
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
  cofport_stats_reply &set_duration_nsec(uint32_t duration_nsec) {
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
                                  cofport_stats_reply const &r) {
    os << "<cofport_stats_reply >" << std::endl;
    os << "<port-no: " << (int)r.get_port_no() << " >" << std::endl;
    os << "<rx-packets: " << (unsigned long long)r.get_rx_packets() << " >"
       << std::endl;
    os << "<tx-packets: " << (unsigned long long)r.get_tx_packets() << " >"
       << std::endl;
    os << "<rx-bytes: " << (unsigned long long)r.get_rx_bytes() << " >"
       << std::endl;
    os << "<tx-bytes: " << (unsigned long long)r.get_tx_bytes() << " >"
       << std::endl;
    os << "<rx-dropped: " << (unsigned long long)r.get_rx_dropped() << " >"
       << std::endl;
    os << "<tx-dropped: " << (unsigned long long)r.get_tx_dropped() << " >"
       << std::endl;
    os << "<rx-errors: " << (unsigned long long)r.get_rx_errors() << " >"
       << std::endl;
    os << "<tx-errors: " << (unsigned long long)r.get_tx_errors() << " >"
       << std::endl;
    os << "<rx-frame-err: " << (unsigned long long)r.get_rx_frame_err() << " >"
       << std::endl;
    os << "<rx-over-err: " << (unsigned long long)r.get_rx_over_err() << " >"
       << std::endl;
    os << "<rx-crc-err: " << (unsigned long long)r.get_rx_crc_err() << " >"
       << std::endl;
    os << "<collisions: " << (unsigned long long)r.get_collisions() << " >"
       << std::endl;
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
  uint64_t rx_packets;
  uint64_t tx_packets;
  uint64_t rx_bytes;
  uint64_t tx_bytes;
  uint64_t rx_dropped;
  uint64_t tx_dropped;
  uint64_t rx_errors;
  uint64_t tx_errors;
  uint64_t rx_frame_err;
  uint64_t rx_over_err;
  uint64_t rx_crc_err;
  uint64_t collisions;
  uint32_t duration_sec;  // since OF1.3
  uint32_t duration_nsec; // since OF1.3
};

} /* end of namespace */
} /* end of namespace */

#endif /* ROFL_COMMON_OPENFLOW_COFPORTSTATS_H */
