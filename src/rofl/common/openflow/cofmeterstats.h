/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofmeterstats.h
 *
 *  Created on: 26.05.2014
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFMETERSTATS_H
#define ROFL_COMMON_OPENFLOW_COFMETERSTATS_H 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofmeterbandstatsarray.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eRofMeterStatsBase : public exception {
public:
  eRofMeterStatsBase(const std::string &__arg = std::string(""))
      : exception(__arg) {
    set_exception("eRofMeterStatsBase");
  };
};
class eRofMeterStatsNotFound : public eRofMeterStatsBase {
public:
  eRofMeterStatsNotFound(const std::string &__arg = std::string(""))
      : eRofMeterStatsBase(__arg) {
    set_exception("eRofMeterStatsNotFound");
  };
};

class cofmeter_stats_request {
public:
  /**
   *
   */
  ~cofmeter_stats_request(){};

  /**
   *
   */
  cofmeter_stats_request(
      uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
      uint8_t *buf = (uint8_t *)0, size_t buflen = 0)
      : of_version(of_version), meter_id(0) {
    if ((buflen == 0) || (nullptr == buf)) {
      return;
    }
    unpack(buf, buflen);
  };

  /**
   *
   */
  cofmeter_stats_request(uint8_t of_version, uint32_t meter_id)
      : of_version(of_version), meter_id(meter_id){};

  /**
   *
   */
  cofmeter_stats_request(const cofmeter_stats_request &req) { *this = req; };

  /**
   *
   */
  cofmeter_stats_request &operator=(const cofmeter_stats_request &req) {
    if (this == &req)
      return *this;

    of_version = req.of_version;
    meter_id = req.meter_id;

    return *this;
  };

  /**
   *
   */
  bool operator==(const cofmeter_stats_request &req) const {
    return (meter_id == req.meter_id);
  };

public:
  /**
   *
   */
  cofmeter_stats_request &set_version(uint8_t of_version) {
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
  cofmeter_stats_request &set_meter_id(uint32_t meter_id) {
    this->meter_id = meter_id;
    return *this;
  };

  /**
   *
   */
  uint32_t get_meter_id() const { return meter_id; };

public:
  /**
   *
   */
  size_t length() const;

  /**
   *
   */
  void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmeter_stats_request const &r) {
    os << "<cofmeter_stats_request >" << std::endl;
    os << "<meter-id: 0x" << std::hex << (int)r.get_meter_id() << std::dec
       << " >" << std::endl;
    return os;
  };

private:
  uint8_t of_version;
  uint32_t meter_id;
};

class cofmeter_stats_reply {
public:
  /**
   *
   */
  ~cofmeter_stats_reply(){};

  /**
   *
   */
  cofmeter_stats_reply(uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
                       uint8_t *buf = (uint8_t *)0, size_t buflen = 0)
      : of_version(of_version), meter_id(0), flow_count(0), packet_in_count(0),
        byte_in_count(0), duration_sec(0), duration_nsec(0),
        mbstats(of_version) {
    if ((buflen == 0) || (nullptr == buf)) {
      return;
    }
    unpack(buf, buflen);
  };

  /**
   *
   */
  cofmeter_stats_reply(uint8_t of_version, uint32_t meter_id,
                       uint32_t flow_count, uint64_t packet_in_count,
                       uint64_t byte_in_count, uint32_t duration_sec,
                       uint32_t duration_nsec,
                       const rofl::openflow::cofmeter_band_stats_array &mbstats)
      : of_version(of_version), meter_id(meter_id), flow_count(flow_count),
        packet_in_count(packet_in_count), byte_in_count(byte_in_count),
        duration_sec(duration_sec), duration_nsec(duration_nsec),
        mbstats(mbstats) {
    this->mbstats.set_version(of_version);
  };

  /**
   *
   */
  cofmeter_stats_reply(const cofmeter_stats_reply &rep) { *this = rep; };

  /**
   *
   */
  cofmeter_stats_reply &operator=(const cofmeter_stats_reply &rep) {
    if (this == &rep)
      return *this;

    of_version = rep.of_version;
    meter_id = rep.meter_id;
    flow_count = rep.flow_count;
    packet_in_count = rep.packet_in_count;
    byte_in_count = rep.byte_in_count;
    duration_sec = rep.duration_sec;
    duration_nsec = rep.duration_nsec;
    mbstats = rep.mbstats;

    return *this;
  };

  /**
   *
   */
  bool operator==(const cofmeter_stats_reply &rep) {
    return ((of_version == rep.of_version) && (meter_id == rep.meter_id) &&
            (flow_count == rep.flow_count) &&
            (packet_in_count == rep.packet_in_count) &&
            (byte_in_count == rep.byte_in_count) &&
            (duration_sec == rep.duration_sec) &&
            (duration_nsec == rep.duration_nsec) && (mbstats == rep.mbstats));
  };

public:
  /**
   *
   */
  cofmeter_stats_reply &set_meter_band_stats(
      const rofl::openflow::cofmeter_band_stats_array &mbstats) {
    (this->mbstats = mbstats).set_version(of_version);
    return *this;
  };

  /**
   *
   */
  rofl::openflow::cofmeter_band_stats_array &set_meter_band_stats() {
    return mbstats;
  };

  /**
   *
   */
  const rofl::openflow::cofmeter_band_stats_array &
  get_meter_band_stats() const {
    return mbstats;
  };

  /**
   *
   */
  cofmeter_stats_reply &set_version(uint8_t of_version) {
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
  cofmeter_stats_reply &set_meter_id(uint32_t meter_id) {
    this->meter_id = meter_id;
    return *this;
  };

  /**
   *
   */
  uint32_t get_meter_id() const { return meter_id; };

  /**
   *
   */
  cofmeter_stats_reply &set_flow_count(uint32_t flow_count) {
    this->flow_count = flow_count;
    return *this;
  };

  /**
   *
   */
  uint32_t get_flow_count() const { return flow_count; };

  /**
   *
   */
  cofmeter_stats_reply &set_packet_in_count(uint64_t packet_in_count) {
    this->packet_in_count = packet_in_count;
    return *this;
  };

  /**
   *
   */
  uint64_t get_packet_in_count() const { return packet_in_count; };

  /**
   *
   */
  cofmeter_stats_reply &set_byte_in_count(uint64_t byte_in_count) {
    this->byte_in_count = byte_in_count;
    return *this;
  };

  /**
   *
   */
  uint64_t get_byte_in_count() const { return byte_in_count; };

  /**
   *
   */
  cofmeter_stats_reply &set_duration_sec(uint32_t duration_sec) {
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
  cofmeter_stats_reply &set_duration_nsec(uint32_t duration_nsec) {
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
  cofmeter_stats_reply &clear() {
    meter_id = 0;
    flow_count = 0;
    packet_in_count = 0;
    byte_in_count = 0;
    duration_sec = 0;
    duration_nsec = 0;
    mbstats.clear();
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
  void pack(uint8_t *buf, size_t buflen);

  /**
   *
   */
  void unpack(uint8_t *buf, size_t buflen);

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  cofmeter_stats_reply const &r) {
    os << "<cofmeter_stats_reply >" << std::endl;
    os << std::hex;
    os << "<meter-id: 0x" << (unsigned int)r.get_meter_id() << " >"
       << std::endl;
    os << "<flow-count: 0x" << (unsigned int)r.get_flow_count() << " >"
       << std::endl;
    os << "<packet-in-count: 0x" << (unsigned long long)r.get_packet_in_count()
       << " >" << std::endl;
    os << "<byte-in-count: 0x" << (unsigned long long)r.get_byte_in_count()
       << " >" << std::endl;
    os << "<duration-sec: 0x" << (unsigned long long)r.get_duration_sec()
       << " >" << std::endl;
    os << "<duration-nsec: 0x" << (unsigned long long)r.get_duration_nsec()
       << " >" << std::endl;
    os << std::dec;
    { os << r.get_meter_band_stats(); }
    return os;
  };

private:
  uint8_t of_version;
  uint32_t meter_id;
  uint32_t flow_count;
  uint64_t packet_in_count;
  uint64_t byte_in_count;
  uint32_t duration_sec;
  uint32_t duration_nsec;
  rofl::openflow::cofmeter_band_stats_array mbstats;
};

typedef cofmeter_stats_reply cofmeter_stats;

} /* end of namespace openflow */
} /* end of namespace rofl */

#endif /* ROFL_COMMON_OPENFLOW_COFMETERSTATS_H */
