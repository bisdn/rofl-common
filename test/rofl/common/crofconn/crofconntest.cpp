/*
 * crofconntest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofconntest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(crofconntest);

void crofconntest::setUp() {}

void crofconntest::tearDown() {}

void crofconntest::test() {
  try {
    test_mode = TEST_MODE_TCP;
    keep_running = 10;
    msg_counter = 0;
    xid = 0xa1a2a3a4;
    num_of_packets = 2048;
    srv_pkts_rcvd = 0;
    srv_pkts_sent = 0;
    cli_pkts_rcvd = 0;
    cli_pkts_sent = 0;
    xid_client = 0;
    xid_server = 0;

    slisten = new rofl::crofsock(this);
    sclient = new rofl::crofconn(this);

    listening_port = 0;

    /* try to find idle port for test */
    bool lookup_idle_port = true;
    while (lookup_idle_port) {
      do {
        listening_port = rand.uint16();
      } while ((listening_port < 10000) || (listening_port > 49000));
      try {
        std::cerr << "trying listening port=" << (int)listening_port
                  << std::endl;
        baddr =
            rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), listening_port);
        /* try to bind address first */
        slisten->set_baddr(baddr).listen();
        std::cerr << "binding to " << baddr.str() << std::endl;
        lookup_idle_port = false;
      } catch (rofl::eSysCall &e) {
        /* port in use, try another one */
      }
    }

    /* create new crofconn instance and connect to peer */
    versionbitmap_dpt.add_ofp_version(rofl::openflow10::OFP_VERSION);
    versionbitmap_dpt.add_ofp_version(rofl::openflow12::OFP_VERSION);
    versionbitmap_dpt.add_ofp_version(rofl::openflow13::OFP_VERSION);
    std::cerr << "connecting to " << baddr.str() << std::endl;
    sclient->set_raddr(baddr).tcp_connect(
        versionbitmap_dpt, rofl::crofconn::MODE_DATAPATH, /*reconnect=*/false);

    while (--keep_running > 0) {
      struct timespec ts;
      ts.tv_sec = 1;
      ts.tv_nsec = 0;
      pselect(0, NULL, NULL, NULL, &ts, NULL);
      std::cerr << "s:" << srv_pkts_rcvd << "(" << cli_pkts_sent << "), ";
      std::cerr << "c:" << cli_pkts_rcvd << "(" << srv_pkts_sent << "), "
                << std::endl;
      if ((cli_pkts_rcvd >= num_of_packets) &&
          (srv_pkts_rcvd >= num_of_packets)) {
        break;
      }
    }
    std::cerr << std::endl;

    std::cerr << "s:" << srv_pkts_rcvd << "(" << cli_pkts_sent << "), ";
    std::cerr << "c:" << cli_pkts_rcvd << "(" << srv_pkts_sent << "), "
              << std::endl;

    delete slisten;
    delete sclient;
    delete sserver;

    std::cerr << "======================================================"
              << std::endl;

  } catch (rofl::eSysCall &e) {
    std::cerr << "crofconntest::test() exception, what: " << e.what()
              << std::endl;
  } catch (std::runtime_error &e) {
    std::cerr << "crofconntest::test() exception, what: " << e.what()
              << std::endl;
  }
}

void crofconntest::handle_listen(rofl::crofsock &socket) {

  for (auto sd : socket.accept()) {

    std::cerr << "crofconntest::handle_listen() sd=" << sd << std::endl;

    switch (test_mode) {
    case TEST_MODE_TCP: {
      versionbitmap_ctl.add_ofp_version(rofl::openflow10::OFP_VERSION);
      versionbitmap_ctl.add_ofp_version(rofl::openflow12::OFP_VERSION);
      versionbitmap_ctl.add_ofp_version(rofl::openflow13::OFP_VERSION);

      sserver = new rofl::crofconn(this);
      sserver->tcp_accept(sd, versionbitmap_ctl,
                          rofl::crofconn::MODE_CONTROLLER);

    } break;
    default: {};
    }
  }
}

void crofconntest::handle_established(rofl::crofconn &conn,
                                      uint8_t ofp_version) {
  std::cerr << "crofconntest::handle_established()" << std::endl;

  if (&conn == sserver) {
    std::cerr << "[Ss], ";
    send_packet_out(ofp_version);
  } else if (&conn == sclient) {
    std::cerr << "[Sc], ";
    send_packet_in(ofp_version);
  }
}

void crofconntest::handle_connect_refused(rofl::crofconn &conn) {
  try {
    std::cerr << "crofconntest::handle_connect_refused()" << std::endl << conn;
  } catch (...) {
  }
  CPPUNIT_ASSERT(false);
}

void crofconntest::handle_connect_failed(rofl::crofconn &conn) {
  try {
    std::cerr << "crofconntest::handle_connect_failed()" << std::endl << conn;
  } catch (...) {
  }
  CPPUNIT_ASSERT(false);
}

void crofconntest::handle_accept_failed(rofl::crofconn &conn) {
  try {
    std::cerr << "crofconntest::handle_accept_failed()" << std::endl << conn;
  } catch (...) {
  }
  CPPUNIT_ASSERT(false);
}

void crofconntest::handle_negotiation_failed(rofl::crofconn &conn) {
  try {
    std::cerr << "crofconntest::handle_negotiation_failed()" << std::endl
              << conn;
  } catch (...) {
  }
  CPPUNIT_ASSERT(false);
  assert(false);
}

void crofconntest::handle_closed(rofl::crofconn &conn) {
  std::cerr << "crofconntest::handle_closed()" << std::endl << conn;
}

void crofconntest::congestion_solved_indication(rofl::crofconn &conn) {
  std::cerr << "crofconntest::handle_send()" << std::endl << conn;
}

void crofconntest::congestion_occurred_indication(rofl::crofconn &conn) {
  std::cerr << "crofconntest::congestion_indication()" << std::endl << conn;
}

void crofconntest::handle_recv(rofl::crofconn &conn,
                               rofl::openflow::cofmsg *pmsg) {
  CPPUNIT_ASSERT(pmsg != nullptr);

  // std::cerr << "crofconntest::handle_recv() " << std::endl << *pmsg;

  dpid = 0xc1c2c3c4c5c6c7c8;
  auxid = 0xd1;
  n_buffers = 0xe1e2e3e4;
  n_tables = 0xff;

  switch (pmsg->get_type()) {
  case rofl::openflow::OFPT_FEATURES_REQUEST: {
    rofl::openflow::cofmsg_features_reply *msg =
        new rofl::openflow::cofmsg_features_reply(pmsg->get_version(),
                                                  pmsg->get_xid(), dpid,
                                                  n_buffers, n_tables, auxid);
    conn.send_message(msg);
  } break;
  case rofl::openflow::OFPT_PACKET_IN: {
    srv_pkts_rcvd++;
    // std::cerr << "RECV(Packet-In): s:" << srv_pkts_rcvd << "(" <<
    // cli_pkts_sent << "), ";
    if (srv_pkts_sent < num_of_packets) {
      send_packet_out(conn.get_version());
    }

  } break;
  case rofl::openflow::OFPT_PACKET_OUT: {
    cli_pkts_rcvd++;
    // std::cerr << "RECV(Packet-Out): c:" << cli_pkts_rcvd << "(" <<
    // srv_pkts_sent << "), ";
    if (cli_pkts_sent < num_of_packets) {
      send_packet_in(conn.get_version());
    }

  } break;
  default: { CPPUNIT_ASSERT(false); };
  }

  delete pmsg;
}

void crofconntest::send_packet_in(uint8_t version) {
  sclient->send_message(
      new rofl::openflow::cofmsg_packet_in(version, ++xid_client));
  cli_pkts_sent++;
}

void crofconntest::send_packet_out(uint8_t version) {
  sserver->send_message(
      new rofl::openflow::cofmsg_packet_out(version, ++xid_server));
  srv_pkts_sent++;
}
