/*
 * crofchantest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofchantest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(crofchantest);

void crofchantest::setUp() {
  LOG(INFO) << __FUNCTION__;
  versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
  baddr = rofl::csockaddr(AF_INET, "127.0.0.1", 16455);
  dpid = 0xa1a2a3a4a5a6a7a8;
  xid = 0xb1b2b3b4;

  tchan1 = new rofl::cthread();
  tchan2 = new rofl::cthread();
  tchan1->start("channel1");
  tchan2->start("channel2");
  rofsock = new rofl::crofsock(tchan2, this);
  channel1 = new rofl::crofchan(tchan1, this);
  channel2 = new rofl::crofchan(tchan2, this);

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
      rofsock->set_baddr(baddr).listen();
      LOG(INFO) << "binding to " << baddr.str() << std::endl;
      lookup_idle_port = false;
    } catch (rofl::eSysCall &e) {
      /* port in use, try another one */
    }
  }
  LOG(INFO) << __FUNCTION__ << ": done";
}

void crofchantest::tearDown() {
  LOG(INFO) << __FUNCTION__;
  tchan1->drop_timer(this, rofl::cthread::ALL_TIMERS);
  tchan1->stop();
  tchan2->stop();

  channel2->close();
  channel1->close();
  rofsock->close();

  sleep(10);

  delete channel2;
  delete channel1;
  delete rofsock;
  sleep(2);
  delete tchan1;
  delete tchan2;
  LOG(INFO) << __FUNCTION__ << ": done";
}

void crofchantest::test_connections() {

  LOG(INFO) << "Starting: " << __FUNCTION__;

  int seconds = 20;
  congested = false;
  keep_running = true;
  num_of_accepts = 0;
  num_of_conns = 32;
  num_of_ctl_established = 0;
  num_of_dpt_established = 0;
  num_of_pkts_rcvd = 0;
  num_of_pkts_sent = 0;
  send_packets = false;

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
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    pselect(0, NULL, NULL, NULL, &ts, NULL);
    LOG(INFO) << ".";
    if (num_of_ctl_established == num_of_conns) {
      break;
    }
  }

  keep_running = false;

  LOG(INFO) << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl
            << ">>>          TERMINATING          <<<" << std::endl
            << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl

            << ">>>>>>>>>>>>> listen <<<<<<<<<<<<<<" << std::endl

            << "num_of_ctl_established = " << num_of_ctl_established
            << std::endl
            << "num_of_dpt_established = " << num_of_dpt_established
            << std::endl
            << "num_of_conns = " << num_of_conns << std::endl
            << "num_of_accepts = " << num_of_accepts << std::endl
            << "channel1.size() = " << channel1->keys().size() << std::endl
            << "channel2.size() = " << channel2->keys().size() << std::endl
            << "keep_running = " << keep_running;
  CPPUNIT_ASSERT(channel1->keys().size() == num_of_conns);
  CPPUNIT_ASSERT(channel2->keys().size() == num_of_conns);

  for (auto auxid : channel1->keys()) {
    if (not channel1->has_conn(auxid)) {
      continue;
    }
    LOG(INFO) << ">>>>>>>>>>>>> client(" << (int)auxid.get_id()
              << ") <<<<<<<<<<<<<<" << std::endl;
  }

  for (auto auxid : channel2->keys()) {
    if (not channel2->has_conn(auxid)) {
      continue;
    }
    channel2->drop_conn(auxid);
  }

  for (auto auxid : channel1->keys()) {
    if (not channel1->has_conn(auxid)) {
      continue;
    }
    channel1->drop_conn(auxid);
  }

  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << " num_of_dpt_established = " << num_of_dpt_established
            << " num_of_conns = " << num_of_conns
            << " num_of_accepts = " << num_of_accepts
            << " channel1.size() = " << channel1->keys().size()
            << " channel2.size() = " << channel2->keys().size();

  CPPUNIT_ASSERT(channel1->keys().size() == 0);
  CPPUNIT_ASSERT(channel2->keys().size() == 0);

  sleep(10);

  LOG(INFO) << __FUNCTION__ << ": done";
}

void crofchantest::test_congestion() {

  LOG(INFO) << "Starting: " << __FUNCTION__;

  int seconds = 30;
  congested = false;
  keep_running = true;
  max_congestion_rounds = 16;
  num_of_accepts = 0;
  num_of_conns = 1;
  num_of_ctl_established = 0;
  num_of_dpt_established = 0;
  num_of_pkts_rcvd = 0;
  num_of_pkts_sent = 0;
  send_packets = true;

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
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    pselect(0, NULL, NULL, NULL, &ts, NULL);
  }

  LOG(INFO) << ">>>>>TERMINATING(listen): num_of_ctl_established = "
            << num_of_ctl_established
            << " num_of_dpt_established = " << num_of_dpt_established
            << " num_of_conns = " << num_of_conns
            << " num_of_accepts = " << num_of_accepts
            << " channel1.size() = " << channel1->keys().size()
            << " channel2.size() = " << channel2->keys().size()
            << " num_of_pkts_sent = " << num_of_pkts_sent
            << " num_of_pkts_rcvd = " << num_of_pkts_rcvd;

  CPPUNIT_ASSERT(channel1->keys().size() == num_of_conns);
  CPPUNIT_ASSERT(channel2->keys().size() == num_of_conns);

  keep_running = false;

  for (auto auxid : channel1->keys()) {
    LOG(INFO) << ">>>>>>>>>>>>> client(" << (int)auxid.get_id()
              << ") <<<<<<<<<<<<<<" << std::endl;
    if (not channel1->has_conn(auxid)) {
      continue;
    }
  }

  for (auto auxid : channel2->keys()) {
    if (not channel2->has_conn(auxid)) {
      continue;
    }
    LOG(INFO) << "dropping channel2 auxid=" << auxid;
    channel2->drop_conn(auxid);
  }

  for (auto auxid : channel1->keys()) {
    if (not channel1->has_conn(auxid)) {
      continue;
    }
    LOG(INFO) << "dropping channel1 auxid=" << auxid;
    channel1->drop_conn(auxid);
  }

  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << " num_of_dpt_established = " << num_of_dpt_established
            << " num_of_conns = " << num_of_conns
            << " num_of_accepts = " << num_of_accepts
            << " channel1.size() = " << channel1->keys().size()
            << " channel2.size() = " << channel2->keys().size();

  sleep(10);

  CPPUNIT_ASSERT(channel1->keys().size() == 0);
  CPPUNIT_ASSERT(channel2->keys().size() == 0);
}

void crofchantest::handle_listen(rofl::crofsock &socket) {
  for (auto sd : socket.accept()) {
    num_of_accepts++;
    LOG(INFO) << "num_of_accepts = " << num_of_accepts << std::endl;

    rofl::crofconn *conn = new rofl::crofconn(tchan2, this);
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

  channel2->add_conn(&conn);

  LOG(INFO) << ">>>>>>>>>>>>> server(" << (int)conn.get_auxid().get_id()
            << ") <<<<<<<<<<<<<<" << std::endl
            << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl
            << ">>> crofchantest::handle_established() -conn-" << std::endl
            << ">>> conn.get_auxid() = " << (int)conn.get_auxid().get_id()
            << std::endl
            << ">>> channel2.size() = " << channel2->size() << std::endl
            << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl;

  CPPUNIT_ASSERT(conn.is_established());

  num_of_ctl_established++;
  LOG(INFO) << "num_of_ctl_established = " << num_of_ctl_established
            << std::endl;
}

void crofchantest::handle_established(rofl::crofchan &chan,
                                      rofl::crofconn &conn,
                                      uint8_t ofp_version) {
  LOG(INFO) << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl
            << ">>> crofchantest::handle_established() -chan-" << std::endl
            << ">>> num_of_accepts = " << num_of_accepts << std::endl
            << ">>> num_of_conns   = " << num_of_conns << std::endl
            << ">>> channel1.size() = " << channel1->size() << std::endl
            << ">>> channel2.size() = " << channel2->size() << std::endl
            << ">>> conn.get_auxid() = " << (int)conn.get_auxid().get_id()
            << std::endl
            << ">>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<" << std::endl
            << std::endl
            << "crofchan::handle_established" << std::endl;

  CPPUNIT_ASSERT(conn.is_established());

  /* we need a large queue size to force a loopback socket into congestion */
  if (channel1 == &chan) {
    conn.set_txqueue_max_size(65536);
  }

  if (send_packets && conn.get_auxid() == rofl::cauxid(0)) {
    tchan1->add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                      rofl::ctimespec().expire_in(1));
  }
}

void crofchantest::handle_timeout(void *userdata) {
  rofl::openflow::cofmsg_packet_in *msg = nullptr;
  int timer_id = (long)userdata;
  switch (timer_id) {
  case TIMER_ID_START_SENDING_PACKET_INS: {
    try {

      uint32_t buffer_id = 0;
      rofl::openflow::cofmatch match(rofl::openflow13::OFP_VERSION);
      match.set_eth_dst(rofl::caddress_ll("ff:ff:ff:ff:ff:ff"));
      match.set_eth_src(rofl::caddress_ll("00:11:22:33:44:55"));
      match.set_eth_type(0x0806);
      rofl::cmemory packet(1500);

      LOG(INFO) << "crofchantest::handle_timeout() starting to overload "
                   "control connection"
                << std::endl;

      while (keep_running) {
        msg = new rofl::openflow::cofmsg_packet_in(
            rofl::openflow13::OFP_VERSION, xid++, buffer_id++,
            /*total_len=*/128,
            /*reason=*/0xa1,
            /*table_id=*/0x10,
            /*cookie=*/0,
            /*in_port=*/1, match, packet.somem(), packet.length());

        switch (channel1->set_conn(rofl::cauxid(0)).send_message(msg)) {
        case rofl::crofsock::MSG_QUEUED: {
          num_of_pkts_sent++;
          if ((num_of_pkts_sent & 0x3f) == 0) {
            tchan1->add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                              rofl::ctimespec().expire_in(0));
            return;
          }
        } break;
        case rofl::crofsock::MSG_QUEUED_CONGESTION: {
          num_of_pkts_sent++;
          VLOG(1) << "MSG-QUEUED-CONGESTION";
          LOG(INFO) << "<C> ";
          /* stop queueing and reschedule this function */
          tchan1->add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                            rofl::ctimespec().expire_in(1));
        }
          return;
        case rofl::crofsock::MSG_QUEUEING_FAILED_QUEUE_FULL: {
          VLOG(1) << "MSG-DROPPED-QUEUE-FULL";
          LOG(INFO) << "<Q> ";
          /* stop queueing and reschedule this function */
          tchan1->add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                            rofl::ctimespec().expire_in(1));
        }
          return;
        case rofl::crofsock::MSG_QUEUEING_FAILED_NOT_ESTABLISHED: {
          VLOG(1) << __FUNCTION__ << ": ERROR: MSG-DROPPED-NOT-ESTABLISHED "
                                     "dropped message due to connection now "
                                     "established ";
          LOG(INFO) << "<N> ";
          /* stop this test, something failed */
          keep_running = false;
        }
          return;
        case rofl::crofsock::MSG_QUEUEING_FAILED_SHUTDOWN_IN_PROGRESS: {
          LOG(INFO) << "<S> ";
          VLOG(1) << "MSG-DROPPED-SHUTDOWN-IN-PROGRESS";
          /* stop this test, something failed */
          keep_running = false;
        } break;
        default: {
          LOG(INFO) << "ERROR: received unhandled return value from "
                       "crofsock::send_message()";
          /* stop this test, something failed */
          keep_running = false;
        }
          return;
        }

        pthread_yield();
      }

    } catch (rofl::eRofQueueFull &e) {
      LOG(INFO) << "exception caught: eRofQueueFull";
      tchan1->add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                        rofl::ctimespec().expire_in(4));
    } catch (rofl::eRofSockNotEstablished &e) {
      LOG(INFO) << "exception caught: eRofSockNotEstablished" << std::endl;
      tchan1->add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                        rofl::ctimespec().expire_in(4));
    }
  } break;
  default:
    assert(0 && "not reached");
  }
}

void crofchantest::handle_recv(rofl::crofchan &chan, rofl::crofconn &conn,
                               rofl::openflow::cofmsg *pmsg) {

  switch (pmsg->get_type()) {
  case rofl::openflow13::OFPT_FEATURES_REQUEST: {

    LOG(INFO) << __FUNCTION__ << ": message: " << *pmsg
              << "sending FEATURES-REPLY auxid: "
              << (int)conn.get_auxid().get_id() << std::endl;

    rofl::openflow::cofmsg_features_reply *msg =
        new rofl::openflow::cofmsg_features_reply(rofl::openflow13::OFP_VERSION,
                                                  pmsg->get_xid(), dpid);
    msg->set_auxid(conn.get_auxid().get_id());

    conn.send_message(msg);

  } break;
  case rofl::openflow13::OFPT_PACKET_IN: {
    VLOG(1) << __FUNCTION__ << " pkt-in";
    num_of_pkts_rcvd++;
    if ((num_of_pkts_sent == num_of_pkts_rcvd) &&
        (max_congestion_rounds <= 0)) {
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
  LOG(INFO) << "crofchan::congestion_solved_indication: "
            << "num_of_pkts_sent=" << num_of_pkts_sent
            << " num_of_pkts_rcvd=" << num_of_pkts_rcvd
            << " max_congestion_rounds=" << max_congestion_rounds << std::endl;

  if (--max_congestion_rounds > 0) {
    tchan1->add_timer(this, TIMER_ID_START_SENDING_PACKET_INS,
                      rofl::ctimespec().expire_in(1));
  } else {
    keep_running = false;
  }
}

void crofchantest::congestion_occurred_indication(rofl::crofchan &chan,
                                                  rofl::crofconn &conn) {
  congested = true;
  LOG(INFO) << "crofchan::congestion_occurred_indication: "
            << "num_of_pkts_sent=" << num_of_pkts_sent
            << " num_of_pkts_rcvd=" << num_of_pkts_rcvd
            << " max_congestion_rounds=" << max_congestion_rounds << std::endl;
}
