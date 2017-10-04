/*
 * crofchantest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#pragma once

#include <atomic>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <glog/logging.h>
#include <set>

#include "rofl/common/crofchan.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/cthread.hpp"

class crofchantest : public CppUnit::TestFixture,
                     public rofl::crofchan_env,
                     public rofl::crofconn_env,
                     public rofl::crofsock_env,
                     public rofl::cthread_timeout_event {
  CPPUNIT_TEST_SUITE(crofchantest);
  CPPUNIT_TEST(test_connections);
  CPPUNIT_TEST(test_congestion);
  CPPUNIT_TEST_SUITE_END();

public:
  virtual ~crofchantest(){};

  crofchantest()
      : num_of_pkts_sent(0), num_of_pkts_rcvd(0), max_congestion_rounds(16){};

  void setUp() override;
  void tearDown() override;
  void test_connections();
  void test_congestion();

private:
  uint16_t listening_port;
  std::atomic_bool keep_running;
  rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
  uint64_t dpid;
  uint32_t xid;

  unsigned int num_of_conns;
  std::atomic_uint num_of_accepts;
  unsigned int num_of_dpt_established;
  std::atomic_uint num_of_ctl_established;
  bool send_packets;

  rofl::crandom rand;
  rofl::csockaddr baddr;
  rofl::crofsock *rofsock;
  rofl::crofchan *channel1;
  rofl::crofchan *channel2;
  rofl::cthread *tchan1;
  rofl::cthread *tchan2;
  rofl::crwlock rwlock;

  rofl::crwlock plock;
  std::set<rofl::crofconn *> pending_conns;

  std::atomic_int num_of_pkts_sent;
  std::atomic_int num_of_pkts_rcvd;
  std::atomic_bool congested;
  std::atomic_int max_congestion_rounds;

  enum crofchantest_timer_t {
    TIMER_ID_START_SENDING_PACKET_INS = 1,
  };

  void handle_timeout(void *userdata) override;

  void handle_established(rofl::crofchan &chan, uint8_t ofp_version) override {
    LOG(INFO) << "crofchan::handle_established";
  };

  void handle_closed(rofl::crofchan &chan) override {
    LOG(INFO) << "crofchan::handle_closed";
  };

  void handle_established(rofl::crofchan &chan, rofl::crofconn &conn,
                          uint8_t ofp_version) override;

  void handle_closed(rofl::crofchan &chan, rofl::crofconn &conn) override {
    LOG(INFO) << "crofchan::handle_closed";
  };

  void handle_connect_refused(rofl::crofchan &chan,
                              rofl::crofconn &conn) override {
    LOG(INFO) << "crofchan::handle_connect_refused";
  };

  void handle_connect_failed(rofl::crofchan &chan,
                             rofl::crofconn &conn) override {
    LOG(INFO) << "crofchan::handle_connect_failed";
  };

  void handle_accept_failed(rofl::crofchan &chan,
                            rofl::crofconn &conn) override {
    LOG(INFO) << "crofchan::handle_accept_failed";
  };

  void handle_negotiation_failed(rofl::crofchan &chan,
                                 rofl::crofconn &conn) override {
    LOG(INFO) << "crofchan::handle_negotiation_failed: pending_conns: "
              << pending_conns.size();
    LOG(INFO) << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<";

    rofl::AcquireReadWriteLock lock(plock);
    CPPUNIT_ASSERT(false);
  };

  void congestion_solved_indication(rofl::crofchan &chan,
                                    rofl::crofconn &conn) override;

  void handle_recv(rofl::crofchan &chan, rofl::crofconn &conn,
                   rofl::openflow::cofmsg *msg) override;

  virtual uint32_t get_async_xid(rofl::crofchan &chan) { return 0; };

  virtual uint32_t get_sync_xid(rofl::crofchan &chan, uint8_t msg_type = 0,
                                uint16_t msg_sub_type = 0) {
    return 0;
  };

  virtual void release_sync_xid(rofl::crofchan &chan, uint32_t xid) {
    LOG(INFO) << "crofchan::release_sync_xid";
  };

  void congestion_occurred_indication(rofl::crofchan &chan,
                                      rofl::crofconn &conn) override;

  void handle_transaction_timeout(rofl::crofchan &chan, rofl::crofconn &conn,
                                  uint32_t xid, uint8_t type,
                                  uint16_t sub_type = 0) override {
    LOG(INFO) << "crofchan::handle_transaction_timeout";
  };

  void handle_established(rofl::crofconn &conn, uint8_t ofp_version) override;

  void handle_connect_refused(rofl::crofconn &conn) override {
    LOG(INFO) << "crofconn::handle_connect_refused";
  };

  void handle_connect_failed(rofl::crofconn &conn) override {
    LOG(INFO) << "crofconn::handle_connect_failed";
  };

  void handle_accept_failed(rofl::crofconn &conn) override {
    LOG(INFO) << "crofconn::handle_accept_failed";
  };

  void handle_negotiation_failed(rofl::crofconn &conn) override {
    LOG(INFO) << "crofconn::handle_negotiation_failed";
    LOG(INFO) << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<";
    CPPUNIT_ASSERT(false);
  };

  void handle_closed(rofl::crofconn &conn) override {
    LOG(INFO) << "crofconn::handle_closed";
    LOG(INFO) << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<";
    CPPUNIT_ASSERT(false);
  }

  void handle_recv(rofl::crofconn &conn, rofl::openflow::cofmsg *msg) override {
    LOG(INFO) << "crofconn::handle_recv XXXX";
  }

  void congestion_occurred_indication(rofl::crofconn &conn) override {
    LOG(INFO) << "crofconn::congestion_occurred_indication";
  };

  void congestion_solved_indication(rofl::crofconn &conn) override {
    LOG(INFO) << "crofconn::congestion_solved_indication";
  };

  void handle_transaction_timeout(rofl::crofconn &conn, uint32_t xid,
                                  uint8_t type,
                                  uint16_t sub_type = 0) override {
    LOG(INFO) << "crofconn::handle_transaction_timeout";
  };

  void handle_listen(rofl::crofsock &socket) override;

  void handle_tcp_connect_refused(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tcp_connect_refused";
  };

  void handle_tcp_connect_failed(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tcp_connect_failed";
  };

  void handle_tcp_connected(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tcp_connected";
  };

  void handle_tcp_accept_refused(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tcp_accept_refused";
  };

  void handle_tcp_accept_failed(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tcp_accept_failed";
  };

  void handle_tcp_accepted(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tcp_accepted";
  };

  void handle_tls_connect_failed(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tls_connect_failed";
  };

  void handle_tls_connected(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tls_connected";
  };

  void handle_tls_accept_failed(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tls_accept_failed";
  };

  void handle_tls_accepted(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_tls_accepted";
  };

  void handle_closed(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::handle_closed";
  };

  void congestion_solved_indication(rofl::crofsock &socket) override {
    LOG(INFO) << "congestion_solved_indication";
  };

  void handle_recv(rofl::crofsock &socket,
                   rofl::openflow::cofmsg *msg) override {
    LOG(INFO) << "crofsock::handle_recv";
  };

  void congestion_occurred_indication(rofl::crofsock &socket) override {
    LOG(INFO) << "crofsock::congestion_occurred_indication";
  };
};
