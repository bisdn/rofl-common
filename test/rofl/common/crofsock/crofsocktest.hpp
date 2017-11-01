/*
 * crofsocktest.hpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#ifndef TEST_SRC_ROFL_COMMON_CROFSOCK_TEST_HPP_
#define TEST_SRC_ROFL_COMMON_CROFSOCK_TEST_HPP_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/crandom.h"
#include "rofl/common/crofsock.h"

class crofsocktest : public CppUnit::TestFixture, public rofl::crofsock_env {
  CPPUNIT_TEST_SUITE(crofsocktest);
  CPPUNIT_TEST(global_initialize);
  CPPUNIT_TEST(test);
/* Google's address sanitizer complains about lots of openssl's
 * code fragments upon shutdown of the test application.
 * We have to rewrite the clean-up code for openssl to cope with
 * the various versions of openssl to be found on various Linux flavours:
 * Fedora, CentOS, Ubuntu, Debian, ... Seeking volunteers for this bleak effort.
 * For now, we disable the TLS test when ASAN is enabled.
 * See [1] for further information regarding openssl's init/deinit blocks.
 * [1] https://wiki.openssl.org/index.php/Library_Initialization */
#ifndef ASAN
  CPPUNIT_TEST(test_tls);
#endif
  CPPUNIT_TEST(global_terminate);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

public:
  void global_initialize();
  void test();
  void test_tls();
  void global_terminate();

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
  rofl::crwlock tlock;

  static std::string cacert;
  static std::string clicert;
  static std::string clikey;
  static std::string srvcert;
  static std::string srvkey;
};

#endif /* TEST_SRC_ROFL_COMMON_CROFSOCK_TEST_HPP_ */
