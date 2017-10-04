/*
 * crofchantest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <glog/logging.h>

#include "crofchantest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(crofchantest);

void crofchantest::setUp() {
  rofl::cthread::pool_terminate(); // in case it was initialized by crofchantest
                                   // itself
  rofl::cthread::pool_initialize(/*hnd*/ 1, /*io*/ 2, /*mgt*/ 1);
  versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
  baddr = rofl::csockaddr(AF_INET, "127.0.0.1", 16455);
  dpid = 0xa1a2a3a4a5a6a7a8;
  xid = 0xb1b2b3b4;

  rofsock = new rofl::crofsock(this);
  channel1 = new rofl::crofchan(this);
  channel2 = new rofl::crofchan(this);
}

void crofchantest::tearDown() {
  rofl::cthread::pool_stop_all_threads();
  delete channel2;
  delete channel1;
  delete rofsock;
  rofl::cthread::pool_terminate();
}

void crofchantest::test_connections() {
  congested = false;
  keep_running = true;
  num_of_conns = 4;
  num_of_accepts = 0;
  num_of_dpt_established = 0;
  num_of_ctl_established = 0;
  num_of_pkts_rcvd = 0;
  num_of_pkts_sent = 0;
  int seconds = 20;
  listening_port = 6653;
  testname = std::string("test_connections");

  LOG(INFO) << "TEST: <== crofchantest::test_connections() ==>" << std::endl;

  /* try to find idle port for test */
  bool lookup_idle_port = true;
  while (lookup_idle_port) {
    try {
      LOG(INFO) << "trying listening port=" << (int)listening_port << std::endl;
      baddr = rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), listening_port);
      /* try to bind address first */
      rofsock->set_backlog(num_of_conns);
      rofsock->set_baddr(baddr).listen();
      LOG(INFO) << "binding to " << baddr.str() << std::endl;
      lookup_idle_port = false;
    } catch (rofl::eSysCall &e) {
      /* port in use, try another one */
    }
    do {
      listening_port = rand.uint16();
    } while ((listening_port < 10000) || (listening_port > 49000));
  }

  for (unsigned int i = 0; i < num_of_conns; i++) {
    rofl::crofconn &conn = channel1->add_conn(rofl::cauxid(i));

    conn.set_raddr(baddr).tcp_connect(versionbitmap,
                                      rofl::crofconn::MODE_DATAPATH, false);

    num_of_dpt_established++;

    LOG(INFO) << "auxid(" << i << "): connecting to " << baddr.str()
              << std::endl;
  }

  while (keep_running && (seconds-- > 0)) {
    struct timespec ts;
    ts.tv_sec = 2;
    ts.tv_nsec = 0;
    pselect(0, NULL, NULL, NULL, &ts, NULL);
    LOG(INFO) << ".";
    if (num_of_ctl_established == num_of_conns) {
      break;
    }
  }
  sleep(2);

  LOG(INFO) << "TEST: status" << std::endl;

  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << std::endl;
  LOG(INFO) << "num_of_dpt_established = " << num_of_dpt_established
            << std::endl;
  LOG(INFO) << "num_of_conns = " << num_of_conns << std::endl;
  LOG(INFO) << "num_of_accepts = " << num_of_accepts << std::endl;
  LOG(INFO) << "channel1.size() = " << channel1->keys().size() << std::endl;
  LOG(INFO) << "channel2.size() = " << channel2->keys().size() << std::endl;
  CPPUNIT_ASSERT(channel1->keys().size() == num_of_conns);
  CPPUNIT_ASSERT(channel2->keys().size() == num_of_conns);

  LOG(INFO) << "TEST: shutdown" << std::endl;

  channel2->drop_conn(rofl::cauxid(0));

  sleep(2);

  channel1->drop_conn(rofl::cauxid(0));

  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << std::endl
            << "num_of_dpt_established = " << num_of_dpt_established
            << std::endl
            << "num_of_conns = " << num_of_conns << std::endl
            << "num_of_accepts = " << num_of_accepts << std::endl
            << "channel1.size() = " << channel1->keys().size() << std::endl
            << "channel2.size() = " << channel2->keys().size() << std::endl;
  CPPUNIT_ASSERT(channel1->keys().size() == 0);
  CPPUNIT_ASSERT(channel2->keys().size() == 0);

  LOG(INFO) << "TEST: done" << std::endl;
}

void crofchantest::test_congestion() {
  congested = false;
  keep_running = true;
  num_of_conns = 32;
  num_of_accepts = 0;
  num_of_dpt_established = 0;
  num_of_ctl_established = 0;
  int seconds = 30;
  listening_port = 6653;
  max_congestion_rounds = 16;
  testname = std::string("test_congestion");

  LOG(INFO) << "<== crofchantest::test_congestion() ==>" << std::endl;

  /* try to find idle port for test */
  bool lookup_idle_port = true;
  while (lookup_idle_port) {
    try {
      LOG(INFO) << "trying listening port=" << (int)listening_port << std::endl;
      baddr = rofl::csockaddr(rofl::caddress_in4("127.0.0.1"), listening_port);
      /* try to bind address first */
      rofsock->set_backlog(num_of_conns);
      rofsock->set_baddr(baddr).listen();
      LOG(INFO) << "binding to " << baddr.str() << std::endl;
      lookup_idle_port = false;
    } catch (rofl::eSysCall &e) {
      /* port in use, try another one */
    }
    do {
      listening_port = rand.uint16();
    } while ((listening_port < 10000) || (listening_port > 49000));
  }

  num_of_pkts_rcvd = 0;
  num_of_pkts_sent = 0;

  for (unsigned int i = 0; i < num_of_conns; i++) {
    rofl::crofconn &conn = channel1->add_conn(rofl::cauxid(i));

    conn.set_raddr(baddr).tcp_connect(versionbitmap,
                                      rofl::crofconn::MODE_DATAPATH, false);

    num_of_dpt_established++;

    LOG(INFO) << "auxid(" << i << "): connecting to " << baddr.str()
              << std::endl;
  }

  while (keep_running && (seconds-- > 0)) {
    struct timespec ts;
    ts.tv_sec = 2;
    ts.tv_nsec = 0;
    pselect(0, NULL, NULL, NULL, &ts, NULL);
    LOG(INFO) << ".";
  }
  keep_running = false;
  sleep(2);

  LOG(INFO) << "TEST: status" << std::endl;

  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << std::endl;
  LOG(INFO) << "num_of_dpt_established = " << num_of_dpt_established
            << std::endl;
  LOG(INFO) << "num_of_conns = " << num_of_conns << std::endl;
  LOG(INFO) << "num_of_accepts = " << num_of_accepts << std::endl;
  LOG(INFO) << "channel1.size() = " << channel1->keys().size() << std::endl;
  LOG(INFO) << "channel2.size() = " << channel2->keys().size() << std::endl;
  LOG(INFO) << "num_of_pkts_sent = " << num_of_pkts_sent << std::endl;
  LOG(INFO) << "num_of_pkts_rcvd = " << num_of_pkts_rcvd << std::endl;
  CPPUNIT_ASSERT(channel1->keys().size() == num_of_conns);
  CPPUNIT_ASSERT(channel2->keys().size() == num_of_conns);

  LOG(INFO) << "TEST: shutdown" << std::endl;

  channel2->drop_conn(rofl::cauxid(0));

  sleep(2);

  channel1->drop_conn(rofl::cauxid(0));

  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << std::endl;
  LOG(INFO) << "num_of_dpt_established = " << num_of_dpt_established
            << std::endl;
  LOG(INFO) << "num_of_conns = " << num_of_conns << std::endl;
  LOG(INFO) << "num_of_accepts = " << num_of_accepts << std::endl;
  LOG(INFO) << "channel1.size() = " << channel1->keys().size() << std::endl;
  LOG(INFO) << "channel2.size() = " << channel2->keys().size() << std::endl;
  CPPUNIT_ASSERT(channel1->keys().size() == 0);
  CPPUNIT_ASSERT(channel2->keys().size() == 0);

  LOG(INFO) << "TEST: done" << std::endl;
}

void crofchantest::handle_listen(rofl::crofsock &socket) {
  rofl::AcquireReadWriteLock lock(tlock);
  for (auto sd : socket.accept()) {
    num_of_accepts++;
    LOG(INFO) << "num_of_accepts = " << num_of_accepts << std::endl;

    rofl::crofconn *conn = new rofl::crofconn(this);
    {
      rofl::AcquireReadWriteLock lock(plock);
      pending_conns.insert(conn);
      LOG(INFO) << "crofchantest::handle_listen() pending_conns: "
                << pending_conns.size() << std::endl;
    }
    conn->tcp_accept(sd, versionbitmap, rofl::crofconn::MODE_CONTROLLER);
  }
}

void crofchantest::handle_established(rofl::crofconn &conn,
                                      uint8_t ofp_version) {
  {
    rofl::AcquireReadWriteLock lock(plock);
    pending_conns.erase(&conn);
    LOG(INFO) << "crofchantest::handle_established() pending_conns: "
              << pending_conns.size() << std::endl;
  }
  rofl::AcquireReadWriteLock lock(tlock);
  channel2->add_conn(&conn);

  LOG(INFO) << "TEST: crofchantest::handle_established() server via -conn-, "
            << "conn.get_auxid() = " << (int)conn.get_auxid().get_id()
            << std::endl;

  CPPUNIT_ASSERT(conn.is_established());

  num_of_ctl_established++;
  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << std::endl;
}

void crofchantest::handle_established(rofl::crofchan &chan,
                                      rofl::crofconn &conn,
                                      uint8_t ofp_version) {

  rofl::AcquireReadWriteLock lock(tlock);
  LOG(INFO) << std::endl;
  LOG(INFO) << "TEST: crofchantest::handle_established() -chan-, "
            << "num_of_accepts = " << num_of_accepts << ", "
            << "num_of_conns = " << num_of_conns << ", "
            << "channel1.size() = " << channel1->size() << ", "
            << "channel2.size() = " << channel2->size() << ", "
            << "conn.get_auxid() = " << (int)conn.get_auxid().get_id()
            << std::endl;

  CPPUNIT_ASSERT(conn.is_established());

  if (testname == std::string("test_congestion")) {

    /* we need a large queue size to force a loopback socket into congestion */
    if (channel1 == &chan) {
      conn.set_txqueue_max_size(65536);
    }

    if (conn.get_auxid() == rofl::cauxid(0)) {
      rofl::cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                     rofl::ctimespec().expire_in(1));
    }
  }
}

void crofchantest::handle_timeout(rofl::cthread &thread, uint32_t timer_id) {
  rofl::AcquireReadWriteLock lock(tlock);
  rofl::openflow::cofmsg_packet_in *msg = nullptr;
  switch (timer_id) {
  case TIMER_ID_START_SENDING_PACKET_INS: {
    try {

      uint32_t buffer_id = 0;
      rofl::openflow::cofmatch match(rofl::openflow13::OFP_VERSION);
      match.set_eth_dst(rofl::caddress_ll("ff:ff:ff:ff:ff:ff"));
      match.set_eth_src(rofl::caddress_ll("00:11:22:33:44:55"));
      match.set_eth_type(0x0806);
      rofl::cmemory packet(1500);

      LOG(INFO) << std::endl
                << "TEST: crofchantest::handle_timeout() starting round "
                << max_congestion_rounds << std::endl;

      while (keep_running) {
        msg = new rofl::openflow::cofmsg_packet_in(
            rofl::openflow13::OFP_VERSION, xid++, buffer_id++,
            /*total_len=*/1500,
            /*reason=*/0xa1,
            /*table_id=*/0x10,
            /*cookie=*/0,
            /*in_port=*/1, match, packet.somem(), packet.length());

        switch (channel1->set_conn(rofl::cauxid(0)).send_message(msg)) {
        case rofl::crofsock::MSG_QUEUED: {
          num_of_pkts_sent++;
          // std::cerr << "MSG-QUEUED" << std::endl;
          std::cerr << "s";
          /* keep going */
        } break;
        case rofl::crofsock::MSG_QUEUED_CONGESTION: {
          num_of_pkts_sent++;
          // std::cerr << "MSG-QUEUED-CONGESTION" << std::endl;
          std::cerr << "<C>";
          /* stop queueing and reschedule this function */
          rofl::cthread::thread(thread_num)
              .add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                         rofl::ctimespec().expire_in(1));
        }
          return;
        case rofl::crofsock::MSG_QUEUEING_FAILED_QUEUE_FULL: {
          // std::cerr << "MSG-DROPPED-QUEUE-FULL" << std::endl;
          std::cerr << "<Q>";
          /* stop queueing and reschedule this function */
          rofl::cthread::thread(thread_num)
              .add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                         rofl::ctimespec().expire_in(1));
        }
          return;
        case rofl::crofsock::MSG_QUEUEING_FAILED_NOT_ESTABLISHED: {
          // std::cerr << "MSG-DROPPED-NOT-ESTABLISHED" << std::endl;
          std::cerr << "<N>" << std::endl;
          // std::cerr << "ERROR: crofsock::send_message() dropped message due
          // to connection now established" << std::endl;
          /* stop this test, something failed */
          keep_running = false;
        }
          return;
        case rofl::crofsock::MSG_QUEUEING_FAILED_SHUTDOWN_IN_PROGRESS: {
          std::cerr << "<S>" << std::endl;
          // std::cerr << "MSG-DROPPED-SHUTDOWN-IN-PROGRESS" << std::endl;
          /* stop this test, something failed */
          keep_running = false;
        } break;
        default: {
          // std::cerr << "ERROR: received unhandled return value from
          // crofsock::send_message()" << std::endl;
          /* stop this test, something failed */
          keep_running = false;
        }
          return;
        }

        pthread_yield();
      }

    } catch (rofl::eRofQueueFull &e) {
      LOG(INFO) << "exception caught: eRofQueueFull" << std::endl;
      rofl::cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                     rofl::ctimespec().expire_in(4));
    } catch (rofl::eRofSockNotEstablished &e) {
      LOG(INFO) << "exception caught: eRofSockNotEstablished" << std::endl;
      rofl::cthread::thread(thread_num)
          .add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                     rofl::ctimespec().expire_in(4));
    }
  } break;
  default: {};
  }
}

void crofchantest::handle_recv(rofl::crofchan &chan, rofl::crofconn &conn,
                               rofl::openflow::cofmsg *pmsg) {
  rofl::AcquireReadWriteLock lock(tlock);
  switch (pmsg->get_type()) {
  case rofl::openflow13::OFPT_FEATURES_REQUEST: {

    LOG(INFO) << "crofchantest::handle_recv() message: " << std::endl
              << *pmsg << "sending FEATURES-REPLY auxid: "
              << (int)conn.get_auxid().get_id() << std::endl;

    rofl::openflow::cofmsg_features_reply *msg =
        new rofl::openflow::cofmsg_features_reply(rofl::openflow13::OFP_VERSION,
                                                  pmsg->get_xid(), dpid);
    msg->set_auxid(conn.get_auxid().get_id());

    conn.send_message(msg);

  } break;
  case rofl::openflow13::OFPT_PACKET_IN: {
    std::cerr << "r";
    num_of_pkts_rcvd++;
    if ((num_of_pkts_sent == num_of_pkts_rcvd) &&
        (max_congestion_rounds <= 0)) {
      LOG(INFO) << "TEST: received all packets, initiating test shutdown "
                << std::endl;
      LOG(INFO) << "sent: " << num_of_pkts_sent << std::endl;
      LOG(INFO) << "rcvd: " << num_of_pkts_rcvd << std::endl;
      keep_running = false;
    }
  } break;
  case rofl::openflow13::OFPT_PACKET_OUT: {

  } break;
  default: {};
  }

  delete pmsg;
}

void crofchantest::congestion_solved_indication(rofl::crofchan &chan,
                                                rofl::crofconn &conn) {
  rofl::AcquireReadWriteLock lock(tlock);
  std::cerr << "<CSI(" << max_congestion_rounds << ") s:" << num_of_pkts_sent
            << " r:" << num_of_pkts_rcvd << ">" << std::endl;

  if (--max_congestion_rounds > 0) {
    rofl::cthread::thread(thread_num)
        .add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                   rofl::ctimespec().expire_in(1));
  } else {
    // keep_running = false;
  }
}

void crofchantest::congestion_occurred_indication(rofl::crofchan &chan,
                                                  rofl::crofconn &conn) {
  rofl::AcquireReadWriteLock lock(tlock);
  congested = true;
  LOG(INFO) << std::endl
            << "<COI(" << max_congestion_rounds << ") s:" << num_of_pkts_sent
            << " r:" << num_of_pkts_rcvd << ">" << std::endl;
}
