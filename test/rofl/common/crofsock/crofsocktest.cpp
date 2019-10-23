/*
 * crofsocktest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <glog/logging.h>

#include "crofsocktest.hpp"

using namespace rofl::openflow;

#ifdef CACERT
/*static*/ std::string crofsocktest::cacert(CACERT);
#else
/*static*/ std::string
    crofsocktest::cacert("../../../../../tools/xca/ca.rofl-core.crt.pem");
#endif
#ifdef CLICERT
/*static*/ std::string crofsocktest::clicert(CLICERT);
#else
/*static*/ std::string
    crofsocktest::clicert("../../../../../tools/xca/client.crt.pem");
#endif
#ifdef CLIKEY
/*static*/ std::string crofsocktest::clikey(CLIKEY);
#else
/*static*/ std::string
    crofsocktest::clikey("../../../../../tools/xca/client.key.pem");
#endif
#ifdef SRVCERT
/*static*/ std::string crofsocktest::srvcert(SRVCERT);
#else
/*static*/ std::string
    crofsocktest::srvcert("../../../../../tools/xca/server.crt.pem");
#endif
#ifdef SRVKEY
/*static*/ std::string crofsocktest::srvkey(SRVKEY);
#else
/*static*/ std::string
    crofsocktest::srvkey("../../../../../tools/xca/server.key.pem");
#endif

CPPUNIT_TEST_SUITE_REGISTRATION(crofsocktest);

void crofsocktest::setUp() {
  baddr = rofl::csockaddr(AF_INET, "0.0.0.0", 0);
  server_msg_counter = 0;
  client_msg_counter = 0;
}

void crofsocktest::tearDown() {}

void crofsocktest::global_initialize() { rofl::cthread::pool_initialize(); }

void crofsocktest::global_terminate() { rofl::cthread::pool_terminate(); }

void crofsocktest::test() {
  try {
    for (unsigned int i = 0; i < 2; i++) {
      std::cerr << "TCP ROUND (" << i << ") START" << std::endl;
      test_mode = TEST_MODE_TCP;
      keep_running = true;
      timeout = 60;
      msg_counter = 0;
      listening_port = 6653;
      server_msg_counter = 0;
      client_msg_counter = 0;

      slisten = new rofl::crofsock(this);
      sclient = new rofl::crofsock(this);

      /* try to find idle port for test */
      bool lookup_idle_port = true;
      while (lookup_idle_port) {
        try {
          LOG(INFO) << "trying listening port=" << (int)listening_port
                    << std::endl;
          baddr =
              rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), listening_port);
          /* try to bind address first */
          slisten->set_baddr(baddr).listen();
          LOG(INFO) << "binding to " << baddr.str() << std::endl;
          lookup_idle_port = false;
          break;
        } catch (rofl::eSysCall &e) {
          /* port in use, try another one */
        }
        do {
          listening_port = rand.uint16();
        } while ((listening_port < 10000) || (listening_port > 49000));
      }

      sclient->set_raddr(baddr).tcp_connect(false);

      pthread_yield();

      while (keep_running && (--timeout > 0)) {
        struct timespec ts;
        ts.tv_sec = 1;
        ts.tv_nsec = 0;
        pselect(0, NULL, NULL, NULL, &ts, NULL);
        std::cerr << ".";
      }

      CPPUNIT_ASSERT(timeout > 0);

      // sleep(1);

      slisten->close();
      sclient->close();
      sserver->close();

      sleep(1);

      delete slisten;
      delete sclient;
      delete sserver;

      sleep(1);

      std::cerr << "TCP ROUND (" << i << ") END" << std::endl;
    }

  } catch (rofl::eSysCall &e) {
    LOG(INFO) << "crofsocktest::test() exception, what: " << e.what()
              << std::endl;
  } catch (std::runtime_error &e) {
    LOG(INFO) << "crofsocktest::test() exception, what: " << e.what()
              << std::endl;
  }
}

void crofsocktest::test_tls() {
  try {
    for (unsigned int i = 0; i < 2; i++) {
      std::cerr << "TLS ROUND (" << i << ") START" << std::endl;
      test_mode = TEST_MODE_TLS;
      keep_running = true;
      timeout = 60;
      msg_counter = 0;
      listening_port = 6653;
      server_msg_counter = 0;
      client_msg_counter = 0;

      slisten = new rofl::crofsock(this);
      sclient = new rofl::crofsock(this);

      /* try to find idle port for test */
      bool lookup_idle_port = true;
      while (lookup_idle_port) {
        try {
          LOG(INFO) << "trying listening port=" << (int)listening_port
                    << std::endl;
          baddr =
              rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), listening_port);
          /* try to bind address first */
          slisten->set_baddr(baddr).listen();
          LOG(INFO) << "binding to " << baddr.str() << std::endl;
          lookup_idle_port = false;
          break;
        } catch (rofl::eSysCall &e) {
          /* port in use, try another one */
        }
        do {
          listening_port = rand.uint16();
        } while ((listening_port < 10000) || (listening_port > 49000));
      }

      sclient->set_raddr(baddr)
          .set_tls_cafile(cacert)
          .set_tls_certfile(clicert)
          .set_tls_keyfile(clikey)
          .tls_connect(false);

      sleep(1);

      while (keep_running && (--timeout > 0)) {
        struct timespec ts;
        ts.tv_sec = 1;
        ts.tv_nsec = 0;
        pselect(0, NULL, NULL, NULL, &ts, NULL);
      }

      CPPUNIT_ASSERT(timeout > 0);

      // sleep(1);

      slisten->close();
      sclient->close();
      sserver->close();

      sleep(1);

      delete slisten;
      delete sclient;
      delete sserver;

      sleep(1);

      std::cerr << "TLS ROUND (" << i << ") END" << std::endl;
    }
  } catch (rofl::eSysCall &e) {
    LOG(INFO) << "crofsocktest::test() exception, what: " << e.what()
              << std::endl;
  } catch (std::runtime_error &e) {
    LOG(INFO) << "crofsocktest::test() exception, what: " << e.what()
              << std::endl;
  }
}

void crofsocktest::handle_listen(rofl::crofsock &socket) {
  LOG(INFO) << "crofsocktest::handle_listen()" << std::endl;

  for (auto sd : socket.accept()) {

    LOG(INFO) << "crofsocktest::handle_listen() sd=" << sd << std::endl;

    sserver = new rofl::crofsock(this);

    switch (test_mode) {
    case TEST_MODE_TCP: {
      sserver->tcp_accept(sd);

    } break;
    case TEST_MODE_TLS: {
      sserver->set_tls_cafile(cacert)
          .set_tls_certfile(srvcert)
          .set_tls_keyfile(srvkey)
          .tls_accept(sd);
    } break;
    default: {
    };
    }
  }
}

void crofsocktest::handle_tcp_connect_refused(rofl::crofsock &socket) {
  LOG(INFO) << "handle tcp connect refused" << std::endl;
}

void crofsocktest::handle_tcp_connect_failed(rofl::crofsock &socket) {
  LOG(INFO) << "handle tcp connect failed" << std::endl;
}

void crofsocktest::handle_tcp_connected(rofl::crofsock &socket) {
  LOG(INFO) << "handle tcp connected" << std::endl;

  switch (test_mode) {
  case TEST_MODE_TCP: {

    rofl::openflow::cofmsg_hello *hello =
        new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

    sclient->send_message(hello);

  } break;
  case TEST_MODE_TLS: {

    /* do nothing, wait for handle_tls_connected */

  } break;
  default: {
  };
  }
}

void crofsocktest::handle_tcp_accept_refused(rofl::crofsock &socket) {
  LOG(INFO) << "handle tcp accept refused" << std::endl;
}

void crofsocktest::handle_tcp_accept_failed(rofl::crofsock &socket) {
  LOG(INFO) << "handle tcp accept failed" << std::endl;
}

void crofsocktest::handle_tcp_accepted(rofl::crofsock &socket) {
  LOG(INFO) << "handle tcp accepted" << std::endl;

  switch (test_mode) {
  case TEST_MODE_TCP: {

    rofl::openflow::cofmsg_features_request *features =
        new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

    sserver->send_message(features);

  } break;
  case TEST_MODE_TLS: {

    /* do nothing, wait for handle_tls_accepted */

  } break;
  default: {
  };
  }
}

void crofsocktest::handle_closed(rofl::crofsock &socket) {
  LOG(INFO) << "handle closed" << std::endl;
}

void crofsocktest::congestion_solved_indication(rofl::crofsock &socket) {
  LOG(INFO) << "handle send" << std::endl;
}

void crofsocktest::congestion_occurred_indication(rofl::crofsock &socket) {
  LOG(INFO) << "congestion indication" << std::endl;
}

void crofsocktest::handle_recv(rofl::crofsock &socket,
                               rofl::openflow::cofmsg *msg) {
  rofl::AcquireReadWriteLock lock(tlock);
  if (&socket == sserver) {
    LOG(INFO) << "sserver => handle recv " << std::endl << *msg;
    delete msg;

    if (server_msg_counter < 10) {
      rofl::openflow::cofmsg_features_request *features =
          new cofmsg_features_request(rofl::openflow13::OFP_VERSION,
                                      0xb1b2b3b4);

      sserver->send_message(features);

      server_msg_counter++;
    }

    if ((server_msg_counter >= 10) && (client_msg_counter >= 10)) {
      LOG(INFO) << "[sserver] test done" << std::endl;
      keep_running = false;
    }

  } else if (&socket == sclient) {
    LOG(INFO) << "sclient => handle recv " << std::endl << *msg;
    delete msg;

    if (client_msg_counter < 10) {
      rofl::openflow::cofmsg_hello *hello =
          new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

      sclient->send_message(hello);

      client_msg_counter++;
    }

    if ((server_msg_counter >= 10) && (client_msg_counter >= 10)) {
      LOG(INFO) << "[sclient] test done" << std::endl;
      keep_running = false;
    }
  }
}

void crofsocktest::handle_tls_connect_failed(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls connect failed" << std::endl;
}

void crofsocktest::handle_tls_connected(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls connected" << std::endl;

  sleep(1);

  rofl::openflow::cofmsg_hello *hello =
      new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

  sclient->send_message(hello);
}

void crofsocktest::handle_tls_accept_failed(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls accept failed" << std::endl;
}

void crofsocktest::handle_tls_accepted(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls accepted" << std::endl;

  sleep(1);

  rofl::openflow::cofmsg_features_request *features =
      new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

  sserver->send_message(features);
}
