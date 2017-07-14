/*
 * crofsocktest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/crandom.h"
#include "rofl/common/crofsock.h"

class crofsocktest : public CppUnit::TestFixture, public rofl::crofsock_env {
  CPPUNIT_TEST_SUITE(crofsocktest);
  CPPUNIT_TEST(test);
  //	CPPUNIT_TEST( test_tls );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void test();
  void test_tls();

private:
  virtual void handle_listen(rofl::crofsock &socket);

  virtual void handle_tcp_connect_refused(rofl::crofsock &socket);

  virtual void handle_tcp_connect_failed(rofl::crofsock &socket);

  virtual void handle_tcp_connected(rofl::crofsock &socket);

  virtual void handle_tcp_accept_refused(rofl::crofsock &socket);

  virtual void handle_tcp_accept_failed(rofl::crofsock &socket);

  virtual void handle_tcp_accepted(rofl::crofsock &socket);

  virtual void handle_tls_connect_failed(rofl::crofsock &socket);

  virtual void handle_tls_connected(rofl::crofsock &socket);

  virtual void handle_tls_accept_failed(rofl::crofsock &socket);

  virtual void handle_tls_accepted(rofl::crofsock &socket);

  virtual void handle_closed(rofl::crofsock &socket);

  virtual void congestion_solved_indication(rofl::crofsock &socket);

  virtual void handle_recv(rofl::crofsock &socket, rofl::openflow::cofmsg *msg);

  virtual void congestion_occurred_indication(rofl::crofsock &socket);

private:
  enum crofsock_test_mode_t {
    TEST_MODE_TCP = 1,
    TEST_MODE_TLS = 2,
  };

  enum crofsock_test_mode_t test_mode;
  std::atomic_bool keep_running;
  int timeout;
  int msg_counter;
  std::atomic_int server_msg_counter;
  std::atomic_int client_msg_counter;
  rofl::crandom rand;
  uint16_t listening_port;
  rofl::csockaddr baddr;
  rofl::crofsock *slisten;
  rofl::crofsock *sclient;
  rofl::crofsock *sserver;
};

#endif /* TEST_SRC_ROFL_COMMON_OPENFLOW_MESSAGES_COFMSGAGGRSTATS_TEST_HPP_ */
