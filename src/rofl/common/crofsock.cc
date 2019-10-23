/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofsock.cpp
 *
 *  Created on: 31.12.2013
 *  Revised on: 20.09.2015
 *      Author: andreas
 */

#include "crofsock.h"

#include <glog/logging.h>

using namespace rofl;

/*static*/ std::set<crofsock_env *> crofsock_env::socket_envs;
/*static*/ crwlock crofsock_env::socket_envs_lock;

crofsock::~crofsock() {
  flag_set(FLAG_DELETE_IN_PROGRESS, true);
  cthread::thread(tx_thread_num).drop(this);
  cthread::thread(rx_thread_num).drop(this);
  close();
}

crofsock::crofsock(crofsock_env *env)
    : env(env), state(STATE_IDLE), mode(MODE_UNKNOWN),
      rx_thread_num(cthread::get_io_thread_num_from_pool()),
      tx_thread_num(cthread::get_io_thread_num_from_pool()),
      reconnect_backoff_max(60 /*secs*/), reconnect_backoff_start(1 /*secs*/),
      reconnect_backoff_current(1 /*secs*/), reconnect_counter(0), sd(-1),
      domain(AF_INET), type(SOCK_STREAM), protocol(IPPROTO_TCP), backlog(64),
      ctx(NULL), ssl(NULL), bio(NULL), capath("."), cafile("ca.pem"),
      certfile("crt.pem"), keyfile("key.pem"), password(""),
      verify_mode("PEER"), verify_depth("1"),
      ciphers("EECDH+ECDSA+AESGCM EECDH+aRSA+AESGCM EECDH+ECDSA+SHA256 "
              "EECDH+aRSA+RC4 EDH+aRSA EECDH RC4 !aNULL !eNULL !LOW !3DES !MD5 "
              "!EXP !PSK !SRP !DSS"),
      rxbuffer((size_t)65536), rx_disabled(false), txbuffer((size_t)65536),
      tx_disabled(false), tx_is_running(false), txqueue_pending_pkts(0),
      txqueue_size_congestion_occurred(0), txqueue_size_tx_threshold(0),
      txqueues(QUEUE_MAX), txweights(QUEUE_MAX) {
  /* scheduler weights for transmission */
  txweights[QUEUE_OAM] = 16;
  txweights[QUEUE_MGMT] = 32;
  txweights[QUEUE_FLOW] = 16;
  txweights[QUEUE_PKT] = 8;
  VLOG(1) << __FUNCTION__ << " "
          << "RX thread: " << cthread::thread(rx_thread_num).get_thread_name()
          << " "
          << "TX thread: " << cthread::thread(tx_thread_num).get_thread_name()
          << " ";
}

void crofsock::close() {
  if (flag_test(FLAG_CLOSING)) {
    return;
  }
  flag_set(FLAG_CLOSING, true);
  AcquireReadWriteLock lock(tlock);

  /* shutdown TLS */
  switch (state.load()) {
  case STATE_TLS_CONNECTING: {

    VLOG(6) << __FUNCTION__ << " STATE_TLS_CONNECTING sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    tls_term_context();
    flag_set(FLAG_TLS_IN_USE, false);

    state = STATE_TCP_ESTABLISHED;

  } break;
  case STATE_TLS_ACCEPTING: {

    VLOG(6) << __FUNCTION__ << " STATE_TLS_ACCEPTING sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    tls_term_context();
    flag_set(FLAG_TLS_IN_USE, false);

    state = STATE_TCP_ESTABLISHED;

  } break;
  case STATE_TLS_ESTABLISHED: {

    VLOG(6) << __FUNCTION__ << " STATE_TLS_ESTABLISHED sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    AcquireReadWriteLock lock(sslock);

    if (ssl) {
      int rc = 0;
      int err_code = 0;

      VLOG(6) << __FUNCTION__ << " TLS: shutdown sd=" << sd;

      {
        AcquireReadWriteLock lock(sslock);
        if ((rc = SSL_shutdown(ssl)) < 0) {
          err_code = SSL_get_error(ssl, rc);
        }
      }

      if (rc < 0) {
        switch (err_code = SSL_get_error(ssl, rc)) {
        case SSL_ERROR_WANT_READ: {
          VLOG(6) << __FUNCTION__ << " TLS: shutdown WANT READ sd=" << sd;
          pthread_yield();
        } break;
        case SSL_ERROR_WANT_WRITE: {
          VLOG(6) << __FUNCTION__ << " TLS: shutdown WANT WRITE sd=" << sd;
          pthread_yield();
        } break;
        case SSL_ERROR_NONE: {
          VLOG(6) << __FUNCTION__
                  << " TLS: shutdown succeeded ERROR NONE sd=" << sd;
        }
          goto out;
        case SSL_ERROR_SSL: {
          VLOG(6) << __FUNCTION__
                  << " TLS: shutdown failed ERROR SSL sd=" << sd;
        }
          goto out;
        case SSL_ERROR_SYSCALL: {
          VLOG(6) << __FUNCTION__
                  << " TLS: shutdown failed ERROR SYSCALL sd=" << sd
                  << " error: " << ERR_reason_error_string(err_code);
        }
          goto out;
        case SSL_ERROR_ZERO_RETURN: {
          VLOG(6) << __FUNCTION__
                  << " TLS: shutdown succeeded ERROR ZERO RETURN sd=" << sd;
        }
          goto out;
        default: {
          VLOG(6) << __FUNCTION__ << " TLS: shutdown failed sd=" << sd;
        };
        }
      }
    out:

      /* block reception of any further data from remote side */
      rx_disable();
      tx_disable();
    }
    tls_term_context();
    flag_set(FLAG_TLS_IN_USE, false);

    state = STATE_TCP_ESTABLISHED;

  } break;
  default: {
  };
  }

  /* shutdown TCP */
  switch (state.load()) {
  case STATE_LISTENING: {

    VLOG(6) << __FUNCTION__ << " STATE_LISTENING sd=" << sd
            << " baddr=" << baddr.str();

    if (sd > 0) {
      cthread::thread(rx_thread_num).drop_read_fd(sd);
      cthread::thread(rx_thread_num).drop_fd(sd);
      cthread::thread(tx_thread_num).drop_fd(sd);
      ::close(sd);
      sd = -1;
    }

    state = STATE_IDLE;

  } break;
  case STATE_TCP_CONNECTING: {

    VLOG(6) << __FUNCTION__ << " STATE_TCP_CONNECTING sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    cthread::thread(tx_thread_num).drop_timer(this, TIMER_ID_RECONNECT);

    if (sd > 0) {
      cthread::thread(rx_thread_num).drop_write_fd(sd);
      cthread::thread(rx_thread_num).drop_fd(sd);
      cthread::thread(tx_thread_num).drop_fd(sd);
      ::close(sd);
      sd = -1;
    }

    state = STATE_IDLE;

  } break;
  case STATE_TCP_ACCEPTING: {

    VLOG(6) << __FUNCTION__ << " STATE_TCP_ACCEPTING sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    if (sd > 0) {
      cthread::thread(rx_thread_num).drop_read_fd(sd, false);
      if (flag_test(FLAG_CONGESTED)) {
        cthread::thread(tx_thread_num).drop_write_fd(sd);
      }
      cthread::thread(rx_thread_num).drop_fd(sd);
      cthread::thread(tx_thread_num).drop_fd(sd);
      ::close(sd);
      sd = -1;
    }

    state = STATE_IDLE;

  } break;
  case STATE_TCP_ESTABLISHED: {

    VLOG(6) << __FUNCTION__ << " STATE_TCP_ESTABLISHED sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    /* block reception of any further data from remote side */
    rx_disable();
    tx_disable();

    cthread::thread(rx_thread_num).drop_read_fd(sd, false);
    if (flag_test(FLAG_CONGESTED)) {
      cthread::thread(tx_thread_num).drop_write_fd(sd);
    }
    shutdown(sd, O_RDWR);

    /* allow socket to send shutdown notification to peer */
    /* sleep(1); // use SO_LINGER option instead */
    if (sd > 0) {
      cthread::thread(rx_thread_num).drop_fd(sd);
      cthread::thread(tx_thread_num).drop_fd(sd);
      ::close(sd);
    }
    sd = -1;

    state = STATE_IDLE;

  } break;
  default: {
  };
  }

  /* reset internal variables for next round */
  switch (state.load()) {
  case STATE_IDLE: {

    VLOG(6) << __FUNCTION__ << " STATE_IDLE sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str()
            << " baddr=" << baddr.str();

    /* TLS down, TCP down => set rx_disabled flag back to false */
    rx_disabled = false;
    tx_disabled = false;

    flag_set(FLAG_CLOSING, false);
  }
    return; // leave while loop
  default: {
  };
  }
}

void crofsock::listen() {
  int rc;

  if (sd >= 0) {
    close();
  }

  /* remove all pending messages from tx queues */
  for (auto queue : txqueues) {
    queue.clear();
  }

  /* cancel potentially pending reconnect timer */
  cthread::thread(rx_thread_num).drop_timer(this, TIMER_ID_RECONNECT);

  /* socket in server mode */
  mode = MODE_LISTEN;

  /* reconnect does not make sense for listening sockets */
  flag_set(FLAG_RECONNECT_ON_FAILURE, false);

  /* open socket */
  if ((sd = ::socket(baddr.get_family(), type, protocol)) < 0) {
    throw eSysCall("eSysCall", "socket", __FILE__, __FUNCTION__, __LINE__);
  }

  /* make socket non-blocking */
  int flags;
  if ((flags = ::fcntl(sd, F_GETFL)) < 0) {
    throw eSysCall("eSysCall", "fcntl (F_GETFL)", __FILE__, __FUNCTION__,
                   __LINE__);
  }
  flags |= O_NONBLOCK;
  if ((rc = ::fcntl(sd, F_SETFL, flags)) < 0) {
    throw eSysCall("eSysCall", "fcntl (F_SETFL)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
    int optval = 1;

    // set SO_REUSEADDR option on TCP sockets
    if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (int *)&optval,
                           sizeof(optval))) < 0) {
      throw eSysCall("eSysCall", "setsockopt (SO_REUSEADDR)", __FILE__,
                     __FUNCTION__, __LINE__);
    }

#if 0
		int on = 1;
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) < 0) {
			throw eSysCall("eSysCall", "setsockopt (SO_REUSEPORT)", __FILE__, __FUNCTION__, __LINE__);
		}
#endif

    // set TCP_NODELAY option on TCP sockets
    if ((rc = ::setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int *)&optval,
                           sizeof(optval))) < 0) {
      throw eSysCall("eSysCall", "setsockopt (TCP_NODELAY)", __FILE__,
                     __FUNCTION__, __LINE__);
    }

    // set SO_RCVLOWAT
    if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_RCVLOWAT, (int *)&optval,
                           sizeof(optval))) < 0) {
      throw eSysCall("eSysCall", "setsockopt (SO_RCVLOWAT)", __FILE__,
                     __FUNCTION__, __LINE__);
    }

    // read TCP_NODELAY option for debugging purposes
    socklen_t optlen = sizeof(int);
    int optvalc;
    if ((rc = ::getsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int *)&optvalc,
                           &optlen)) < 0) {
      throw eSysCall("eSysCall", "getsockopt (TCP_NODELAY)", __FILE__,
                     __FUNCTION__, __LINE__);
    }
  }

  /* bind to local address */
  if ((rc = ::bind(sd, baddr.ca_saddr, (socklen_t)(baddr.salen))) < 0) {
    throw eSysCall("eSysCall", "bind", __FILE__, __FUNCTION__, __LINE__);
  }

  /* listen on socket */
  if ((rc = ::listen(sd, backlog)) < 0) {
    throw eSysCall("eSysCall", "listen", __FILE__, __FUNCTION__, __LINE__);
  }

  state = STATE_LISTENING;

  VLOG(6) << __FUNCTION__ << " STATE_LISTENING sd=" << sd
          << " baddr=" << baddr.str();

  /* instruct cthread::thread(rx_thread_num) to read from socket descriptor */
  cthread::thread(rx_thread_num).add_read_fd(this, sd);
}

std::list<int> crofsock::accept() {
  std::list<int> sockfds;

  bool read_more = true;
  while (read_more) {
    int sockfd;

    if (flag_test(FLAG_CLOSING)) {
      sockfds.clear();
      return sockfds;
    }

    /* extract new connection from listening queue */
    if ((sockfd = ::accept(this->sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
      switch (errno) {
      case EAGAIN: {
        read_more = false;
      } break;
      default: {
        throw eSysCall("eSysCall", "accept", __FILE__, __FUNCTION__, __LINE__);
      };
      }
    } else {
      sockfds.push_back(sockfd);
    }
  }

  return sockfds;
}

void crofsock::tcp_accept(int sd) {
  if (get_state() != STATE_IDLE) {
    close();
  }
  this->sd = sd;

  /* remove all pending messages from tx queues */
  for (auto queue : txqueues) {
    queue.clear();
  }

  /* cancel potentially pending reconnect timer */
  cthread::thread(rx_thread_num).drop_timer(this, TIMER_ID_RECONNECT);

  /* socket in server mode */
  mode = MODE_SERVER;

  /* reconnect is not possible for server sockets */
  flag_set(FLAG_RECONNECT_ON_FAILURE, false);

  /* new state */
  state = STATE_TCP_ACCEPTING;

  VLOG(6) << __FUNCTION__ << " STATE_TCP_ACCEPTING sd=" << sd
          << " laddr=" << laddr.str() << " raddr=" << raddr.str();

  /* make socket non-blocking, as this status is not inherited */
  int sockflags;
  if ((sockflags = ::fcntl(sd, F_GETFL)) < 0) {
    throw eSysCall("eSysCall", "fcntl (F_GETFL)", __FILE__, __FUNCTION__,
                   __LINE__);
  }
  sockflags |= O_NONBLOCK;
  if ((::fcntl(sd, F_SETFL, sockflags)) < 0) {
    throw eSysCall("eSysCall", "fcntl (F_SETFL)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  socklen_t optlen = 0;
  if ((::getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
    throw eSysCall("eSysCall", "getsockname", __FILE__, __FUNCTION__, __LINE__);
  }

  if ((::getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
    throw eSysCall("eSysCall", "getpeername", __FILE__, __FUNCTION__, __LINE__);
  }

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
  optlen = sizeof(domain);
  if ((::getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
    throw eSysCall("eSysCall", "getsockopt (SO_DOMAIN)", __FILE__, __FUNCTION__,
                   __LINE__);
  }
#endif

  optlen = sizeof(type);
  if ((::getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
    throw eSysCall("eSysCall", "getsockopt (SO_TYPE)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
  optlen = sizeof(protocol);
  if ((::getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
    throw eSysCall("eSysCall", "getsockopt (SO_PROTOCOL)", __FILE__,
                   __FUNCTION__, __LINE__);
  }
#endif

  /* set REUSEADDR and TCP_NODELAY options (for TCP sockets only) */
  if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
    int optval = 1;

    /* set SO_REUSEADDR option */
    if (::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) <
        0) {
      throw eSysCall("eSysCall", "setsockopt (SO_REUSEADDR)", __FILE__,
                     __FUNCTION__, __LINE__);
    }

    /* set TCP_NODELAY option */
    if (::setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) <
        0) {
      throw eSysCall("eSysCall", "setsockopt (TCP_NODELAY)", __FILE__,
                     __FUNCTION__, __LINE__);
    }
  }

  /* get SO_LINGER option */
  struct linger l;
  socklen_t l_len = sizeof(l);
  if ((::getsockopt(sd, SOL_SOCKET, SO_LINGER, &l, &l_len)) < 0) {
    throw eSysCall("eSysCall", "getsockopt (SO_LINGER)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  /* set SO_LINGER option */
  l.l_onoff = 1;  /* activate SO_LINGER option */
  l.l_linger = 1; /* second(s) */
  if ((::setsockopt(sd, SOL_SOCKET, SO_LINGER, &l, l_len)) < 0) {
    throw eSysCall("eSysCall", "setsockopt (SO_LINGER)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  state = STATE_TCP_ESTABLISHED;

  VLOG(6) << __FUNCTION__ << " STATE_TCP_ESTABLISHED sd=" << sd
          << " laddr=" << laddr.str() << " raddr=" << raddr.str();

  /* instruct cthread::thread(rx_thread_num) to read from socket descriptor */
  cthread::thread(rx_thread_num).add_read_fd(this, sd, true, true);
  cthread::thread(rx_thread_num).wakeup(this);

  if (flag_test(FLAG_TLS_IN_USE)) {
    crofsock::tls_accept(sd);
  } else {
    crofsock_env::call_env(env).handle_tcp_accepted(*this);
  }
}

void crofsock::tcp_connect(bool reconnect) {
  int rc;

  if (get_state() != STATE_IDLE) {
    close();
  }

  /* remove all pending messages from tx queues */
  for (auto queue : txqueues) {
    queue.clear();
  }

  /* cancel potentially pending reconnect timer */
  cthread::thread(rx_thread_num).drop_timer(this, TIMER_ID_RECONNECT);

  /* we do an active connect */
  mode = MODE_CLIENT;

  /* reconnect in case of an error? */
  flag_set(FLAG_RECONNECT_ON_FAILURE, reconnect);

  /* new state */
  state = STATE_TCP_CONNECTING;

  /* open socket */
  if ((sd = ::socket(raddr.get_family(), type, protocol)) < 0) {
    throw eSysCall("eSysCall", "socket", __FILE__, __FUNCTION__, __LINE__);
  }

  VLOG(6) << __FUNCTION__ << " STATE_TCP_CONNECTING sd=" << sd
          << " laddr=" << laddr.str() << " raddr=" << raddr.str();

  /* make socket non-blocking */
  int sockflags;
  if ((sockflags = ::fcntl(sd, F_GETFL)) < 0) {
    throw eSysCall("eSysCall", "fcntl (F_GETCL)", __FILE__, __FUNCTION__,
                   __LINE__);
  }
  sockflags |= O_NONBLOCK;
  if ((rc = ::fcntl(sd, F_SETFL, sockflags)) < 0) {
    throw eSysCall("eSysCall", "fcntl (F_SETCL)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  /* set REUSEADDR and TCP_NODELAY options (for TCP sockets only) */
  if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
    int optval = 1;

    /* set SO_REUSEADDR option */
    if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval,
                           sizeof(optval))) < 0) {
      throw eSysCall("eSysCall", "setsockopt (SO_REUSEADDR)", __FILE__,
                     __FUNCTION__, __LINE__);
    }

    /* set TCP_NODELAY option */
    if ((rc = ::setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval,
                           sizeof(optval))) < 0) {
      throw eSysCall("eSysCall", "setsockopt (TCP_NODELAY)", __FILE__,
                     __FUNCTION__, __LINE__);
    }
  }

  /* get SO_LINGER option */
  struct linger l;
  socklen_t l_len = sizeof(l);
  if ((rc = ::getsockopt(sd, SOL_SOCKET, SO_LINGER, &l, &l_len)) < 0) {
    throw eSysCall("eSysCall", "getsockopt (SO_LINGER)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  /* set SO_LINGER option */
  l.l_onoff = 1;  /* activate SO_LINGER option */
  l.l_linger = 1; /* second(s) */
  if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_LINGER, &l, l_len)) < 0) {
    throw eSysCall("eSysCall", "setsockopt (SO_LINGER)", __FILE__, __FUNCTION__,
                   __LINE__);
  }

  /* bind to local address */
  if (laddr.get_family() != AF_UNSPEC) {
    if ((rc = ::bind(sd, laddr.ca_saddr, (socklen_t)(laddr.salen))) < 0) {
      throw eSysCall("eSysCall", "bind", __FILE__, __FUNCTION__, __LINE__);
    }
  }

  /* connect to remote address */
  if ((rc = ::connect(sd, (const struct sockaddr *)raddr.ca_saddr,
                      (socklen_t)raddr.salen)) < 0) {
    /* connect did not succeed, handle error */
    switch (errno) {
    case EINPROGRESS: {
      VLOG(6) << __FUNCTION__ << " TCP: EINPROGRESS sd=" << sd;
      /* register socket descriptor for write operations */
      cthread::thread(rx_thread_num).add_write_fd(this, sd);
    } break;
    case ECONNREFUSED: {
      VLOG(6) << __FUNCTION__ << " TCP: ECONNREFUSED sd=" << sd;
      close();

      crofsock_env::call_env(env).handle_tcp_connect_refused(*this);

      if (flag_test(FLAG_RECONNECT_ON_FAILURE)) {
        backoff_reconnect(false);
      }
    } break;
    default: {
      VLOG(6) << __FUNCTION__ << " TCP: connect error: " << errno << ": "
              << strerror(errno) << " sd=" << sd;
      close();

      crofsock_env::call_env(env).handle_tcp_connect_failed(*this);

      if (flag_test(FLAG_RECONNECT_ON_FAILURE)) {
        backoff_reconnect(false);
      }
    };
    }
  } else {
    /* connect succeeded */

    if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
      throw eSysCall("eSysCall", "getsockname", __FILE__, __FUNCTION__,
                     __LINE__);
    }

    if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
      throw eSysCall("eSysCall", "getpeername", __FILE__, __FUNCTION__,
                     __LINE__);
    }

    state = STATE_TCP_ESTABLISHED;

    VLOG(6) << __FUNCTION__ << " STATE_TCP_ESTABLISHED sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    /* register socket descriptor for read operations */
    cthread::thread(rx_thread_num).add_read_fd(this, sd, true, true);
    cthread::thread(rx_thread_num).wakeup(this);

    if (flag_test(FLAG_TLS_IN_USE)) {
      crofsock::tls_connect(flag_test(FLAG_RECONNECT_ON_FAILURE));
    } else {
      crofsock_env::call_env(env).handle_tcp_connected(*this);
    }
  }
}

void crofsock::tls_init_context() {

  if (ctx) {
    tls_term_context();
  }

  AcquireReadWriteLock lock(sslock);

#if (OPENSSL_VERSION_NUMBER >= 0x1010000fL)
  // openssl 1.1.0
  ctx = SSL_CTX_new(TLS_method());
#else
  ctx = SSL_CTX_new(TLSv1_2_method());
#endif

  // certificate
  if (!SSL_CTX_use_certificate_file(ctx, certfile.c_str(), SSL_FILETYPE_PEM)) {
    throw eLibCall("eLibCall", "SSL_CTX_use_certificate_file", __FILE__,
                   __FUNCTION__, __LINE__)
        .set_key("certfile", certfile);
  }

  // private key
  SSL_CTX_set_default_passwd_cb(ctx, &crofsock::tls_pswd_cb);
  SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)this);

  if (!SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM)) {
    throw eLibCall("eLibCall", "SSL_CTX_use_PrivateKey_file", __FILE__,
                   __FUNCTION__, __LINE__)
        .set_key("keyfile", keyfile);
  }
  // ciphers
  if ((not ciphers.empty()) &&
      (0 == SSL_CTX_set_cipher_list(ctx, ciphers.c_str()))) {
    throw eLibCall("eLibCall", "SSL_CTX_set_cipher_list", __FILE__,
                   __FUNCTION__, __LINE__)
        .set_key("ciphers", ciphers);
  }

  // capath/cafile
  if (!SSL_CTX_load_verify_locations(ctx,
                                     cafile.empty() ? NULL : cafile.c_str(),
                                     capath.empty() ? NULL : capath.c_str())) {
    throw eLibCall("eLibCall", "SSL_CTX_load_verify_locations", __FILE__,
                   __FUNCTION__, __LINE__)
        .set_key("cafile", cafile)
        .set_key("capath", capath);
  }

  int mode = SSL_VERIFY_NONE;
  if (verify_mode == "NONE") {
    mode = SSL_VERIFY_NONE;
  } else if (verify_mode == "PEER") {
    mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
  }

  SSL_CTX_set_verify(ctx, mode, NULL);

  int depth;
  std::istringstream(verify_depth) >> depth;

  SSL_CTX_set_verify_depth(ctx, depth);
}

void crofsock::tls_term_context() {
  AcquireReadWriteLock lock(sslock);
  if (ssl) {
    SSL_free(ssl);
    ssl = NULL;
    bio = NULL;
  }
  if (ctx) {
    SSL_CTX_free(ctx);
    ctx = NULL;
  }
}

int crofsock::tls_pswd_cb(char *buf, int size, int rwflag, void *userdata) {
  if (userdata == NULL)
    return 0;

  crofsock &socket = *(static_cast<crofsock *>(userdata));

  if (socket.get_tls_pswd().empty()) {
    return 0;
  }

  strncpy(buf, socket.get_tls_pswd().c_str(), size);

  return strnlen(buf, size);
}

void crofsock::tls_accept(int sockfd) {
  switch (state.load()) {
  case STATE_IDLE:
  case STATE_TCP_ACCEPTING: {

    flag_set(FLAG_TLS_IN_USE, true);

    VLOG(6) << __FUNCTION__
            << " TLS: establish passive TCP connection sd=" << sd;

    crofsock::tcp_accept(sockfd);

  } break;
  case STATE_TCP_ESTABLISHED: {

    tls_init_context();

    {
      AcquireReadWriteLock lock(sslock);

      if ((ssl = SSL_new(ctx)) == NULL) {
        throw eLibCall("eLibCall", "SSL_new", __FILE__, __FUNCTION__, __LINE__);
      }

      SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE |
                            SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

      if ((bio = BIO_new(BIO_s_socket())) == NULL) {
        throw eLibCall("eLibCall", "BIO_new", __FILE__, __FUNCTION__, __LINE__);
      }

      BIO_set_fd(bio, sd, BIO_NOCLOSE);

      SSL_set_bio(ssl, /*rbio*/ bio, /*wbio*/ bio);

      SSL_set_accept_state(ssl);

      state = STATE_TLS_ACCEPTING;
    }

    VLOG(6) << __FUNCTION__ << " TLS: start passive connection sd=" << sd;
    crofsock::tls_accept(sd);
  } break;
  case STATE_TLS_ACCEPTING: {
    int rc = 0, err_code = 0;

    {
      AcquireReadWriteLock lock(sslock);

      VLOG(6) << __FUNCTION__
              << " TLS: run SSL_accept on passive connection sd=" << sd;

      if ((rc = SSL_accept(ssl)) < 0) {
        err_code = SSL_get_error(ssl, rc);
      }

      VLOG(6) << __FUNCTION__ << " TLS: SSL_accept on sd=" << sd
              << " rc: " << rc << " err_code: " << err_code;
    }

    if (rc <= 0) {
      switch (err_code) {
      case SSL_ERROR_WANT_READ: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_accept WANT READ on sd=" << sd;
      }
        return;
      case SSL_ERROR_WANT_WRITE: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_accept WANT WRITE on sd=" << sd;
      }
        return;
      case SSL_ERROR_WANT_ACCEPT: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_accept WANT ACCEPT on sd=" << sd;
      }
        return;
      case SSL_ERROR_WANT_CONNECT: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_accept WANT CONNECT on sd=" << sd;
      }
        return;

      case SSL_ERROR_NONE: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_accept succeeded ERROR NONE on sd=" << sd;
      }
        return;
      case SSL_ERROR_SSL: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_accept failed ERROR SSL on sd=" << sd;
      } break;
      case SSL_ERROR_SYSCALL: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_accept failed ERROR SYSCALL on sd=" << sd;
      } break;
      case SSL_ERROR_ZERO_RETURN: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_accept failed ERROR ZERO RETURN on sd=" << sd;
      } break;
      default: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_accept failed on sd=" << sd;
      };
      }

      tls_log_errors();

      tls_term_context();

      crofsock::close();

      crofsock_env::call_env(env).handle_tls_accept_failed(*this);

      return;

    } else if (rc == 1) {

      if (not tls_verify_ok()) {
        VLOG(6) << __FUNCTION__
                << " TLS: accept peer verification failed sd=" << sd;

        tls_log_errors();

        tls_term_context();

        crofsock::close();

        crofsock_env::call_env(env).handle_tls_accept_failed(*this);

        return;
      }

      VLOG(6) << __FUNCTION__ << " TLS: SSL_accept succeeded on sd=" << sd;

      state = STATE_TLS_ESTABLISHED;

      crofsock_env::call_env(env).handle_tls_accepted(*this);
    }

  } break;
  case STATE_TLS_ESTABLISHED: { /* do nothing */
    VLOG(6) << __FUNCTION__
            << " TLS: established on passive connection sd=" << sd;
  } break;
  default: {

    throw eRofSockError("crofsock::tls_accept() called in invalid state",
                        __FILE__, __FUNCTION__, __LINE__);
  };
  }
}

void crofsock::tls_connect(bool reconnect) {
  switch (state.load()) {
  case STATE_IDLE:
  case STATE_TCP_CONNECTING: {

    flag_set(FLAG_TLS_IN_USE, true);

    VLOG(6) << __FUNCTION__
            << " TLS: establish active TCP connection sd=" << sd;

    crofsock::tcp_connect(reconnect);

  } break;
  case STATE_TCP_ESTABLISHED: {

    tls_init_context();

    if ((ssl = SSL_new(ctx)) == NULL) {
      throw eLibCall("eLibCall", "SSL_new", __FILE__, __FUNCTION__, __LINE__);
    }

    SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE |
                          SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

    if ((bio = BIO_new(BIO_s_socket())) == NULL) {
      throw eLibCall("eLibCall", "BIO_new", __FILE__, __FUNCTION__, __LINE__);
    }

    BIO_set_fd(bio, sd, BIO_NOCLOSE);

    SSL_set_bio(ssl, /*rbio*/ bio, /*wbio*/ bio);

    SSL_set_connect_state(ssl);

    state = STATE_TLS_CONNECTING;

    VLOG(6) << __FUNCTION__ << " TLS: start active connection sd=" << sd;
    crofsock::tls_connect(reconnect);

  } break;
  case STATE_TLS_CONNECTING: {
    int rc = 0, err_code = 0;

    {
      AcquireReadWriteLock lock(sslock);

      VLOG(6) << __FUNCTION__
              << " TLS: run SSL_connect on active connection sd=" << sd;

      if ((rc = SSL_connect(ssl)) < 0) {
        err_code = SSL_get_error(ssl, rc);
      }

      VLOG(6) << __FUNCTION__ << " TLS: SSL_connect on sd=" << sd
              << " rc: " << rc << " err_code: " << err_code;
    }

    if (rc <= 0) {
      switch (err_code) {
      case SSL_ERROR_WANT_READ: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_connect WANT READ on sd=" << sd;
      }
        return;
      case SSL_ERROR_WANT_WRITE: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_connect WANT WRITE on sd=" << sd;
      }
        return;
      case SSL_ERROR_WANT_ACCEPT: {
        VLOG(6) << __FUNCTION__ << " TLS: SSL_connect WANT ACCEPT on sd=" << sd;
      }
        return;
      case SSL_ERROR_WANT_CONNECT: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_connect WANT CONNECT on sd=" << sd;
      }
        return;

      case SSL_ERROR_NONE: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_connect succeeded ERROR NONE on sd=" << sd;
      } break;
      case SSL_ERROR_SSL: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_connect failed ERROR SSL on sd=" << sd;
      } break;
      case SSL_ERROR_SYSCALL: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_connect failed ERROR SYSCALL on sd=" << sd;
      } break;
      case SSL_ERROR_ZERO_RETURN: {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_connect failed ERROR ZERO RETURN on sd=" << sd;
      } break;
      default: {
        VLOG(6) << __FUNCTION__ << " TLS: connect failed on sd=" << sd;
      };
      }

      tls_log_errors();

      tls_term_context();

      crofsock::close();

      crofsock_env::call_env(env).handle_tls_connect_failed(*this);

      return;

    } else if (rc == 1) {

      if (not tls_verify_ok()) {
        VLOG(6) << __FUNCTION__
                << " TLS: SSL_connect peer verification failed on sd=" << sd;

        tls_log_errors();

        tls_term_context();

        crofsock::close();

        crofsock_env::call_env(env).handle_tls_connect_failed(*this);

        return;
      }

      VLOG(6) << __FUNCTION__ << " TLS: SSL_connect succeeded on sd=" << sd;

      state = STATE_TLS_ESTABLISHED;

      crofsock_env::call_env(env).handle_tls_connected(*this);
    }

  } break;
  case STATE_TLS_ESTABLISHED: { /* do nothing */
    VLOG(6) << __FUNCTION__
            << " TLS: established on active connection sd=" << sd;
  } break;
  default: {

    throw eRofSockError("crofsock::tls_connect() called in invalid state",
                        __FILE__, __FUNCTION__, __LINE__);
  };
  }
}

bool crofsock::tls_verify_ok() {
  /* strategy:
   * - always check peer certificate in client mode
   * - check peer certificate in server mode when explicitly enabled (mode ==
   * SSL_VERIFY_PEER)
   */
  if ((verify_mode == "PEER") || (STATE_TLS_CONNECTING == state)) {

    /*
     * there must be a certificate presented by the peer in mode SSL_VERIFY_PEER
     */
    X509 *cert = (X509 *)NULL;
    if ((cert = SSL_get_peer_certificate(ssl)) == NULL) {

      VLOG(6) << __FUNCTION__
              << " TLS: no certificate presented by peer sd=" << sd;
      tls_log_errors();

      tls_term_context();

      crofsock::close();

      crofsock_env::call_env(env).handle_tcp_accept_refused(*this);

      return false;
    }
    /*
     * check verification result
     */
    long result = 0;
    if ((result = SSL_get_verify_result(ssl)) != X509_V_OK) {

      switch (result) {
      case X509_V_OK: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: ok sd=" << sd;
      } break;
      case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unable to get issuer "
                   "certificate sd="
                << sd;
      } break;
      case X509_V_ERR_UNABLE_TO_GET_CRL: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unable to get "
                   "certificate CRL sd="
                << sd;
      } break;
      case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unable to decrypt "
                   "certificate's signature sd="
                << sd;
      } break;
      case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unable to decrypt "
                   "CRL's signature sd="
                << sd;
      } break;
      case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unable to decode "
                   "issuer public key sd="
                << sd;
      } break;
      case X509_V_ERR_CERT_SIGNATURE_FAILURE: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: certificate signature "
                   "failure sd="
                << sd;
      } break;
      case X509_V_ERR_CRL_SIGNATURE_FAILURE: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: CRL signature failure sd="
            << sd;
      } break;
      case X509_V_ERR_CERT_NOT_YET_VALID: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: certificate is not yet "
               "valid sd="
            << sd;
      } break;
      case X509_V_ERR_CERT_HAS_EXPIRED: {
        VLOG(6)
            << "TLS: peer certificate verification: certificate has expired sd="
            << sd;
      } break;
      case X509_V_ERR_CRL_NOT_YET_VALID: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: CRL is not yet valid sd="
            << sd;
      } break;
      case X509_V_ERR_CRL_HAS_EXPIRED: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: CRL has expired sd="
                << sd;
      } break;
      case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: format error in "
                   "certificate's notBefore field sd="
                << sd;
      } break;
      case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: format error in "
                   "certificate's notAfter field sd="
                << sd;
      } break;
      case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: format error in CRL's "
                   "lastUpdate field sd="
                << sd;
      } break;
      case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: format error in CRL's "
                   "nextUpdate field sd="
                << sd;
      } break;
      case X509_V_ERR_OUT_OF_MEM: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: out of memory sd="
                << sd;
      } break;
      case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT: {
        VLOG(6)
            << "TLS: peer certificate verification: self signed certificate sd="
            << sd;
      } break;
      case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: self signed "
                   "certificate in certificate chain sd="
                << sd;
      } break;
      case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unable to get local "
                   "issuer certificate sd="
                << sd;
      } break;
      case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unable to verify the "
                   "first certificate sd="
                << sd;
      } break;
      case X509_V_ERR_CERT_CHAIN_TOO_LONG: {
        VLOG(6) << "TLS: peer certificate verification: certificate chain too "
                   "long sd="
                << sd;
      } break;
      case X509_V_ERR_CERT_REVOKED: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: certificate revoked sd="
            << sd;
      } break;
      case X509_V_ERR_INVALID_CA: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: invalid CA certificate sd="
            << sd;
      } break;
      case X509_V_ERR_PATH_LENGTH_EXCEEDED: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: path length exceeded sd="
            << sd;
      } break;
      case X509_V_ERR_INVALID_PURPOSE: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: invalid purpose sd="
                << sd;
      } break;
      case X509_V_ERR_CERT_UNTRUSTED: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: certificate untrusted sd="
            << sd;
      } break;
      case X509_V_ERR_CERT_REJECTED: {
        VLOG(6)
            << __FUNCTION__
            << " TLS: peer certificate verification: certificate rejected sd="
            << sd;
      } break;
      case X509_V_ERR_SUBJECT_ISSUER_MISMATCH: {
        VLOG(6)
            << "TLS: peer certificate verification: subject issuer mismatch sd="
            << sd;
      } break;
      case X509_V_ERR_AKID_SKID_MISMATCH: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: authority key id and "
                   "subject key id mismatch sd="
                << sd;
      } break;
      case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: authority and issuer "
                   "serial number mismatch sd="
                << sd;
      } break;
      case X509_V_ERR_KEYUSAGE_NO_CERTSIGN: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: key usage does not "
                   "include certificate signing sd="
                << sd;
      } break;
      case X509_V_ERR_APPLICATION_VERIFICATION: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: application "
                   "verification failure sd="
                << sd;
      } break;
      default: {
        VLOG(6) << __FUNCTION__
                << " TLS: peer certificate verification: unknown error sd="
                << sd;
      };
      }

      return false;
    }
  }

  return true;
}

void crofsock::tls_log_errors() {
  BIO *ebio = BIO_new(BIO_s_mem());

  ERR_print_errors(ebio);

  while (!BIO_eof(ebio)) {
    rofl::cmemory mem(1024);
    BIO_read(ebio, mem.somem(), mem.length() - 1);
    VLOG(1) << __FUNCTION__ << " TLS: error history "
            << (const char *)mem.somem();
  }

  BIO_free(ebio);
}

void crofsock::backoff_reconnect(bool reset_timeout) {
  if (cthread::thread(rx_thread_num).has_timer(this, TIMER_ID_RECONNECT)) {
    return;
  }

  if (reset_timeout) {

    reconnect_backoff_current = reconnect_backoff_start;
    reconnect_counter = 0;

  } else {

    if (reconnect_backoff_current >= reconnect_backoff_max) {
      reconnect_backoff_current = reconnect_backoff_max;
    } else {
      reconnect_backoff_current *= 2;
    }
  }

  VLOG(6) << __FUNCTION__
          << " scheduled reconnect in: " << reconnect_backoff_current << " secs"
          << " laddr" << laddr.str() << " raddr=" << raddr.str();

  cthread::thread(rx_thread_num)
      .add_timer(this, TIMER_ID_RECONNECT,
                 ctimespec().expire_in(reconnect_backoff_current, 0));

  ++reconnect_counter;
}

bool crofsock::is_established() const {
  return (STATE_TCP_ESTABLISHED <= state);
}

bool crofsock::is_tls_encrypted() const { return flag_test(FLAG_TLS_IN_USE); }

bool crofsock::is_passive() const { return (MODE_SERVER == mode); }

bool crofsock::is_congested() const { return flag_test(FLAG_CONGESTED); }

bool crofsock::is_rx_disabled() const { return rx_disabled; }

void crofsock::rx_disable() {
  rx_disabled = true;
  switch (state.load()) {
  case STATE_TCP_ESTABLISHED:
  case STATE_TLS_ESTABLISHED: {
    cthread::thread(rx_thread_num).drop_read_fd(sd, false);
    VLOG(6) << __FUNCTION__ << " disable reception"
            << " sd=" << sd << " laddr=" << laddr.str()
            << " raddr=" << raddr.str();
  } break;
  default: {
  };
  }
}

void crofsock::rx_enable() {
  rx_disabled = false;
  switch (state.load()) {
  case STATE_TCP_ESTABLISHED:
  case STATE_TLS_ESTABLISHED: {
    cthread::thread(rx_thread_num).add_read_fd(this, sd, false);
    VLOG(6) << __FUNCTION__ << " enable reception"
            << " sd=" << sd << " laddr=" << laddr.str()
            << " raddr=" << raddr.str();
    cthread::thread(rx_thread_num).wakeup(this);
  } break;
  default: {
  };
  }
}

void crofsock::tx_disable() {
  tx_disabled = true;
  VLOG(6) << __FUNCTION__ << " disable transmission sd=" << sd
          << " laddr=" << laddr.str() << " raddr=" << raddr.str();
}

void crofsock::tx_enable() {
  tx_disabled = false;
  VLOG(6) << __FUNCTION__ << " enable transmission sd=" << sd
          << " laddr=" << laddr.str() << " raddr=" << raddr.str();
  cthread::thread(tx_thread_num).wakeup(this);
}

void crofsock::handle_timeout(cthread &thread, uint32_t timer_id) {
  if (delete_in_progress()) {
    return;
  }
  switch (timer_id) {
  case TIMER_ID_RECONNECT: {
    VLOG(6) << __FUNCTION__ << " TCP: reconnecting sd=" << sd
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();
    if (flag_test(FLAG_TLS_IN_USE)) {
      tls_connect(true);
    } else {
      tcp_connect(true);
    }
  } break;
  default: /* do nothing */
    break;
  }
}

crofsock::msg_result_t crofsock::send_message(rofl::openflow::cofmsg *msg,
                                              bool enforce_queueing) {

  VLOG(6) << __FUNCTION__ << " sd=" << sd << " msg=" << msg
          << " txqueue_pending_pkts=" << txqueue_pending_pkts
          << " tx_disabled=" << tx_disabled
          << " tx_is_running=" << tx_is_running;

  if (tx_disabled) {
    delete msg;
    /* connection shutdown in progress, a very specific form of congestion ...,
     * message is deleted */
    return MSG_QUEUEING_FAILED_SHUTDOWN_IN_PROGRESS;
  }

  if ((state != STATE_TCP_ESTABLISHED) && (state != STATE_TLS_ESTABLISHED)) {

    delete msg;

    /* connection is not established: message is deleted */
    return MSG_QUEUEING_FAILED_NOT_ESTABLISHED;
  }

  if ((msg->get_type() == rofl::openflow::OFPT_ECHO_REQUEST) ||
      (msg->get_type() == rofl::openflow::OFPT_ECHO_REPLY)) {
    enforce_queueing = true;
  }

  try {

    /* enqueue the message in rofl's internal queue, as long
     * as these are not exhausted */
    switch (msg->get_version()) {
    case rofl::openflow10::OFP_VERSION: {
      switch (msg->get_type()) {
      case rofl::openflow10::OFPT_PACKET_IN:
      case rofl::openflow10::OFPT_PACKET_OUT: {
        txqueues[QUEUE_PKT].store(msg, enforce_queueing);
      } break;
      case rofl::openflow10::OFPT_FLOW_MOD:
      case rofl::openflow10::OFPT_FLOW_REMOVED:
      case rofl::openflow10::OFPT_BARRIER_REPLY:
      case rofl::openflow10::OFPT_BARRIER_REQUEST: {
        txqueues[QUEUE_FLOW].store(msg, enforce_queueing);
      } break;
      case rofl::openflow10::OFPT_ECHO_REQUEST:
      case rofl::openflow10::OFPT_ECHO_REPLY: {
        txqueues[QUEUE_OAM].store(msg, enforce_queueing);
      } break;
      default: {
        txqueues[QUEUE_MGMT].store(msg, enforce_queueing);
      };
      }
    } break;
    case rofl::openflow12::OFP_VERSION: {
      switch (msg->get_type()) {
      case rofl::openflow12::OFPT_PACKET_IN:
      case rofl::openflow12::OFPT_PACKET_OUT: {
        txqueues[QUEUE_PKT].store(msg, enforce_queueing);
      } break;
      case rofl::openflow12::OFPT_FLOW_MOD:
      case rofl::openflow12::OFPT_FLOW_REMOVED:
      case rofl::openflow12::OFPT_GROUP_MOD:
      case rofl::openflow12::OFPT_PORT_MOD:
      case rofl::openflow12::OFPT_TABLE_MOD:
      case rofl::openflow12::OFPT_BARRIER_REPLY:
      case rofl::openflow12::OFPT_BARRIER_REQUEST: {
        txqueues[QUEUE_FLOW].store(msg, enforce_queueing);
      } break;
      case rofl::openflow12::OFPT_ECHO_REQUEST:
      case rofl::openflow12::OFPT_ECHO_REPLY: {
        txqueues[QUEUE_OAM].store(msg, enforce_queueing);
      } break;
      default: {
        txqueues[QUEUE_MGMT].store(msg, enforce_queueing);
      };
      }
    } break;
    case rofl::openflow13::OFP_VERSION:
    default: {
      switch (msg->get_type()) {
      case rofl::openflow13::OFPT_PACKET_IN:
      case rofl::openflow13::OFPT_PACKET_OUT: {
        txqueues[QUEUE_PKT].store(msg, enforce_queueing);
      } break;
      case rofl::openflow13::OFPT_FLOW_MOD:
      case rofl::openflow13::OFPT_FLOW_REMOVED:
      case rofl::openflow13::OFPT_GROUP_MOD:
      case rofl::openflow13::OFPT_PORT_MOD:
      case rofl::openflow13::OFPT_TABLE_MOD:
      case rofl::openflow13::OFPT_BARRIER_REPLY:
      case rofl::openflow13::OFPT_BARRIER_REQUEST: {
        txqueues[QUEUE_FLOW].store(msg, enforce_queueing);
      } break;
      case rofl::openflow13::OFPT_ECHO_REQUEST:
      case rofl::openflow13::OFPT_ECHO_REPLY: {
        txqueues[QUEUE_OAM].store(msg, enforce_queueing);
      } break;
      default: {
        txqueues[QUEUE_MGMT].store(msg, enforce_queueing);
      };
      }
    };
    }

    txqueue_pending_pkts++;

    if (not tx_is_running) {
      cthread::thread(tx_thread_num).wakeup(this);
    }

    if (flag_test(FLAG_TX_BLOCK_QUEUEING)) {
      /* message was queued, but congestion prevents us from sending it */
      return MSG_QUEUED_CONGESTION;
    }

    /* message was queued, waiting for transmission */
    return MSG_QUEUED;

  } catch (eRofQueueFull &e) {
    VLOG(6) << __FUNCTION__ << " txqueue exhausted, "
            << " msg=" << msg
            << " txqueue_pending_pkts=" << txqueue_pending_pkts
            << " tx_disabled=" << tx_disabled
            << " tx_is_running=" << tx_is_running;
    delete msg;
    /* message was not stored in txqueue and deleted here */
    return MSG_QUEUEING_FAILED_QUEUE_FULL;
  }
}

void crofsock::handle_wakeup(cthread &thread) {
  if (flag_test(FLAG_CLOSING) || delete_in_progress()) {
    return;
  }

  if (&thread == &cthread::thread(rx_thread_num)) {
    // recv_message();
    handle_read_event_rxthread(thread, sd);
  } else if (&thread == &cthread::thread(tx_thread_num)) {
    send_from_queue();
  }
}

void crofsock::handle_write_event(cthread &thread, int fd) {
  if (flag_test(FLAG_CLOSING) || delete_in_progress()) {
    return;
  }

  if (&thread == &cthread::thread(tx_thread_num)) {
    assert(fd == sd);
    flag_set(FLAG_CONGESTED, false);
    cthread::thread(tx_thread_num).drop_write_fd(sd);
    send_from_queue();
  } else if (&thread == &cthread::thread(rx_thread_num)) {
    assert(fd == sd);
    handle_read_event_rxthread(thread, fd);
  }
}

void crofsock::send_from_queue() {
  if ((get_state() <= STATE_IDLE) || delete_in_progress()) {
    VLOG(6) << __FUNCTION__ << " sd=" << sd
            << " dropping message, no connection "
               "established"
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();
    return;
  }

  tx_is_running = true;

  bool reschedule;
  do {
    reschedule = false;
    for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

      for (unsigned int num = 0; num < txweights[queue_id]; ++num) {

        if ((tx_disabled) || (get_state() < STATE_TCP_ESTABLISHED)) {
          tx_is_running = false;
          return;
        }

        /* no pending fragment */
        if (txbuffer.empty()) {
          rofl::openflow::cofmsg *msg = nullptr;

          /* fetch a new message for transmission from tx queue */
          if ((msg = txqueues[queue_id].retrieve()) == NULL)
            break;
          // txqueues[queue_id].pop();

          /* pack message into txbuffer */
          msg->pack(txbuffer.sowmem(), txbuffer.wmemlen());
          txbuffer.wseek(msg->length());

          VLOG(6) << __FUNCTION__ << " sd=" << sd
                  << " message sent: " << msg->str().c_str()
                  << " laddr=" << laddr.str() << " raddr=" << raddr.str();

          /* remove C++ message object from heap */
          delete msg;
        }

        switch (state.load()) {
        case STATE_TCP_ESTABLISHED: {

          /* send memory block via socket in non-blocking mode */
          int nbytes = ::send(sd, txbuffer.sormem(), txbuffer.rmemlen(),
                              MSG_DONTWAIT | MSG_NOSIGNAL);

          /* error occurred */
          if (nbytes < 0) {
            switch (errno) {
            case EAGAIN: /* socket would block */ {
              tx_is_running = false;
              flag_set(FLAG_CONGESTED, true);
              cthread::thread(tx_thread_num).add_write_fd(this, sd);

              if (not flag_test(FLAG_TX_BLOCK_QUEUEING)) {
                /* block transmission of further packets */
                flag_set(FLAG_TX_BLOCK_QUEUEING, true);
                /* remember queue size, when congestion occurred */
                txqueue_size_congestion_occurred = txqueue_pending_pkts;
                /* threshold for re-enabling acceptance of packets */
                txqueue_size_tx_threshold = txqueue_pending_pkts / 2;

                VLOG(6) << __FUNCTION__ << " sd=" << sd
                        << " congestion occurred"
                        << " txqueue_pending_pkts: " << txqueue_pending_pkts
                        << " txqueue_size_congestion_occurred: "
                        << txqueue_size_congestion_occurred
                        << " txqueue_size_tx_threshold: "
                        << txqueue_size_tx_threshold << " laddr=" << laddr.str()
                        << " raddr=" << raddr.str();

                crofsock_env::call_env(env).congestion_occurred_indication(
                    *this);
              }
            }
              return;
            case SIGPIPE:
            default: {
              VLOG(1) << __FUNCTION__ << " sd=" << sd
                      << " ::send() syscall failed, error: " << errno << ": "
                      << strerror(errno);
              tx_is_running = false;
            }
              return;
            }

            /* at least some bytes were sent successfully */
          } else {
            txbuffer.rseek(nbytes);
            flag_set(FLAG_CONGESTED, false);

            if (txbuffer.empty()) {
              txqueue_pending_pkts--;
            }

            VLOG(6) << __FUNCTION__ << " sd=" << sd << " sent " << nbytes
                    << " bytes, remaining_bytes_to_sent=" << txbuffer.rmemlen()
                    << " tx_fragment_pending="
                    << (txbuffer.empty() ? "no" : "yes")
                    << " txqueue_pending_pkts=" << txqueue_pending_pkts;
          }

        } break;
        case STATE_TLS_ESTABLISHED: {
          int err_code = 0, nbytes = 0;

          {
            AcquireReadWriteLock lock(sslock);

            /* send memory block via OpenSSL structure in non-blocking mode */
            if ((nbytes = SSL_write(ssl, txbuffer.sormem(),
                                    txbuffer.rmemlen())) <= 0) {
              err_code = SSL_get_error(ssl, nbytes);
            }

            VLOG(6) << __FUNCTION__ << " TLS: SSL_write on sd=" << sd
                    << " nbytes: " << nbytes << " errno: " << errno << " ("
                    << strerror(errno) << ")";
          }

          if (nbytes < 0) {
            switch (err_code) {
            case SSL_ERROR_WANT_READ: { /* waiting for more data, stop
                                           processing for now */
              VLOG(6) << __FUNCTION__ << " TLS: SSL_write WANT READ sd=" << sd;
              // add_read_fd(sd) is always active, so we can skip it here
            }
              return;
            case SSL_ERROR_WANT_WRITE: { /* assumption: underlying socket is
                                            blocking */
              VLOG(6) << __FUNCTION__ << " TLS: SSL_write WANT WRITE sd=" << sd;
              tx_is_running = false;
              flag_set(FLAG_CONGESTED, true);
              cthread::thread(tx_thread_num).add_write_fd(this, sd);

              if (not flag_test(FLAG_TX_BLOCK_QUEUEING)) {
                /* block transmission of further packets */
                flag_set(FLAG_TX_BLOCK_QUEUEING, true);
                /* remember queue size, when congestion occurred */
                txqueue_size_congestion_occurred = txqueue_pending_pkts;
                /* threshold for re-enabling acceptance of packets */
                txqueue_size_tx_threshold = txqueue_pending_pkts / 2;

                VLOG(6) << __FUNCTION__ << " sd=" << sd
                        << " congestion occurred"
                        << " txqueue_pending_pkts: " << txqueue_pending_pkts
                        << " txqueue_size_congestion_occurred: "
                        << txqueue_size_congestion_occurred
                        << " txqueue_size_tx_threshold: "
                        << txqueue_size_tx_threshold << " laddr=" << laddr.str()
                        << " raddr=" << raddr.str();

                crofsock_env::call_env(env).congestion_occurred_indication(
                    *this);
              }
            }
              return;
            case SSL_ERROR_WANT_ACCEPT: { /* should never happen here, though */
              VLOG(6) << __FUNCTION__
                      << " TLS: SSL_write WANT ACCEPT on sd=" << sd;
              tx_is_running = false;
            }
              return;
            case SSL_ERROR_WANT_CONNECT: { /* should never happen here, though
                                            */
              VLOG(6) << __FUNCTION__
                      << " TLS: SSL_write WANT CONNECT on sd=" << sd;
              tx_is_running = false;
            }
              return;

            case SSL_ERROR_NONE: { /* no error occured, just continue */
              VLOG(6) << __FUNCTION__
                      << " TLS: SSL_write succeeded ERROR NONE on sd=" << sd;
            } break;
            case SSL_ERROR_SSL: { /* error on SSL layer */
              VLOG(6) << __FUNCTION__
                      << " TLS: SSL_write failed ERROR SSL on sd=" << sd;
              tx_is_running = false;
            }
              return;
            case SSL_ERROR_SYSCALL: { /* error on underlying system call */
              VLOG(6) << __FUNCTION__
                      << " TLS: SSL_write failed ERROR SYSCALL on sd=" << sd
                      << " nbytes: " << nbytes << " errno: " << errno << " ("
                      << strerror(errno) << ")";
              tx_is_running = false;
            }
              return;
            case SSL_ERROR_ZERO_RETURN: { /* peer initiated shutdown */
              VLOG(6) << __FUNCTION__
                      << " TLS: SSL_write failed ERROR ZERO RETURN on sd="
                      << sd;
              tx_is_running = false;
            }
              return;
            default: {
              VLOG(6) << __FUNCTION__ << " TLS: SSL_write failed on sd=" << sd;
              tx_is_running = false;
            }
              return;
            }

            /* at least some bytes were sent successfully */
          } else {
            txbuffer.rseek(nbytes);
            flag_set(FLAG_CONGESTED, false);

            if (txbuffer.empty()) {
              txqueue_pending_pkts--;
            }

            VLOG(6) << __FUNCTION__ << " sd=" << sd << " sent " << nbytes
                    << " bytes, remaining_bytes_to_sent=" << txbuffer.rmemlen()
                    << " tx_fragment_pending="
                    << (txbuffer.empty() ? "no" : "yes")
                    << " txqueue_pending_pkts=" << txqueue_pending_pkts;
          }

        } break;
        default: {

          VLOG(6) << __FUNCTION__ << " unable to send in state=" << str()
                  << " on sd=" << sd;
        };
        }
      }

      if (not txqueues[queue_id].empty()) {
        reschedule = true;
      }
    }

    if ((not flag_test(FLAG_CONGESTED)) && flag_test(FLAG_TX_BLOCK_QUEUEING)) {
      if (txqueue_pending_pkts <= txqueue_size_tx_threshold) {
        flag_set(FLAG_TX_BLOCK_QUEUEING, false);
        VLOG(6) << __FUNCTION__ << " sd=" << sd << " congestion solved"
                << " txqueue_pending_pkts" << txqueue_pending_pkts
                << " txqueue_size_congestion_occurred"
                << txqueue_size_congestion_occurred
                << " txqueue_size_tx_threshold" << txqueue_size_tx_threshold
                << " laddr=" << laddr.str() << " raddr=" << raddr.str();

        crofsock_env::call_env(env).congestion_solved_indication(*this);
      }
    }

  } while (reschedule);

  tx_is_running = false;

  if ((txqueue_pending_pkts > 0) && (not flag_test(FLAG_TX_BLOCK_QUEUEING))) {
    cthread::thread(tx_thread_num).wakeup(this);
  }
}

void crofsock::handle_read_event(cthread &thread, int fd) {
  if (flag_test(FLAG_CLOSING) || delete_in_progress()) {
    return;
  }
  if (&thread == &cthread::thread(rx_thread_num)) {
    handle_read_event_rxthread(thread, fd);
  }
}

void crofsock::handle_read_event_rxthread(cthread &thread, int fd) {
  VLOG(6) << __FUNCTION__ << " RX event on sd=" << sd
          << " laddr=" << laddr.str() << " raddr=" << raddr.str();

  try {
    switch (state.load()) {
    case STATE_LISTENING: {

      VLOG(6) << __FUNCTION__
              << " STATE_LISTENING, new incoming connection(s) on sd=" << sd
              << " laddr=" << laddr.str() << " raddr=" << raddr.str();

      crofsock_env::call_env(env).handle_listen(*this);

    } break;
    case STATE_TCP_CONNECTING: {

      int rc;
      int optval = 0;
      int optlen = sizeof(optval);
      if ((rc = getsockopt(sd, SOL_SOCKET, SO_ERROR, (void *)&optval,
                           (socklen_t *)&optlen)) < 0) {
        throw eSysCall("eSysCall", "getsockopt (SO_ERROR)", __FILE__,
                       __FUNCTION__, __LINE__);
      }

      switch (optval) {
      case 0:
      case EISCONN: {
        cthread::thread(rx_thread_num).drop_write_fd(sd);

        if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
          throw eSysCall("eSysCall", "getsockname", __FILE__, __FUNCTION__,
                         __LINE__);
        }

        if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
          throw eSysCall("eSysCall", "getpeername", __FILE__, __FUNCTION__,
                         __LINE__);
        }

        state = STATE_TCP_ESTABLISHED;

        VLOG(6) << __FUNCTION__ << " STATE_TCP_ESTABLISHED sd=" << sd
                << " laddr=" << laddr.str() << " raddr=" << raddr.str();

        /* register socket descriptor for read operations */
        cthread::thread(rx_thread_num).add_read_fd(this, sd, true, true);

        if (flag_test(FLAG_TLS_IN_USE)) {
          crofsock::tls_connect(flag_test(FLAG_RECONNECT_ON_FAILURE));
        } else {
          crofsock_env::call_env(env).handle_tcp_connected(*this);
        }

        cthread::thread(rx_thread_num).wakeup(this);

      } break;
      case EINPROGRESS: {
        /* connect still pending, just wait */
        VLOG(6) << __FUNCTION__ << " TCP: EINPROGRESS sd=" << sd
                << " laddr=" << laddr.str() << " raddr=" << raddr.str();
      } break;
      case ECONNREFUSED: {
        VLOG(6) << __FUNCTION__ << " TCP: ECONNREFUSED sd=" << sd
                << " laddr=" << laddr.str() << " raddr=" << raddr.str();
        close();

        crofsock_env::call_env(env).handle_tcp_connect_refused(*this);

        if (flag_test(FLAG_RECONNECT_ON_FAILURE)) {
          backoff_reconnect(false);
        }
      } break;
      default: {
        VLOG(6) << __FUNCTION__ << " TCP: connect error: " << errno << ": "
                << strerror(errno) << " sd=" << sd << " laddr=" << laddr.str()
                << " raddr=" << raddr.str();
        close();

        crofsock_env::call_env(env).handle_tcp_connect_failed(*this);

        if (flag_test(FLAG_RECONNECT_ON_FAILURE)) {
          backoff_reconnect(false);
        }
      };
      }

    } break;
    case STATE_TCP_ACCEPTING: {

    } break;
    case STATE_TLS_CONNECTING: {

      tls_connect(flag_test(FLAG_RECONNECT_ON_FAILURE));
      if (STATE_TLS_ESTABLISHED == state) {
        recv_message();
      }

    } break;
    case STATE_TLS_ACCEPTING: {

      tls_accept(fd);
      if (STATE_TLS_ESTABLISHED == state) {
        recv_message();
      }

    } break;
    case STATE_TCP_ESTABLISHED: {

      if (flag_test(FLAG_TLS_IN_USE))
        return;
      recv_message();

    } break;
    case STATE_TLS_ESTABLISHED: {

      if (not flag_test(FLAG_TLS_IN_USE))
        return;
      recv_message();

    } break;
    default: /* do nothing */
      break;
    }

  } catch (std::runtime_error &e) {
    VLOG(1) << __FUNCTION__ << "() runtime exception caught: " << e.what();
  } catch (std::exception &e) {
    VLOG(1) << __FUNCTION__ << "() exception caught: " << e.what();
  } catch (...) {
    VLOG(1) << __FUNCTION__ << "() unknown exception caught";
  }
}

void crofsock::recv_message() {
  int nbytes = 0;
  while (not rx_disabled) {

    if ((get_state() <= STATE_IDLE) || delete_in_progress()) {
      VLOG(6) << __FUNCTION__ << "() ignoring message sd=" << sd
              << " laddr=" << laddr.str() << " raddr=" << raddr.str()
              << " state=" << state;
      return;
    }

    uint16_t msg_len = 0;

    /* how many bytes do we have to read? */
    if (rxbuffer.rmemlen() < sizeof(struct openflow::ofp_header)) {
      msg_len = sizeof(struct openflow::ofp_header);
    } else {
      struct openflow::ofp_header *header =
          (struct openflow::ofp_header *)(rxbuffer.somem());
      msg_len = be16toh(header->length);
    }

    /* sanity check: 8 <= msg_len <= 2^16 */
    if (msg_len < sizeof(struct openflow::ofp_header)) {
      /* out-of-sync => enforce reconnect in client mode */
      VLOG(6) << __FUNCTION__ << " TCP: openflow out-of-sync sd=" << sd
              << " laddr=" << laddr.str() << " raddr=" << raddr.str();
      goto on_error;
    }

    switch (state.load()) {
    case STATE_TCP_ESTABLISHED: {
      /* read from socket more bytes, at most "msg_len - msg_bytes_read" */
      nbytes = ::recv(sd, (void *)(rxbuffer.sowmem()),
                      msg_len - rxbuffer.rmemlen(), MSG_DONTWAIT);

      if (nbytes < 0) {
        switch (errno) {
        case EAGAIN: {
          /* do not continue and let kernel inform us, once more data is
           * available
           */
          VLOG(6) << __FUNCTION__ << " EAGAIN on sd=" << sd;
          return;
        } break;
        default: {
          VLOG(1) << __FUNCTION__
                  << " ::recv() syscall failed, error: " << errno << ": "
                  << strerror(errno) << " laddr=" << laddr.str()
                  << " raddr=" << raddr.str();
          goto on_error;
        };
        }
      } else if (nbytes == 0) {
        /* shutdown from peer */
        VLOG(6) << __FUNCTION__ << " TCP: peer shutdown sd=" << sd
                << " laddr=" << laddr.str() << " raddr=" << raddr.str();
        goto on_error;
      }
    } break;
    case STATE_TLS_ESTABLISHED: {
      int err_code = 0;

      {
        AcquireReadWriteLock lock(sslock);

        /* read from socket more bytes, at most "msg_len - msg_bytes_read" */
        if ((nbytes = SSL_read(ssl, (void *)(rxbuffer.sowmem()),
                               (msg_len - rxbuffer.rmemlen()))) <= 0) {
          err_code = SSL_get_error(ssl, nbytes);
        }

        VLOG(6) << __FUNCTION__ << " TLS: SSL_read on sd=" << sd
                << " nbytes: " << nbytes << " errno: " << errno << " ("
                << strerror(errno) << ")";
      }

      if (nbytes <= 0) {

        switch (err_code) {
        case SSL_ERROR_WANT_READ: {
          /* do not continue and let kernel inform us, once more data is
           * available
           */
          VLOG(6) << __FUNCTION__ << " TLS: SSL_read WANT READ on sd=" << sd
                  << " error: " << ERR_reason_error_string(err_code);
          // add_read_fd(sd) is always active, so we can skip it here
        }
          return;
        case SSL_ERROR_WANT_WRITE: {
          VLOG(6) << __FUNCTION__ << " TLS: SSL_read WANT WRITE on sd=" << sd;
          /* we call SSL_write in the TX thread. Is this solving a
           * SSL_ERROR_WANT_WRITE on the receiving side? */
          cthread::thread(tx_thread_num).add_write_fd(this, sd);
        }
          return;
        case SSL_ERROR_WANT_ACCEPT: { /* should never happen here, though */
          VLOG(6) << __FUNCTION__ << " TLS: SSL_read WANT ACCEPT on sd=" << sd;
        }
          return;
        case SSL_ERROR_WANT_CONNECT: { /* should never happen here, though */
          VLOG(6) << __FUNCTION__ << " TLS: SSL_read WANT CONNECT on sd=" << sd;
        }
          return;

        case SSL_ERROR_NONE: { /* successful reception */
          VLOG(6) << __FUNCTION__
                  << " TLS: SSL_read succeeded ERROR NONE on sd=" << sd;
        } break;
        case SSL_ERROR_SSL: { /* error on SSL layer */
          VLOG(6) << __FUNCTION__
                  << " TLS: SSL_read failed ERROR SSL on sd=" << sd;
        }
          goto on_error;
        case SSL_ERROR_SYSCALL: { /* error on underlying system call */
          VLOG(6) << __FUNCTION__
                  << " TLS: SSL_read failed ERROR SYSCALL on sd=" << sd
                  << " error: " << ERR_reason_error_string(err_code)
                  << " nbytes: " << nbytes << " errno: " << errno << " ("
                  << strerror(errno) << ")";

          tls_log_errors();
        }
          goto on_error;
        case SSL_ERROR_ZERO_RETURN: { /* peer initiated shutdown */
          VLOG(6) << __FUNCTION__
                  << " TLS: SSL_read failed ERROR ZERO RETURN on sd=" << sd;

          VLOG(6) << __FUNCTION__ << " TLS: peer shutdown on sd=" << sd
                  << " laddr=" << laddr.str() << " raddr=" << raddr.str();
        }
          goto on_error;
        default: {
          VLOG(6) << __FUNCTION__ << " TLS: SSL_read failed sd=" << sd;
        }
          return;
        }
      }

    } break;
    default: {

      VLOG(6) << __FUNCTION__ << " unable to receive in state=" << str()
              << " on sd=" << sd;
    };
    }

    rxbuffer.wseek(nbytes);

    /* minimum message length received, check completeness of message */
    if (rxbuffer.rmemlen() >= sizeof(struct openflow::ofp_header)) {
      struct openflow::ofp_header *header =
          (struct openflow::ofp_header *)(rxbuffer.somem());
      uint16_t msg_len = be16toh(header->length);

      /* ok, message was received completely */
      if (msg_len == rxbuffer.rmemlen()) {
        parse_message();
      }
    }
  }

  return;

on_error:

  switch (state.load()) {
  case STATE_TCP_ESTABLISHED:
  case STATE_TLS_ESTABLISHED: {
    close();

    if (flag_test(FLAG_RECONNECT_ON_FAILURE)) {
      backoff_reconnect(true);
    }

    try {
      crofsock_env::call_env(env).handle_closed(*this);
    } catch (std::runtime_error &e) {
      VLOG(1) << __FUNCTION__ << " sd=" << sd
              << " caught runtime error, what: %s" << e.what()
              << " laddr=" << laddr.str() << " raddr=" << raddr.str();
    }
    // WARNING: handle_closed might delete this socket, don't call anything here
  } break;
  default: {
    VLOG(6) << __FUNCTION__ << " error in state=" << str();
  };
  }
}

void crofsock::parse_message() {
  struct rofl::openflow::ofp_header *hdr =
      (struct rofl::openflow::ofp_header *)rxbuffer.somem();

  rofl::openflow::cofmsg *msg = (rofl::openflow::cofmsg *)0;
  try {
    if (rxbuffer.length() < sizeof(struct rofl::openflow::ofp_header)) {
      throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                              __LINE__);
    }

    /* make sure to have a valid cofmsg* msg object after parsing */
    switch (hdr->version) {
    case rofl::openflow10::OFP_VERSION: {
      parse_of10_message(&msg);
    } break;
    case rofl::openflow12::OFP_VERSION: {
      parse_of12_message(&msg);
    } break;
    case rofl::openflow13::OFP_VERSION: {
      parse_of13_message(&msg);
    } break;
    default: {
      throw eBadRequestBadVersion("eBadRequestBadVersion", __FILE__,
                                  __FUNCTION__, __LINE__);
    };
    }

    if ((get_state() <= STATE_IDLE) || delete_in_progress()) {
      return;
    }

    VLOG(6) << __FUNCTION__ << " sd=" << sd
            << " message rcvd: " << msg->str().c_str()
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    crofsock_env::call_env(env).handle_recv(*this, msg);

  } catch (eBadRequestBadType &e) {

    VLOG(1) << __FUNCTION__ << " dropping message xid=" << be32toh(hdr->xid)
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    send_message(new rofl::openflow::cofmsg_error_bad_request_bad_type(
        hdr->version, be32toh(hdr->xid), rxbuffer.somem(),
        (rxbuffer.rmemlen() > 64) ? 64 : rxbuffer.rmemlen()));

  } catch (eBadRequestBadStat &e) {

    VLOG(1) << __FUNCTION__ << " dropping message xid=" << be32toh(hdr->xid)
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    send_message(new rofl::openflow::cofmsg_error_bad_request_bad_stat(
        hdr->version, be32toh(hdr->xid), rxbuffer.somem(),
        (rxbuffer.rmemlen() > 64) ? 64 : rxbuffer.rmemlen()));

  } catch (eBadRequestBadVersion &e) {

    VLOG(1) << __FUNCTION__ << " dropping message xid=" << be32toh(hdr->xid)
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    if (msg)
      delete msg;

    send_message(new rofl::openflow::cofmsg_error_bad_request_bad_version(
        hdr->version, be32toh(hdr->xid), rxbuffer.somem(),
        (rxbuffer.rmemlen() > 64) ? 64 : rxbuffer.rmemlen()));

  } catch (eBadRequestBadLen &e) {

    VLOG(1) << __FUNCTION__ << " sd=" << sd
            << " dropping message xid=" << be32toh(hdr->xid)
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    if (msg)
      delete msg;

    send_message(new rofl::openflow::cofmsg_error_bad_request_bad_len(
        hdr->version, be32toh(hdr->xid), rxbuffer.somem(),
        (rxbuffer.rmemlen() > 64) ? 64 : rxbuffer.rmemlen()));

  } catch (rofl::exception &e) {

    VLOG(1) << __FUNCTION__ << " sd=" << sd
            << " dropping message xid=" << be32toh(hdr->xid)
            << " laddr=" << laddr.str() << " raddr=" << raddr.str();

    // if (msg) delete msg;

  } catch (std::runtime_error &e) {
    VLOG(1) << __FUNCTION__ << " sd=" << sd << " std::runtime_error: %s"
            << e.what() << " laddr=" << laddr.str() << " raddr=" << raddr.str();
  }
}

void crofsock::parse_of10_message(rofl::openflow::cofmsg **pmsg) {
  struct openflow::ofp_header *header =
      (struct openflow::ofp_header *)rxbuffer.somem();

  switch (header->type) {
  case rofl::openflow10::OFPT_HELLO: {
    *pmsg = new rofl::openflow::cofmsg_hello();
  } break;
  case rofl::openflow10::OFPT_ERROR: {
    *pmsg = new rofl::openflow::cofmsg_error();
  } break;
  case rofl::openflow10::OFPT_ECHO_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_echo_request();
  } break;
  case rofl::openflow10::OFPT_ECHO_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_echo_reply();
  } break;
  case rofl::openflow10::OFPT_VENDOR: {
    *pmsg = new rofl::openflow::cofmsg_experimenter();
  } break;
  case rofl::openflow10::OFPT_FEATURES_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_features_request();
  } break;
  case rofl::openflow10::OFPT_FEATURES_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_features_reply();
  } break;
  case rofl::openflow10::OFPT_GET_CONFIG_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_get_config_request();
  } break;
  case rofl::openflow10::OFPT_GET_CONFIG_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_get_config_reply();
  } break;
  case rofl::openflow10::OFPT_SET_CONFIG: {
    *pmsg = new rofl::openflow::cofmsg_set_config();
  } break;
  case rofl::openflow10::OFPT_PACKET_OUT: {
    *pmsg = new rofl::openflow::cofmsg_packet_out();
  } break;
  case rofl::openflow10::OFPT_PACKET_IN: {
    *pmsg = new rofl::openflow::cofmsg_packet_in();
  } break;
  case rofl::openflow10::OFPT_FLOW_MOD: {
    *pmsg = new rofl::openflow::cofmsg_flow_mod();
  } break;
  case rofl::openflow10::OFPT_FLOW_REMOVED: {
    *pmsg = new rofl::openflow::cofmsg_flow_removed();
  } break;
  case rofl::openflow10::OFPT_PORT_MOD: {
    *pmsg = new rofl::openflow::cofmsg_port_mod();
  } break;
  case rofl::openflow10::OFPT_PORT_STATUS: {
    *pmsg = new rofl::openflow::cofmsg_port_status();
  } break;
  case rofl::openflow10::OFPT_STATS_REQUEST: {
    if (rxbuffer.length() <
        sizeof(struct rofl::openflow10::ofp_stats_request)) {
      throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                              __LINE__);
    }
    uint16_t stats_type = be16toh(
        ((struct rofl::openflow10::ofp_stats_request *)rxbuffer.somem())->type);
    switch (stats_type) {
    case rofl::openflow10::OFPST_DESC: {
      *pmsg = new rofl::openflow::cofmsg_desc_stats_request();
    } break;
    case rofl::openflow10::OFPST_FLOW: {
      *pmsg = new rofl::openflow::cofmsg_flow_stats_request();
    } break;
    case rofl::openflow10::OFPST_AGGREGATE: {
      *pmsg = new rofl::openflow::cofmsg_aggr_stats_request();
    } break;
    case rofl::openflow10::OFPST_TABLE: {
      *pmsg = new rofl::openflow::cofmsg_table_stats_request();
    } break;
    case rofl::openflow10::OFPST_PORT: {
      *pmsg = new rofl::openflow::cofmsg_port_stats_request();
    } break;
    case rofl::openflow10::OFPST_QUEUE: {
      *pmsg = new rofl::openflow::cofmsg_queue_stats_request();
    } break;
    case rofl::openflow10::OFPST_VENDOR: {
      *pmsg = new rofl::openflow::cofmsg_experimenter_stats_request();
    } break;
    default: {
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
    };
    }
  } break;
  case rofl::openflow10::OFPT_STATS_REPLY: {
    if (rxbuffer.length() < sizeof(struct rofl::openflow10::ofp_stats_reply)) {
      throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                              __LINE__);
    }
    uint16_t stats_type = be16toh(
        ((struct rofl::openflow10::ofp_stats_reply *)rxbuffer.somem())->type);
    switch (stats_type) {
    case rofl::openflow10::OFPST_DESC: {
      *pmsg = new rofl::openflow::cofmsg_desc_stats_reply();
    } break;
    case rofl::openflow10::OFPST_FLOW: {
      *pmsg = new rofl::openflow::cofmsg_flow_stats_reply();
    } break;
    case rofl::openflow10::OFPST_AGGREGATE: {
      *pmsg = new rofl::openflow::cofmsg_aggr_stats_reply();
    } break;
    case rofl::openflow10::OFPST_TABLE: {
      *pmsg = new rofl::openflow::cofmsg_table_stats_reply();
    } break;
    case rofl::openflow10::OFPST_PORT: {
      *pmsg = new rofl::openflow::cofmsg_port_stats_reply();
    } break;
    case rofl::openflow10::OFPST_QUEUE: {
      *pmsg = new rofl::openflow::cofmsg_queue_stats_reply();
    } break;
    case rofl::openflow10::OFPST_VENDOR: {
      *pmsg = new rofl::openflow::cofmsg_experimenter_stats_reply();
    } break;
    default: {
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
    };
    }
  } break;
  case rofl::openflow10::OFPT_BARRIER_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_barrier_request();
  } break;
  case rofl::openflow10::OFPT_BARRIER_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_barrier_reply();
  } break;
  case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_queue_get_config_request();
  } break;
  case rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_queue_get_config_reply();
  } break;
  default: {
    throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                             __LINE__);
  };
  }

  (*(*pmsg)).unpack(rxbuffer.sormem(), rxbuffer.rmemlen());
  rxbuffer.reset();
}

void crofsock::parse_of12_message(rofl::openflow::cofmsg **pmsg) {
  struct openflow::ofp_header *header =
      (struct openflow::ofp_header *)rxbuffer.somem();

  switch (header->type) {
  case rofl::openflow12::OFPT_HELLO: {
    *pmsg = new rofl::openflow::cofmsg_hello();
  } break;
  case rofl::openflow12::OFPT_ERROR: {
    *pmsg = new rofl::openflow::cofmsg_error();
  } break;
  case rofl::openflow12::OFPT_ECHO_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_echo_request();
  } break;
  case rofl::openflow12::OFPT_ECHO_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_echo_reply();
  } break;
  case rofl::openflow12::OFPT_EXPERIMENTER: {
    *pmsg = new rofl::openflow::cofmsg_experimenter();
  } break;
  case rofl::openflow12::OFPT_FEATURES_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_features_request();
  } break;
  case rofl::openflow12::OFPT_FEATURES_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_features_reply();
  } break;
  case rofl::openflow12::OFPT_GET_CONFIG_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_get_config_request();
  } break;
  case rofl::openflow12::OFPT_GET_CONFIG_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_get_config_reply();
  } break;
  case rofl::openflow12::OFPT_SET_CONFIG: {
    *pmsg = new rofl::openflow::cofmsg_set_config();
  } break;
  case rofl::openflow12::OFPT_PACKET_OUT: {
    *pmsg = new rofl::openflow::cofmsg_packet_out();
  } break;
  case rofl::openflow12::OFPT_PACKET_IN: {
    *pmsg = new rofl::openflow::cofmsg_packet_in();
  } break;
  case rofl::openflow12::OFPT_FLOW_MOD: {
    *pmsg = new rofl::openflow::cofmsg_flow_mod();
  } break;
  case rofl::openflow12::OFPT_FLOW_REMOVED: {
    *pmsg = new rofl::openflow::cofmsg_flow_removed();
  } break;
  case rofl::openflow12::OFPT_GROUP_MOD: {
    *pmsg = new rofl::openflow::cofmsg_group_mod();
  } break;
  case rofl::openflow12::OFPT_PORT_MOD: {
    *pmsg = new rofl::openflow::cofmsg_port_mod();
  } break;
  case rofl::openflow12::OFPT_PORT_STATUS: {
    *pmsg = new rofl::openflow::cofmsg_port_status();
  } break;
  case rofl::openflow12::OFPT_TABLE_MOD: {
    *pmsg = new rofl::openflow::cofmsg_table_mod();
  } break;
  case rofl::openflow12::OFPT_STATS_REQUEST: {
    if (rxbuffer.length() <
        sizeof(struct rofl::openflow12::ofp_stats_request)) {
      throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                              __LINE__);
    }
    uint16_t stats_type = be16toh(
        ((struct rofl::openflow12::ofp_stats_request *)rxbuffer.somem())->type);
    switch (stats_type) {
    case rofl::openflow12::OFPST_DESC: {
      *pmsg = new rofl::openflow::cofmsg_desc_stats_request();
    } break;
    case rofl::openflow12::OFPST_FLOW: {
      *pmsg = new rofl::openflow::cofmsg_flow_stats_request();
    } break;
    case rofl::openflow12::OFPST_AGGREGATE: {
      *pmsg = new rofl::openflow::cofmsg_aggr_stats_request();
    } break;
    case rofl::openflow12::OFPST_TABLE: {
      *pmsg = new rofl::openflow::cofmsg_table_stats_request();
    } break;
    case rofl::openflow12::OFPST_PORT: {
      *pmsg = new rofl::openflow::cofmsg_port_stats_request();
    } break;
    case rofl::openflow12::OFPST_QUEUE: {
      *pmsg = new rofl::openflow::cofmsg_queue_stats_request();
    } break;
    case rofl::openflow12::OFPST_GROUP: {
      *pmsg = new rofl::openflow::cofmsg_group_stats_request();
    } break;
    case rofl::openflow12::OFPST_GROUP_DESC: {
      *pmsg = new rofl::openflow::cofmsg_group_desc_stats_request();
    } break;
    case rofl::openflow12::OFPST_GROUP_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_group_features_stats_request();
    } break;
    case rofl::openflow12::OFPST_EXPERIMENTER: {
      *pmsg = new rofl::openflow::cofmsg_experimenter_stats_request();
    } break;
    default: {
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
    };
    }
  } break;
  case rofl::openflow12::OFPT_STATS_REPLY: {
    if (rxbuffer.length() < sizeof(struct rofl::openflow12::ofp_stats_reply)) {
      throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                              __LINE__);
    }
    uint16_t stats_type = be16toh(
        ((struct rofl::openflow12::ofp_stats_reply *)rxbuffer.somem())->type);
    switch (stats_type) {
    case rofl::openflow12::OFPST_DESC: {
      *pmsg = new rofl::openflow::cofmsg_desc_stats_reply();
    } break;
    case rofl::openflow12::OFPST_FLOW: {
      *pmsg = new rofl::openflow::cofmsg_flow_stats_reply();
    } break;
    case rofl::openflow12::OFPST_AGGREGATE: {
      *pmsg = new rofl::openflow::cofmsg_aggr_stats_reply();
    } break;
    case rofl::openflow12::OFPST_TABLE: {
      *pmsg = new rofl::openflow::cofmsg_table_stats_reply();
    } break;
    case rofl::openflow12::OFPST_PORT: {
      *pmsg = new rofl::openflow::cofmsg_port_stats_reply();
    } break;
    case rofl::openflow12::OFPST_QUEUE: {
      *pmsg = new rofl::openflow::cofmsg_queue_stats_reply();
    } break;
    case rofl::openflow12::OFPST_GROUP: {
      *pmsg = new rofl::openflow::cofmsg_group_stats_reply();
    } break;
    case rofl::openflow12::OFPST_GROUP_DESC: {
      *pmsg = new rofl::openflow::cofmsg_group_desc_stats_reply();
    } break;
    case rofl::openflow12::OFPST_GROUP_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_group_features_stats_reply();
    } break;
    case rofl::openflow12::OFPST_EXPERIMENTER: {
      *pmsg = new rofl::openflow::cofmsg_experimenter_stats_reply();
    } break;
    default: {
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
    };
    }
  } break;
  case rofl::openflow12::OFPT_BARRIER_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_barrier_request();
  } break;
  case rofl::openflow12::OFPT_BARRIER_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_barrier_reply();
  } break;
  case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_queue_get_config_request();
  } break;
  case rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_queue_get_config_reply();
  } break;
  case rofl::openflow12::OFPT_ROLE_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_role_request();
  } break;
  case rofl::openflow12::OFPT_ROLE_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_role_reply();
  } break;
  case rofl::openflow12::OFPT_GET_ASYNC_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_get_async_config_request();
  } break;
  case rofl::openflow12::OFPT_GET_ASYNC_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_get_async_config_reply();
  } break;
  case rofl::openflow12::OFPT_SET_ASYNC: {
    *pmsg = new rofl::openflow::cofmsg_set_async_config();
  } break;
  default: {
    throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                             __LINE__);
  };
  }

  (*(*pmsg)).unpack(rxbuffer.sormem(), rxbuffer.rmemlen());
  rxbuffer.reset();
}

void crofsock::parse_of13_message(rofl::openflow::cofmsg **pmsg) {
  struct openflow::ofp_header *header =
      (struct openflow::ofp_header *)rxbuffer.somem();

  switch (header->type) {
  case rofl::openflow13::OFPT_HELLO: {
    *pmsg = new rofl::openflow::cofmsg_hello();
  } break;
  case rofl::openflow13::OFPT_ERROR: {
    *pmsg = new rofl::openflow::cofmsg_error();
  } break;
  case rofl::openflow13::OFPT_ECHO_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_echo_request();
  } break;
  case rofl::openflow13::OFPT_ECHO_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_echo_reply();
  } break;
  case rofl::openflow13::OFPT_EXPERIMENTER: {
    *pmsg = new rofl::openflow::cofmsg_experimenter();
  } break;
  case rofl::openflow13::OFPT_FEATURES_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_features_request();
  } break;
  case rofl::openflow13::OFPT_FEATURES_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_features_reply();
  } break;
  case rofl::openflow13::OFPT_GET_CONFIG_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_get_config_request();
  } break;
  case rofl::openflow13::OFPT_GET_CONFIG_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_get_config_reply();
  } break;
  case rofl::openflow13::OFPT_SET_CONFIG: {
    *pmsg = new rofl::openflow::cofmsg_set_config();
  } break;
  case rofl::openflow13::OFPT_PACKET_OUT: {
    *pmsg = new rofl::openflow::cofmsg_packet_out();
  } break;
  case rofl::openflow13::OFPT_PACKET_IN: {
    *pmsg = new rofl::openflow::cofmsg_packet_in();
  } break;
  case rofl::openflow13::OFPT_FLOW_MOD: {
    *pmsg = new rofl::openflow::cofmsg_flow_mod();
  } break;
  case rofl::openflow13::OFPT_FLOW_REMOVED: {
    *pmsg = new rofl::openflow::cofmsg_flow_removed();
  } break;
  case rofl::openflow13::OFPT_GROUP_MOD: {
    *pmsg = new rofl::openflow::cofmsg_group_mod();
  } break;
  case rofl::openflow13::OFPT_PORT_MOD: {
    *pmsg = new rofl::openflow::cofmsg_port_mod();
  } break;
  case rofl::openflow13::OFPT_PORT_STATUS: {
    *pmsg = new rofl::openflow::cofmsg_port_status();
  } break;
  case rofl::openflow13::OFPT_TABLE_MOD: {
    *pmsg = new rofl::openflow::cofmsg_table_mod();
  } break;
  case rofl::openflow13::OFPT_MULTIPART_REQUEST: {
    if (rxbuffer.memlen() <
        sizeof(struct rofl::openflow13::ofp_multipart_request)) {
      throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                              __LINE__);
    }
    uint16_t stats_type = be16toh(
        ((struct rofl::openflow13::ofp_multipart_request *)rxbuffer.somem())
            ->type);
    switch (stats_type) {
    case rofl::openflow13::OFPMP_DESC: {
      *pmsg = new rofl::openflow::cofmsg_desc_stats_request();
    } break;
    case rofl::openflow13::OFPMP_FLOW: {
      *pmsg = new rofl::openflow::cofmsg_flow_stats_request();
    } break;
    case rofl::openflow13::OFPMP_AGGREGATE: {
      *pmsg = new rofl::openflow::cofmsg_aggr_stats_request();
    } break;
    case rofl::openflow13::OFPMP_TABLE: {
      *pmsg = new rofl::openflow::cofmsg_table_stats_request();
    } break;
    case rofl::openflow13::OFPMP_PORT_STATS: {
      *pmsg = new rofl::openflow::cofmsg_port_stats_request();
    } break;
    case rofl::openflow13::OFPMP_QUEUE: {
      *pmsg = new rofl::openflow::cofmsg_queue_stats_request();
    } break;
    case rofl::openflow13::OFPMP_GROUP: {
      *pmsg = new rofl::openflow::cofmsg_group_stats_request();
    } break;
    case rofl::openflow13::OFPMP_GROUP_DESC: {
      *pmsg = new rofl::openflow::cofmsg_group_desc_stats_request();
    } break;
    case rofl::openflow13::OFPMP_GROUP_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_group_features_stats_request();
    } break;
    case rofl::openflow13::OFPMP_METER: {
      *pmsg = new rofl::openflow::cofmsg_meter_stats_request();
    } break;
    case rofl::openflow13::OFPMP_METER_CONFIG: {
      *pmsg = new rofl::openflow::cofmsg_meter_config_stats_request();
    } break;
    case rofl::openflow13::OFPMP_METER_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_meter_features_stats_request();
    } break;
    case rofl::openflow13::OFPMP_TABLE_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_table_features_stats_request();
    } break;
    case rofl::openflow13::OFPMP_PORT_DESC: {
      *pmsg = new rofl::openflow::cofmsg_port_desc_stats_request();
    } break;
    case rofl::openflow13::OFPMP_EXPERIMENTER: {
      *pmsg = new rofl::openflow::cofmsg_experimenter_stats_request();
    } break;
    default: {
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
    };
    }
  } break;
  case rofl::openflow13::OFPT_MULTIPART_REPLY: {
    if (rxbuffer.memlen() <
        sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
      throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __FUNCTION__,
                              __LINE__);
    }
    uint16_t stats_type = be16toh(
        ((struct rofl::openflow13::ofp_multipart_reply *)rxbuffer.somem())
            ->type);
    switch (stats_type) {
    case rofl::openflow13::OFPMP_DESC: {
      *pmsg = new rofl::openflow::cofmsg_desc_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_FLOW: {
      *pmsg = new rofl::openflow::cofmsg_flow_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_AGGREGATE: {
      *pmsg = new rofl::openflow::cofmsg_aggr_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_TABLE: {
      *pmsg = new rofl::openflow::cofmsg_table_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_PORT_STATS: {
      *pmsg = new rofl::openflow::cofmsg_port_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_QUEUE: {
      *pmsg = new rofl::openflow::cofmsg_queue_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_GROUP: {
      *pmsg = new rofl::openflow::cofmsg_group_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_GROUP_DESC: {
      *pmsg = new rofl::openflow::cofmsg_group_desc_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_GROUP_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_group_features_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_METER: {
      *pmsg = new rofl::openflow::cofmsg_meter_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_METER_CONFIG: {
      *pmsg = new rofl::openflow::cofmsg_meter_config_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_METER_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_meter_features_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_TABLE_FEATURES: {
      *pmsg = new rofl::openflow::cofmsg_table_features_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_PORT_DESC: {
      *pmsg = new rofl::openflow::cofmsg_port_desc_stats_reply();
    } break;
    case rofl::openflow13::OFPMP_EXPERIMENTER: {
      *pmsg = new rofl::openflow::cofmsg_experimenter_stats_reply();
    } break;
    default: {
      throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __FUNCTION__,
                               __LINE__);
    };
    }
  } break;
  case rofl::openflow13::OFPT_BARRIER_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_barrier_request();
  } break;
  case rofl::openflow13::OFPT_BARRIER_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_barrier_reply();
  } break;
  case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_queue_get_config_request();
  } break;
  case rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_queue_get_config_reply();
  } break;
  case rofl::openflow13::OFPT_ROLE_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_role_request();
  } break;
  case rofl::openflow13::OFPT_ROLE_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_role_reply();
  } break;
  case rofl::openflow13::OFPT_GET_ASYNC_REQUEST: {
    *pmsg = new rofl::openflow::cofmsg_get_async_config_request();
  } break;
  case rofl::openflow13::OFPT_GET_ASYNC_REPLY: {
    *pmsg = new rofl::openflow::cofmsg_get_async_config_reply();
  } break;
  case rofl::openflow13::OFPT_SET_ASYNC: {
    *pmsg = new rofl::openflow::cofmsg_set_async_config();
  } break;
  case rofl::openflow13::OFPT_METER_MOD: {
    *pmsg = new rofl::openflow::cofmsg_meter_mod();
  } break;
  default: {
    throw eBadRequestBadType("eBadRequestBadType", __FILE__, __FUNCTION__,
                             __LINE__);
  };
  }

  (*(*pmsg)).unpack(rxbuffer.sormem(), rxbuffer.rmemlen());
  rxbuffer.reset();
}
