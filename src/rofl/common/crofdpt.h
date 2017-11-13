/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofdpt.h
 *
 *  Created on: 07.01.2014
 *  Revised on: 03.10.2015
 *      Author: andreas
 */

#ifndef CROFDPT_H
#define CROFDPT_H 1

#include <algorithm>
#include <atomic>
#include <bitset>
#include <inttypes.h>
#include <map>
#include <set>
#include <stdio.h>
#include <strings.h>

#include "rofl/common/cmemory.h"

#include "rofl/common/cauxid.h"
#include "rofl/common/cdpid.h"
#include "rofl/common/cdptid.h"
#include "rofl/common/crandom.h"
#include "rofl/common/crofchan.h"
#include "rofl/common/crofqueue.h"
#include "rofl/common/exception.hpp"

#include "rofl/common/openflow/cofasyncconfig.h"
#include "rofl/common/openflow/cofflowmod.h"
#include "rofl/common/openflow/cofgroupmod.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/openflow/cofmeterbands.h"
#include "rofl/common/openflow/cofmeterconfig.h"
#include "rofl/common/openflow/cofmeterfeatures.h"
#include "rofl/common/openflow/cofmeterstats.h"
#include "rofl/common/openflow/cofpacketqueue.h"
#include "rofl/common/openflow/cofports.h"
#include "rofl/common/openflow/cofrole.h"
#include "rofl/common/openflow/coftables.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/openflow.h"

#include "rofl/common/locking.hpp"

namespace rofl {

/* error classes */
class eRofDptBase : public exception {
public:
  eRofDptBase(const std::string &__arg) : exception(__arg){};
};
class eRofDptNotFound : public eRofDptBase {
public:
  eRofDptNotFound(const std::string &__arg) : eRofDptBase(__arg){};
};

class crofdpt;

/**
 * @interface crofdpt_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofdpt instance.
 *
 * This class defines the interface to the environment required
 * by an instance of class rofl::crofdpt. Its API comprises two
 * groups of methods:
 *
 * 1. Methods for receiving OpenFlow control channel and
 * connections related notifications.
 * 2. Methods for receiving OpenFlow messages, once the control
 * channel has been established.
 *
 * Overwrite any of these methods for receiving certain event
 * notifications from the associated rofl::crofdpt instance.
 */
class crofdpt_env {
  friend class crofdpt;
  static std::set<crofdpt_env *> rofdpt_envs;
  static crwlock rofdpt_envs_lock;

public:
  static crofdpt_env &call_env(crofdpt_env *env) {
    AcquireReadLock lock(crofdpt_env::rofdpt_envs_lock);
    if (crofdpt_env::rofdpt_envs.find(env) == crofdpt_env::rofdpt_envs.end()) {
      throw eRofDptNotFound(
          "crofdpt_env::call_env() crofdpt_env instance not found");
    }
    return *(env);
  };

public:
  virtual ~crofdpt_env() {
    AcquireReadWriteLock lock(crofdpt_env::rofdpt_envs_lock);
    crofdpt_env::rofdpt_envs.erase(this);
  };
  crofdpt_env() {
    AcquireReadWriteLock lock(crofdpt_env::rofdpt_envs_lock);
    crofdpt_env::rofdpt_envs.insert(this);
  };

protected:
  /**
   * @name 	Event handlers for management notifications for controller
   * entities
   *
   * Overwrite any of these methods for receiving controller related event
   * notifications.
   */

  /**@{*/

  /**
   * @brief	Called after establishing the associated OpenFlow control
   * channel.
   *
   * This method is called once the associated OpenFlow control channel has
   * been established, i.e., its main connection has been accepted by the remote
   * site.
   *
   * @param dpt datapath instance
   * @param ofp_version openflow version negotiated for control channel
   */
  virtual void handle_established(crofdpt &dpt, uint8_t ofp_version){};

  /**
   * @brief 	Called when the control channel has been closed by the peer
   * entity.
   *
   * @param dpt datapath instance
   */
  virtual void handle_closed(crofdpt &dpt){};

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * established.
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   * @param ofp_version openflow version negotiated for control connection
   */
  virtual void handle_established(crofdpt &dpt, crofconn &conn,
                                  uint8_t ofp_version){};

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * closed by the peer entity.
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   */
  virtual void handle_closed(crofdpt &dpt, crofconn &conn){};

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * been
   * refused.
   *
   * This event occurs when the C-library's connect() system call fails
   * with the ECONNREFUSED error code. This indicates typically a problem on
   * the remote site.
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   */
  virtual void handle_connect_refused(crofdpt &dpt, crofconn &conn){};

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * failed.
   *
   * This event occurs when some failure occurs while calling the underlying
   * C-library connect() system call, e.g., no route to destination, etc. This
   * may
   * indicate a local configuration problem inside or outside of the
   * application.
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   */
  virtual void handle_connect_failed(crofdpt &dpt, crofconn &conn){};

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * failed.
   *
   * This event occurs when some failure occurs while negotiating the underlying
   * transport (e.g., TLS).
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   */
  virtual void handle_accept_failed(crofdpt &dpt, crofconn &conn){};

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * failed.
   *
   * This event occurs when the openflow negotiation failed, e.g., no acceptable
   * protocol version was presented by the peer.
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   */
  virtual void handle_negotiation_failed(crofdpt &dpt, crofconn &conn){};

  /**
   * @brief	Called when a congestion situation on the control connection is
   * occurring
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   */
  virtual void congestion_occurred_indication(crofdpt &dpt, crofconn &conn){};

  /**
   * @brief	Called when a congestion situation on the control connection has
   * been solved.
   *
   * A control channel may face congestion situations when insufficient
   * bandwidth
   * on the underlying IP path or some backpressuring by the remote site
   * requires
   * to throttle the overall message transmission rate on a control connection.
   * A congestion situation is indicated by the return values obtained from the
   * various
   * send-methods defined within rofl::crofdpt.
   *
   * @param dpt datapath instance
   * @param conn control connection instance
   */
  virtual void congestion_solved_indication(crofdpt &dpt, crofconn &conn){};

  /**@}*/

protected:
  /**
   * @name 	Event handlers for OpenFlow message notifications received from
   * datapath elements
   *
   * Overwrite any of these methods for receiving messages from the attached
   * datapath element.
   * Once this method terminates, rofl will destroy the message object. If you
   * want to save
   * a message or parts of it, you must create a local copy. All messages are
   * internally
   * allocated on the heap and both allocation and deallocation is handled by
   * rofl-common.
   * Do not delete any of these messages or its internals.
   */

  /**@{*/

  /**
   * @brief	OpenFlow Features-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_features_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                        rofl::openflow::cofmsg_features_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Features-Reply message.
   *
   * No Features-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_features_reply_timeout(rofl::crofdpt &dpt,
                                             uint32_t xid){};

  /**
   * @brief	OpenFlow Get-Config-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_get_config_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg_get_config_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Get-Config-Reply
   * message.
   *
   * No Get-Config-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_get_config_reply_timeout(rofl::crofdpt &dpt,
                                               uint32_t xid){};

  /**
   * @brief	OpenFlow Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                                  rofl::openflow::cofmsg_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Stats-Reply message.
   *
   * No Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   * @param stats_type statistics message subtype
   */
  virtual void handle_stats_reply_timeout(rofl::crofdpt &dpt, uint32_t xid,
                                          uint8_t stats_type){};

  /**
   * @brief	OpenFlow Desc-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_desc_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg_desc_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Desc-Stats-Reply
   * message.
   *
   * No Desc-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_desc_stats_reply_timeout(rofl::crofdpt &dpt,
                                               uint32_t xid){};

  /**
   * @brief	OpenFlow Table-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_table_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                           rofl::openflow::cofmsg_table_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply
   * message.
   *
   * No Table-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_table_stats_reply_timeout(rofl::crofdpt &dpt,
                                                uint32_t xid){};

  /**
   * @brief	OpenFlow Port-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_port_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg_port_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Port-Stats-Reply
   * message.
   *
   * No Port-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_port_stats_reply_timeout(rofl::crofdpt &dpt,
                                               uint32_t xid){};

  /**
   * @brief	OpenFlow Flow-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_flow_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg_flow_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Flow-Stats-Reply
   * message.
   *
   * No Flow-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_flow_stats_reply_timeout(rofl::crofdpt &dpt,
                                               uint32_t xid){};

  /**
   * @brief	OpenFlow Aggregate-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_aggregate_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                               rofl::openflow::cofmsg_aggr_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Aggregate-Stats-Reply
   * message.
   *
   * No Aggregate-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_aggregate_stats_reply_timeout(rofl::crofdpt &dpt,
                                                    uint32_t xid){};

  /**
   * @brief	OpenFlow Queue-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_queue_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                           rofl::openflow::cofmsg_queue_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Queue-Stats-Reply
   * message.
   *
   * No Queue-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_queue_stats_reply_timeout(rofl::crofdpt &dpt,
                                                uint32_t xid){};

  /**
   * @brief	OpenFlow Group-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_group_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                           rofl::openflow::cofmsg_group_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Group-Stats-Reply
   * message.
   *
   * No Group-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_group_stats_reply_timeout(rofl::crofdpt &dpt,
                                                uint32_t xid){};

  /**
   * @brief	OpenFlow Group-Desc-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_group_desc_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_group_desc_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Group-Desc-Stats-Reply
   * message.
   *
   * No Group-Desc-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_group_desc_stats_reply_timeout(rofl::crofdpt &dpt,
                                                     uint32_t xid){};

  /**
   * @brief	OpenFlow Group-Features-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_group_features_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_group_features_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow
   * Group-Features-Stats-Reply message.
   *
   * No Group-Features-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_group_features_stats_reply_timeout(rofl::crofdpt &dpt,
                                                         uint32_t xid){};

  /**
   * @brief	OpenFlow Meter-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_meter_stats_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                           rofl::openflow::cofmsg_meter_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Meter-Stats-Reply
   * message.
   *
   * No Meter-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_meter_stats_reply_timeout(rofl::crofdpt &dpt,
                                                uint32_t xid){};

  /**
   * @brief	OpenFlow Meter-Config-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_meter_config_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_meter_config_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow
   * Meter-Config-Stats-Reply
   * message.
   *
   * No Meter-Config-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_meter_config_stats_reply_timeout(rofl::crofdpt &dpt,
                                                       uint32_t xid){};

  /**
   * @brief	OpenFlow Meter-Features-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_meter_features_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_meter_features_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow
   * Meter-Features-Stats-Reply message.
   *
   * No Meter-Features-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_meter_features_stats_reply_timeout(rofl::crofdpt &dpt,
                                                         uint32_t xid){};

  /**
   * @brief	OpenFlow Table-Features-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_table_features_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_table_features_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow
   * Table-Features-Stats-Reply message.
   *
   * No Table-Features-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_table_features_stats_reply_timeout(rofl::crofdpt &dpt,
                                                         uint32_t xid){};

  /**
   * @brief	OpenFlow Port-Desc-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_port_desc_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_port_desc_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Port-Desc-Stats-Reply
   * message.
   *
   * No Port-Desc-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_port_desc_stats_reply_timeout(rofl::crofdpt &dpt,
                                                    uint32_t xid){};

  /**
   * @brief	OpenFlow Experimenter-Stats-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_experimenter_stats_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_experimenter_stats_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow
   * Experimenter-Stats-Reply
   * message.
   *
   * No Experimenter-Stats-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_experimenter_stats_reply_timeout(rofl::crofdpt &dpt,
                                                       uint32_t xid){};

  /**
   * @brief	OpenFlow Packet-In message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_packet_in(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                                rofl::openflow::cofmsg_packet_in &msg){};

  /**
   * @brief	OpenFlow Barrier-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_barrier_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                       rofl::openflow::cofmsg_barrier_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Barrier-Reply message.
   *
   * No Barrier-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_barrier_reply_timeout(rofl::crofdpt &dpt, uint32_t xid){};

  /**
   * @brief	OpenFlow Flow-Removed message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_flow_removed(rofl::crofdpt &dpt,
                                   const rofl::cauxid &auxid,
                                   rofl::openflow::cofmsg_flow_removed &msg){};

  /**
   * @brief	OpenFlow Port-Status-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_port_status(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                                  rofl::openflow::cofmsg_port_status &msg){};

  /**
   * @brief	OpenFlow Queue-Get-Config-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_queue_get_config_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_queue_get_config_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply
   * message.
   *
   * No Table-Stats-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_queue_get_config_reply_timeout(rofl::crofdpt &dpt,
                                                     uint32_t xid){};

  /**
   * @brief	OpenFlow Error message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_error_message(rofl::crofdpt &dpt,
                                    const rofl::cauxid &auxid,
                                    rofl::openflow::cofmsg_error &msg){};

  /**
   * @brief	OpenFlow Experimenter message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_experimenter_message(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                              rofl::openflow::cofmsg_experimenter &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Experimenter message.
   *
   * No Experimenter message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_experimenter_timeout(rofl::crofdpt &dpt, uint32_t xid){};

  /**
   * @brief	OpenFlow Role-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_role_reply(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                                 rofl::openflow::cofmsg_role_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Role-Reply message.
   *
   * No Role-Reply message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_role_reply_timeout(rofl::crofdpt &dpt, uint32_t xid){};

  /**
   * @brief	OpenFlow Get-Async-Config-Reply message received.
   *
   * @param dpt datapath instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_get_async_config_reply(
      rofl::crofdpt &dpt, const rofl::cauxid &auxid,
      rofl::openflow::cofmsg_get_async_config_reply &msg){};

  /**
   * @brief	Timer expired while waiting for OpenFlow Get-Async-Config-Reply
   * message.
   *
   * No Get-Async-Config-Reply message was received in the specified time
   * interval
   * for the given OpenFlow transaction identifier.
   *
   * @param dpt datapath instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_get_async_config_reply_timeout(rofl::crofdpt &dpt,
                                                     uint32_t xid){};

  /**@}*/
};

/**
 * @ingroup common_devel_workflow
 * @brief	Class representing a remote datapath element
 *
 * This class encapsulates properties of a single remote datapath element
 * including the OpenFlow control channel, its lists of ports and tables,
 * and general configuration. Its public API offers methods to manage
 * the OpenFlow control channel, i.e. CRUD methods for individual control
 * connections. Once the OpenFlow control channel has been established,
 * rofl::crofdpt will query the datapath element for its internal configuration
 * (ports, tables and general configuration) and exposes these data to any
 * higher logic. Note that no periodic updates on these data is done by
 * this instance and re-querying the datapath must be initiated by higher
 * logic.
 *
 * rofl::crofdpt expects an instance of class rofl::crofdpt_env as surrounding
 * environment and sends various notifications via this interface. Class
 * rofl::crofbase implements this interface and may be used as base class
 * for advanced applications. However, you may implement rofl::crofdpt_env
 * directly as well.
 *
 */
class crofdpt : public rofl::crofchan_env {

  enum crofdpt_state_t {
    STATE_RUNNING,
    STATE_DELETE_IN_PROGRESS,
  };

public:
  /**
   * @brief	crofdpt destructor
   *
   * Closes all control connections and does a general clean-up.
   */
  virtual ~crofdpt();

  /**
   * @brief 	crofdpt constructor
   *
   * @param env pointer to rofl::crofdpt_env instance defining the environment
   * for this object
   * @param dptid rofl-common's internal identifier for this instance
   */
  crofdpt(rofl::crofdpt_env *env, const rofl::cdptid &dptid);

public:
  /**
   * @brief	Reset environment.
   */
  crofdpt &set_env(crofdpt_env *env) {
    this->env = env;
    return *this;
  };

  /**
   * @brief	Returns rofl-common's internal rofl::cdptid identifier for this
   * instance
   *
   * @return internal datapath element identifier (not DPID)
   */
  const rofl::cdptid &get_dptid() const { return dptid; };

public:
  /**
   * @name	Methods granting access to the datapath element's basic
   * properties
   */

  /**@{*/

  /**
   * @brief	Returns true when snooping is enabled
   */
  bool get_snooping() const { return snoop; };

  /**
   * @brief	Enables/disables snooping
   */
  crofdpt &set_snooping(bool snoop) {
    this->snoop = snoop;
    return *this;
  };

  /**
   * @brief	Returns OpenFlow datapath identifier for this instance
   *
   * @return OpenFlow datapath identifier
   */
  const rofl::cdpid &get_dpid() const { return dpid; };

  /**
   *
   */
  uint32_t get_n_buffers() const { return n_buffers; };

  /**
   *
   */
  uint8_t get_n_tables() const { return n_tables; };

  /**
   *
   */
  uint32_t get_capabilities() const { return capabilities; };

  /**
   *
   */
  const caddress_ll &get_hwaddr() const { return hwaddr; };

  /**
   *
   */
  uint16_t get_miss_send_len() const { return miss_send_len; };

  /**
   *
   */
  uint16_t get_flags() const { return flags; };

  /**
   *
   */
  const rofl::openflow::cofports &get_ports() const { return ports; };

  /**
   *
   */
  const rofl::openflow::coftables &get_tables() const { return tables; };

  /**
   *
   */
  const rofl::openflow::cofgroup_features_stats_reply &get_groups() const {
    return groups;
  };

  /**@}*/

public:
  /**
   * @name	Methods for purging and resetting a datapath element
   */

  /**@{*/

  /**
   * @brief	Removes all flow-table entries from the attached datapath
   * element.
   *
   * Sends a Flow-Mod-Delete message to the attached datapath element for
   * removing
   * all flow-table entries.
   */
  void flow_mod_reset();

  /**
   * @brief	Removes all group-table entries from the attached datapath
   * element.
   *
   * Sends a Group-Mod-Delete message to the attached datapath element for
   * removing
   * all group-table entries.
   */
  void group_mod_reset();

  /**
   * @brief	Drops packet identified by buffer-id from the attached datapath
   * element.
   *
   * Drops a packet stored on the datapath element identified by the given
   * buffer-id
   * by sending a Packet-Out message with an empty action list.
   *
   * @param auxid control connection identifier
   * @param buffer_id identifier of the packet slot to be removed
   * @param inport field of the Packet-Out message sent to the datapath element
   */
  void drop_buffer(const rofl::cauxid &auxid, uint32_t buffer_id,
                   uint32_t inport = rofl::openflow::OFPP_CONTROLLER) {
    rofl::openflow::cofactions actions(get_version());
    send_packet_out_message(auxid, buffer_id, inport, actions, NULL, 0);
  };

  /**@}*/

public:
  /**
   *
   */
  bool is_established() const { return rofchan.is_established(); };

  /**
   *
   */
  uint8_t get_version() const { return rofchan.get_version(); };

public:
  /**
   *
   */
  size_t size() const { return rofchan.size(); };

  /**
   *
   */
  std::list<cauxid> get_conn_ids() const { return rofchan.get_conn_ids(); };

  /**
   *
   */
  std::list<cauxid> keys() const { return rofchan.keys(); };

  /**
   *
   */
  void clear() { rofchan.clear(); };

  /**
   *
   */
  crofconn &add_conn() { return rofchan.add_conn(); };

  /**
   *
   */
  crofconn &add_conn(const cauxid &auxid) { return rofchan.add_conn(auxid); };

  /**
   *
   */
  crofconn &add_conn(crofconn *conn) {
    if (nullptr != conn) {
      dpid = cdpid(conn->get_dpid());
    }
    return rofchan.add_conn(conn);
  };

  /**
   *
   */
  crofconn &set_conn(const cauxid &auxid) { return rofchan.set_conn(auxid); };

  /**
   *
   */
  const crofconn &get_conn(const cauxid &auxid) const {
    return rofchan.get_conn(auxid);
  };

  /**
   *
   */
  bool drop_conn(const cauxid &auxid) { return rofchan.drop_conn(auxid); };

  /**
   *
   */
  bool has_conn(const cauxid &auxid) const { return rofchan.has_conn(auxid); };

public:
  /**
   * @name	Methods for sending OpenFlow messages
   *
   * These methods may be called by a derived class for sending
   * a specific OpenFlow message.
   */

  /**@{*/

  /**
   * @brief	Sends OpenFlow Features-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_features_request(const rofl::cauxid &auxid,
                        int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                        uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Get-Config-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_get_config_request(const rofl::cauxid &auxid,
                          int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                          uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Table-Features-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_table_features_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Desc-Stats-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_desc_stats_request(const rofl::cauxid &auxid, uint16_t stats_flags,
                          int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                          uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Flow-Stats-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_flow_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      const rofl::openflow::cofflow_stats_request &flow_stats_request,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Aggregate-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param aggr_stats_request OpenFlow Aggregate-Stats-Request payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_aggr_stats_request(
      const rofl::cauxid &auxid, uint16_t flags,
      const rofl::openflow::cofaggr_stats_request &aggr_stats_request,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Table-Stats-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_table_stats_request(const rofl::cauxid &auxid, uint16_t stats_flags = 0,
                           int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                           uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Port-Stats-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param port_stats_request OpenFlow Port-Stats-Request payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_port_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      const rofl::openflow::cofport_stats_request &port_stats_request,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Queue-Stats-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param queue_stats_request OpenFlow Queue-Stats-Request payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_queue_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      const rofl::openflow::cofqueue_stats_request &queue_stats_request,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Group-Stats-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param group_stats_request OpenFlow Group-Stats-Request payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_group_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      const rofl::openflow::cofgroup_stats_request &group_stats_request,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Group-Desc-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_group_desc_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags = 0,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Group-Features-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_group_features_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Port-Desc-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_port_desc_stats_request(const rofl::cauxid &auxid, uint16_t stats_flags,
                               int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                               uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Experimenter-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param exp_id OpenFlow experimenter identifier
   * @param exp_type OpenFlow experimenter type
   * @param body experimenter statistics payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_experimenter_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags, uint32_t exp_id,
      uint32_t exp_type, const cmemory &body,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Meter-Stats-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param meter_stats_request OpenFlow Meter-Stats-Request payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_meter_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      const rofl::openflow::cofmeter_stats_request &meter_stats_request,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Meter-Config-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param meter_config_stats_request OpenFlow Meter-Config-Stats-Request
   * payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_meter_config_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      const rofl::openflow::cofmeter_config_request &meter_config_stats_request,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Meter-Features-Stats-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param stats_flags OpenFlow statistics flags
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_meter_features_stats_request(
      const rofl::cauxid &auxid, uint16_t stats_flags,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Packet-Out message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param buffer_id OpenFlow packet buffer identifier
   * @param in_port incoming port for OpenFlow matches
   * @param actions OpenFlow actions list
   * @param data start of packet frame
   * @param datalen length of packet frame
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_packet_out_message(
      const rofl::cauxid &auxid, uint32_t buffer_id, uint32_t in_port,
      const rofl::openflow::cofactions &actions, uint8_t *data = NULL,
      size_t datalen = 0, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Barrier-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_barrier_request(const rofl::cauxid &auxid,
                       int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                       uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Role-Request message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param role OpenFlow role definition
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_role_request(
      const rofl::cauxid &auxid, const rofl::openflow::cofrole &role,
      int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Flow-Mod message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param flowmod OpenFlow flow mod entry
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_flow_mod_message(const rofl::cauxid &auxid,
                        const rofl::openflow::cofflowmod &flowmod,
                        uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Group-Mod message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param groupmod OpenFlow group mod entry
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_group_mod_message(const rofl::cauxid &auxid,
                         const rofl::openflow::cofgroupmod &groupmod,
                         uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Table-Mod message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param table_id OpenFlow table identifier
   * @param config OpenFlow table configuration
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_table_mod_message(const rofl::cauxid &auxid,
                                                      uint8_t table_id,
                                                      uint32_t config,
                                                      uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Port-Mod message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param portno OpenFlow port number
   * @param hwaddr hardware address assigned to this port
   * @param config OpenFlow port configuration
   * @param mask OpenFlow port mask
   * @param OpenFlow port advertise field
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_port_mod_message(const rofl::cauxid &auxid, uint32_t port_no,
                        const rofl::caddress_ll &hwaddr, uint32_t config,
                        uint32_t mask, uint32_t advertise,
                        uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Set-Config message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param flags OpenFlow datapath element's flags
   * @param miss_send_len OpenFlow miss_send_len parameter
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_set_config_message(const rofl::cauxid &auxid, uint16_t flags,
                          uint16_t miss_send_len, uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Queue-Get-Config-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param portno OpenFlow port number
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_queue_get_config_request(const rofl::cauxid &auxid, uint32_t port,
                                int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                                uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Get-Async-Config-Request message to attached
   * datapath element.
   *
   * @param auxid controller connection identifier
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_get_async_config_request(const rofl::cauxid &auxid,
                                int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT,
                                uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Set-Async-Config message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param async_config OpenFlow async_config payload
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_set_async_config_message(
      const rofl::cauxid &auxid,
      const rofl::openflow::cofasync_config &async_config,
      uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Meter-Mod message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param command OpenFlow Meter-Mod command
   * @param flags OpenFlow Meter-Mod flags
   * @param meter_id OpenFlow Meter-Mod identifier
   * @param meter_bands OpenFlow meterbands
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_meter_mod_message(const rofl::cauxid &auxid, uint16_t command,
                         uint16_t flags, uint32_t meter_id,
                         const rofl::openflow::cofmeter_bands &meter_bands,
                         uint32_t *xid = nullptr);

  /**
   * @brief	Sends OpenFlow Error message to attached datapath element.
   *
   * @param auxid controller connection identifier
   * @param xid OpenFlow transaction identifier
   * @param type OpenFlow error type
   * @param code OpenFlow error code
   * @param data start of error message payload
   * @param datalen length of error message payload
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_error_message(const rofl::cauxid &auxid, uint32_t xid, uint16_t type,
                     uint16_t code, uint8_t *data = NULL, size_t datalen = 0);

  /**
   * @brief	Sends OpenFlow Experimenter message to attached datapath
   * element.
   *
   * @param auxid controller connection identifier
   * @param exp_id OpenFlow experimenter identifier
   * @param exp_type OpenFlow experimenter type
   * @param body start of experimenter message payload
   * @param bodylen length of experimenter message payload
   * @param timeout until this request expires
   * @return OpenFlow transaction ID assigned to this request
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t
  send_experimenter_message(const rofl::cauxid &auxid, uint32_t xid,
                            uint32_t exp_id, uint32_t exp_type,
                            uint8_t *body = NULL, size_t bodylen = 0,
                            int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT);

  /**@}*/

public:
  /**
   * @brief 	Predicate for finding a rofl::crofdpt instance by its
   * rofl::cdptid.
   */
  class crofdpt_find_by_dptid {
    cdptid dptid;

  public:
    crofdpt_find_by_dptid(const rofl::cdptid &dptid) : dptid(dptid){};
    bool operator()(const crofdpt *rofdpt) {
      return (rofdpt->get_dptid() == dptid);
    };
  };

  /**
   * @brief 	Predicate for finding a rofl::crofdpt instance by its
   * rofl::cdpid.
   */
  class crofdpt_find_by_dpid {
    cdpid dpid;

  public:
    crofdpt_find_by_dpid(const cdpid &dpid) : dpid(dpid){};
    bool operator()(const std::pair<cdptid, crofdpt *> &p) {
      return (p.second->get_dpid() == dpid);
    };
  };

public:
  friend std::ostream &operator<<(std::ostream &os, const crofdpt &dpt) {
    os << "<crofdpt >";
    { os << dpt.get_dptid(); }
    { os << dpt.rofchan; }
    return os;
  };

  /**
   * @brief	Returns a one-liner string with basic information about this
   * instance.
   */
  std::string str() const {
    std::stringstream ss;
    ss << "dpid: " << get_dpid() << " ";
    return ss.str();
  };

private:
  virtual void handle_established(crofchan &chan, uint8_t ofp_version) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_established(*this, ofp_version);
  };

  virtual void handle_closed(crofchan &chan) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_closed(*this);
  };

  virtual void handle_established(crofchan &chan, crofconn &conn,
                                  uint8_t ofp_version) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_established(*this, conn, ofp_version);
  };

  virtual void handle_closed(crofchan &chan, crofconn &conn) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_closed(*this, conn);
  };

  virtual void handle_connect_refused(crofchan &chan, crofconn &conn) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_connect_refused(*this, conn);
  };

  virtual void handle_connect_failed(crofchan &chan, crofconn &conn) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_connect_failed(*this, conn);
  };

  virtual void handle_accept_failed(crofchan &chan, crofconn &conn) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_accept_failed(*this, conn);
  };

  virtual void handle_negotiation_failed(crofchan &chan, crofconn &conn) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).handle_negotiation_failed(*this, conn);
  };

  virtual void congestion_occurred_indication(crofchan &chan, crofconn &conn) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).congestion_occurred_indication(*this, conn);
  };

  virtual void congestion_solved_indication(crofchan &chan, crofconn &conn) {
    if (delete_in_progress())
      return;
    crofdpt_env::call_env(env).congestion_solved_indication(*this, conn);
  };

  virtual void handle_transaction_timeout(crofchan &chan, crofconn &conn,
                                          uint32_t xid, uint8_t type,
                                          uint16_t sub_type);

private:
  virtual void handle_recv(rofl::crofchan &chan, rofl::crofconn &conn,
                           rofl::openflow::cofmsg *msg);

private:
  void experimenter_rcvd(const rofl::cauxid &auxid,
                         rofl::openflow::cofmsg *msg);

  void error_rcvd(const rofl::cauxid &auxid, rofl::openflow::cofmsg *msg);

  void features_reply_rcvd(const rofl::cauxid &auxid,
                           rofl::openflow::cofmsg *msg);

  void get_config_reply_rcvd(const rofl::cauxid &auxid,
                             rofl::openflow::cofmsg *msg);

  void multipart_reply_rcvd(const rofl::cauxid &auxid,
                            rofl::openflow::cofmsg *msg);

  void desc_stats_reply_rcvd(const rofl::cauxid &auxid,
                             rofl::openflow::cofmsg *msg);

  void table_stats_reply_rcvd(const rofl::cauxid &auxid,
                              rofl::openflow::cofmsg *msg);

  void port_stats_reply_rcvd(const rofl::cauxid &auxid,
                             rofl::openflow::cofmsg *msg);

  void flow_stats_reply_rcvd(const rofl::cauxid &auxid,
                             rofl::openflow::cofmsg *msg);

  void aggregate_stats_reply_rcvd(const rofl::cauxid &auxid,
                                  rofl::openflow::cofmsg *msg);

  void queue_stats_reply_rcvd(const rofl::cauxid &auxid,
                              rofl::openflow::cofmsg *msg);

  void group_stats_reply_rcvd(const rofl::cauxid &auxid,
                              rofl::openflow::cofmsg *msg);

  void group_desc_stats_reply_rcvd(const rofl::cauxid &auxid,
                                   rofl::openflow::cofmsg *msg);

  void group_features_stats_reply_rcvd(const rofl::cauxid &auxid,
                                       rofl::openflow::cofmsg *msg);

  void meter_stats_reply_rcvd(const rofl::cauxid &auxid,
                              rofl::openflow::cofmsg *msg);

  void meter_config_stats_reply_rcvd(const rofl::cauxid &auxid,
                                     rofl::openflow::cofmsg *msg);

  void meter_features_stats_reply_rcvd(const rofl::cauxid &auxid,
                                       rofl::openflow::cofmsg *msg);

  void table_features_stats_reply_rcvd(const rofl::cauxid &auxid,
                                       rofl::openflow::cofmsg *msg);

  void port_desc_stats_reply_rcvd(const rofl::cauxid &auxid,
                                  rofl::openflow::cofmsg *msg);

  void experimenter_stats_reply_rcvd(const rofl::cauxid &auxid,
                                     rofl::openflow::cofmsg *msg);

  void barrier_reply_rcvd(const rofl::cauxid &auxid,
                          rofl::openflow::cofmsg *msg);

  void port_mod_sent(rofl::openflow::cofmsg *pack);

  void packet_in_rcvd(const rofl::cauxid &auxid, rofl::openflow::cofmsg *msg);

  void flow_removed_rcvd(const rofl::cauxid &auxid,
                         rofl::openflow::cofmsg *msg);

  void port_status_rcvd(const rofl::cauxid &auxid, rofl::openflow::cofmsg *msg);

  void role_reply_rcvd(const rofl::cauxid &auxid, rofl::openflow::cofmsg *msg);

  void queue_get_config_reply_rcvd(const rofl::cauxid &auxid,
                                   rofl::openflow::cofmsg *msg);

  void get_async_config_reply_rcvd(const rofl::cauxid &auxid,
                                   rofl::openflow::cofmsg *msg);

  bool delete_in_progress() const {
    return (STATE_DELETE_IN_PROGRESS == state);
  };

private:
  // environment
  rofl::crofdpt_env *env;

  // state
  std::atomic<crofdpt_state_t> state;

  // handle for this crofdpt instance
  rofl::cdptid dptid;

  // enable snooping of ports, tables, ...
  bool snoop;

  // OFP control channel
  rofl::crofchan rofchan;

  // random numbers generator
  rofl::crandom random;

  // last xid sent
  std::atomic_uint_fast32_t xid_last;

  // default request timeout
  static const time_t DEFAULT_REQUEST_TIMEOUT = 0; // seconds (0 : no timeout)

  // datapath identifier
  rofl::cdpid dpid;

  // num of buffers
  uint32_t n_buffers;

  // num of tables
  uint8_t n_tables;

  // capabilities
  uint32_t capabilities;

  // link layer address
  rofl::caddress_ll hwaddr;

  // config miss_send_len
  uint16_t miss_send_len;

  // config flags
  uint16_t flags;

  // ports
  rofl::openflow::cofports ports;

  // tables
  rofl::openflow::coftables tables;

  // groups
  rofl::openflow::cofgroup_features_stats_reply groups;
};

}; // end of namespace

#endif
