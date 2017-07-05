/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofconn.cc
 *
 *  Created on: 31.12.2013
 *  Revised on: 27.09.2015
 *      Author: andreas
 */

#include "crofconn.h"
#include <glog/logging.h>

using namespace rofl;

/*static*/ std::set<crofconn_env *> crofconn_env::connection_envs;
/*static*/ crwlock crofconn_env::connection_envs_lock;
/*static*/ const int crofconn::RXQUEUE_MAX_SIZE_DEFAULT = 128;
/*static*/ const unsigned int crofconn::DEFAULT_SEGMENTATION_THRESHOLD = 65535;
/*static*/ const time_t crofconn::DEFAULT_HELLO_TIMEOUT = 3;
/*static*/ const time_t crofconn::DEFAULT_FEATURES_TIMEOUT = 5;
/*static*/ const time_t crofconn::DEFAULT_ECHO_TIMEOUT = 3;
/*static*/ const time_t crofconn::DEFAULT_LIFECHECK_TIMEOUT = 1;
/*static*/ const time_t crofconn::DEFAULT_SEGMENTS_TIMEOUT = 60;
/*static*/ const unsigned int crofconn::DEFAULT_PENDING_SEGMENTS_MAX = 256;

crofconn::~crofconn() {
  /* stop worker thread */
  set_state(STATE_CLOSING);
  thread.stop();
}

crofconn::crofconn(crofconn_env *env)
    : env(env), thread(this), rofsock(this), dpid(0), auxid(0),
      ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN), mode(MODE_UNKNOWN),
      state(STATE_DISCONNECTED), flag_hello_sent(false), flag_hello_rcvd(false),
      rxweights(QUEUE_MAX), rxqueues(QUEUE_MAX), rx_thread_working(false),
      rx_thread_scheduled(false), rxqueue_max_size(RXQUEUE_MAX_SIZE_DEFAULT),
      segmentation_threshold(DEFAULT_SEGMENTATION_THRESHOLD),
      timeout_hello(DEFAULT_HELLO_TIMEOUT),
      timeout_features(DEFAULT_FEATURES_TIMEOUT),
      timeout_echo(DEFAULT_ECHO_TIMEOUT),
      timeout_lifecheck(DEFAULT_LIFECHECK_TIMEOUT),
      xid_hello_last(random.uint32()),
      xid_features_request_last(random.uint32()),
      xid_echo_request_last(random.uint32()),
      timeout_segments(DEFAULT_SEGMENTS_TIMEOUT),
      pending_segments_max(DEFAULT_PENDING_SEGMENTS_MAX) {
  /* scheduler weights for transmission */
  rxweights[QUEUE_OAM] = 16;
  rxweights[QUEUE_MGMT] = 32;
  rxweights[QUEUE_FLOW] = 16;
  rxweights[QUEUE_PKT] = 8;
  /* set maximum queue size */
  for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; queue_id++) {
    rxqueues[queue_id].set_queue_max_size(rxqueue_max_size);
  }
  /* start worker thread */
  thread.start("crofconn");
}

void crofconn::close() { set_state(STATE_CLOSING); };

void crofconn::tcp_accept(
    int sd, const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
    enum crofconn_mode_t mode) {
  set_versionbitmap(versionbitmap);
  set_mode(mode);
  set_state(STATE_ACCEPT_PENDING);
  rofsock.tcp_accept(sd);
};

void crofconn::tcp_connect(
    const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
    enum crofconn_mode_t mode, bool reconnect) {
  set_versionbitmap(versionbitmap);
  set_mode(mode);
  set_state(STATE_CONNECT_PENDING);
  rofsock.tcp_connect(reconnect);
};

void crofconn::tls_accept(
    int sd, const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
    enum crofconn_mode_t mode) {
  set_versionbitmap(versionbitmap);
  set_mode(mode);
  set_state(STATE_ACCEPT_PENDING);
  rofsock.tls_accept(sd);
};

void crofconn::tls_connect(
    const rofl::openflow::cofhello_elem_versionbitmap &versionbitmap,
    enum crofconn_mode_t mode, bool reconnect) {
  set_auxid(auxid);
  set_versionbitmap(versionbitmap);
  set_mode(mode);
  set_state(STATE_CONNECT_PENDING);
  rofsock.tls_connect(reconnect);
};

void crofconn::handle_timeout(cthread &thread, uint32_t timer_id) {
  switch (timer_id) {
  case TIMER_ID_NEED_LIFE_CHECK: {
    send_echo_request();
  } break;
  case TIMER_ID_WAIT_FOR_HELLO: {
    hello_expired();
  } break;
  case TIMER_ID_WAIT_FOR_ECHO: {
    echo_request_expired();
  } break;
  case TIMER_ID_WAIT_FOR_FEATURES: {
    features_request_expired();
  } break;
  case TIMER_ID_PENDING_REQUESTS: {
    check_pending_requests();
  } break;
  case TIMER_ID_PENDING_SEGMENTS: {
    check_pending_segments();
  } break;
  default: {
    VLOG(1) << __FUNCTION__ << " unknown timer type: " << (unsigned int)timer_id
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
  };
  }
}

void crofconn::set_state(enum crofconn_state_t new_state) {
  try {
    switch (state = new_state) {
    case STATE_NEGOTIATION_FAILED: {
      VLOG(2) << __FUNCTION__
              << " STATE_NEGOTIATION_FAILED laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      set_state(STATE_CLOSING);
      crofconn_env::call_env(env).handle_negotiation_failed(*this);

    } break;
    case STATE_CLOSING: {
      VLOG(2) << __FUNCTION__
              << " STATE_CLOSING laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      rofsock.close();
      set_state(STATE_DISCONNECTED);

    } break;
    case STATE_DISCONNECTED: {
      VLOG(2) << __FUNCTION__
              << " STATE_DISCONNECTED laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();

      /* stop periodic checks for connection state (OAM) */
      thread.drop_timer(TIMER_ID_NEED_LIFE_CHECK);

      clear_pending_requests();
      clear_pending_segments();

      versionbitmap_peer.clear();
      set_version(rofl::openflow::OFP_VERSION_UNKNOWN);
      flag_hello_sent = false;
      flag_hello_rcvd = false;

    } break;
    case STATE_CONNECT_PENDING: {
      VLOG(2) << __FUNCTION__
              << " STATE_CONNECT_PENDING laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      versionbitmap_peer.clear();
      set_version(rofl::openflow::OFP_VERSION_UNKNOWN);

      for (auto rxqueue : rxqueues) {
        rxqueue.clear();
      }

    } break;
    case STATE_ACCEPT_PENDING: {
      VLOG(2) << __FUNCTION__
              << " STATE_ACCEPT_PENDING laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      versionbitmap_peer.clear();
      set_version(rofl::openflow::OFP_VERSION_UNKNOWN);

      for (auto rxqueue : rxqueues) {
        rxqueue.clear();
      }

    } break;
    case STATE_NEGOTIATING: {
      VLOG(2) << __FUNCTION__
              << " STATE_NEGOTIATING offered versions=" << versionbitmap.str()
              << " laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      if (not flag_hello_rcvd) {
        thread.add_timer(TIMER_ID_WAIT_FOR_HELLO,
                         ctimespec().expire_in(timeout_hello));
      }
      if (not flag_hello_sent) {
        send_hello_message();
      }

    } break;
    case STATE_NEGOTIATING2: {
      VLOG(2) << __FUNCTION__ << " STATE_NEGOTIATING2 peer versions="
              << versionbitmap_peer.str()
              << " laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      thread.drop_timer(TIMER_ID_WAIT_FOR_HELLO);
      send_features_request();

    } break;
    case STATE_ESTABLISHED: {
      VLOG(2) << __FUNCTION__ << " STATE_ESTABLISHED negotiated version="
              << static_cast<unsigned>(ofp_version.load())
              << " laddr=" << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      thread.drop_timer(TIMER_ID_WAIT_FOR_HELLO);
      /* start periodic checks for connection state (OAM) */
      thread.add_timer(TIMER_ID_NEED_LIFE_CHECK,
                       ctimespec().expire_in(timeout_lifecheck));
      crofconn_env::call_env(env).handle_established(*this, ofp_version);

    } break;
    }

  } catch (std::runtime_error &e) {
    VLOG(1) << __FUNCTION__ << " runtime error: " << e.what();
  }
};

void crofconn::error_rcvd(rofl::openflow::cofmsg *pmsg) {
  rofl::openflow::cofmsg_error *msg =
      dynamic_cast<rofl::openflow::cofmsg_error *>(pmsg);

  if (nullptr == msg) {
    VLOG(1) << __FUNCTION__ << " msg is not of type cofmsg_error laddr="
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
    return;
  }

  thread.drop_timer(TIMER_ID_WAIT_FOR_HELLO);

  try {

    switch (get_state()) {
    case STATE_NEGOTIATING: {

      switch (msg->get_err_type()) {
      case rofl::openflow13::OFPET_HELLO_FAILED: {

        switch (msg->get_err_code()) {
        case rofl::openflow13::OFPHFC_INCOMPATIBLE: {
          VLOG(1) << __FUNCTION__
                  << "HELLO-INCOMPATIBLE.error rcvd in state NEGOTIATING, "
                     "closing connection laddr="
                  << rofsock.get_laddr().str()
                  << " raddr=" << rofsock.get_raddr().str();
        } break;
        case rofl::openflow13::OFPHFC_EPERM: {
          VLOG(1) << __FUNCTION__
                  << "HELLO-EPERM.error rcvd in state NEGOTIATING, closing "
                     "connection laddr="
                  << rofsock.get_laddr().str()
                  << " raddr=" << rofsock.get_raddr().str();
        } break;
        default: {
          VLOG(1) << __FUNCTION__
                  << "HELLO.error rcvd in state NEGOTIATING, closing "
                     "connection laddr="
                  << rofsock.get_laddr().str()
                  << " raddr=" << rofsock.get_raddr().str();
        };
        }

        set_state(STATE_NEGOTIATION_FAILED);

      } break;
      default: {};
      }

    } break;
    case STATE_NEGOTIATING2: {

      switch (msg->get_err_type()) {
      case rofl::openflow13::OFPET_BAD_REQUEST: {

        VLOG(1) << __FUNCTION__
                << "BAD-REQUEST.error rcvd in state NEGOTIATING2, closing "
                   "connection laddr="
                << rofsock.get_laddr().str()
                << " raddr=" << rofsock.get_raddr().str();

        set_state(STATE_NEGOTIATION_FAILED);

      } break;
      default: {};
      }

    } break;
    default: {};
    }

  } catch (std::runtime_error &e) {
    VLOG(1) << __FUNCTION__ << " runtime error: " << e.what()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
  }

  delete msg;
}

void crofconn::send_hello_message() {
  try {
    AcquireReadWriteLock lock(hello_lock);
    if (flag_hello_sent)
      return;
    flag_hello_sent = true;

    rofl::openflow::cofhelloelems helloIEs;
    helloIEs.add_hello_elem_versionbitmap() = versionbitmap;

    rofl::openflow::cofmsg_hello *msg = new rofl::openflow::cofmsg_hello(
        versionbitmap.get_highest_ofp_version(), ++xid_hello_last, helloIEs);
    VLOG(3) << __FUNCTION__ << " state: " << state
            << " message sent: " << msg->str().c_str()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();

    rofsock.send_message(msg);

  } catch (rofl::exception &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
    set_state(STATE_NEGOTIATION_FAILED);
  }
}

void crofconn::hello_rcvd(rofl::openflow::cofmsg *pmsg) {
  rofl::openflow::cofmsg_hello *msg =
      dynamic_cast<rofl::openflow::cofmsg_hello *>(pmsg);

  if (nullptr == msg) {
    VLOG(1) << __FUNCTION__ << " msg is not of type cofmsg_hello laddr="
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
    return;
  }

  if (not flag_hello_sent) {
    send_hello_message();
  }

  flag_hello_rcvd = true;

  thread.drop_timer(TIMER_ID_WAIT_FOR_HELLO);

  try {

    /* Step 1: extract version information from HELLO message */

    versionbitmap_peer.clear();

    switch (msg->get_version()) {
    case rofl::openflow10::OFP_VERSION:
    case rofl::openflow12::OFP_VERSION: {

      versionbitmap_peer.add_ofp_version(msg->get_version());

    } break;
    default: { // msg->get_version() should contain the highest number of
               // supported OFP versions encoded in versionbitmap

      rofl::openflow::cofhelloelems helloIEs(msg->get_helloelems());

      if (not helloIEs.has_hello_elem_versionbitmap()) {
        VLOG(1) << __FUNCTION__
                << " HELLO message rcvd without HelloIE -VersionBitmap- laddr="
                << rofsock.get_laddr().str()
                << " raddr=" << rofsock.get_raddr().str();
        versionbitmap_peer.add_ofp_version(msg->get_version());

      } else {
        versionbitmap_peer = helloIEs.get_hello_elem_versionbitmap();
        // sanity check
        if (not versionbitmap_peer.has_ofp_version(msg->get_version())) {
          VLOG(1) << __FUNCTION__
                  << " malformed HelloIE -VersionBitmap- => does not contain "
                     "version defined in OFP message header: "
                  << (int)msg->get_version()
                  << " laddr=" << rofsock.get_laddr().str()
                  << " raddr=" << rofsock.get_raddr().str();
        }
      }
    };
    }

    /* Step 2: select highest supported protocol version on both sides */

    rofl::openflow::cofhello_elem_versionbitmap versionbitmap_common =
        versionbitmap & versionbitmap_peer;
    if (versionbitmap_common.get_highest_ofp_version() ==
        rofl::openflow::OFP_VERSION_UNKNOWN) {
      throw eHelloIncompatible();
    }

    ofp_version = versionbitmap_common.get_highest_ofp_version();

    /* move on finite state machine */
    if (ofp_version == rofl::openflow::OFP_VERSION_UNKNOWN) {
      VLOG(2) << __FUNCTION__
              << " no common OFP version supported, closing connection laddr="
              << rofsock.get_laddr().str()
              << " raddr=" << rofsock.get_raddr().str();
      set_state(STATE_DISCONNECTED);

    } else {
      switch (mode) {
      case MODE_CONTROLLER: {
        /* get auxid via FEATURES.request for OFP1.3 and above */
        if (ofp_version >= rofl::openflow13::OFP_VERSION) {
          set_state(STATE_NEGOTIATING2);
          /* otherwise: connection establishment succeeded */
        } else {
          set_state(STATE_ESTABLISHED);
        }

      } break;
      case MODE_DATAPATH: {
        /* connection establishment succeeded */
        set_state(STATE_ESTABLISHED);

      } break;
      default: {
        VLOG(1) << __FUNCTION__ << " unable to handle undefined mode laddr="
                << rofsock.get_laddr().str()
                << " raddr=" << rofsock.get_raddr().str();
        set_state(STATE_CLOSING);
      }
        return;
      }
    }

  } catch (eHelloIncompatible &e) {

    VLOG(2) << __FUNCTION__
            << " sending -HelloIncompatible- error message to peer laddr="
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();

    size_t len = (msg->length() < 64) ? msg->length() : 64;
    rofl::cmemory mem(msg->length());
    msg->pack(mem.somem(), mem.length());

    rofsock.send_message(
        new rofl::openflow::cofmsg_error_hello_failed_incompatible(
            msg->get_version(), msg->get_xid(), mem.somem(), len));

    sleep(1);

    set_state(STATE_NEGOTIATION_FAILED);

  } catch (eHelloEperm &e) {

    VLOG(2) << __FUNCTION__ << " sending -HelloEperm- error message to peer"
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();

    size_t len = (msg->length() < 64) ? msg->length() : 64;
    rofl::cmemory mem(msg->length());
    msg->pack(mem.somem(), mem.length());

    rofsock.send_message(new rofl::openflow::cofmsg_error_hello_failed_eperm(
        msg->get_version(), msg->get_xid(), mem.somem(), len));

    sleep(1);

    set_state(STATE_NEGOTIATION_FAILED);

  } catch (std::runtime_error &e) {

    VLOG(2) << __FUNCTION__
            << " sending -HelloIncompatible- error message to peer laddr="
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();

    size_t len = (msg->length() < 64) ? msg->length() : 64;
    rofl::cmemory mem(msg->length());
    msg->pack(mem.somem(), mem.length());

    rofsock.send_message(
        new rofl::openflow::cofmsg_error_hello_failed_incompatible(
            msg->get_version(), msg->get_xid(), mem.somem(), len));

    sleep(1);

    set_state(STATE_NEGOTIATION_FAILED);
  }

  delete msg;
}

void crofconn::hello_expired() {
  VLOG(1) << __FUNCTION__ << " HELLO expired state=" << state
          << " laddr=" << rofsock.get_laddr().str()
          << " raddr=" << rofsock.get_raddr().str();

  switch (state) {
  case STATE_ESTABLISHED: {
    /* ignore event */
    VLOG(1) << __FUNCTION__
            << " HELLO expired, ignoring event, already established laddr="
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
  } break;
  default: {
    if (flag_hello_rcvd) {
      VLOG(1)
          << __FUNCTION__
          << " HELLO expired, ignoring event, HELLO from peer received laddr="
          << rofsock.get_laddr().str()
          << " raddr=" << rofsock.get_raddr().str();
      return;
    }
    set_state(STATE_NEGOTIATION_FAILED);
  };
  }
}

void crofconn::send_features_request() {
  try {
    thread.add_timer(TIMER_ID_WAIT_FOR_FEATURES,
                     ctimespec().expire_in(timeout_features));

    rofl::openflow::cofmsg_features_request *msg =
        new rofl::openflow::cofmsg_features_request(
            ofp_version, ++xid_features_request_last);

    VLOG(3) << __FUNCTION__ << " state: " << state
            << " message sent: " << msg->str().c_str()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();

    rofsock.send_message(msg);

  } catch (rofl::exception &e) {
    VLOG(1) << __FUNCTION__ << ": errormsg=" << e.what();
    set_state(STATE_NEGOTIATION_FAILED);
  }
}

void crofconn::features_reply_rcvd(rofl::openflow::cofmsg *pmsg) {
  rofl::openflow::cofmsg_features_reply *msg =
      dynamic_cast<rofl::openflow::cofmsg_features_reply *>(pmsg);

  if (nullptr == msg) {
    VLOG(1) << __FUNCTION__
            << " msg is not of type cofmsg_features_reply laddr="
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
    return;
  }

  thread.drop_timer(TIMER_ID_WAIT_FOR_FEATURES);

  try {
    set_dpid(msg->get_dpid());

    if (ofp_version >= rofl::openflow13::OFP_VERSION) {
      set_auxid(msg->get_auxid());
    } else {
      set_auxid(0);
    }

    set_state(STATE_ESTABLISHED);

  } catch (std::runtime_error &e) {
    VLOG(1) << __FUNCTION__ << " runtime error: " << e.what()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
  }

  delete msg;
}

void crofconn::features_request_expired() {
  VLOG(1) << __FUNCTION__
          << " Features Request expired laddr=" << rofsock.get_laddr().str()
          << " raddr=" << rofsock.get_raddr().str();

  set_state(STATE_NEGOTIATION_FAILED);
}

void crofconn::send_echo_request() {
  try {
    thread.add_timer(TIMER_ID_WAIT_FOR_ECHO,
                     ctimespec().expire_in(timeout_echo));

    rofl::openflow::cofmsg_echo_request *msg =
        new rofl::openflow::cofmsg_echo_request(ofp_version,
                                                ++xid_echo_request_last);

    VLOG(3) << __FUNCTION__ << " state: " << state
            << " message sent: " << msg->str().c_str()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();

    rofsock.send_message(msg);

  } catch (rofl::exception &e) {
    VLOG(1) << __FUNCTION__ << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str() << " error=" << e.what();
  }
}

void crofconn::echo_reply_rcvd(rofl::openflow::cofmsg *pmsg) {
  rofl::openflow::cofmsg_echo_reply *msg =
      dynamic_cast<rofl::openflow::cofmsg_echo_reply *>(pmsg);

  assert(nullptr != msg);

  thread.drop_timer(TIMER_ID_WAIT_FOR_ECHO);

  try {
    delete msg;

    thread.add_timer(TIMER_ID_NEED_LIFE_CHECK,
                     ctimespec().expire_in(timeout_lifecheck));

  } catch (std::runtime_error &e) {
    VLOG(1) << __FUNCTION__ << " runtime error: " << e.what()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
  }
}

void crofconn::echo_request_expired() {
  VLOG(1) << __FUNCTION__
          << " Echo Request expired laddr=" << rofsock.get_laddr().str()
          << " raddr=" << rofsock.get_raddr().str();

  set_state(STATE_CLOSING);
  handle_closed(rofsock);
}

void crofconn::echo_request_rcvd(rofl::openflow::cofmsg *pmsg) {
  rofl::openflow::cofmsg_echo_request *msg =
      dynamic_cast<rofl::openflow::cofmsg_echo_request *>(pmsg);

  if (nullptr == msg) {
    VLOG(1) << __FUNCTION__ << " msg is not of type cofmsg_echo_request laddr="
            << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
    return;
  }

  try {
    rofl::openflow::cofmsg_echo_reply *reply =
        new rofl::openflow::cofmsg_echo_reply(
            msg->get_version(), msg->get_xid(), msg->get_body().somem(),
            msg->get_body().memlen());

    VLOG(3) << __FUNCTION__ << " state: " << state
            << " message sent: " << reply->str().c_str()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();

    rofsock.send_message(reply);

  } catch (std::runtime_error &e) {
    VLOG(1) << __FUNCTION__ << " runtime error: " << e.what()
            << " laddr=" << rofsock.get_laddr().str()
            << " raddr=" << rofsock.get_raddr().str();
  }

  delete msg;
}

void crofconn::handle_tcp_connect_refused(crofsock &rofsock) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TCP connection -connect- refused";

    set_state(STATE_DISCONNECTED);
    crofconn_env::call_env(env).handle_connect_refused(*this);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tcp_connect_failed(crofsock &rofsock) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TCP connection -connect- failed";

    set_state(STATE_DISCONNECTED);
    crofconn_env::call_env(env).handle_connect_failed(*this);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tcp_connected(crofsock &rofsock) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TCP connection -connect- established";

    set_state(STATE_NEGOTIATING);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tcp_accept_refused(crofsock &socket) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TCP connection -accept- refused";

    set_state(STATE_DISCONNECTED);
    crofconn_env::call_env(env).handle_accept_failed(*this);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tcp_accept_failed(crofsock &socket) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TCP connection -accept- failed";

    set_state(STATE_DISCONNECTED);
    crofconn_env::call_env(env).handle_accept_failed(*this);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tcp_accepted(crofsock &socket) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TCP connection -accept- established";

    set_state(STATE_NEGOTIATING);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tls_connect_failed(crofsock &socket) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TLS connection -connect- failed";

    set_state(STATE_DISCONNECTED);
    crofconn_env::call_env(env).handle_connect_failed(*this);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tls_connected(crofsock &socket) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TLS connection -connect- established";

    set_state(STATE_NEGOTIATING);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tls_accept_failed(crofsock &socket) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TLS connection -accept- failed";

    set_state(STATE_DISCONNECTED);
    crofconn_env::call_env(env).handle_accept_failed(*this);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_tls_accepted(crofsock &socket) {
  try {
    VLOG(2) << __FUNCTION__
            << " socket indicates TLS connection -accept- established";

    set_state(STATE_NEGOTIATING);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::handle_closed(crofsock &rofsock) {
  try {
    VLOG(2) << __FUNCTION__ << " socket indicates close";

    /* work on packets in reception queue first, then signal shutdown */
    unsigned int waiting = 60 /*seconds*/;
    while ((--waiting > 0) && (rx_thread_scheduled || rx_thread_working)) {
      VLOG(2) << __FUNCTION__
              << " socket indicates close, waiting for pending rx packets";
      sleep(1);
    }

    set_state(STATE_DISCONNECTED);
    crofconn_env::call_env(env).handle_closed(*this);

  } catch (eRofConnNotFound &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
  }
};

void crofconn::congestion_solved_indication(crofsock &rofsock) {
  VLOG(2) << __FUNCTION__ << " congestion solved indication; action: "
                             "message transmission unblocked";
  crofconn_env::call_env(env).congestion_solved_indication(*this);
};

void crofconn::congestion_occured_indication(crofsock &rofsock) {
  VLOG(2)
      << __FUNCTION__
      << " congestion occured indication; action: message transmission blocked";
  crofconn_env::call_env(env).congestion_occured_indication(*this);
};

void crofconn::handle_recv(crofsock &socket, rofl::openflow::cofmsg *msg) {
  /* This method is executed in crofsock's (not crofconn's!) rxthread context.
   * It deals with messages during connection establishment and negotiation
   * and handles incoming Echo-Requests directly. All other messages
   * are stored in the appropriate rxqueue and crofconn's internal
   * thread is called for handling these messages. */

  VLOG(3) << __FUNCTION__ << " state: " << state
          << " message rcvd: " << msg->str().c_str();

  switch (get_state()) {
  case STATE_CONNECT_PENDING:
  case STATE_ACCEPT_PENDING:
  case STATE_NEGOTIATING: {
    /* Include state STATE_ACCEPT_PENDING here, as the kernel might interrupt
     * the thread calling crofconn::tcp_accept() and schedule
     * crofsock's RX-thread before we have entered STATE_NEOGTIATING. If this
     * is happening, we may receive a Hello message before entering
     * STATE_NEGOTIATING and thus dropping the message. Method hello_rcvd()
     * will enter STATE_NEGOTIATING2 directly under these circumstances. */

    if (msg->get_version() == rofl::openflow::OFP_VERSION_UNKNOWN) {
      VLOG(2) << __FUNCTION__ << " message with invalid version received "
                                 "state=STATE_NEGOTIATING, rcvd version="
              << msg->get_version() << ", negotiated version=" << ofp_version;

      size_t len = msg->length() < 64 ? msg->length() : 64;
      rofl::cmemory mem(msg->length());
      msg->pack(mem.somem(), mem.length());

      send_message(new rofl::openflow::cofmsg_error_bad_request_bad_version(
          ofp_version, msg->get_xid(), mem.somem(), len));

      delete msg;
      return;
    }

    if (rofl::openflow::OFPT_HELLO == msg->get_type()) {
      hello_rcvd(msg);
    } else if (rofl::openflow::OFPT_ERROR == msg->get_type()) {
      error_rcvd(msg);
    } else {
      /* drop all non-HELLO messages in this state */
      VLOG(2) << __FUNCTION__
              << " invalid message type received while negotiating "
                 "state=STATE_NEGOTIATING, msgtype="
              << msg->get_type();
      delete msg;
    }
  }
    return; // immediate return
  case STATE_NEGOTIATING2: {

    if (msg->get_version() != ofp_version) {
      VLOG(2) << __FUNCTION__ << " message with invalid version received "
                                 "state=STATE_NEGOTIATING2, rcvd version="
              << msg->get_version() << ", negotiated version=" << ofp_version;

      size_t len = msg->length() < 64 ? msg->length() : 64;
      rofl::cmemory mem(msg->length());
      msg->pack(mem.somem(), mem.length());

      send_message(new rofl::openflow::cofmsg_error_bad_request_bad_version(
          ofp_version, msg->get_xid(), mem.somem(), len));

      delete msg;
      return;
    }

    if (rofl::openflow::OFPT_FEATURES_REPLY == msg->get_type()) {
      features_reply_rcvd(msg);
      return; // immediate return
    } else if (rofl::openflow::OFPT_ERROR == msg->get_type()) {
      error_rcvd(msg);
      return; // immediate return
    } else if (rofl::openflow::OFPT_ECHO_REQUEST == msg->get_type()) {
      echo_request_rcvd(msg);
      return; // immediate return
    }

    /* store all other messages for later handling, see below */

  } break;
  case STATE_ESTABLISHED: {

    /* sanity check: message version must match negotiated version */
    if (msg->get_version() != ofp_version) {
      VLOG(2) << __FUNCTION__ << "message with invalid version received "
                                 "state=STATE_ESTABLISHED rcvd version="
              << msg->get_version() << " negotiated version=" << ofp_version;

      rofl::cmemory mem(msg->length() < 64 ? msg->length() : 64);
      msg->pack(mem.somem(), mem.length());

      send_message(new rofl::openflow::cofmsg_error_bad_request_bad_version(
          ofp_version, msg->get_xid(), mem.somem(), mem.length()));

      delete msg;
      return;
    }

    if (rofl::openflow::OFPT_ECHO_REQUEST == msg->get_type()) {
      echo_request_rcvd(msg);
      return; // immediate return
    } else if (rofl::openflow::OFPT_ECHO_REPLY == msg->get_type()) {
      echo_reply_rcvd(msg);
      return; // immediate return
    }

  } break;
  default: {

    VLOG(2) << __FUNCTION__
            << "message received in invalid state, dropping state=" << state
            << " message=" << msg->str();

    /* drop messages in any other state */
    delete msg;
    return;
  };
  }

  try {

    /* check pending xids */
    if (has_pending_request(msg->get_xid())) {
      drop_pending_request(msg->get_xid());
    }

    /* Store message in appropriate rxqueue:
     * Strategy: we enforce queueing of successful received messages
     * in rxqueues in any case and never drop messages. However, we
     * disable reception of further messages from our peer, if we
     * exceed the rxqueues capacity threshold. Once, the application
     * starts reading message from the rxqueues, we reenable the
     * socket. See method crofconn::handle_rx_messages() for details. */
    switch (ofp_version.load()) {
    case rofl::openflow10::OFP_VERSION: {
      switch (msg->get_type()) {
      case rofl::openflow10::OFPT_PACKET_IN:
      case rofl::openflow10::OFPT_PACKET_OUT: {
        rxqueues[QUEUE_PKT].store(msg, true);
        if (rxqueues[QUEUE_PKT].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_PKT] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      case rofl::openflow10::OFPT_FLOW_MOD:
      case rofl::openflow10::OFPT_FLOW_REMOVED:
      case rofl::openflow10::OFPT_STATS_REQUEST:
      case rofl::openflow10::OFPT_STATS_REPLY:
      case rofl::openflow10::OFPT_BARRIER_REQUEST:
      case rofl::openflow10::OFPT_BARRIER_REPLY: {
        rxqueues[QUEUE_FLOW].store(msg, true);
        if (rxqueues[QUEUE_FLOW].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_FLOW] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      case rofl::openflow10::OFPT_HELLO:
      case rofl::openflow10::OFPT_ECHO_REQUEST:
      case rofl::openflow10::OFPT_ECHO_REPLY: {
        rxqueues[QUEUE_OAM].store(msg, true);
        if (rxqueues[QUEUE_OAM].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_OAM] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      default: {
        rxqueues[QUEUE_MGMT].store(msg, true);

        if (rxqueues[QUEUE_MGMT].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_MGMT] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      };
      }
    } break;
    case rofl::openflow12::OFP_VERSION: {
      switch (msg->get_type()) {
      case rofl::openflow12::OFPT_PACKET_IN:
      case rofl::openflow12::OFPT_PACKET_OUT: {
        rxqueues[QUEUE_PKT].store(msg, true);
        if (rxqueues[QUEUE_PKT].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_PKT] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      case rofl::openflow12::OFPT_FLOW_MOD:
      case rofl::openflow12::OFPT_FLOW_REMOVED:
      case rofl::openflow12::OFPT_GROUP_MOD:
      case rofl::openflow12::OFPT_PORT_MOD:
      case rofl::openflow12::OFPT_TABLE_MOD:
      case rofl::openflow12::OFPT_STATS_REQUEST:
      case rofl::openflow12::OFPT_STATS_REPLY:
      case rofl::openflow12::OFPT_BARRIER_REQUEST:
      case rofl::openflow12::OFPT_BARRIER_REPLY: {
        rxqueues[QUEUE_FLOW].store(msg, true);
        if (rxqueues[QUEUE_FLOW].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_FLOW] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      case rofl::openflow12::OFPT_HELLO:
      case rofl::openflow12::OFPT_ECHO_REQUEST:
      case rofl::openflow12::OFPT_ECHO_REPLY: {
        rxqueues[QUEUE_OAM].store(msg, true);
        if (rxqueues[QUEUE_OAM].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_OAM] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      default: {
        rxqueues[QUEUE_MGMT].store(msg, true);
        if (rxqueues[QUEUE_MGMT].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_MGMT] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      };
      }
    } break;
    case rofl::openflow13::OFP_VERSION: {
      switch (msg->get_type()) {
      case rofl::openflow13::OFPT_PACKET_IN:
      case rofl::openflow13::OFPT_PACKET_OUT: {
        rxqueues[QUEUE_PKT].store(msg, true);
        if (rxqueues[QUEUE_PKT].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_PKT] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      case rofl::openflow13::OFPT_FLOW_MOD:
      case rofl::openflow13::OFPT_FLOW_REMOVED:
      case rofl::openflow13::OFPT_GROUP_MOD:
      case rofl::openflow13::OFPT_PORT_MOD:
      case rofl::openflow13::OFPT_TABLE_MOD:
      case rofl::openflow13::OFPT_MULTIPART_REQUEST:
      case rofl::openflow13::OFPT_MULTIPART_REPLY:
      case rofl::openflow13::OFPT_BARRIER_REQUEST:
      case rofl::openflow13::OFPT_BARRIER_REPLY: {
        rxqueues[QUEUE_FLOW].store(msg, true);
        if (rxqueues[QUEUE_FLOW].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_FLOW] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      case rofl::openflow13::OFPT_HELLO:
      case rofl::openflow13::OFPT_ECHO_REQUEST:
      case rofl::openflow13::OFPT_ECHO_REPLY: {
        rxqueues[QUEUE_OAM].store(msg, true);
        if (rxqueues[QUEUE_OAM].capacity() == 0) {
          if (not rofsock.is_rx_disabled()) {
            VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_OAM] capacity "
                                       "exhausted; action: disabling "
                                       "reception of messages";
          }
          rofsock.rx_disable();
        }
      } break;
      default: {
        rxqueues[QUEUE_MGMT].store(msg, true);
        if (rxqueues[QUEUE_MGMT].capacity() == 0) {
          VLOG(2) << __FUNCTION__ << " rxqueues[QUEUE_MGMT] capacity "
                                     "exhausted; action: disabling "
                                     "reception of messages";
          rofsock.rx_disable();
        }
      };
      }
    } break;
    default: {
      delete msg;
      return;
    };
    }

  } catch (eRofQueueFull &e) {
    VLOG(1) << __FUNCTION__ << " error: " << e.what();
    /* throttle reception of further messages */
    rofsock.rx_disable();
  }

  /* wakeup working thread in state ESTABLISHED; otherwise keep sleeping
   * and enqueue message until state ESTABLISHED is reached */
  if ((STATE_ESTABLISHED == state) && (not rx_thread_working)) {
    thread.wakeup();
  }
}

void crofconn::handle_wakeup(cthread &thread) { handle_rx_messages(); }

void crofconn::handle_rx_messages() {
  /* we start with handling incoming messages */
  rx_thread_working = true;
  rx_thread_scheduled = false;

  thread.drop_timer(TIMER_ID_NEED_LIFE_CHECK);

  unsigned int keep_running = 1;

  do {
    try {
      /* iterate over all rxqueues */
      for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

        if (STATE_ESTABLISHED != state) {
          rx_thread_working = false;
          return;
        }

        if (rxqueues[queue_id].empty()) {
          continue; // no messages at all in this queue
        }

        /* get not more than rxweights[queue_id] messages from rxqueue[queue_id]
         */
        for (unsigned int num = 0; num < rxweights[queue_id]; ++num) {

          rofl::openflow::cofmsg *msg = (rofl::openflow::cofmsg *)0;

          if (STATE_ESTABLISHED != state) {
            rx_thread_working = false;
            return;
          }

          if ((msg = rxqueues[queue_id].retrieve()) == NULL) {
            break; // no further messages in this queue
          }

          /* segmentation and reassembly */
          switch (ofp_version.load()) {
          case rofl::openflow10::OFP_VERSION:
          case rofl::openflow12::OFP_VERSION: {
            VLOG(3) << __FUNCTION__
                    << " call application: " << msg->str().c_str();
            // no segmentation and reassembly below OFP1.3, so hand over message
            // directly to higher layers
            crofconn_env::call_env(env).handle_recv(*this, msg);
          } break;
          default: {
            switch (msg->get_type()) {
            case rofl::openflow13::OFPT_MULTIPART_REQUEST:
            case rofl::openflow13::OFPT_MULTIPART_REPLY: {
              handle_rx_multipart_message(msg);
            } break;
            default: {
              VLOG(3) << __FUNCTION__
                      << " call application: " << msg->str().c_str();
              crofconn_env::call_env(env).handle_recv(*this, msg);
            };
            }
          };
          }
        }

        /* reschedule this method */
        if (not rxqueues[queue_id].empty()) {
          thread.wakeup();
        }
      }

      /* not connected any more, stop running working thread */
      if (STATE_ESTABLISHED != state) {
        rx_thread_working = false;
        rx_thread_scheduled = true;
        return;
      }

    } catch (eRofConnNotFound &e) {
      /* environment not found */
      VLOG(1) << __FUNCTION__ << " error: " << e.what();
      keep_running = false;

    } catch (std::runtime_error &e) {
      VLOG(1) << __FUNCTION__ << " runtime error: " << e.what();
    }

  } while (keep_running--);

  rx_thread_working = false;

  thread.add_timer(TIMER_ID_NEED_LIFE_CHECK,
                   ctimespec().expire_in(timeout_lifecheck));

  /* reenable reception of messages on socket */
  if (rofsock.is_rx_disabled()) {
    VLOG(2) << __FUNCTION__
            << " re-enabling reception of messages on underlying socket";
    rofsock.rx_enable();
  }
}

void crofconn::handle_rx_multipart_message(rofl::openflow::cofmsg *msg) {
  switch (msg->get_type()) {
  case OFPT_MULTIPART_REQUEST: {
    /*
     * add multipart support here for receiving messages
     */
    rofl::openflow::cofmsg_stats_request *stats =
        dynamic_cast<rofl::openflow::cofmsg_stats_request *>(msg);

    if (NULL == stats) {
      VLOG(1) << __FUNCTION__
              << " dropping multipart request, invalid message type";
      delete msg;
      return;
    }

    // start new or continue pending transaction
    if (stats->get_stats_flags() & rofl::openflow13::OFPMPF_REQ_MORE) {

      set_pending_segment(msg->get_xid(), stats->get_type(),
                          stats->get_stats_type())
          .store_and_merge_msg(*msg);
      delete msg; // delete msg here, we store a copy in the transaction

      // end pending transaction or multipart message with single message only
    } else {

      if (has_pending_segment(msg->get_xid())) {

        set_pending_segment(msg->get_xid(), stats->get_type(),
                            stats->get_stats_type())
            .store_and_merge_msg(*msg);

        rofl::openflow::cofmsg *reassembled_msg =
            set_pending_segment(msg->get_xid()).retrieve_and_detach_msg();

        drop_pending_segment(msg->get_xid());

        delete msg; // delete msg here, we may get an exception from the next
                    // line

        crofconn_env::call_env(env).handle_recv(*this, reassembled_msg);
      } else {
        VLOG(3) << __FUNCTION__ << " call application: " << msg->str().c_str();
        // do not delete msg here, will be done by higher layers
        crofconn_env::call_env(env).handle_recv(*this, msg);
      }
    }
  } break;
  case OFPT_MULTIPART_REPLY: {
    /*
     * add multipart support here for receiving messages
     */
    rofl::openflow::cofmsg_stats_reply *stats =
        dynamic_cast<rofl::openflow::cofmsg_stats_reply *>(msg);

    if (NULL == stats) {
      VLOG(1) << __FUNCTION__
              << " dropping multipart reply, invalid message type";
      delete msg;
      return;
    }

    // start new or continue pending transaction
    if (stats->get_stats_flags() & rofl::openflow13::OFPMPF_REQ_MORE) {

      set_pending_segment(msg->get_xid(), stats->get_type(),
                          stats->get_stats_type())
          .store_and_merge_msg(*msg);
      delete msg; // delete msg here, we store a copy in the transaction

      // end pending transaction or multipart message with single message only
    } else {

      if (has_pending_segment(msg->get_xid())) {

        set_pending_segment(msg->get_xid(), stats->get_type(),
                            stats->get_stats_type())
            .store_and_merge_msg(*msg);

        rofl::openflow::cofmsg *reassembled_msg =
            set_pending_segment(msg->get_xid()).retrieve_and_detach_msg();

        drop_pending_segment(msg->get_xid());

        delete msg; // delete msg here, we may get an exception from the next
                    // line

        crofconn_env::call_env(env).handle_recv(*this, reassembled_msg);
      } else {
        VLOG(3) << __FUNCTION__ << " call application: " << msg->str().c_str();
        // do not delete msg here, will be done by higher layers
        crofconn_env::call_env(env).handle_recv(*this, msg);
      }
    }
  } break;
  default: {};
  }
}

rofl::crofsock::msg_result_t crofconn::send_message(rofl::openflow::cofmsg *msg,
                                                    const ctimespec &ts) {

  switch (msg->get_version()) {
  case rofl::openflow10::OFP_VERSION: {

    switch (msg->get_type()) {
    case rofl::openflow10::OFPT_STATS_REQUEST: {
      add_pending_request(
          msg->get_xid(), ts, msg->get_type(),
          dynamic_cast<rofl::openflow::cofmsg_stats_request *>(msg)
              ->get_stats_type());
    } break;
    default: { add_pending_request(msg->get_xid(), ts, msg->get_type()); };
    }

  } break;
  case rofl::openflow12::OFP_VERSION: {

    switch (msg->get_type()) {
    case rofl::openflow12::OFPT_STATS_REQUEST: {
      add_pending_request(
          msg->get_xid(), ts, msg->get_type(),
          dynamic_cast<rofl::openflow::cofmsg_stats_request *>(msg)
              ->get_stats_type());
    } break;
    default: { add_pending_request(msg->get_xid(), ts, msg->get_type()); };
    }

  } break;
  case rofl::openflow13::OFP_VERSION: {

    switch (msg->get_type()) {
    case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
      add_pending_request(
          msg->get_xid(), ts, msg->get_type(),
          dynamic_cast<rofl::openflow::cofmsg_stats_request *>(msg)
              ->get_stats_type());
    } break;
    default: { add_pending_request(msg->get_xid(), ts, msg->get_type()); };
    }

  } break;
  default: {};
  }

  return segment_and_send_message(msg);
};

rofl::crofsock::msg_result_t
crofconn::segment_and_send_message(rofl::openflow::cofmsg *msg) {
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_IGNORED;

  VLOG(3) << __FUNCTION__ << " state: " << state
          << " message sent: " << msg->str().c_str();

  if (msg->length() <= segmentation_threshold) {
    msg_result = rofsock.send_message(
        msg); // default behaviour for now: send message directly to rofsock

  } else {

    // segment the packet
    switch (msg->get_version()) {
    case rofl::openflow12::OFP_VERSION: {

      switch (msg->get_type()) {
      case rofl::openflow12::OFPT_STATS_REPLY: {

        switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply *>(msg)
                    ->get_stats_type()) {
        case rofl::openflow12::OFPST_FLOW: {
          msg_result = segment_flow_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply *>(msg));
        } break;
        case rofl::openflow12::OFPST_TABLE: {
          msg_result = segment_table_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_table_stats_reply *>(msg));
        } break;
        case rofl::openflow12::OFPST_QUEUE: {
          msg_result = segment_queue_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply *>(msg));
        } break;
        case rofl::openflow12::OFPST_GROUP: {
          msg_result = segment_group_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_group_stats_reply *>(msg));
        } break;
        case rofl::openflow12::OFPST_GROUP_DESC: {
          msg_result = segment_group_desc_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply *>(
                  msg));
        } break;
        }

      } break;
      }
    } break;
    case rofl::openflow13::OFP_VERSION: {

      switch (msg->get_type()) {
      case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

        switch (dynamic_cast<rofl::openflow::cofmsg_stats_request *>(msg)
                    ->get_stats_type()) {
        case rofl::openflow13::OFPMP_TABLE_FEATURES: {
          msg_result = segment_table_features_stats_request(
              dynamic_cast<
                  rofl::openflow::cofmsg_table_features_stats_request *>(msg));
        } break;
        }

      } break;
      case rofl::openflow13::OFPT_MULTIPART_REPLY: {

        switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply *>(msg)
                    ->get_stats_type()) {
        case rofl::openflow13::OFPMP_FLOW: {
          msg_result = segment_flow_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply *>(msg));
        } break;
        case rofl::openflow13::OFPMP_TABLE: {
          msg_result = segment_table_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_table_stats_reply *>(msg));
        } break;
        case rofl::openflow13::OFPMP_PORT_STATS: {
          msg_result = segment_port_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_port_stats_reply *>(msg));
        } break;
        case rofl::openflow13::OFPMP_QUEUE: {
          msg_result = segment_queue_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply *>(msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP: {
          msg_result = segment_group_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_group_stats_reply *>(msg));
        } break;
        case rofl::openflow13::OFPMP_GROUP_DESC: {
          msg_result = segment_group_desc_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply *>(
                  msg));
        } break;
        case rofl::openflow13::OFPMP_TABLE_FEATURES: {
          msg_result = segment_table_features_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply *>(
                  msg));
        } break;
        case rofl::openflow13::OFPMP_PORT_DESC: {
          msg_result = segment_port_desc_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply *>(
                  msg));
        } break;
        case rofl::openflow13::OFPMP_METER: {
          msg_result = segment_meter_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply *>(msg));
        } break;
        case rofl::openflow13::OFPMP_METER_CONFIG: {
          msg_result = segment_meter_config_stats_reply(
              dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply *>(
                  msg));
        } break;
        case rofl::openflow13::OFPMP_METER_FEATURES: {
          // no array in meter-features, so no need to segment
          msg_result = rofsock.send_message(
              msg); // default behaviour for now: send message
                    // directly to rofsock
        } break;
        }

      } break;
      }
    } break;
    }
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_table_features_stats_request(
    rofl::openflow::cofmsg_table_features_stats_request *msg) {
  std::list<rofl::openflow::cofmsg_table_features_stats_request *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto tableids = msg->get_tables().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not tableids.empty()) {
    rofl::openflow::coftables array(msg->get_version());

    while ((not tableids.empty()) && (array.length() < MAX_LENGTH)) {
      uint8_t tableid = tableids.front();
      tableids.pop_front();
      array.add_table(tableid) = msg->get_tables().get_table(tableid);
    }

    segments.push_back(new rofl::openflow::cofmsg_table_features_stats_request(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_table_features_stats_request *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REQ_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_table_features_stats_reply(
    rofl::openflow::cofmsg_table_features_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_table_features_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto tableids = msg->get_tables().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not tableids.empty()) {
    rofl::openflow::coftables array(msg->get_version());

    while ((not tableids.empty()) && (array.length() < MAX_LENGTH)) {
      uint8_t tableid = tableids.front();
      tableids.pop_front();
      array.add_table(tableid) = msg->get_tables().get_table(tableid);
    }

    segments.push_back(new rofl::openflow::cofmsg_table_features_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_table_features_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_flow_stats_reply(
    rofl::openflow::cofmsg_flow_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_flow_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto flowids = msg->get_flow_stats_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not flowids.empty()) {
    rofl::openflow::cofflowstatsarray array(msg->get_version());

    while ((not flowids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t flowid = flowids.front();
      flowids.pop_front();
      array.add_flow_stats(flowid) =
          msg->get_flow_stats_array().get_flow_stats(flowid);
    }

    segments.push_back(new rofl::openflow::cofmsg_flow_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_flow_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_group_desc_stats_reply(
    rofl::openflow::cofmsg_group_desc_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_group_desc_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto groupids = msg->get_group_desc_stats_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not groupids.empty()) {
    rofl::openflow::cofgroupdescstatsarray array(msg->get_version());

    while ((not groupids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t groupid = groupids.front();
      groupids.pop_front();
      array.add_group_desc_stats(groupid) =
          msg->get_group_desc_stats_array().get_group_desc_stats(groupid);
    }

    segments.push_back(new rofl::openflow::cofmsg_group_desc_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_group_desc_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_group_stats_reply(
    rofl::openflow::cofmsg_group_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_group_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto groupids = msg->get_group_stats_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not groupids.empty()) {
    rofl::openflow::cofgroupstatsarray array(msg->get_version());

    while ((not groupids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t groupid = groupids.front();
      groupids.pop_front();
      array.add_group_stats(groupid) =
          msg->get_group_stats_array().get_group_stats(groupid);
    }

    segments.push_back(new rofl::openflow::cofmsg_group_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_group_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_table_stats_reply(
    rofl::openflow::cofmsg_table_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_table_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto tableids = msg->get_table_stats_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not tableids.empty()) {
    rofl::openflow::coftablestatsarray array(msg->get_version());

    while ((not tableids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t tableid = tableids.front();
      tableids.pop_front();
      array.add_table_stats(tableid) =
          msg->get_table_stats_array().get_table_stats(tableid);
    }

    segments.push_back(new rofl::openflow::cofmsg_table_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_table_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_port_stats_reply(
    rofl::openflow::cofmsg_port_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_port_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto portids = msg->get_port_stats_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not portids.empty()) {
    rofl::openflow::cofportstatsarray array(msg->get_version());

    while ((not portids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t portid = portids.front();
      portids.pop_front();
      array.add_port_stats(portid) =
          msg->get_port_stats_array().get_port_stats(portid);
    }

    segments.push_back(new rofl::openflow::cofmsg_port_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_port_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_queue_stats_reply(
    rofl::openflow::cofmsg_queue_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_queue_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto portids = msg->get_queue_stats_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not portids.empty()) {
    rofl::openflow::cofqueuestatsarray array(msg->get_version());

    while ((not portids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t portid = portids.front();
      portids.pop_front();

      for (auto queueid : msg->get_queue_stats_array().keys(portid)) {
        array.add_queue_stats(portid, queueid) =
            msg->get_queue_stats_array().get_queue_stats(portid, queueid);
      }
    }

    segments.push_back(new rofl::openflow::cofmsg_queue_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_queue_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_port_desc_stats_reply(
    rofl::openflow::cofmsg_port_desc_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_port_desc_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto portids = msg->get_ports().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not portids.empty()) {
    rofl::openflow::cofports ports(msg->get_version());

    while ((not portids.empty()) && (ports.length() < MAX_LENGTH)) {
      uint32_t portid = portids.front();
      portids.pop_front();
      ports.add_port(portid) = msg->get_ports().get_port(portid);
    }

    segments.push_back(new rofl::openflow::cofmsg_port_desc_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), ports));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_port_desc_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_meter_stats_reply(
    rofl::openflow::cofmsg_meter_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_meter_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto meterids = msg->get_meter_stats_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not meterids.empty()) {
    rofl::openflow::cofmeterstatsarray array(msg->get_version());

    while ((not meterids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t meterid = meterids.front();
      meterids.pop_front();
      array.add_meter_stats(meterid) =
          msg->get_meter_stats_array().get_meter_stats(meterid);
    }

    segments.push_back(new rofl::openflow::cofmsg_meter_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_meter_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}

rofl::crofsock::msg_result_t crofconn::segment_meter_config_stats_reply(
    rofl::openflow::cofmsg_meter_config_stats_reply *msg) {
  std::list<rofl::openflow::cofmsg_meter_config_stats_reply *> segments;
  const int MAX_LENGTH = 64000 /*bytes*/;
  auto meterids = msg->get_meter_config_array().keys();
  rofl::crofsock::msg_result_t msg_result = rofl::crofsock::MSG_QUEUED;

  /* create fragments */
  while (not meterids.empty()) {
    rofl::openflow::cofmeterconfigarray array(msg->get_version());

    while ((not meterids.empty()) && (array.length() < MAX_LENGTH)) {
      uint32_t meterid = meterids.front();
      meterids.pop_front();
      array.add_meter_config(meterid) =
          msg->get_meter_config_array().get_meter_config(meterid);
    }

    segments.push_back(new rofl::openflow::cofmsg_meter_config_stats_reply(
        msg->get_version(), msg->get_xid(), msg->get_stats_flags(), array));
  }

  /* delete original message */
  delete msg;

  /* send fragments */
  while (not segments.empty()) {
    rofl::openflow::cofmsg_meter_config_stats_reply *msg = segments.front();
    segments.pop_front();
    // set MORE flag on all segments except last one
    if (not segments.empty()) {
      msg->set_stats_flags(msg->get_stats_flags() |
                           rofl::openflow13::OFPMPF_REPLY_MORE);
    }
    /* when enforcing queueing, there are only two return values possible:
     * MSQ_QUEUED and MSG_QUEUED_CONGESTION. We return the result received for
     * the last fragment. */
    msg_result = rofsock.send_message(msg, /*enforce-queueing*/ true);
  }

  return msg_result;
}
