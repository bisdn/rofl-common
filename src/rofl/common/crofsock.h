/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofsock.h
 *
 *  Created on: 31.12.2013
 *  Revised on: 20.09.2015
 *      Author: andreas
 */

#ifndef CROFSOCK_H_
#define CROFSOCK_H_

#include <algorithm>
#include <assert.h>
#include <atomic>
#include <bitset>
#include <deque>
#include <fcntl.h>
#include <inttypes.h>
#include <iostream>
#include <list>
#include <map>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <set>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#include <openssl/ssl.h>

#include "rofl/common/cbuffer.hpp"
#include "rofl/common/cmemory.h"

#include "rofl/common/crandom.h"
#include "rofl/common/crofqueue.h"
#include "rofl/common/csockaddr.h"
#include "rofl/common/cthread.hpp"
#include "rofl/common/exception.hpp"

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/messages/cofmsg_aggr_stats.h"
#include "rofl/common/openflow/messages/cofmsg_async_config.h"
#include "rofl/common/openflow/messages/cofmsg_barrier.h"
#include "rofl/common/openflow/messages/cofmsg_config.h"
#include "rofl/common/openflow/messages/cofmsg_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_echo.h"
#include "rofl/common/openflow/messages/cofmsg_error.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter.h"
#include "rofl/common/openflow/messages/cofmsg_experimenter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_features.h"
#include "rofl/common/openflow/messages/cofmsg_flow_mod.h"
#include "rofl/common/openflow/messages/cofmsg_flow_removed.h"
#include "rofl/common/openflow/messages/cofmsg_flow_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_group_mod.h"
#include "rofl/common/openflow/messages/cofmsg_group_stats.h"
#include "rofl/common/openflow/messages/cofmsg_hello.h"
#include "rofl/common/openflow/messages/cofmsg_meter_config_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_meter_mod.h"
#include "rofl/common/openflow/messages/cofmsg_meter_stats.h"
#include "rofl/common/openflow/messages/cofmsg_packet_in.h"
#include "rofl/common/openflow/messages/cofmsg_packet_out.h"
#include "rofl/common/openflow/messages/cofmsg_port_desc_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_mod.h"
#include "rofl/common/openflow/messages/cofmsg_port_stats.h"
#include "rofl/common/openflow/messages/cofmsg_port_status.h"
#include "rofl/common/openflow/messages/cofmsg_queue_get_config.h"
#include "rofl/common/openflow/messages/cofmsg_queue_stats.h"
#include "rofl/common/openflow/messages/cofmsg_role.h"
#include "rofl/common/openflow/messages/cofmsg_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_features_stats.h"
#include "rofl/common/openflow/messages/cofmsg_table_mod.h"
#include "rofl/common/openflow/messages/cofmsg_table_stats.h"

namespace rofl {

class eRofSockBase : public exception {
public:
  eRofSockBase(const std::string &__arg,
               const std::string &__file = std::string(""),
               const std::string &__func = std::string(""), int __line = 0)
      : exception(__arg, __file, __func, __line) {
    set_exception("eRofSockBase");
  };
};
class eRofSockInvalid : public eRofSockBase {
public:
  eRofSockInvalid(const std::string &__arg,
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eRofSockBase(__arg, __file, __func, __line) {
    set_exception("eRofSockInvalid");
  };
};
class eRofSockNotFound : public eRofSockBase {
public:
  eRofSockNotFound(const std::string &__arg,
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eRofSockBase(__arg, __file, __func, __line) {
    set_exception("eRofSockNotFound");
  };
};
class eRofSockError : public eRofSockBase {
public:
  eRofSockError(const std::string &__arg,
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eRofSockBase(__arg, __file, __func, __line) {
    set_exception("eRofSockError");
  };
};
class eRofSockNotEstablished : public eRofSockBase {
public:
  eRofSockNotEstablished(const std::string &__arg,
                         const std::string &__file = std::string(""),
                         const std::string &__func = std::string(""),
                         int __line = 0)
      : eRofSockBase(__arg, __file, __func, __line) {
    set_exception("eRofSockNotEstablished");
  };
};
class eRofSockCongested : public eRofSockBase {
public:
  eRofSockCongested(const std::string &__arg,
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eRofSockBase(__arg, __file, __func, __line) {
    set_exception("eRofSockCongested");
  };
};

class crofsock; // forward declaration

/**
 * @interface crofsock_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofsock instance.
 */
class crofsock_env {
  friend class crofsock;

public:
  static crofsock_env &call_env(crofsock_env *env) {
    AcquireReadLock lock(crofsock_env::socket_envs_lock);
    if (crofsock_env::socket_envs.find(env) ==
        crofsock_env::socket_envs.end()) {
      throw eRofSockNotFound(
          "crofsock_env::call_env() crofsock_env instance not found")
          .set_func(__FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    return *(env);
  };

public:
  virtual ~crofsock_env() {
    AcquireReadWriteLock lock(crofsock_env::socket_envs_lock);
    crofsock_env::socket_envs.erase(this);
  };
  crofsock_env() {
    AcquireReadWriteLock lock(crofsock_env::socket_envs_lock);
    crofsock_env::socket_envs.insert(this);
  };

protected:
  virtual void handle_listen(crofsock &socket) = 0;

  virtual void handle_tcp_connect_refused(crofsock &socket) = 0;

  virtual void handle_tcp_connect_failed(crofsock &socket) = 0;

  virtual void handle_tcp_connected(crofsock &socket) = 0;

  virtual void handle_tcp_accept_refused(crofsock &socket) = 0;

  virtual void handle_tcp_accept_failed(crofsock &socket) = 0;

  virtual void handle_tcp_accepted(crofsock &socket) = 0;

  virtual void handle_tls_connect_failed(crofsock &socket) = 0;

  virtual void handle_tls_connected(crofsock &socket) = 0;

  virtual void handle_tls_accept_failed(crofsock &socket) = 0;

  virtual void handle_tls_accepted(crofsock &socket) = 0;

  virtual void handle_closed(crofsock &socket) = 0;

  virtual void handle_recv(crofsock &socket, rofl::openflow::cofmsg *msg) = 0;

  virtual void congestion_occurred_indication(crofsock &socket) = 0;

  virtual void congestion_solved_indication(crofsock &socket) = 0;

private:
  static std::set<crofsock_env *> socket_envs;
  static crwlock socket_envs_lock;
};

/**
 * @ingroup common_devel_workflow
 * @brief	A socket capable of talking OpenFlow via TCP and vice versa
 */
class crofsock : public cthread_env {
  enum outqueue_type_t {
    QUEUE_OAM = 0,  // Echo.request/Echo.reply
    QUEUE_MGMT = 1, // all remaining packets, except ...
    QUEUE_FLOW = 2, // Flow-Mod/Flow-Removed
    QUEUE_PKT = 3,  // Packet-In/Packet-Out
    QUEUE_MAX,      // do not use
  };

  enum crofsock_flag_t {
    FLAG_DELETE_IN_PROGRESS,
    FLAG_CONGESTED,
    FLAG_TX_BLOCK_QUEUEING,
    FLAG_RECONNECT_ON_FAILURE,
    FLAG_TLS_IN_USE,
    FLAG_CLOSING,
  };

  enum socket_mode_t {
    MODE_UNKNOWN,
    MODE_LISTEN,
    MODE_CLIENT,
    MODE_SERVER,
  };

  enum socket_state_t {
    STATE_IDLE,
    STATE_LISTENING,
    STATE_TCP_CONNECTING,
    STATE_TLS_CONNECTING,
    STATE_TCP_ACCEPTING,
    STATE_TLS_ACCEPTING,
    STATE_TCP_ESTABLISHED,
    STATE_TLS_ESTABLISHED,
  };

  enum crofsock_timer_t {
    TIMER_ID_UNKNOWN,
    TIMER_ID_RECONNECT,
    TIMER_ID_PEER_SHUTDOWN,
  };

public:
  enum msg_result_t {
    MSG_IGNORED,
    MSG_QUEUED,
    MSG_QUEUED_CONGESTION,
    MSG_QUEUEING_FAILED_QUEUE_FULL,
    MSG_QUEUEING_FAILED_NOT_ESTABLISHED,
    MSG_QUEUEING_FAILED_SHUTDOWN_IN_PROGRESS,
  };

public:
  /**
   *
   */
  virtual ~crofsock();

  /**
   *
   */
  crofsock(crofsock_env *env);

public:
  /**
   *
   */
  void close();

  /**
   *
   */
  virtual void listen();

  std::list<int> accept();

  /**
   *
   */
  virtual void tcp_accept(int sd);

  /**
   *
   */
  virtual void tcp_connect(bool reconnect = true);

  /**
   *
   */
  virtual void tls_accept(int sd);

  /**
   *
   */
  virtual void tls_connect(bool reconnect = true);

public:
  /**
   *
   */
  rofl::crofsock::msg_result_t send_message(rofl::openflow::cofmsg *msg,
                                            bool enforce_queueing = false);

  /**
   *
   */
  bool is_established() const;

  /**
   *
   */
  bool is_tls_encrypted() const;

  /**
   *
   */
  bool is_passive() const;

  /**
   *
   */
  bool is_congested() const;

  /**
   *
   */
  bool is_rx_disabled() const;

  /**
   * @brief	Disable reception of messages on this socket.
   */
  void rx_disable();

  /**
   * @brief	Reenable reception of messages on this socket.
   */
  void rx_enable();

  /**
   * @brief	Disable transmission of messages on this socket.
   */
  void tx_disable();

  /**
   * @brief	Reenable transmission of messages on this socket.
   */
  void tx_enable();

  /**
   * @brief	Return backlog for listening mode
   */
  int get_backlog() const { return backlog; };

  /**
   * @brief	Set backlog for listening mode
   */
  crofsock &set_backlog(int backlog) {
    this->backlog = backlog;
    return *this;
  };

  /**
   * @brief Return state
   */
  enum socket_state_t get_state() const {
    AcquireReadLock lock(tlock);
    return state;
  };

public:
  /**
   * @brief	Returns capacity of transmission queues in messages
   */
  size_t get_txqueue_max_size() const {
    return txqueue_size_congestion_occurred;
  };

  /**
   * @brief	Sets capacity of transmission queues in messages
   */
  crofsock &set_txqueue_max_size(size_t txqueue_max_size) {
    this->txqueue_size_congestion_occurred = txqueue_max_size;
    for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; queue_id++) {
      txqueues[queue_id].set_queue_max_size(txqueue_max_size);
    }
    return *this;
  };

public:
  /**
   *
   */
  crofsock &set_baddr(const csockaddr &baddr) {
    this->baddr = baddr;
    return *this;
  };

  /**
   *
   */
  const csockaddr &get_baddr() const { return baddr; };

  /**
   *
   */
  crofsock &set_laddr(const csockaddr &laddr) {
    this->laddr = laddr;
    return *this;
  };

  /**
   *
   */
  const csockaddr &get_laddr() const { return laddr; };

  /**
   *
   */
  crofsock &set_raddr(const csockaddr &raddr) {
    this->raddr = raddr;
    return *this;
  };

  /**
   *
   */
  const csockaddr &get_raddr() const { return raddr; };

public:
  /**
   *
   */
  const std::string &get_tls_capath() const { return capath; };

  /**
   *
   */
  crofsock &set_tls_capath(const std::string &capath) {
    this->capath = capath;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_cafile() const { return cafile; };

  /**
   *
   */
  crofsock &set_tls_cafile(const std::string &cafile) {
    this->cafile = cafile;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_certfile() const { return certfile; };

  /**
   *
   */
  crofsock &set_tls_certfile(const std::string &certfile) {
    this->certfile = certfile;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_keyfile() const { return keyfile; };

  /**
   *
   */
  crofsock &set_tls_keyfile(const std::string &keyfile) {
    this->keyfile = keyfile;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_pswd() const { return password; };

  /**
   *
   */
  crofsock &set_tls_pswd(const std::string &password) {
    this->password = password;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_verify_mode() const { return verify_mode; };

  /**
   *
   */
  crofsock &set_tls_verify_mode(const std::string &verify_mode) {
    this->verify_mode = verify_mode;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_verify_depth() const { return verify_depth; };

  /**
   *
   */
  crofsock &set_tls_verify_depth(const std::string &verify_depth) {
    this->verify_depth = verify_depth;
    return *this;
  };

public:
  /**
   *
   */
  const std::string &get_tls_ciphers() const { return ciphers; };

  /**
   *
   */
  crofsock &set_tls_ciphers(const std::string &ciphers) {
    this->ciphers = ciphers;
    return *this;
  };

  friend std::ostream &operator<<(std::ostream &os, crofsock const &rofsock) {
    os << "<crofsock: transport-connection-established: "
       << rofsock.is_established() << ">" << std::endl;
    return os;
  };

  std::string str() const {
    std::stringstream ss;
    switch (state.load()) {
    case STATE_IDLE: {
      ss << "state: -IDLE- ";
    } break;
    case STATE_LISTENING: {
      ss << "state: -LISTENING- ";
    } break;
    case STATE_TCP_ACCEPTING: {
      ss << "state: -ACCEPTING- ";
    } break;
    case STATE_TLS_ACCEPTING: {
      ss << "state: -TLS-ACCEPTING- ";
    } break;
    case STATE_TCP_CONNECTING: {
      ss << "state: -CONNECTING- ";
    } break;
    case STATE_TLS_CONNECTING: {
      ss << "state: -TLS-CONNECTING- ";
    } break;
    case STATE_TCP_ESTABLISHED: {
      ss << "state: -ESTABLISHED- ";
    } break;
    case STATE_TLS_ESTABLISHED: {
      ss << "state: -TLS-ESTABLISHED- ";
    } break;
    default: { ss << "state: -unknown- "; };
    }
    return ss.str();
  };

private:
  /**
   * @brief	Private copy constructor for suppressing any copy attempt.
   */
  crofsock(const crofsock &socket);

  /**
   * @brief	Private assignment operator.
   */
  crofsock &operator=(const crofsock &socket);

private:
  void flag_set(crofsock_flag_t __flag, bool value) {
    AcquireReadWriteLock lock(flags_lock);
    flags.set(__flag, value);
  };

  bool flag_test(crofsock_flag_t __flag) const {
    AcquireReadLock lock(flags_lock);
    return flags.test(__flag);
  };

  bool delete_in_progress() const {
    return (flag_test(FLAG_DELETE_IN_PROGRESS));
  };

  void set_state(enum socket_state_t state) {
    AcquireReadWriteLock lock(tlock);
    this->state = state;
  };

private:
  virtual void handle_wakeup(cthread &thread);

  virtual void handle_timeout(cthread &thread, uint32_t timer_id);

  virtual void handle_read_event(cthread &thread, int fd);

  virtual void handle_write_event(cthread &thread, int fd);

private:
  void tls_init_context();

  void tls_term_context();

  static int tls_pswd_cb(char *buf, int size, int rwflag, void *userdata);

  bool tls_verify_ok();

  void tls_log_errors();

private:
  void recv_message();

  void parse_message();

  void parse_of10_message(rofl::openflow::cofmsg **pmsg);

  void parse_of12_message(rofl::openflow::cofmsg **pmsg);

  void parse_of13_message(rofl::openflow::cofmsg **pmsg);

  void send_from_queue();

private:
  void backoff_reconnect(bool reset_timeout = false);

  void handle_read_event_rxthread(cthread &thread, int fd);

  void handle_write_event_rxthread(cthread &thread, int fd);

private:
  // environment for this crofsock instance
  crofsock_env *env;

  // various flags for this crofsock instance
  std::bitset<32> flags;

  // and the associated rwlock
  mutable rofl::crwlock flags_lock;

  // rwlock for internal state
  mutable rofl::crwlock tlock;

  // connection state
  std::atomic<socket_state_t> state;

  // socket mode (TCP_SERVER, TCP CLIENT)
  std::atomic<socket_mode_t> mode;

  // RX thread
  uint32_t rx_thread_num;

  // TX thread
  uint32_t tx_thread_num;

  /*
   * reconnect
   */
  int reconnect_backoff_max;
  int reconnect_backoff_start;
  int reconnect_backoff_current;
  int reconnect_counter;

  /* socket parameters */

  // socket descriptor
  int sd;

  // socket domain
  int domain;

  // socket type
  int type;

  // socket protocol
  int protocol;

  // socket backlog
  int backlog;

  /* binding addrees */
  csockaddr baddr;

  /* local address */
  csockaddr laddr;

  /* remote address */
  csockaddr raddr;

  /*
   * OpenSSL related structures
   */

  // SSL context
  SSL_CTX *ctx;

  // SSL session
  SSL *ssl;

  // basic input/output
  BIO *bio;

  // openssl rwlock
  crwlock sslock;

  std::string capath;
  std::string cafile;
  std::string certfile;
  std::string keyfile;
  std::string password;
  std::string verify_mode;
  std::string verify_depth;
  std::string ciphers;

  /*
   * receiving messages
   */

  // incomplete fragment message fragment received in last round
  cbuffer rxbuffer;

  // flag for RX reception on socket
  std::atomic_bool rx_disabled;

  /*
   * sending messages
   */

  // transmission buffer for packing cofmsg instances
  cbuffer txbuffer;

  // flag for TX reception on socket
  std::atomic_bool tx_disabled;

  // txthread is actively sending messages
  std::atomic_bool tx_is_running;

  /*
   * scheduler and txqueues
   */

  // number of packets waiting for transmission
  std::atomic_uint txqueue_pending_pkts;

  // size of tx queue when congestion occurred
  unsigned int txqueue_size_congestion_occurred;

  // size of tx queue for reallowing transmissions
  unsigned int txqueue_size_tx_threshold;

  // QUEUE_MAX txqueues
  std::vector<crofqueue> txqueues;

  // relative scheduling weights for txqueues
  std::vector<unsigned int> txweights;
};

} /* namespace rofl */

#endif /* CROFSOCK_H_ */
