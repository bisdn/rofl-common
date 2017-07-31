/*
 * crofconntest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/crofconn.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/cthread.hpp"

class crofconntest : public CppUnit::TestFixture,
                     public rofl::crofconn_env,
                     public rofl::crofsock_env {
  CPPUNIT_TEST_SUITE(crofconntest);
  CPPUNIT_TEST(test);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void test();

private:
  virtual void handle_listen(rofl::crofsock &socket);

  virtual void handle_tcp_connect_refused(rofl::crofsock &socket){};

  virtual void handle_tcp_connect_failed(rofl::crofsock &socket){};

  virtual void handle_tcp_connected(rofl::crofsock &socket){};

  virtual void handle_tcp_accept_refused(rofl::crofsock &socket){};

  virtual void handle_tcp_accept_failed(rofl::crofsock &socket){};

  virtual void handle_tcp_accepted(rofl::crofsock &socket){};

  virtual void handle_tls_connect_failed(rofl::crofsock &socket){};

  virtual void handle_tls_connected(rofl::crofsock &socket){};

  virtual void handle_tls_accept_failed(rofl::crofsock &socket){};

  virtual void handle_tls_accepted(rofl::crofsock &socket){};

  virtual void handle_closed(rofl::crofsock &socket){};

  virtual void congestion_solved_indication(rofl::crofsock &socket){};

  virtual void handle_recv(rofl::crofsock &socket,
                           rofl::openflow::cofmsg *msg){};

  virtual void congestion_occurred_indication(rofl::crofsock &socket){};

private:
  virtual void handle_established(rofl::crofconn &conn, uint8_t ofp_version);

  virtual void handle_connect_refused(rofl::crofconn &conn);

  virtual void handle_connect_failed(rofl::crofconn &conn);

  virtual void handle_accept_failed(rofl::crofconn &conn);

  virtual void handle_negotiation_failed(rofl::crofconn &conn);

  virtual void handle_closed(rofl::crofconn &conn);

  virtual void congestion_solved_indication(rofl::crofconn &conn);

  virtual void handle_recv(rofl::crofconn &conn, rofl::openflow::cofmsg *msg);

  virtual void congestion_occurred_indication(rofl::crofconn &conn);

  virtual void handle_transaction_timeout(rofl::crofconn &conn, uint32_t xid,
                                          uint8_t type,
                                          uint16_t sub_type = 0){};

private:
  void send_packet_in(uint8_t version);

  void send_packet_out(uint8_t version);

private:
  enum crofconn_test_mode_t {
    TEST_MODE_TCP = 1,
  };

  rofl::openflow::cofhello_elem_versionbitmap versionbitmap_ctl;
  rofl::openflow::cofhello_elem_versionbitmap versionbitmap_dpt;
  enum crofconn_test_mode_t test_mode;
  int keep_running;
  int msg_counter;
  rofl::crofsock *slisten;
  rofl::crofconn *sclient;
  rofl::crofconn *sserver;
  rofl::cthread tclient;
  rofl::cthread tserver;
  std::atomic_uint_fast32_t xid_server;
  uint32_t xid_client;
  uint16_t listening_port;
  rofl::crandom rand;
  rofl::csockaddr baddr;

  uint32_t xid;
  std::atomic_uint_fast64_t dpid;
  std::atomic_uint_fast8_t auxid;
  std::atomic_uint_fast32_t n_buffers;
  std::atomic_uint_fast8_t n_tables;
  rofl::openflow::cofports ports;

  std::atomic_int num_of_packets;
  std::atomic_int srv_pkts_rcvd;
  std::atomic_int srv_pkts_sent;
  std::atomic_int cli_pkts_rcvd;
  std::atomic_int cli_pkts_sent;
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_ */
