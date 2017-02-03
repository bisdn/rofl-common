/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coftablestats.h
 *
 *  Created on: 14.03.2013
 *  Revised on: 07.11.2015
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFTABLEFEATURES_H
#define ROFL_COMMON_OPENFLOW_COFTABLEFEATURES_H 1

#include <inttypes.h>

#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/coftablefeatureprops.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eOFTableFeaturesBase : public exception {
public:
  eOFTableFeaturesBase(const std::string &__arg = std::string(""))
      : exception(__arg) {
    set_exception("eOFTableFeaturesBase");
  };
};
class eOFTableFeaturesNotFound : public eOFTableFeaturesBase {
public:
  eOFTableFeaturesNotFound(const std::string &__arg = std::string(""))
      : eOFTableFeaturesBase(__arg) {
    set_exception("eOFTableFeaturesNotFound");
  };
};

class coftable_features {
public:
  /**
   *
   */
  virtual ~coftable_features(){};

  /**
   *
   */
  coftable_features(uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN)
      : ofp_version(ofp_version), table_id(0), metadata_match(0),
        metadata_write(0), config(0), max_entries(0), properties(ofp_version){};

  /**
   *
   */
  coftable_features(const coftable_features &table_features) {
    *this = table_features;
  }

  /**
   *
   */
  coftable_features &operator=(const coftable_features &table_features) {
    if (this == &table_features)
      return *this;

    ofp_version = table_features.ofp_version;
    table_id = table_features.table_id;
    name = table_features.name;
    metadata_match = table_features.metadata_match;
    metadata_write = table_features.metadata_write;
    config = table_features.config;
    max_entries = table_features.max_entries;
    properties = table_features.properties;

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
  /**
   *
   */
  coftable_features &set_version(uint8_t ofp_version) {
    this->ofp_version = ofp_version;
    properties.set_version(ofp_version);
    return *this;
  };

  /**
   *
   */
  uint8_t get_version() const { return ofp_version; };

  /**
   *
   */
  coftable_features &set_table_id(uint8_t table_id) {
    this->table_id = table_id;
    return *this;
  };

  /**
   *
   */
  uint8_t get_table_id() const { return table_id; };

  /**
   *
   */
  coftable_features &set_name(const std::string &name) {
    this->name = name;
    return *this;
  };

  /**
   *
   */
  const std::string &get_name() const { return name; };

  /**
   *
   */
  coftable_features &set_metadata_match(uint64_t metadata_match) {
    this->metadata_match = metadata_match;
    return *this;
  };

  /**
   *
   */
  uint64_t get_metadata_match() const { return metadata_match; };

  /**
   *
   */
  coftable_features &set_metadata_write(uint64_t metadata_write) {
    this->metadata_write = metadata_write;
    return *this;
  };

  /**
   *
   */
  uint64_t get_metadata_write() const { return metadata_write; };

  /**
   *
   */
  coftable_features &set_config(uint32_t config) {
    this->config = config;
    return *this;
  };

  /**
   *
   */
  uint32_t get_config() const { return config; };

  /**
   *
   */
  coftable_features &set_max_entries(uint32_t max_entries) {
    this->max_entries = max_entries;
    return *this;
  };

  /**
   *
   */
  uint32_t get_max_entries() const { return max_entries; };

  /**
   *
   */
  coftable_features &
  set_properties(const rofl::openflow::coftable_feature_props &properties) {
    (this->properties = properties).set_version(ofp_version);
    return *this;
  };

  /**
   *
   */
  rofl::openflow::coftable_feature_props &set_properties() {
    return properties;
  };

  /**
   *
   */
  const rofl::openflow::coftable_feature_props &get_properties() const {
    return properties;
  };

public:
  friend std::ostream &operator<<(std::ostream &os,
                                  coftable_features const &msg) {
    os << "<coftable_features >" << std::endl;
    os << "<table-id: " << (int)msg.get_table_id() << " >" << std::endl;
    os << "<name: " << msg.get_name() << " >" << std::endl;
    os << std::hex;
    os << "<metadata-match: 0x" << (unsigned long long)msg.get_metadata_match()
       << " >" << std::endl;
    os << "<metadata-write: 0x" << (unsigned long long)msg.get_metadata_write()
       << " >" << std::endl;
    os << "<config: 0x" << (unsigned int)msg.get_config() << " >" << std::endl;
    os << "<max-entries: 0x" << (unsigned int)msg.get_max_entries() << " >"
       << std::endl;
    os << std::dec;

    os << msg.get_properties();
    return os;
  };

private:
  uint8_t ofp_version;
  uint8_t table_id;
  std::string name;
  uint64_t metadata_match;
  uint64_t metadata_write;
  uint32_t config;
  uint32_t max_entries;
  rofl::openflow::coftable_feature_props properties;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFTABLEFEATURES_H */
