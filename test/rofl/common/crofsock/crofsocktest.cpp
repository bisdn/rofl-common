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

CPPUNIT_TEST_SUITE_REGISTRATION(crofsocktest);

void crofsocktest::setUp() {
  tclient = new rofl::cthread();
  tserver = new rofl::cthread();
  tclient->start("tclient");
  tserver->start("tserver");
  baddr = rofl::csockaddr(AF_INET, "0.0.0.0", 0);

  slisten = new rofl::crofsock(tserver, this);
  /* try to find idle port for test */
  bool lookup_idle_port = true;
  while (lookup_idle_port) {
    do {
      listening_port = rand.uint16();
    } while ((listening_port < 10000) || (listening_port > 49000));
    try {
      LOG(INFO) << "trying listening port=" << (int)listening_port << std::endl;
      baddr = rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), listening_port);
      /* try to bind address first */
      slisten->set_baddr(baddr).listen();
      LOG(INFO) << "binding to " << baddr.str() << std::endl;
      lookup_idle_port = false;
    } catch (rofl::eSysCall &e) {
      /* port in use, try another one */
    }
  }
}

void crofsocktest::tearDown() {
  tclient->stop();
  tserver->stop();
  delete slisten;
  delete tclient;
  delete tserver;
}

void crofsocktest::test() {
  try {
    for (unsigned int i = 0; i < 2; i++) {
      test_mode = TEST_MODE_TCP;
      keep_running = true;
      timeout = 60;
      msg_counter = 0;

      sclient = new rofl::crofsock(tclient, this);
      sclient->set_raddr(baddr).tcp_connect(true);

      while (keep_running && (--timeout > 0)) {
        struct timespec ts;
        ts.tv_sec = 1;
        ts.tv_nsec = 0;
        pselect(0, NULL, NULL, NULL, &ts, NULL);
      }

      CPPUNIT_ASSERT(timeout > 0);

      sclient->close();
      sserver->close();

      sleep(2);

      delete sclient;
      delete sserver;
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
    test_mode = TEST_MODE_TLS;
    keep_running = true;
    timeout = 60;
    msg_counter = 0;

    sclient = new rofl::crofsock(tclient, this);
    sclient->set_raddr(baddr)
        .set_tls_cafile("../../../../../tools/xca/ca.rofl-core.crt.pem")
        .set_tls_certfile("../../../../../tools/xca/client.crt.pem")
        .set_tls_keyfile("../../../../../tools/xca/client.key.pem")
        .tls_connect(true);

    while (keep_running && (--timeout > 0)) {
      struct timespec ts;
      ts.tv_sec = 1;
      ts.tv_nsec = 0;
      pselect(0, NULL, NULL, NULL, &ts, NULL);
    }

    CPPUNIT_ASSERT(timeout > 0);

    sclient->close();
    sserver->close();

    sleep(2);

    delete sclient;
    delete sserver;

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

    sserver = new rofl::crofsock(tserver, this);

    switch (test_mode) {
    case TEST_MODE_TCP: {
      sserver->tcp_accept(sd);

    } break;
    case TEST_MODE_TLS: {
      sserver->set_tls_cafile("../../../../../tools/xca/ca.rofl-core.crt.pem")
          .set_tls_certfile("../../../../../tools/xca/server.crt.pem")
          .set_tls_keyfile("../../../../../tools/xca/server.key.pem")
          .tls_accept(sd);
    } break;
    default: {};
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
  LOG(INFO) << "handle connected" << std::endl;

  switch (test_mode) {
  case TEST_MODE_TCP: {

    rofl::openflow::cofmsg_hello *hello =
        new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

    sclient->send_message(hello);

  } break;
  case TEST_MODE_TLS: {

    rofl::openflow::cofmsg_hello *hello =
        new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

    sclient->send_message(hello);

  } break;
  default: {};
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

    rofl::openflow::cofmsg_features_request *features =
        new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

    sserver->send_message(features);

  } break;
  default: {};
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
  int rv;
  if (&socket == sserver) {
    LOG(INFO) << "sserver => handle recv " << std::endl << *msg;

    if (server_msg_counter < 10) {
      rofl::openflow::cofmsg_features_request *features =
          new cofmsg_features_request(rofl::openflow13::OFP_VERSION,
                                      0xb1b2b3b4);

      rv = sserver->send_message(features);
      LOG(INFO) << __FUNCTION__ << ": send_message returned " << rv;

      server_msg_counter++;
    }

    if ((server_msg_counter >= 10) && (client_msg_counter >= 10)) {
      LOG(INFO) << "[sserver] test done" << std::endl;
      keep_running = false;
    }

  } else if (&socket == sclient) {
    LOG(INFO) << "sclient => handle recv " << std::endl << *msg;

    if (client_msg_counter < 10) {
      rofl::openflow::cofmsg_hello *hello =
          new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

      rv = sclient->send_message(hello);
      LOG(INFO) << __FUNCTION__ << ": send_message returned " << rv;

      client_msg_counter++;
    }

    if ((server_msg_counter >= 10) && (client_msg_counter >= 10)) {
      LOG(INFO) << "[sclient] test done" << std::endl;
      keep_running = false;
    }
  } else {
    LOG(FATAL) << __FUNCTION__;
  }

  delete msg;
}

void crofsocktest::handle_tls_connect_failed(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls connect failed" << std::endl;
}

void crofsocktest::handle_tls_connected(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls connected" << std::endl;

  rofl::openflow::cofmsg_hello *hello =
      new cofmsg_hello(rofl::openflow13::OFP_VERSION, 0xa1a2a3a4);

  sclient->send_message(hello);
}

void crofsocktest::handle_tls_accept_failed(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls accept failed" << std::endl;
}

void crofsocktest::handle_tls_accepted(rofl::crofsock &socket) {
  LOG(INFO) << "handle tls accepted" << std::endl;

  rofl::openflow::cofmsg_features_request *features =
      new cofmsg_features_request(rofl::openflow13::OFP_VERSION, 0xb1b2b3b4);

  sserver->send_message(features);
}
