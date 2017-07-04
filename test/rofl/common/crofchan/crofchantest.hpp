/*
 * crofchantest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_

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

  crofchantest() :
	  thread(this),
	  num_of_pkts_sent(0),
	  num_of_pkts_rcvd(0),
	  max_congestion_rounds(16) {};

public:
  void setUp();
  void tearDown();

public:
  void test_connections();
  void test_congestion();

private:
  uint16_t listening_port;
  bool keep_running;
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

  int num_of_pkts_sent;
  int num_of_pkts_rcvd;
  std::atomic_bool congested;

  rofl::cthread thread;
  int max_congestion_rounds;

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
    std::cerr << "crofchan::handle_established" << std::endl;
  };

  virtual void handle_closed(rofl::crofchan &chan) {
    std::cerr << "crofchan::handle_closed" << std::endl;
  };

  virtual void handle_established(rofl::crofchan &chan, rofl::crofconn &conn,
                                  uint8_t ofp_version);

  virtual void handle_closed(rofl::crofchan &chan, rofl::crofconn &conn) {
    std::cerr << "crofchan::handle_closed" << std::endl;
  };

  virtual void handle_connect_refused(rofl::crofchan &chan,
                                      rofl::crofconn &conn) {
    std::cerr << "crofchan::handle_connect_refused" << std::endl;
  };

  virtual void handle_connect_failed(rofl::crofchan &chan,
                                     rofl::crofconn &conn) {
    std::cerr << "crofchan::handle_connect_failed" << std::endl;
  };

  virtual void handle_accept_failed(rofl::crofchan &chan,
                                    rofl::crofconn &conn) {
    std::cerr << "crofchan::handle_accept_failed" << std::endl;
  };

  virtual void handle_negotiation_failed(rofl::crofchan &chan,
                                         rofl::crofconn &conn) {
    std::cerr << "crofchan::handle_negotiation_failed: pending_conns: "
              << pending_conns.size() << std::endl;
    std::cerr << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
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
    std::cerr << "crofchan::release_sync_xid" << std::endl;
  };

  virtual void congestion_occured_indication(rofl::crofchan &chan,
                                             rofl::crofconn &conn);

  virtual void handle_transaction_timeout(rofl::crofchan &chan,
                                          rofl::crofconn &conn, uint32_t xid,
                                          uint8_t type, uint16_t sub_type = 0) {
    std::cerr << "crofchan::handle_transaction_timeout" << std::endl;
  };

private:
  virtual void handle_established(rofl::crofconn &conn, uint8_t ofp_version);

  virtual void handle_connect_refused(rofl::crofconn &conn) {
    std::cerr << "crofconn::handle_connect_refused" << std::endl;
  };

  virtual void handle_connect_failed(rofl::crofconn &conn) {
    std::cerr << "crofconn::handle_connect_failed" << std::endl;
  };

  virtual void handle_accept_failed(rofl::crofconn &conn) {
    std::cerr << "crofconn::handle_accept_failed" << std::endl;
  };

  virtual void handle_negotiation_failed(rofl::crofconn &conn) {
    std::cerr << "crofconn::handle_negotiation_failed" << std::endl;
    std::cerr << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<" << std::endl;
    CPPUNIT_ASSERT(false);
  };

  virtual void handle_closed(rofl::crofconn &conn) {
    std::cerr << "crofconn::handle_closed" << std::endl;
    std::cerr << ">>>>>>>>>>>>> auxid=" << (int)conn.get_auxid().get_id()
              << " <<<<<<<<<<<<<<" << std::endl;
    CPPUNIT_ASSERT(false);
  };

  virtual void handle_recv(rofl::crofconn &conn, rofl::openflow::cofmsg *msg) {
    std::cerr << "crofconn::handle_recv" << std::endl;
  };

  virtual void congestion_occured_indication(rofl::crofconn &conn) {
    std::cerr << "crofconn::congestion_occured_indication" << std::endl;
  };

  virtual void congestion_solved_indication(rofl::crofconn &conn) {
    std::cerr << "crofconn::congestion_solved_indication" << std::endl;
  };

  virtual void handle_transaction_timeout(rofl::crofconn &conn, uint32_t xid,
                                          uint8_t type, uint16_t sub_type = 0) {
    std::cerr << "crofconn::handle_transaction_timeout" << std::endl;
  };

private:
  virtual void handle_listen(rofl::crofsock &socket);

  virtual void handle_tcp_connect_refused(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tcp_connect_refused" << std::endl;
  };

  virtual void handle_tcp_connect_failed(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tcp_connect_failed" << std::endl;
  };

  virtual void handle_tcp_connected(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tcp_connected" << std::endl;
  };

  virtual void handle_tcp_accept_refused(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tcp_accept_refused" << std::endl;
  };

  virtual void handle_tcp_accept_failed(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tcp_accept_failed" << std::endl;
  };

  virtual void handle_tcp_accepted(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tcp_accepted" << std::endl;
  };

  virtual void handle_tls_connect_failed(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tls_connect_failed" << std::endl;
  };

  virtual void handle_tls_connected(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tls_connected" << std::endl;
  };

  virtual void handle_tls_accept_failed(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tls_accept_failed" << std::endl;
  };

  virtual void handle_tls_accepted(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_tls_accepted" << std::endl;
  };

  virtual void handle_closed(rofl::crofsock &socket) {
    std::cerr << "crofsock::handle_closed" << std::endl;
  };

  virtual void congestion_solved_indication(rofl::crofsock &socket) {
    std::cerr << "congestion_solved_indication" << std::endl;
  };

  virtual void handle_recv(rofl::crofsock &socket,
                           rofl::openflow::cofmsg *msg) {
    std::cerr << "crofsock::handle_recv" << std::endl;
  };

  virtual void congestion_occured_indication(rofl::crofsock &socket) {
    std::cerr << "crofsock::congestion_occured_indication" << std::endl;
  };
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_ */
