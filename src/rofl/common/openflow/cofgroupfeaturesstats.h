/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofflowstatsrequest.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef ROFL_COMMON_OPENFLOW_COFGROUPFEATURESSTATS_H
#define ROFL_COMMON_OPENFLOW_COFGROUPFEATURESSTATS_H 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/cofactions.h"
#include "rofl/common/openflow/cofinstructions.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class cofgroup_features_stats_reply {
public:
  /**
   *
   */
  virtual ~cofgroup_features_stats_reply(){};

  /**
   *
   */
  cofgroup_features_stats_reply(uint8_t of_version = 0,
                                uint8_t *buf = (uint8_t *)0, size_t buflen = 0)
      : of_version(of_version), types(0), capabilities(0) {
    max_groups.resize(4);
    actions.resize(4);
    if ((buflen == 0) || (nullptr == buf)) {
      return;
    }
    unpack(buf, buflen);
  };

  /**
   *
   */
  cofgroup_features_stats_reply(uint8_t of_version, uint32_t types,
                                uint32_t capabilities,
                                const std::vector<uint32_t> &max_groups,
                                const std::vector<uint32_t> &actions)
      : of_version(of_version), types(types), capabilities(capabilities),
        max_groups(max_groups), actions(actions) {
    if (max_groups.size() < 4)
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);
    if (actions.size() < 4)
      throw eInvalid("eInvalid", __FILE__, __FUNCTION__, __LINE__);
  };

  /**
   *
   */
  cofgroup_features_stats_reply(
      const cofgroup_features_stats_reply &group_features_stats) {
    *this = group_features_stats;
  };

  /**
   *
   */
  cofgroup_features_stats_reply &
  operator=(const cofgroup_features_stats_reply &group_features_stats) {
    if (this == &group_features_stats)
      return *this;

    of_version = group_features_stats.of_version;
    types = group_features_stats.types;
    capabilities = group_features_stats.capabilities;
    max_groups = group_features_stats.max_groups;
    actions = group_features_stats.actions;

    return *this;
  };

public:
  /**
   *
   */
  cofgroup_features_stats_reply &set_version(uint8_t of_version) {
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
  cofgroup_features_stats_reply &set_types(uint32_t types) {
    this->types = types;
    return *this;
  };

  /**
   *
   */
  uint32_t get_types() const { return types; };

  /**
   *
   */
  cofgroup_features_stats_reply &set_capabilities(uint32_t capabilities) {
    this->capabilities = capabilities;
    return *this;
  };

  /**
   *
   */
  uint32_t get_capabilities() const { return capabilities; };

  /**
   *
   */
  cofgroup_features_stats_reply &
  set_max_groups(const std::vector<uint32_t> &max_groups) {
    this->max_groups = max_groups;
    return *this;
  };

  /**
   *
   */
  std::vector<uint32_t> &set_max_groups() { return max_groups; };

  /**
   *
   */
  const std::vector<uint32_t> &get_max_groups() const { return max_groups; };

  /**
   *
   */
  cofgroup_features_stats_reply &
  set_actions(const std::vector<uint32_t> &actions) {
    this->actions = actions;
    return *this;
  };

  /**
   *
   */
  std::vector<uint32_t> &set_actions() { return actions; };

  /**
   *
   */
  const std::vector<uint32_t> &get_actions() const { return actions; };

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
                                  cofgroup_features_stats_reply const &reply) {
    os << "<cofgroup_features_stats_reply >" << std::endl;
    os << "<types: 0x" << std::hex << (int)reply.get_types() << std::dec << " >"
       << std::endl;
    os << "<capabilities: 0x" << std::hex << (int)reply.get_capabilities()
       << std::dec << " >" << std::endl;
    for (unsigned int i = 0; i < reply.max_groups.size(); ++i) {
      os << "<max-groups[" << i << "]: " << (int)reply.max_groups[i] << " >"
         << std::endl;
    }
    for (unsigned int i = 0; i < reply.actions.size(); ++i) {
      os << "<actions[" << i << "]: 0x" << std::hex << (int)reply.max_groups[i]
         << std::dec << " >" << std::endl;
    }
    return os;
  };

private:
  uint8_t of_version;
  uint32_t types;
  uint32_t capabilities;
  std::vector<uint32_t> max_groups;
  std::vector<uint32_t> actions;
};

} // namespace openflow
} // namespace rofl

#endif /* ROFL_COMMON_OPENFLOW_COFGROUPFEATURESSTATS_H */
