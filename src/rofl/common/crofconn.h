/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *  Revised on: 27.09.2015
 *      Author: andreas
 */

#ifndef CROFCONN_H_
#define CROFCONN_H_

#include <atomic>
#include <bitset>
#include <inttypes.h>
#include <set>

#include "rofl/common/cauxid.h"
#include "rofl/common/crandom.h"
#include "rofl/common/crofqueue.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/csegment.hpp"
#include "rofl/common/cthread.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/cofhelloelems.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"

namespace rofl {

class eRofConnBase : public exception {
public:
  eRofConnBase(const std::string &__arg) : exception(__arg){};
};
class eRofConnBusy : public eRofConnBase {
public:
  eRofConnBusy(const std::string &__arg) : eRofConnBase(__arg){};
}; // connection already established
class eRofConnNotFound : public eRofConnBase {
public:
  eRofConnNotFound(const std::string &__arg) : eRofConnBase(__arg){};
};
class eRofConnInvalid : public eRofConnBase {
public:
  eRofConnInvalid(const std::string &__arg) : eRofConnBase(__arg){};
};
class eRofConnNotConnected : public eRofConnBase {
public:
  eRofConnNotConnected(const std::string &__arg) : eRofConnBase(__arg){};
};

class crofconn; // forward declaration

/**
 * @interface crofconn_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofconn instance.
 */
class crofconn_env {
  friend class crofconn;

public:
  static crofconn_env &call_env(crofconn_env *env) {
    AcquireReadLock lock(crofconn_env::connection_envs_lock);
    if (crofconn_env::connection_envs.find(env) ==
        crofconn_env::connection_envs.end()) {
      throw eRofConnNotFound(
          "crofconn_env::call_env() crofconn_env instance not found")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    return *(env);
  };

public:
  virtual ~crofconn_env() {
    AcquireReadWriteLock lock(crofconn_env::connection_envs_lock);
    crofconn_env::connection_envs.erase(this);
  };
  crofconn_env() {
    AcquireReadWriteLock lock(crofconn_env::connection_envs_lock);
    crofconn_env::connection_envs.insert(this);
  };

protected:
  virtual void handle_established(crofconn &conn, uint8_t ofp_version) = 0;

  virtual void handle_connect_refused(crofconn &conn) = 0;

  virtual void handle_connect_failed(crofconn &conn) = 0;

  virtual void handle_accept_failed(crofconn &conn) = 0;

  virtual void handle_negotiation_failed(crofconn &conn) = 0;

  virtual void handle_closed(crofconn &conn) = 0;

  virtual void handle_recv(crofconn &conn, rofl::openflow::cofmsg *msg) = 0;

  virtual void congestion_occurred_indication(crofconn &conn) = 0;

  virtual void congestion_solved_indication(crofconn &conn) = 0;

  virtual void handle_transaction_timeout(crofconn &conn, uint32_t xid,
                                          uint8_t type,
                                          uint16_t sub_type = 0) = 0;

private:
  static std::set<crofconn_env *> connection_envs;
  static crwlock connection_envs_lock;
};

/**
 * @ingroup common_devel_workflow
 * @brief	A single OpenFlow control connection
 */
class crofconn : public cthread_env, public crofsock_env {

  enum crofconn_flag_t {
    FLAG_DELETE_IN_PROGRESS,
  };

  enum msg_type_t {
    OFPT_HELLO = 0,
    OFPT_ERROR = 1,
    OFPT_ECHO_REQUEST = 2,
    OFPT_ECHO_REPLY = 3,
    OFPT_FEATURES_REQUEST = 5,
    OFPT_FEATURES_REPLY = 6,
    OFPT_MULTIPART_REQUEST = 18,
    OFPT_MULTIPART_REPLY = 19,
  };

  enum crofconn_state_t {
    STATE_NEGOTIATION_FAILED,
    STATE_CLOSING,
    STATE_DISCONNECTED,
    STATE_CONNECT_PENDING,
    STATE_CONNECT_REFUSED,
    STATE_CONNECT_FAILED,
    STATE_ACCEPT_PENDING,
    STATE_ACCEPT_REFUSED,
    STATE_ACCEPT_FAILED,
    STATE_NEGOTIATING,
    STATE_NEGOTIATING2,
    STATE_ESTABLISHED,
  };

  enum crofconn_timer_t {
    TIMER_ID_WAIT_FOR_HELLO,
    TIMER_ID_WAIT_FOR_FEATURES,
    TIMER_ID_WAIT_FOR_ECHO,
    TIMER_ID_NEED_LIFE_CHECK,
    TIMER_ID_RX_THREAD,
    TIMER_ID_PENDING_REQUESTS,
    TIMER_ID_PENDING_SEGMENTS,
  };

public:
  enum outqueue_type_t {
    QUEUE_OAM,  // Echo.request/Echo.reply
    QUEUE_MGMT, // all remaining packets, except ...
    QUEUE_FLOW, // Flow-Mod/Flow-Removed
    QUEUE_PKT,  // Packet-In/Packet-Out
    QUEUE_MAX,  // do not use
  };

  enum crofconn_mode_t {
    MODE_UNKNOWN = 0,
    MODE_CONTROLLER = 1,
    MODE_DATAPATH = 2,
  };

public:
  /**
   *
   */
  virtual ~crofconn();

  /**
   *
   */
  crofconn(crofconn_env *env);

public:
  /**
   *
   */
  virtual void close();

  /**
   *
   */
  virtual void
  tcp_accept(int sd,
             const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
             enum crofconn_mode_t mode);

  /**
   *
   */
  virtual void
  tcp_connect(const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
              enum crofconn_mode_t mode, bool reconnect = true);

  /**
   *
   */
  virtual void
  tls_accept(int sd,
             const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
             enum crofconn_mode_t mode);

  /**
   *
   */
  virtual void
  tls_connect(const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
              enum crofconn_mode_t mode, bool reconnect = true);

public:
  /**
   *
   */
  enum crofconn_mode_t get_mode() const { return mode; };

  /**
   *
   */
  enum crofconn_state_t get_state() const { return state; };

  /**
   * @brief	Returns true for an established connection
   */
  bool is_established() const { return (STATE_ESTABLISHED == get_state()); }

  /**
   * @brief	Returns true in case of a congested underlying TCP connection
   */
  bool is_congested() const { return rofsock.is_congested(); };

  /**
   *
   */
  bool is_transport_established() const { return rofsock.is_established(); };

  /**
   *
   */
  bool is_tls_encrypted() const { return rofsock.is_tls_encrypted(); };

  /**
   *
   */
  bool is_passive() const { return rofsock.is_passive(); };

  /**
   * @brief	Disable reception of messages on this socket.
   */
  void rx_disable() { rofsock.rx_disable(); };

  /**
   * @brief	Reenable reception of messages on this socket.
   */
  void rx_enable() { rofsock.rx_enable(); };

  /**
   * @brief	Send OFP message via socket
   */
  rofl::crofsock::msg_result_t send_message(rofl::openflow::cofmsg *msg) {
    return segment_and_send_message(msg);
  };

  /**
   * @brief	Send OFP message via socket with expiration timer
   */
  rofl::crofsock::msg_result_t send_message(rofl::openflow::cofmsg *msg,
                                            const ctimespec &ts);

public:
  /**
   * @brief	Returns a reference to the versionbitmap announced by this
   * entity
   */
  rofl::openflow::cofhello_elem_versionbitmap &get_versionbitmap() {
    return versionbitmap;
  };

  /**
   * @brief	Returns a reference to the versionbitmap seen from the peer
   */
  rofl::openflow::cofhello_elem_versionbitmap &get_versionbitmap_peer() {
    return versionbitmap_peer;
  };

  /**
   * @brief	Returns the negotiated OFP version (or OFP_UNKNOWN)
   */
  uint8_t get_version() const { return ofp_version; };

  /**
   * @brief	Returns data pagth id assigned to this connection
   */
  uint64_t get_dpid() const { return dpid; };

public:
  /**
   * @brief	Return auxiliary_id
   */
  cauxid const &get_auxid() const { return auxid; };

  /**
   *
   */
  void set_auxid(const cauxid &auxid) { this->auxid = auxid; };

public:
  /**
   * @brief	Returns capacity of reception queues in messages
   */
  size_t get_rxqueue_max_size(outqueue_type_t queue_id) const {
    if (rxqueues.size() <= queue_id) {
      throw eRofConnInvalid("crofconn::get_rxqueue_max_size() invalid queue_id")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    return rxqueues[queue_id].get_queue_max_size();
  };

  /**
   * @brief	Sets capacity of reception queues in messages
   */
  crofconn &set_rxqueue_max_size(outqueue_type_t queue_id,
                                 size_t rxqueue_max_size) {
    if (rxqueues.size() <= queue_id) {
      throw eRofConnInvalid("crofconn::set_rxqueue_max_size() invalid queue_id")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    rxqueues[queue_id].set_queue_max_size(rxqueue_max_size);
    return *this;
  };

public:
  /**
   *
   */
  crofconn &set_env(crofconn_env *env) {
    this->env = env;
    return *this;
  };

public:
  /**
   * @brief	Returns capacity of transmission queues in messages
   */
  size_t get_txqueue_max_size() const {
    return rofsock.get_txqueue_max_size();
  };

  /**
   * @brief	Sets capacity of transmission queues in messages
   */
  crofconn &set_txqueue_max_size(size_t txqueue_max_size) {
    rofsock.set_txqueue_max_size(txqueue_max_size);
    return *this;
  };

public:
  /**
   *
   */
  const csockaddr &get_laddr() const { return rofsock.get_laddr(); };

  /**
   *
   */
  crofconn &set_laddr(const csockaddr &laddr) {
    rofsock.set_laddr(laddr);
    return *this;
  };

public:
  /**
   *
   */
  const csockaddr &get_raddr() const { return rofsock.get_raddr(); };

  /**
   *
   */
  crofconn &set_raddr(const csockaddr &raddr) {
    rofsock.set_raddr(raddr);
    return *this;
  };

public:
  /**
   *
   */
  unsigned int get_timeout_hello() const { return timeout_hello; };

  /**
   *
   */
  crofconn &set_timeout_hello(unsigned int timeout_hello) {
    this->timeout_hello = timeout_hello;
    return *this;
  };

public:
  /**
   *
   */
  unsigned int get_timeout_features() const { return timeout_features; };

  /**
   *
   */
  crofconn &set_timeout_features(unsigned int timeout_features) {
    this->timeout_features = timeout_features;
    return *this;
  };

public:
  /**
   *
   */
  unsigned int get_timeout_echo() const { return timeout_echo; };

  /**
   *
   */
  crofconn &set_timeout_echo(unsigned int timeout_echo) {
    this->timeout_echo = timeout_echo;
    return *this;
  };

public:
  /**
   *
   */
  unsigned int get_timeout_lifecheck() const { return timeout_lifecheck; };

  /**
   *
   */
  crofconn &set_timeout_lifecheck(unsigned int timeout_lifecheck) {
    this->timeout_lifecheck = timeout_lifecheck;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_capath() const {
    return rofsock.get_tls_capath();
  };

  /**
   *
   */
  crofconn &set_tls_capath(const std::string &capath) {
    rofsock.set_tls_capath(capath);
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_cafile() const {
    return rofsock.get_tls_cafile();
  };

  /**
   *
   */
  crofconn &set_tls_cafile(const std::string &cafile) {
    rofsock.set_tls_cafile(cafile);
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_certfile() const {
    return rofsock.get_tls_certfile();
  };

  /**
   *
   */
  crofconn &set_tls_certfile(const std::string &certfile) {
    rofsock.set_tls_certfile(certfile);
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_keyfile() const {
    return rofsock.get_tls_keyfile();
  };

  /**
   *
   */
  crofconn &set_tls_keyfile(const std::string &keyfile) {
    rofsock.set_tls_keyfile(keyfile);
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_pswd() const { return rofsock.get_tls_pswd(); };

  /**
   *
   */
  crofconn &set_tls_pswd(const std::string &password) {
    rofsock.set_tls_pswd(password);
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_verify_mode() const {
    return rofsock.get_tls_verify_mode();
  };

  /**
   *
   */
  crofconn &set_tls_verify_mode(const std::string &verify_mode) {
    rofsock.set_tls_verify_mode(verify_mode);
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_verify_depth() const {
    return rofsock.get_tls_verify_depth();
  };

  /**
   *
   */
  crofconn &set_tls_verify_depth(const std::string &verify_depth) {
    rofsock.set_tls_verify_depth(verify_depth);
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_ciphers() const {
    return rofsock.get_tls_ciphers();
  };

  /**
   *
   */
  crofconn &set_tls_ciphers(const std::string &ciphers) {
    rofsock.set_tls_ciphers(ciphers);
    return *this;
  };

public:
  /**
   *
   */
  unsigned int get_pending_segments_max() const {
    return pending_segments_max;
  };

  /**
   *
   */
  crofconn &set_pending_segments_max(unsigned int pending_segments_max) {
    this->pending_segments_max = pending_segments_max;
    return *this;
  };

  friend std::ostream &operator<<(std::ostream &os, const crofconn &conn) {
    os << "<crofconn ofp-version: " << (int)conn.ofp_version
       << " openflow-connection-established: " << conn.is_established()
       << " transport-connection-established: "
       << conn.is_transport_established() << " >" << std::endl;
    { os << conn.get_auxid(); }
    if (conn.get_state() == STATE_NEGOTIATION_FAILED) {
      os << "<state: -NEGOTIATION-FAILED- >" << std::endl;
    } else if (conn.get_state() == STATE_CLOSING) {
      os << "<state: -CLOSING- >" << std::endl;
    } else if (conn.get_state() == STATE_DISCONNECTED) {
      os << "<state: -DISCONNECTED- >" << std::endl;
    } else if (conn.get_state() == STATE_CONNECT_PENDING) {
      os << "<state: -CONNECT-PENDING- >" << std::endl;
    } else if (conn.get_state() == STATE_ACCEPT_PENDING) {
      os << "<state: -ACCEPT-PENDING- >" << std::endl;
    } else if (conn.get_state() == STATE_NEGOTIATING) {
      os << "<state: -NEGOTIATING- >" << std::endl;
    } else if (conn.get_state() == STATE_NEGOTIATING2) {
      os << "<state: -NEGOTIATING2- >" << std::endl;
    } else if (conn.get_state() == STATE_ESTABLISHED) {
      os << "<state: -ESTABLISHED- >" << std::endl;
    }
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    ss << "<crofconn ofp-version:" << (int)ofp_version
       << " openflow-connection-established: " << is_established()
       << " transport-connection-established: " << is_transport_established();
    if (get_state() == STATE_NEGOTIATION_FAILED) {
      ss << "state: -NEGOTIATION-FAILED- ";
    } else if (get_state() == STATE_CLOSING) {
      ss << "state: -CLOSING- ";
    } else if (get_state() == STATE_DISCONNECTED) {
      ss << "state: -DISCONNECTED- ";
    } else if (get_state() == STATE_CONNECT_PENDING) {
      ss << "state: -CONNECT-PENDING- ";
    } else if (get_state() == STATE_ACCEPT_PENDING) {
      ss << "state: -ACCEPT-PENDING- ";
    } else if (get_state() == STATE_NEGOTIATING) {
      ss << "state: -NEGOTIATING- ";
    } else if (get_state() == STATE_NEGOTIATING2) {
      ss << "state: -NEGOTIATING2- ";
    } else if (get_state() == STATE_ESTABLISHED) {
      ss << "state: -ESTABLISHED- ";
    }
    ss << ">";
    return ss.str();
  };

private:
  void set_mode(enum crofconn_mode_t mode) { this->mode = mode; };

  void set_state(enum crofconn_state_t state) { this->state = state; };

  void run_finite_state_machine(enum crofconn_state_t state);

  void set_versionbitmap(
      const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap) {
    if (versionbitmap.get_highest_ofp_version() ==
        rofl::openflow::OFP_VERSION_UNKNOWN) {
      throw eRofConnInvalid(
          "crofconn::set_versionbitmap() versionbitmap invalid")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    this->versionbitmap = versionbitmap;
  };

  void set_versionbitmap_peer(
      const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap_peer) {
    this->versionbitmap_peer = versionbitmap_peer;
  };

  void set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

  void set_dpid(uint64_t dpid) { this->dpid = dpid; };

private:
  virtual void handle_listen(crofsock &socket){/* not used */};

  virtual void handle_tcp_connect_refused(crofsock &rofsock);

  virtual void handle_tcp_connect_failed(crofsock &rofsock);

  virtual void handle_tcp_connected(crofsock &rofsock);

  virtual void handle_tcp_accept_refused(crofsock &socket);

  virtual void handle_tcp_accept_failed(crofsock &socket);

  virtual void handle_tcp_accepted(crofsock &socket);

  virtual void handle_tls_connect_failed(crofsock &socket);

  virtual void handle_tls_connected(crofsock &socket);

  virtual void handle_tls_accept_failed(crofsock &socket);

  virtual void handle_tls_accepted(crofsock &socket);

  virtual void handle_closed(crofsock &rofsock);

  virtual void handle_recv(crofsock &socket, rofl::openflow::cofmsg *msg);

  virtual void congestion_occurred_indication(crofsock &socket);

  virtual void congestion_solved_indication(crofsock &rofsock);

  void flag_set(crofconn_flag_t __flag, bool value) {
    AcquireReadWriteLock lock(flags_lock);
    flags.set(__flag, value);
  };

  bool flag_test(crofconn_flag_t __flag) const {
    AcquireReadLock lock(flags_lock);
    return flags.test(__flag);
  };

private:
  virtual void handle_wakeup(cthread &thread);

  virtual void handle_timeout(cthread &thread, uint32_t timer_id);

  virtual void handle_read_event(cthread &thread, int fd){};

  virtual void handle_write_event(cthread &thread, int fd){};

private:
  void handle_rx_messages();

  void handle_rx_multipart_message(rofl::openflow::cofmsg *msg);

private:
  void error_rcvd(rofl::openflow::cofmsg *msg);

  void echo_request_rcvd(rofl::openflow::cofmsg *msg);

private:
  void send_hello_message();

  void hello_rcvd(rofl::openflow::cofmsg *msg);

  void hello_expired();

private:
  void send_features_request();

  void features_reply_rcvd(rofl::openflow::cofmsg *msg);

  void features_request_expired();

private:
  void send_echo_request();

  void echo_reply_rcvd(rofl::openflow::cofmsg *msg);

  void echo_request_expired();

private:
  /**
   *
   */
  rofl::crofsock::msg_result_t
  segment_and_send_message(rofl::openflow::cofmsg *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t segment_table_features_stats_request(
      rofl::openflow::cofmsg_table_features_stats_request *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t
  segment_flow_stats_reply(rofl::openflow::cofmsg_flow_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t
  segment_table_stats_reply(rofl::openflow::cofmsg_table_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t
  segment_port_stats_reply(rofl::openflow::cofmsg_port_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t
  segment_queue_stats_reply(rofl::openflow::cofmsg_queue_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t
  segment_group_stats_reply(rofl::openflow::cofmsg_group_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t segment_group_desc_stats_reply(
      rofl::openflow::cofmsg_group_desc_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t segment_table_features_stats_reply(
      rofl::openflow::cofmsg_table_features_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t segment_port_desc_stats_reply(
      rofl::openflow::cofmsg_port_desc_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t
  segment_meter_stats_reply(rofl::openflow::cofmsg_meter_stats_reply *msg);

  /**
   *
   */
  rofl::crofsock::msg_result_t segment_meter_config_stats_reply(
      rofl::openflow::cofmsg_meter_config_stats_reply *msg);

private:
  /**
   *
   */
  class ctransaction {
  public:
    uint32_t xid;
    ctimespec tspec;
    uint8_t type;
    uint16_t subtype;

  public:
    /**
     *
     */
    ctransaction() : xid(0), type(0), subtype(0){};

    /**
     *
     */
    ctransaction(uint32_t xid, const ctimespec tspec, uint8_t type,
                 uint16_t subtype = 0)
        : xid(xid), tspec(tspec), type(type), subtype(subtype){};

    /**
     *
     */
    ctransaction(const ctransaction &ta) { *this = ta; };

    /**
     *
     */
    ctransaction &operator=(const ctransaction &ta) {
      if (this == &ta)
        return *this;
      xid = ta.xid;
      tspec = ta.tspec;
      type = ta.type;
      subtype = ta.subtype;
      return *this;
    };

    /**
     *
     */
    bool operator<(const ctransaction &ta) const { return (tspec < ta.tspec); };

  public:
    uint32_t get_xid() const { return xid; };

    uint8_t get_type() const { return type; };

    uint16_t get_subtype() const { return subtype; };

    const ctimespec &get_tspec() const { return tspec; };

  public:
    class ctransaction_find_by_xid {
      uint32_t xid;

    public:
      ctransaction_find_by_xid(uint32_t xid) : xid(xid){};
      bool operator()(const ctransaction &ta) const {
        return (ta.get_xid() == xid);
      };
    };
  };

  /**
   *
   */
  void clear_pending_requests() {
    AcquireReadWriteLock rwlock(pending_requests_rwlock);
    pending_requests.clear();
    cthread::thread(thread_num).drop_timer(this, TIMER_ID_PENDING_REQUESTS);
  };

  /**
   *
   */
  void add_pending_request(uint32_t xid, const ctimespec &ts, uint8_t type,
                           uint16_t sub_type = 0) {
    AcquireReadWriteLock rwlock(pending_requests_rwlock);
    if (not pending_requests.empty()) {
      uint32_t xid_first = pending_requests.begin()->get_xid();
      pending_requests.insert(ctransaction(xid, ts, type, sub_type));
      const ctransaction &ta_first = *(pending_requests.begin());
      if (xid_first != ta_first.get_xid()) {
        cthread::thread(thread_num)
            .add_timer(this, TIMER_ID_PENDING_REQUESTS, ta_first.tspec);
      }
    } else {
      pending_requests.insert(ctransaction(xid, ts, type, sub_type));
      cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_PENDING_REQUESTS,
                     pending_requests.begin()->tspec);
    }
  };

  /**
   *
   */
  void drop_pending_request(uint32_t xid) {
    AcquireReadWriteLock rwlock(pending_requests_rwlock);
    std::set<ctransaction>::iterator it;
    while ((it = find_if(pending_requests.begin(), pending_requests.end(),
                         ctransaction::ctransaction_find_by_xid(xid))) !=
           pending_requests.end()) {
      pending_requests.erase(it);
    }
  };

  /**
   *
   */
  bool has_pending_request(uint32_t xid) const {
    AcquireReadLock rlock(pending_requests_rwlock);
    std::set<ctransaction>::iterator it;
    if ((it = find_if(pending_requests.begin(), pending_requests.end(),
                      ctransaction::ctransaction_find_by_xid(xid))) !=
        pending_requests.end()) {
      return true;
    }
    return false;
  };

  /**
   *
   */
  void check_pending_requests() {
    while (true) {
      ctransaction ta;
      {
        AcquireReadWriteLock rwlock(pending_requests_rwlock);
        if (pending_requests.empty()) {
          return;
        }
        ta = *(pending_requests.begin());
        if (not ta.get_tspec().is_expired()) {
          cthread::thread(thread_num)
              .add_timer(this, TIMER_ID_PENDING_REQUESTS, ta.get_tspec());
          return;
        }
        pending_requests.erase(pending_requests.begin());
      } // release rwlock
      try {
        crofconn_env::call_env(env).handle_transaction_timeout(
            *this, ta.get_xid(), ta.get_type(), ta.get_subtype());
      } catch (eRofConnNotFound &e) {
        return;
      }
    };
  };

private:
  /**
   *
   */
  void clear_pending_segments() {
    AcquireReadWriteLock rwlock(pending_segments_rwlock);
    pending_segments.clear();
    cthread::thread(thread_num).drop_timer(this, TIMER_ID_PENDING_SEGMENTS);
  };

  /**
   *
   */
  csegment &add_pending_segment(uint32_t xid, uint8_t msg_type,
                                uint16_t msg_multipart_type) {
    AcquireReadWriteLock rwlock(pending_segments_rwlock);
    if (not(pending_segments.size() < pending_segments_max)) {
      throw eRofConnInvalid("crofconn::add_pending_segment() too many segments "
                            "in transit, dropping")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    pending_segments[xid] =
        csegment(xid, ctimespec().expire_in(timeout_segments), msg_type,
                 msg_multipart_type);
    if (not cthread::thread(thread_num)
                .has_timer(this, TIMER_ID_PENDING_SEGMENTS)) {
      cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_PENDING_SEGMENTS,
                     ctimespec().expire_in(timeout_segments));
    }
    return pending_segments[xid];
  };

  /**
   *
   */
  csegment &set_pending_segment(uint32_t xid, uint8_t msg_type,
                                uint16_t msg_multipart_type) {
    AcquireReadWriteLock rwlock(pending_segments_rwlock);
    if (not(pending_segments.size() < pending_segments_max)) {
      throw eRofConnInvalid("crofconn::set_pending_segment() too many segments "
                            "in transit, dropping")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    if (pending_segments.find(xid) == pending_segments.end()) {
      pending_segments[xid] =
          csegment(xid, ctimespec().expire_in(timeout_segments), msg_type,
                   msg_multipart_type);
    }
    if (not cthread::thread(thread_num)
                .has_timer(this, TIMER_ID_PENDING_SEGMENTS)) {
      cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_PENDING_SEGMENTS,
                     ctimespec().expire_in(timeout_segments));
    }
    return pending_segments[xid];
  };

  /**
   *
   */
  csegment &set_pending_segment(uint32_t xid) {
    AcquireReadWriteLock rwlock(pending_segments_rwlock);
    if (not(pending_segments.size() < pending_segments_max)) {
      throw eRofConnInvalid("crofconn::set_pending_segment() too many segments "
                            "in transit, dropping")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    if (pending_segments.find(xid) == pending_segments.end()) {
      throw eRofConnNotFound("crofconn::set_pending_segment() xid not found")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    if (not cthread::thread(thread_num)
                .has_timer(this, TIMER_ID_PENDING_SEGMENTS)) {
      cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_PENDING_SEGMENTS,
                     ctimespec().expire_in(timeout_segments));
    }
    return pending_segments[xid];
  };

  /**
   *
   */
  const csegment &get_pending_segment(uint32_t xid) const {
    AcquireReadLock rlock(pending_segments_rwlock);
    if (pending_segments.find(xid) == pending_segments.end()) {
      throw eRofConnNotFound("crofconn::get_pending_segment() xid not found")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    return pending_segments.at(xid);
  };

  /**
   *
   */
  bool drop_pending_segment(uint32_t xid) {
    AcquireReadWriteLock rwlock(pending_segments_rwlock);
    if (pending_segments.find(xid) == pending_segments.end()) {
      return false;
    }
    pending_segments.erase(xid);
    return true;
  };

  /**
   *
   */
  bool has_pending_segment(uint32_t xid) const {
    AcquireReadLock rlock(pending_segments_rwlock);
    return (not(pending_segments.find(xid) == pending_segments.end()));
  };

  /**
   *
   */
  void check_pending_segments() {
    AcquireReadWriteLock rwlock(pending_segments_rwlock);
    std::map<uint32_t, csegment>::iterator it;
    while ((it = find_if(pending_segments.begin(), pending_segments.end(),
                         csegment::csegment_is_expired())) !=
           pending_segments.end()) {
      pending_segments.erase(it);
    }
    if (not pending_segments.empty()) {
      cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_PENDING_SEGMENTS,
                     ctimespec().expire_in(timeout_segments));
    }
  };

private:
  // environment for this instance
  rofl::crofconn_env *env;

  // crofconn rwlock
  mutable rofl::crwlock tlock;

  // various flags for this crofsock instance
  std::bitset<32> flags;

  // and the associated rwlock
  mutable rofl::crwlock flags_lock;

  // internal thread for application specific context
  uint32_t thread_num;

  // crofsock instance
  rofl::crofsock rofsock;

  // OpenFlow datapath id
  uint64_t dpid;

  // connection identifier
  cauxid auxid;

  // bitmap of acceptable OpenFlow versions
  rofl::openflow::cofhello_elem_versionbitmap versionbitmap;

  // bitmap of offered OpenFlow versions by peer
  rofl::openflow::cofhello_elem_versionbitmap versionbitmap_peer;

  // OpenFlow version negotiated
  std::atomic_uint_fast8_t ofp_version;

  // random number generator
  rofl::crandom random;

  // acts in controller or datapath mode (orthogonal to TCP client/server mode)
  std::atomic<crofconn_mode_t> mode;

  // internal state of finite state machine
  std::atomic<crofconn_state_t> state;

  // hello message sent to peer
  std::atomic_bool flag_hello_sent;

  // hello message from peer rcvd
  std::atomic_bool flag_hello_rcvd;

  // hello lock
  rofl::crwlock hello_lock;

  // relative scheduling weights for rxqueues
  std::vector<unsigned int> rxweights;

  // queues for storing received messages
  std::vector<crofqueue> rxqueues;

  // internal thread is working on pending messages
  std::atomic_bool rx_thread_working;

  // internal thread is scheduled for working on pending messages
  std::atomic_bool rx_thread_scheduled;

  // max size of rx queue
  size_t rxqueue_max_size;
  static const int RXQUEUE_MAX_SIZE_DEFAULT;

  // maximum number of bytes for a multipart message before being segmented
  size_t segmentation_threshold;

  // default segmentation threshold: 65535 bytes
  static const unsigned int DEFAULT_SEGMENTATION_THRESHOLD;

  // timeout value for HELLO messages
  time_t timeout_hello;
  static const time_t DEFAULT_HELLO_TIMEOUT;

  // timeout value for FEATURES.request messages
  time_t timeout_features;
  static const time_t DEFAULT_FEATURES_TIMEOUT;

  // timeout value for ECHO.request messages
  time_t timeout_echo;
  static const time_t DEFAULT_ECHO_TIMEOUT;

  // timeout value for lifecheck
  time_t timeout_lifecheck;
  static const time_t DEFAULT_LIFECHECK_TIMEOUT;

  // set of pending requests
  std::set<ctransaction> pending_requests;

  // .. and associated rwlock
  crwlock pending_requests_rwlock;

  // hello xid
  uint32_t xid_hello_last;

  // features request xid
  uint32_t xid_features_request_last;

  // echo request xid
  uint32_t xid_echo_request_last;

  // set of pending OpenFlow message segments
  std::map<uint32_t, csegment> pending_segments;

  // ... and associated rwlock
  crwlock pending_segments_rwlock;

  // timeout value for pending segments
  time_t timeout_segments;
  static const time_t DEFAULT_SEGMENTS_TIMEOUT;

  // maximum number of pending segments in parallel
  unsigned int pending_segments_max;
  static const unsigned int DEFAULT_PENDING_SEGMENTS_MAX;
};

}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
