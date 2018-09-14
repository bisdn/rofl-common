/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofctl.h
 *
 *  Created on: 07.01.2014
 *  Revised on: 30.09.2015
 *      Author: andreas
 */

#ifndef CROFCTL_H
#define CROFCTL_H 1

#include <bitset>
#include <map>
#include <string>

#include "rofl/common/cauxid.h"
#include "rofl/common/cctlid.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/crofchan.h"
#include "rofl/common/exception.hpp"
#include "rofl/common/locking.hpp"

#include "rofl/common/crandom.h"

#include "rofl/common/openflow/cofaggrstats.h"
#include "rofl/common/openflow/cofasyncconfig.h"
#include "rofl/common/openflow/cofdescstats.h"
#include "rofl/common/openflow/cofflowstatsarray.h"
#include "rofl/common/openflow/cofgroupdescstatsarray.h"
#include "rofl/common/openflow/cofgroupfeaturesstats.h"
#include "rofl/common/openflow/cofgroupstatsarray.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/openflow/cofmatch.h"
#include "rofl/common/openflow/cofmeterconfigarray.h"
#include "rofl/common/openflow/cofmeterfeatures.h"
#include "rofl/common/openflow/cofmeterstatsarray.h"
#include "rofl/common/openflow/cofpacketqueues.h"
#include "rofl/common/openflow/cofport.h"
#include "rofl/common/openflow/cofports.h"
#include "rofl/common/openflow/cofportstatsarray.h"
#include "rofl/common/openflow/cofqueuestatsarray.h"
#include "rofl/common/openflow/cofrole.h"
#include "rofl/common/openflow/coftables.h"
#include "rofl/common/openflow/coftablestatsarray.h"
#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {

class eRofCtlBase : public exception {
public:
  eRofCtlBase(const std::string &__arg) : exception(__arg){};
};
class eRofCtlNotFound : public eRofCtlBase {
public:
  eRofCtlNotFound(const std::string &__arg) : eRofCtlBase(__arg){};
};

class crofctl;

/**
 * @interface crofctl_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofctl instance.
 *
 * This class defines the interface to the environment required
 * by an instance of class rofl::crofctl. Its API comprises two
 * groups of methods:
 *
 * 1. Methods for receiving OpenFlow control channel and
 * connections related notifications.
 * 2. Methods for receiving OpenFlow messages, once the control
 * channel has been established.
 *
 * Overwrite any of these methods for receiving certain event
 * notifications from the associated rofl::crofctl instance.
 */
class crofctl_env {
  friend class crofctl;
  static std::set<crofctl_env *> rofctl_envs;
  static crwlock rofctl_envs_lock;

public:
  static crofctl_env &call_env(crofctl_env *env) {
    AcquireReadLock lock(crofctl_env::rofctl_envs_lock);
    if (crofctl_env::rofctl_envs.find(env) == crofctl_env::rofctl_envs.end()) {
      throw eRofCtlNotFound(
          "crofctl_env::call_env() crofctl_env instance not found");
    }
    return *(env);
  };

public:
  virtual ~crofctl_env() {
    AcquireReadWriteLock lock(crofctl_env::rofctl_envs_lock);
    crofctl_env::rofctl_envs.erase(this);
  };
  crofctl_env() {
    AcquireReadWriteLock lock(crofctl_env::rofctl_envs_lock);
    crofctl_env::rofctl_envs.insert(this);
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
   * @param ctl controller instance
   * @param ofp_version openflow version negotiated for control channel
   */
  virtual void handle_established(__attribute__((unused)) crofctl &ctl,
                                  __attribute__((unused)) uint8_t ofp_version) {
  }

  /**
   * @brief 	Called when the control channel has been closed by the peer
   * entity.
   *
   * @param ctl controller instance
   */
  virtual void handle_closed(__attribute__((unused)) crofctl &ctl) {}

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * established.
   *
   * @param ctl controller instance
   * @param conn control connection instance
   * @param ofp_version openflow version negotiated for control connection
   */
  virtual void handle_established(__attribute__((unused)) crofctl &ctl,
                                  __attribute__((unused)) crofconn &conn,
                                  __attribute__((unused)) uint8_t ofp_version) {
  }

  /**
   * @brief 	Called when a control connection (main or auxiliary) has been
   * closed by the peer entity.
   *
   * @param ctl controller instance
   * @param conn control connection instance
   */
  virtual void handle_closed(__attribute__((unused)) crofctl &ctl,
                             __attribute__((unused)) crofconn &conn) {}

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * been
   * refused.
   *
   * This event occurs when the C-library's connect() system call fails
   * with the ECONNREFUSED error code. This indicates typically a problem on
   * the remote site.
   *
   * @param ctl controller instance
   * @param conn control connection instance
   */
  virtual void handle_connect_refused(__attribute__((unused)) crofctl &ctl,
                                      __attribute__((unused)) crofconn &conn) {}

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
   * @param ctl controller instance
   * @param conn control connection instance
   */
  virtual void handle_connect_failed(__attribute__((unused)) crofctl &ctl,
                                     __attribute__((unused)) crofconn &conn) {}

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * failed.
   *
   * This event occurs when some failure occurs while negotiating the underlying
   * transport (e.g., TLS).
   *
   * @param ctl controller instance
   * @param conn control connection instance
   */
  virtual void handle_accept_failed(__attribute__((unused)) crofctl &ctl,
                                    __attribute__((unused)) crofconn &conn) {}

  /**
   * @brief 	Called when an attempt to establish a control connection has
   * failed.
   *
   * This event occurs when the openflow negotiation failed, e.g., no acceptable
   * protocol version was presented by the peer.
   *
   * @param ctl controller instance
   * @param conn control connection instance
   */
  virtual void handle_negotiation_failed(__attribute__((unused)) crofctl &ctl,
                                         __attribute__((unused))
                                         crofconn &conn) {}

  /**
   * @brief	Called when a congestion situation on the control connection is
   * occurring
   *
   * @param ctl controller instance
   * @param conn control connection instance
   */
  virtual void congestion_occurred_indication(__attribute__((unused))
                                              crofctl &ctl,
                                              __attribute__((unused))
                                              crofconn &conn) {}

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
   * send-methods defined within rofl::crofctl.
   *
   * @param ctl controller instance
   * @param conn control connection instance
   */
  virtual void congestion_solved_indication(__attribute__((unused))
                                            crofctl &ctl,
                                            __attribute__((unused))
                                            crofconn &conn) {}

  /**@}*/

  /**
   * @brief	called once a cofctl instance has received a role request
   *
   *
   */
  virtual void role_request_rcvd(__attribute__((unused)) rofl::crofctl &ctl,
                                 __attribute__((unused)) uint32_t role,
                                 __attribute__((unused))
                                 uint64_t rcvd_generation_id) {}

protected:
  /**
   * @name 	Event handlers for OpenFlow message notifications received from
   * controller entities
   *
   * Overwrite any of these methods for receiving messages from the attached
   * controller entity.
   * Once this method terminates, rofl will destroy the message object. If you
   * want to save
   * a message or parts of it, you must create a local copy. All messages are
   * internally
   * allocated on the heap and both allocation and deallocation is handled by
   * rofl-common.
   * Do not delete any of these messages or its internals.
   *
   * Higher logic implemented on top of crofbase and acting in datapath role
   * should handle
   * incoming requests, e.g., sending back a reply to the controller entity.
   * rofl-common does
   * not generate reply messages automatically. You may throw any of the
   * OpenFlow related exceptions
   * defined by rofl-common within these handler methods. rofl-common will
   * detect such exceptions
   * and generate appropriate error messages for transmission to the peer
   * entity.
   */

  /**@{*/

  /**
   * @brief	OpenFlow Features-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_features_request(__attribute__((unused)) rofl::crofctl &ctl,
                          __attribute__((unused)) const rofl::cauxid &auxid,
                          __attribute__((unused))
                          rofl::openflow::cofmsg_features_request &msg) {}

  /**
   * @brief	OpenFlow Get-Config-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_get_config_request(__attribute__((unused)) rofl::crofctl &ctl,
                            __attribute__((unused)) const rofl::cauxid &auxid,
                            __attribute__((unused))
                            rofl::openflow::cofmsg_get_config_request &msg) {}

  /**
   * @brief	OpenFlow Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  void handle_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                            __attribute__((unused)) const rofl::cauxid &auxid,
                            __attribute__((unused))
                            rofl::openflow::cofmsg_stats_request &msg) {}

  /**
   * @brief	OpenFlow Desc-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_desc_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                            __attribute__((unused)) const rofl::cauxid &auxid,
                            __attribute__((unused))
                            rofl::openflow::cofmsg_desc_stats_request &msg) {}

  /**
   * @brief	OpenFlow Table-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_table_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                             __attribute__((unused)) const rofl::cauxid &auxid,
                             __attribute__((unused))
                             rofl::openflow::cofmsg_table_stats_request &msg) {}

  /**
   * @brief	OpenFlow Port-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_port_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                            __attribute__((unused)) const rofl::cauxid &auxid,
                            __attribute__((unused))
                            rofl::openflow::cofmsg_port_stats_request &msg) {}

  /**
   * @brief	OpenFlow Flow-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_flow_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                            __attribute__((unused)) const rofl::cauxid &auxid,
                            __attribute__((unused))
                            rofl::openflow::cofmsg_flow_stats_request &msg) {}

  /**
   * @brief	OpenFlow Aggregate-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_aggregate_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused)) rofl::openflow::cofmsg_aggr_stats_request &msg) {}

  /**
   * @brief	OpenFlow Queue-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_queue_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                             __attribute__((unused)) const rofl::cauxid &auxid,
                             __attribute__((unused))
                             rofl::openflow::cofmsg_queue_stats_request &msg) {}

  /**
   * @brief	OpenFlow Group-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_group_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                             __attribute__((unused)) const rofl::cauxid &auxid,
                             __attribute__((unused))
                             rofl::openflow::cofmsg_group_stats_request &msg) {}

  /**
   * @brief	OpenFlow Group-Desc-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_group_desc_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_group_desc_stats_request &msg) {}

  /**
   * @brief	OpenFlow Group-Features-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_group_features_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_group_features_stats_request &msg) {}

  /**
   * @brief	OpenFlow Meter-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_meter_stats_request(__attribute__((unused)) rofl::crofctl &ctl,
                             __attribute__((unused)) const rofl::cauxid &auxid,
                             __attribute__((unused))
                             rofl::openflow::cofmsg_meter_stats_request &msg) {}

  /**
   * @brief	OpenFlow Meter-Config-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_meter_config_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_meter_config_stats_request &msg) {}

  /**
   * @brief	OpenFlow Meter-Features-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_meter_features_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_meter_features_stats_request &msg) {}

  /**
   * @brief	OpenFlow Table-Features-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_table_features_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_table_features_stats_request &msg) {}

  /**
   * @brief	OpenFlow Port-Desc-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_port_desc_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_port_desc_stats_request &msg) {}

  /**
   * @brief	OpenFlow Experimenter-Stats-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_experimenter_stats_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_experimenter_stats_request &msg) {}

  /**
   * @brief	OpenFlow Packet-Out message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_packet_out(__attribute__((unused)) rofl::crofctl &ctl,
                                 __attribute__((unused))
                                 const rofl::cauxid &auxid,
                                 __attribute__((unused))
                                 rofl::openflow::cofmsg_packet_out &msg) {}

  /**
   * @brief	OpenFlow Barrier-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_barrier_request(__attribute__((unused)) rofl::crofctl &ctl,
                         __attribute__((unused)) const rofl::cauxid &auxid,
                         __attribute__((unused))
                         rofl::openflow::cofmsg_barrier_request &msg) {}

  /**
   * @brief	OpenFlow Flow-Mod message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_flow_mod(__attribute__((unused)) rofl::crofctl &ctl,
                               __attribute__((unused))
                               const rofl::cauxid &auxid,
                               __attribute__((unused))
                               rofl::openflow::cofmsg_flow_mod &msg) {}

  /**
   * @brief	OpenFlow Group-Mod message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_group_mod(__attribute__((unused)) rofl::crofctl &ctl,
                                __attribute__((unused))
                                const rofl::cauxid &auxid,
                                __attribute__((unused))
                                rofl::openflow::cofmsg_group_mod &msg) {}

  /**
   * @brief	OpenFlow Table-Mod message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_table_mod(__attribute__((unused)) rofl::crofctl &ctl,
                                __attribute__((unused))
                                const rofl::cauxid &auxid,
                                __attribute__((unused))
                                rofl::openflow::cofmsg_table_mod &msg) {}

  /**
   * @brief	OpenFlow Port-Mod message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_port_mod(__attribute__((unused)) rofl::crofctl &ctl,
                               __attribute__((unused))
                               const rofl::cauxid &auxid,
                               __attribute__((unused))
                               rofl::openflow::cofmsg_port_mod &msg) {}

  /**
   * @brief	OpenFlow Queue-Get-Config-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_queue_get_config_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_queue_get_config_request &msg) {}

  /**
   * @brief	OpenFlow Set-Config message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_set_config(__attribute__((unused)) rofl::crofctl &ctl,
                                 __attribute__((unused))
                                 const rofl::cauxid &auxid,
                                 __attribute__((unused))
                                 rofl::openflow::cofmsg_set_config &msg) {}

  /**
   * @brief	OpenFlow Experimenter message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_experimenter_message(__attribute__((unused)) rofl::crofctl &ctl,
                              __attribute__((unused)) const rofl::cauxid &auxid,
                              __attribute__((unused))
                              rofl::openflow::cofmsg_experimenter &msg) {}

  /**
   * @brief	Timer expired while waiting for OpenFlow Experimenter message.
   *
   * No Experimenter message was received in the specified time interval
   * for the given OpenFlow transaction identifier.
   *
   * @param ctl controller instance
   * @param xid OpenFlow transaction identifier
   */
  virtual void handle_experimenter_timeout(__attribute__((unused))
                                           rofl::crofctl &ctl,
                                           __attribute__((unused))
                                           uint32_t xid) {}

  /**
   * @brief	OpenFlow error message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_error_message(__attribute__((unused)) rofl::crofctl &ctl,
                                    __attribute__((unused))
                                    const rofl::cauxid &auxid,
                                    __attribute__((unused))
                                    rofl::openflow::cofmsg_error &msg) {}

  /**
   * @brief	OpenFlow Role-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_role_request(__attribute__((unused)) rofl::crofctl &ctl,
                                   __attribute__((unused))
                                   const rofl::cauxid &auxid,
                                   __attribute__((unused))
                                   rofl::openflow::cofmsg_role_request &msg) {}

  /**
   * @brief	OpenFlow Get-Async-Config-Request message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_get_async_config_request(
      __attribute__((unused)) rofl::crofctl &ctl,
      __attribute__((unused)) const rofl::cauxid &auxid,
      __attribute__((unused))
      rofl::openflow::cofmsg_get_async_config_request &msg) {}

  /**
   * @brief	OpenFlow Set-Async-Config message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void
  handle_set_async_config(__attribute__((unused)) rofl::crofctl &ctl,
                          __attribute__((unused)) const rofl::cauxid &auxid,
                          __attribute__((unused))
                          rofl::openflow::cofmsg_set_async_config &msg) {}

  /**
   * @brief	OpenFlow Meter-Mod message received.
   *
   * @param ctl controller instance
   * @param auxid control connection identifier
   * @param msg OpenFlow message instance
   */
  virtual void handle_meter_mod(__attribute__((unused)) rofl::crofctl &ctl,
                                __attribute__((unused))
                                const rofl::cauxid &auxid,
                                __attribute__((unused))
                                rofl::openflow::cofmsg_meter_mod &msg) {}

  /**@}*/
};

/**
 * @ingroup common_devel_workflow
 * @brief	Class representing a remote controller entity
 *
 * This class encapsulates properties of a single remote controller entity
 * including the OpenFlow control channel, its role and the set of
 * asynchronous event notifications to be sent to this controller.
 * Its public API offers methods to manage
 * the OpenFlow control channel, i.e. CRUD methods for individual control
 * connections.
 *
 * rofl::crofctl expects an instance of class rofl::crofctl_env as surrounding
 * environment and sends various notifications via this interface. Class
 * rofl::crofbase implements this interface and may be used as base class
 * for advanced applications. However, you may implement rofl::crofctl_env
 * directly as well.
 *
 */
class crofctl : public rofl::crofchan_env {

  enum crofctl_state_t {
    STATE_RUNNING,
    STATE_DELETE_IN_PROGRESS,
  };

public:
  /**
   * @brief	crofctl destructor
   *
   * Closes all control connections and does a general clean-up.
   */
  virtual ~crofctl();

  /**
   * @brief 	crofctl constructor
   *
   * @param env pointer to rofl::crofctl_env instance defining the environment
   * for this object
   * @param ctlid rofl-common's internal identifier for this instance
   */
  crofctl(crofctl_env *env, const cctlid &ctlid);

public:
  /**
   * @brief	Reset environment.
   */
  crofctl &set_env(crofctl_env *env) {
    this->env = env;
    return *this;
  };

public:
  /**
   * @name	Methods granting access to the controller entity's basic
   * properties
   */

  /**@{*/

  /**
   * @brief	Returns rofl-common's internal rofl::cctlid identifier for this
   * instance
   *
   * @return internal controller entity identifier
   */
  const rofl::cctlid &get_ctlid() const { return ctlid; };

  /**
   * @brief	Returns a reference to the current role object of this
   * controller
   * entity.
   */
  rofl::openflow::cofrole &set_role() { return role; };

  /**
   * @brief	Returns a const reference to the current role object of this
   * controller entity.
   */
  const rofl::openflow::cofrole &get_role() const { return role; };

  /**
   * @brief	Returns true, when the control entity is in role -slave-.
   */
  bool is_slave() const {
    switch (rofchan.get_version()) {
    case rofl::openflow12::OFP_VERSION:
      return (rofl::openflow12::OFPCR_ROLE_SLAVE == role.get_role());
    case rofl::openflow13::OFP_VERSION:
      return (rofl::openflow13::OFPCR_ROLE_SLAVE == role.get_role());
    default:
      return false;
    }
  };

  /**
   * @brief	Throws exception, when control entity is in role -slave-.
   */
  void check_role() const {
    if (is_slave())
      throw eBadRequestIsSlave("eBadRequestIsSlave", __FILE__, __FUNCTION__,
                               __LINE__);
  };

  /**
   * @brief	Returns a reference to the current asynchronous event
   * configuration of this controller entity.
   */
  rofl::openflow::cofasync_config &set_async_config() { return async_config; };

  /**
   * @brief	Returns a const reference to the current asynchronous event
   * configuration of this controller entity.
   */
  const rofl::openflow::cofasync_config &get_async_config() const {
    return async_config;
  };

  /**
   * @brief	Returns a const reference to the default asynchronous event
   * configuration for controller entities defined by OpenFlow.
   */
  const rofl::openflow::cofasync_config &
  get_async_config_role_default_template() const {
    return async_config_role_default_template;
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
  void close() { rofchan.close(); };

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
  crofconn &add_conn(crofconn *conn) { return rofchan.add_conn(conn); };

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
   * @brief	Sends OpenFlow Features-Reply message to attached controller
   * entity.
   *
   * @param auxid controller connection identifier
   * @param xid OpenFlow transaction identifier
   * @param dpid OpenFlow datapath identifier
   * @param n_buffers number of buffers available in this datapath element's I/O
   * subsystem
   * @param n_tables number of tables available in this datapath element's
   * pipeline
   * @param capabilities OpenFlow capabilities
   * @param of13_auxiliary_id control connection identifier since OpenFlow 1.3
   * @param of10_actions_bitmap set of available actions for OpenFlow 1.0 only
   * @param ports OpenFlow list of ports
   * @exception rofl::eRofBaseNotConnected
   * @exception rofl::eRofBaseCongested
   */
  rofl::crofsock::msg_result_t send_features_reply(
      const rofl::cauxid &auxid, uint32_t xid, uint64_t dpid,
      uint32_t n_buffers, uint8_t n_tables, uint32_t capabilities,
      uint8_t of13_auxiliary_id = 0, uint32_t of10_actions_bitmap = 0,
      const rofl::openflow::cofports &ports = rofl::openflow::cofports());

  /**
   * @brief	Sends OpenFlow Get-Config-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param flags datapath element flags
   * @param miss_send_len default miss_send_len value
   */
  rofl::crofsock::msg_result_t send_get_config_reply(const rofl::cauxid &auxid,
                                                     uint32_t xid,
                                                     uint16_t flags,
                                                     uint16_t miss_send_len);

  /**
   * @brief	Sends OpenFlow Desc-Stats-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param desc_stats body of DESC-STATS.reply
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t
  send_desc_stats_reply(const rofl::cauxid &auxid, uint32_t xid,
                        const rofl::openflow::cofdesc_stats_reply &desc_stats,
                        uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Flow-Stats-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param flow_stats array of flow_stats bodies
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t
  send_flow_stats_reply(const rofl::cauxid &auxid, uint32_t xid,
                        const rofl::openflow::cofflowstatsarray &flow_stats,
                        uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Aggregate-Stats-Reply message to attached
   * controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param aggr_stats aggr_stats body
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t
  send_aggr_stats_reply(const rofl::cauxid &auxid, uint32_t xid,
                        const rofl::openflow::cofaggr_stats_reply &aggr_stats,
                        uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Table-Stats-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param table_stats array of table_stats bodies
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t send_table_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::coftablestatsarray &tablestatsarray,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Port-Stats-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param port_stats array of port_stats bodies
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t
  send_port_stats_reply(const rofl::cauxid &auxid, uint32_t xid,
                        const rofl::openflow::cofportstatsarray &portstatsarray,
                        uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Queue-Stats-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param port_stats array of port_stats bodies
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t send_queue_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofqueuestatsarray &queuestatsarray,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Group-Stats-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param group_stats array of group_stats bodies
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t send_group_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofgroupstatsarray &groupstatsarray,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Group-Desc-Stats-Reply message to attached
   * controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param group_desc_stats array of group_desc_stats bodies
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t send_group_desc_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofgroupdescstatsarray &groupdescs,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Group-Features-Stats-Reply message to attached
   * controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param group_features_stats group_features_stats body
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t send_group_features_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofgroup_features_stats_reply &group_features_stats,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Table-Features-Stats-Reply message to attached
   * controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param tables tables body
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t
  send_table_features_stats_reply(const rofl::cauxid &auxid, uint32_t xid,
                                  const rofl::openflow::coftables &tables,
                                  uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Port-Desc-Stats-Reply message to attached
   * controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param ports ports body
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t
  send_port_desc_stats_reply(const rofl::cauxid &auxid, uint32_t xid,
                             const rofl::openflow::cofports &ports,
                             uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Experimenter-Stats-Reply message to attached
   * controller entity.
   *
   * @param auxid auxiliary connection id
   * @param xid OpenFlow transaction identifier
   * @param exp_id experimenter ID
   * @param exp_type user defined type
   * @param body start of user defined body
   * @param bodylen length of user defined body
   * @param more flag if multiple STATS replies will be sent
   */
  rofl::crofsock::msg_result_t
  send_experimenter_stats_reply(const rofl::cauxid &auxid, uint32_t xid,
                                uint32_t exp_id, uint32_t exp_type,
                                const cmemory &body, uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Meter-Stats-Reply message to attached controller
   * entity.
   *
   * @param auxid auxiliary connection id
   * @param xid OpenFlow transaction identifier
   * @param meter_stats_array instance of type cofmeterstatsarray
   * @param stats_flags flags for OpenFlow statistics messages, if any (default:
   * 0)
   */
  rofl::crofsock::msg_result_t send_meter_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofmeterstatsarray &meter_stats_array,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Meter-Config-Stats-Reply message to attached
   * controller entity.
   *
   * @param auxid auxiliary connection id
   * @param xid OpenFlow transaction identifier
   * @param meter_config_array instance of type cofmeterconfigarray
   * @param stats_flags flags for OpenFlow statistics messages, if any (default:
   * 0)
   */
  rofl::crofsock::msg_result_t send_meter_config_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofmeterconfigarray &meter_config_array,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Meter-Features-Stats-Reply message to attached
   * controller entity.
   *
   * @param auxid auxiliary connection id
   * @param xid OpenFlow transaction identifier
   * @param meter_config_array instance of type cofmeterfeatures
   * @param stats_flags flags for OpenFlow statistics messages, if any (default:
   * 0)
   */
  rofl::crofsock::msg_result_t send_meter_features_stats_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofmeter_features &meter_features,
      uint16_t stats_flags = 0);

  /**
   * @brief	Sends OpenFlow Packet-In message to attached controller entity.
   *
   * @param buffer_id buffer ID assigned by data path
   * @param total_len Full length of frame
   * @param reason reason packet is being sent (one of OFPR_* flags)
   * @param table_id ID of table that generated the PACKET-IN event
   * @param cookie cookie of FlowMod entry that generated the PACKET-IN event
   * @param in_port port on which frame was received
   * @param match match structure generated by data path element for data packet
   * @param data data packet
   * @param datalen length of data packet
   */
  rofl::crofsock::msg_result_t send_packet_in_message(
      const rofl::cauxid &auxid, uint32_t buffer_id, uint16_t total_len,
      uint8_t reason, uint8_t table_id, uint64_t cookie,
      uint16_t in_port, // for OF1.0
      const rofl::openflow::cofmatch &match, uint8_t *data, size_t datalen);

  /**
   * @brief	Sends OpenFlow Barrier-Reply message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   */
  rofl::crofsock::msg_result_t send_barrier_reply(const rofl::cauxid &auxid,
                                                  uint32_t xid);

  /**
   * @brief	Sends OpenFlow Error message to attached controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param type one of OpenFlow's OFPET_* flags
   * @param code one of OpenFlow's error codes
   * @param data first (at least 64) bytes of failed request
   * @param datalen length of failed request appended to error message
   */
  rofl::crofsock::msg_result_t
  send_error_message(const rofl::cauxid &auxid, uint32_t xid, uint16_t type,
                     uint16_t code, uint8_t *data = NULL, size_t datalen = 0);

  /**
   * @brief	Sends OpenFlow Experimenter message to attached controller
   * entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param experimenter_id exp_id as assigned by ONF
   * @param exp_type exp_type as defined by the ONF member
   * @param body pointer to opaque experimenter message body (optional)
   * @param bodylen length of body (optional)
   * @result transaction ID assigned to this request
   */
  rofl::crofsock::msg_result_t
  send_experimenter_message(const rofl::cauxid &auxid, uint32_t xid,
                            uint32_t experimenter_id, uint32_t exp_type,
                            uint8_t *body = NULL, size_t bodylen = 0,
                            int timeout_in_secs = DEFAULT_REQUEST_TIMEOUT);

  /**
   * @brief	Sends OpenFlow Flow-Removed message to attached controller
   * entity.
   *
   * @param match match structure defined in FlowMod entry
   * @param cookie cookie defined in FlowMod entry
   * @param priority priority level defined in FlowMOd entry
   * @param reason one of OpenFlow's OFPRR_* constants
   * @param table_id ID of table from which the FlowMod entry was removed
   * @param duration_sec time flow was alive in seconds
   * @param duration_nsec time flow was alive in nanoseconds beyond duration_sec
   * @param idle_timeout idle timeout from original flow mod
   * @param idle_timeout hard timeout from original flow mod
   * @param packet_count number of packets handled by this flow mod
   * @param byte_count number of bytes handled by this flow mod
   */
  rofl::crofsock::msg_result_t send_flow_removed_message(
      const rofl::cauxid &auxid, const rofl::openflow::cofmatch &match,
      uint64_t cookie, uint16_t priority, uint8_t reason, uint8_t table_id,
      uint32_t duration_sec, uint32_t duration_nsec, uint16_t idle_timeout,
      uint16_t hard_timeout, uint64_t packet_count, uint64_t byte_count);

  /**
   * @brief	Sends OpenFlow Port-Status message to attached controller
   * entity.
   *
   * @param reason one of OpenFlow's OFPPR_* constants
   * @param port rofl::openflow::cofport instance that changed its status
   */
  rofl::crofsock::msg_result_t
  send_port_status_message(const rofl::cauxid &auxid, uint8_t reason,
                           const rofl::openflow::cofport &port);

  /**
   * @brief	Sends OpenFlow Queue-Get-Config-Reply message to attached
   * controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param portno OpenFlow number assigned to port
   */
  rofl::crofsock::msg_result_t
  send_queue_get_config_reply(const rofl::cauxid &auxid, uint32_t xid,
                              uint32_t portno,
                              const rofl::openflow::cofpacket_queues &queues);

  /**
   * @brief	Sends OpenFlow Role-Reply message to attached controller entity.
   *
   * @param xid OpenFlow transaction identifier
   * @param role defined role from data path
   * @param generation_id gen_id as defined by OpenFlow
   */
  rofl::crofsock::msg_result_t
  send_role_reply(const rofl::cauxid &auxid, uint32_t xid,
                  const rofl::openflow::cofrole &role);

  /**
   * @brief	Sends OpenFlow Get-Async-Config-Reply message to attached
   * controller entity.
   *
   * @param xid OpenFlow transaction identifier
   */
  rofl::crofsock::msg_result_t send_get_async_config_reply(
      const rofl::cauxid &auxid, uint32_t xid,
      const rofl::openflow::cofasync_config &async_config);

  /**@}*/

public:
  /**
   *
   */
  class crofctl_find_by_ctlid {
    cctlid ctlid;

  public:
    crofctl_find_by_ctlid(const cctlid &ctlid) : ctlid(ctlid){};
    bool operator()(const crofctl *rofctl) {
      return (rofctl->get_ctlid() == ctlid);
    };
  };

public:
  friend std::ostream &operator<<(std::ostream &os, const crofctl &ctl) {
    os << "<crofctl ctlid:0x" << ctl.ctlid.str() << ">" << std::endl;
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    ss << "ctlid: " << get_ctlid().get_ctlid_s() << " ";
    return ss.str();
  };

private:
  virtual void handle_established(__attribute__((unused)) crofchan &chan,
                                  uint8_t ofp_version) {
    if (delete_in_progress())
      return;
    switch (ofp_version) {
    case rofl::openflow12::OFP_VERSION: {
      role.set_role(rofl::openflow12::OFPCR_ROLE_EQUAL);
    } break;
    case rofl::openflow13::OFP_VERSION: {
      role.set_role(rofl::openflow13::OFPCR_ROLE_EQUAL);
    } break;
    }
    crofctl_env::call_env(env).handle_established(*this, ofp_version);
  }

  virtual void handle_closed(crofchan &chan) {
    if (delete_in_progress())
      return;
    switch (chan.get_version()) {
    case rofl::openflow12::OFP_VERSION: {
      role.set_role(rofl::openflow12::OFPCR_ROLE_EQUAL);
    } break;
    case rofl::openflow13::OFP_VERSION: {
      role.set_role(rofl::openflow13::OFPCR_ROLE_EQUAL);
    } break;
    }
    crofctl_env::call_env(env).handle_closed(*this);
  };

  virtual void handle_established(__attribute__((unused)) crofchan &chan,
                                  crofconn &conn, uint8_t ofp_version) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).handle_established(*this, conn, ofp_version);
  }

  virtual void handle_closed(__attribute__((unused)) crofchan &chan,
                             crofconn &conn) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).handle_closed(*this, conn);
  }

  virtual void handle_connect_refused(__attribute__((unused)) crofchan &chan,
                                      crofconn &conn) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).handle_connect_refused(*this, conn);
  }

  virtual void handle_connect_failed(__attribute__((unused)) crofchan &chan,
                                     crofconn &conn) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).handle_connect_failed(*this, conn);
  };

  virtual void handle_accept_failed(__attribute__((unused)) crofchan &chan,
                                    crofconn &conn) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).handle_accept_failed(*this, conn);
  };

  virtual void handle_negotiation_failed(__attribute__((unused)) crofchan &chan,
                                         crofconn &conn) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).handle_negotiation_failed(*this, conn);
  };

  virtual void congestion_occurred_indication(__attribute__((unused))
                                              crofchan &chan,
                                              crofconn &conn) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).congestion_occurred_indication(*this, conn);
  };

  virtual void congestion_solved_indication(__attribute__((unused))
                                            crofchan &chan,
                                            crofconn &conn) {
    if (delete_in_progress())
      return;
    crofctl_env::call_env(env).congestion_solved_indication(*this, conn);
  };

  virtual void handle_transaction_timeout(crofchan &chan, crofconn &conn,
                                          uint32_t xid, uint8_t type,
                                          uint16_t sub_type);

private:
  virtual void handle_recv(rofl::crofchan &chan, rofl::crofconn &conn,
                           rofl::openflow::cofmsg *msg);

private:
  void init_async_config_role_default_template();

  bool delete_in_progress() const {
    return (STATE_DELETE_IN_PROGRESS == state);
  };

private:
  // environment
  rofl::crofctl_env *env;

  // state
  std::atomic<crofctl_state_t> state;

  // handle for this crofctl instance
  rofl::cctlid ctlid;

  // OpenFlow channel
  rofl::crofchan rofchan;

  // random numbers generator
  rofl::crandom random;

  // last xid sent
  uint32_t xid_last;

  // default request timeout
  static const time_t DEFAULT_REQUEST_TIMEOUT = 0; // seconds (0 : no timeout)

  // default async config template
  rofl::openflow::cofasync_config async_config_role_default_template;

  // current async config
  rofl::openflow::cofasync_config async_config;

  // role of associated remote controller
  rofl::openflow::cofrole role;
};

}; // namespace rofl

#endif
