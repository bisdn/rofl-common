/*
 * crofchantest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_CROFCHAN_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_CROFCHAN_TEST_HPP_

#include <atomic>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <set>

#include "rofl/common/crofchan.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/cthread.hpp"

class crofchantest : public CppUnit::TestFixture,
                     public rofl::crofchan_env,
                     public rofl::crofconn_env,
                     public rofl::crofsock_env,
                     public rofl::cthread_env {
  CPPUNIT_TEST_SUITE(crofchantest);
  CPPUNIT_TEST(test_connections);
  CPPUNIT_TEST(test_congestion);
  CPPUNIT_TEST_SUITE_END();

public:
  virtual ~crofchantest(){};

  crofchantest()
      : listening_port(0), keep_running(true), dpid(0), xid(0), num_of_conns(0),
        num_of_accepts(0), num_of_dpt_established(0), num_of_ctl_established(0),
        rofsock(nullptr), channel1(nullptr), channel2(nullptr),
        num_of_pkts_sent(0), num_of_pkts_rcvd(0),
        thread_num(rofl::cthread::get_mgt_thread_num_from_pool()),
        max_congestion_rounds(16){};

public:
  void setUp();
  void tearDown();

public:
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

  rofl::crandom rand;
  rofl::csockaddr baddr;
  rofl::crofsock *rofsock;
  rofl::crofchan *channel1;
  rofl::crofchan *channel2;
  rofl::crwlock rwlock;

  rofl::crwlock plock;
  std::set<rofl::crofconn *> pending_conns;

  std::atomic_int num_of_pkts_sent;
  std::atomic_int num_of_pkts_rcvd;
  std::atomic_bool congested;

  uint32_t thread_num;
  std::atomic_int max_congestion_rounds;

  rofl::crwlock tlock;

  std::string testname;

private:
  enum crofchantest_timer_t {
    TIMER_ID_START_SENDING_PACKET_INS = 1,
  };

  virtual void handle_wakeup(rofl::cthread &thread){};

  virtual void handle_timeout(rofl::cthread &thread, uint32_t timer_id);

  virtual void handle_read_event(rofl::cthread &thread, int fd){};

  virtual void handle_write_event(rofl::cthread &thread, int fd){};

private:
  virtual void handle_established(rofl::crofchan &chan, uint8_t ofp_version) {
    LOG(INFO) << "crofchan::handle_established" << std::endl;
  };

  virtual void handle_closed(rofl::crofchan &chan) {
    LOG(INFO) << "crofchan::handle_closed" << std::endl;
  };

  virtual void handle_established(rofl::crofchan &chan, rofl::crofconn &conn,
                                  uint8_t ofp_version);

  virtual void handle_closed(rofl::crofchan &chan, rofl::crofconn &conn) {
    LOG(INFO) << "crofchan::handle_closed" << std::endl;
  };

  virtual void handle_connect_refused(rofl::crofchan &chan,
                                      rofl::crofconn &conn) {
    LOG(INFO) << "crofchan::handle_connect_refused" << std::endl;
  };

  virtual void handle_connect_failed(rofl::crofchan &chan,
                                     rofl::crofconn &conn) {
    LOG(INFO) << "crofchan::handle_connect_failed" << std::endl;
  };

  virtual void handle_accept_failed(rofl::crofchan &chan,
                                    rofl::crofconn &conn) {
    LOG(INFO) << "crofchan::handle_accept_failed" << std::endl;
  };

  virtual void handle_negotiation_failed(rofl::crofchan &chan,
                                         rofl::crofconn &conn) {
    LOG(INFO) << "crofchan::handle_negotiation_failed: pending_conns: "
              << pending_conns.size() << std::endl;
    LOG(INFO) << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<" << std::endl;

    rofl::AcquireReadWriteLock lock(plock);
    CPPUNIT_ASSERT(false);
  };

  virtual void congestion_solved_indication(rofl::crofchan &chan,
                                            rofl::crofconn &conn);

  virtual void handle_recv(rofl::crofchan &chan, rofl::crofconn &conn,
                           rofl::openflow::cofmsg *msg);

  virtual uint32_t get_async_xid(rofl::crofchan &chan) { return 0; };

  virtual uint32_t get_sync_xid(rofl::crofchan &chan, uint8_t msg_type = 0,
                                uint16_t msg_sub_type = 0) {
    return 0;
  };

  virtual void release_sync_xid(rofl::crofchan &chan, uint32_t xid) {
    LOG(INFO) << "crofchan::release_sync_xid" << std::endl;
  };

  virtual void congestion_occurred_indication(rofl::crofchan &chan,
                                              rofl::crofconn &conn);

  virtual void handle_transaction_timeout(rofl::crofchan &chan,
                                          rofl::crofconn &conn, uint32_t xid,
                                          uint8_t type, uint16_t sub_type = 0) {
    LOG(INFO) << "crofchan::handle_transaction_timeout" << std::endl;
  };

private:
  virtual void handle_established(rofl::crofconn &conn, uint8_t ofp_version);

  virtual void handle_connect_refused(rofl::crofconn &conn) {
    LOG(INFO) << "crofconn::handle_connect_refused" << std::endl;
  };

  virtual void handle_connect_failed(rofl::crofconn &conn) {
    LOG(INFO) << "crofconn::handle_connect_failed" << std::endl;
  };

  virtual void handle_accept_failed(rofl::crofconn &conn) {
    LOG(INFO) << "crofconn::handle_accept_failed" << std::endl;
  };

  virtual void handle_negotiation_failed(rofl::crofconn &conn) {
    LOG(INFO) << "crofconn::handle_negotiation_failed" << std::endl;
    LOG(INFO) << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<" << std::endl;
    CPPUNIT_ASSERT(false);
  };

  virtual void handle_closed(rofl::crofconn &conn) {
    LOG(INFO) << "crofconn::handle_closed" << std::endl;
    LOG(INFO) << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<" << std::endl;
    CPPUNIT_ASSERT(false);
  };

  virtual void handle_recv(rofl::crofconn &conn, rofl::openflow::cofmsg *msg) {
    LOG(INFO) << "crofconn::handle_recv" << std::endl;
  };

  virtual void congestion_occurred_indication(rofl::crofconn &conn) {
    LOG(INFO) << "crofconn::congestion_occurred_indication" << std::endl;
  };

  virtual void congestion_solved_indication(rofl::crofconn &conn) {
    LOG(INFO) << "crofconn::congestion_solved_indication" << std::endl;
  };

  virtual void handle_transaction_timeout(rofl::crofconn &conn, uint32_t xid,
                                          uint8_t type, uint16_t sub_type = 0) {
    LOG(INFO) << "crofconn::handle_transaction_timeout" << std::endl;
  };

private:
  virtual void handle_listen(rofl::crofsock &socket);

  virtual void handle_tcp_connect_refused(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tcp_connect_refused" << std::endl;
  };

  virtual void handle_tcp_connect_failed(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tcp_connect_failed" << std::endl;
  };

  virtual void handle_tcp_connected(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tcp_connected" << std::endl;
  };

  virtual void handle_tcp_accept_refused(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tcp_accept_refused" << std::endl;
  };

  virtual void handle_tcp_accept_failed(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tcp_accept_failed" << std::endl;
  };

  virtual void handle_tcp_accepted(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tcp_accepted" << std::endl;
  };

  virtual void handle_tls_connect_failed(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tls_connect_failed" << std::endl;
  };

  virtual void handle_tls_connected(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tls_connected" << std::endl;
  };

  virtual void handle_tls_accept_failed(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tls_accept_failed" << std::endl;
  };

  virtual void handle_tls_accepted(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_tls_accepted" << std::endl;
  };

  virtual void handle_closed(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::handle_closed" << std::endl;
  };

  virtual void congestion_solved_indication(rofl::crofsock &socket) {
    LOG(INFO) << "congestion_solved_indication" << std::endl;
  };

  virtual void handle_recv(rofl::crofsock &socket,
                           rofl::openflow::cofmsg *msg) {
    LOG(INFO) << "crofsock::handle_recv" << std::endl;
  };

  virtual void congestion_occurred_indication(rofl::crofsock &socket) {
    LOG(INFO) << "crofsock::congestion_occurred_indication" << std::endl;
  };
};

#endif /* TEST_SRC_ROFL_COMMON_CROFCHAN_TEST_HPP_ */
