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

using namespace rofl;

/*static*/std::set<crofsock_env*>  crofsock_env::socket_envs;
/*static*/crwlock                  crofsock_env::socket_envs_lock;
/*static*/crwlock                  crofsock::rwlock;
/*static*/bool                     crofsock::tls_initialized = false;

crofsock::~crofsock()
{
	close();
}



crofsock::crofsock(
		crofsock_env* env) :
				journal(this),
				env(env),
				state(STATE_IDLE),
				mode(MODE_UNKNOWN),
				rxthread(this),
				txthread(this),
				reconnect_backoff_max(60/*secs*/),
				reconnect_backoff_start(1/*secs*/),
				reconnect_backoff_current(1/*secs*/),
				reconnect_counter(0),
				sd(-1),
				domain(AF_INET),
				type(SOCK_STREAM),
				protocol(IPPROTO_TCP),
				backlog(10),
				ctx(NULL),
				ssl(NULL),
				bio(NULL),
				capath("."),
				cafile("ca.pem"),
				certfile("crt.pem"),
				keyfile("key.pem"),
				password(""),
				verify_mode("PEER"),
				verify_depth("1"),
				ciphers("EECDH+ECDSA+AESGCM EECDH+aRSA+AESGCM EECDH+ECDSA+SHA256 EECDH+aRSA+RC4 EDH+aRSA EECDH RC4 !aNULL !eNULL !LOW !3DES !MD5 !EXP !PSK !SRP !DSS"),
				rx_fragment_pending(false),
				rxbuffer((size_t)65536),
				msg_bytes_read(0),
				max_pkts_rcvd_per_round(0),
				rx_disabled(false),
				tx_disabled(false),
				txqueue_pending_pkts(0),
				txqueue_size_congestion_occured(0),
				txqueue_size_tx_threshold(0),
				txqueues(QUEUE_MAX),
				txweights(QUEUE_MAX),
				tx_is_running(false),
				tx_fragment_pending(false),
				txbuffer((size_t)65536),
				msg_bytes_sent(0),
				txlen(0)
{
	/* scheduler weights for transmission */
	txweights[QUEUE_OAM ] = 16;
	txweights[QUEUE_MGMT] = 32;
	txweights[QUEUE_FLOW] = 16;
	txweights[QUEUE_PKT ] =  8;
}



void
crofsock::close()
{
	switch (state) {
	case STATE_IDLE: {

		journal.log(LOG_INFO, "TCP: state -IDLE-");

		/* TLS down, TCP down => set rx_disabled flag back to false */
		rx_disabled = false;
		tx_disabled = false;

	} break;
	case STATE_CLOSED: {

		journal.log(LOG_INFO, "TCP: state -CLOSED-");

		/* stop threads */
		rxthread.stop();
		txthread.stop();

		sleep(1);

		/* remove all pending messages from tx queues */
		for (auto queue : txqueues) {
			queue.clear();
		}

		sleep(1);

		state = STATE_IDLE;

		crofsock::close();

	} break;
	case STATE_LISTENING: {

		journal.log(LOG_INFO, "TCP: state -LISTENING-");

		if (sd > 0) {
			rxthread.drop_read_fd(sd);
			::close(sd); sd = -1;
		}

		state = STATE_CLOSED;

		crofsock::close();

	} break;
	case STATE_TCP_CONNECTING: {

		journal.log(LOG_INFO, "TCP: state -CONNECTING-");

		txthread.drop_timer(TIMER_ID_RECONNECT);

		rxthread.drop_write_fd(sd);
		::close(sd); sd = -1;

		state = STATE_CLOSED;

		crofsock::close();

	} break;
	case STATE_TCP_ACCEPTING: {

		journal.log(LOG_INFO, "TCP: state -ACCEPTING-");

		rxthread.drop_read_fd(sd, false);
		if (flags.test(FLAG_CONGESTED)) {
			txthread.drop_write_fd(sd);
		}
		::close(sd); sd = -1;

		state = STATE_CLOSED;

		crofsock::close();

	} break;
	case STATE_TCP_ESTABLISHED: {

		journal.log(LOG_INFO, "TCP: state -ESTABLISHED-");

		/* block reception of any further data from remote side */
		rx_disable();
		tx_disable();

		rxthread.drop_read_fd(sd, false);
		if (flags.test(FLAG_CONGESTED)) {
			txthread.drop_write_fd(sd);
		}
		shutdown(sd, O_RDWR);
		sleep(1);
		::close(sd); sd = -1;

		state = STATE_CLOSED;

		crofsock::close();

	} break;
 	case STATE_TLS_CONNECTING:
	case STATE_TLS_ACCEPTING: {

		if (ssl) {
			SSL_free(ssl); ssl = NULL;
		}
		tls_destroy_context();
		flags.reset(FLAG_TLS_IN_USE);

		state = STATE_TCP_ESTABLISHED;

		crofsock::close();

	} break;
	case STATE_TLS_ESTABLISHED: {

		/* block reception of any further data from remote side */
		rx_disable();
		tx_disable();

		if (ssl) {
			SSL_shutdown(ssl);
			SSL_free(ssl); ssl = NULL;
		}
		tls_destroy_context();
		flags.reset(FLAG_TLS_IN_USE);

		state = STATE_TCP_ESTABLISHED;

		crofsock::close();

	} break;
	default: {

	};
	}
}



void
crofsock::listen()
{
	int rc;

	if (sd >= 0) {
		close();
	}

	/* start thread */
	rxthread.start();

	/* cancel potentially pending reconnect timer */
	rxthread.drop_timer(TIMER_ID_RECONNECT);

	/* socket in server mode */
	mode = MODE_LISTEN;

	/* reconnect does not make sense for listening sockets */
	flags.set(FLAG_RECONNECT_ON_FAILURE, false);

	/* open socket */
	if ((sd = ::socket(baddr.get_family(), type, protocol)) < 0) {
		throw eSysCall("eSysCall", "socket", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	/* make socket non-blocking */
	long flags;
	if ((flags = ::fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("eSysCall", "fcntl (F_GETFL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
	flags |= O_NONBLOCK;
	if ((rc = ::fcntl(sd, F_SETFL, flags)) < 0) {
		throw eSysCall("eSysCall", "fcntl (F_SETFL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
		int optval = 1;

		// set SO_REUSEADDR option on TCP sockets
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("eSysCall", "setsockopt (SO_REUSEADDR)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

#if 0
		int on = 1;
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on))) < 0) {
			throw eSysCall("eSysCall", "setsockopt (SO_REUSEPORT)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
#endif

		// set TCP_NODELAY option on TCP sockets
		if ((rc = ::setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("eSysCall", "setsockopt (TCP_NODELAY)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		// set SO_RCVLOWAT
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_RCVLOWAT, (int*)&optval, sizeof(optval))) < 0) {
			throw eSysCall("eSysCall", "setsockopt (SO_RCVLOWAT)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		// read TCP_NODELAY option for debugging purposes
		socklen_t optlen = sizeof(int);
		int optvalc;
		if ((rc = ::getsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (int*)&optvalc, &optlen)) < 0) {
			throw eSysCall("eSysCall", "getsockopt (TCP_NODELAY)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
	}

	/* bind to local address */
	if ((rc = ::bind(sd, baddr.ca_saddr, (socklen_t)(baddr.salen))) < 0) {
		throw eSysCall("eSysCall", "bind", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	/* listen on socket */
	if ((rc = ::listen(sd, backlog)) < 0) {
		throw eSysCall("eSysCall", "listen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	state = STATE_LISTENING;

	journal.log(LOG_INFO, "TCP: state -LISTENING-");

	/* instruct rxthread to read from socket descriptor */
	rxthread.add_read_fd(sd);
}



void
crofsock::tcp_accept(
		int sockfd)
{
	if (sd >= 0) {
		close();
	}

	/* start thread */
	rxthread.start();
	txthread.start();

	/* cancel potentially pending reconnect timer */
	rxthread.drop_timer(TIMER_ID_RECONNECT);

	/* socket in server mode */
	mode = MODE_SERVER;

	/* reconnect is not possible for server sockets */
	flags.set(FLAG_RECONNECT_ON_FAILURE, false);

	/* new state */
	state = STATE_TCP_ACCEPTING;

	journal.log(LOG_INFO, "TCP: state -ACCEPTING-");

	/* extract new connection from listening queue */
	if ((sd = ::accept(sockfd, laddr.ca_saddr, &(laddr.salen))) < 0) {
		throw eSysCall("eSysCall", "accept", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	/* make socket non-blocking, as this status is not inherited */
	long sockflags = 0;
	if ((sockflags = ::fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("eSysCall", "fcntl (F_GETFL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
	sockflags |= O_NONBLOCK;
	if ((::fcntl(sd, F_SETFL, sockflags)) < 0) {
		throw eSysCall("eSysCall", "fcntl (F_SETFL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	socklen_t optlen = 0;
	if ((::getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
		throw eSysCall("eSysCall", "getsockname", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	if ((::getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
		throw eSysCall("eSysCall", "getpeername", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
	optlen = sizeof(domain);
	if ((::getsockopt(sd, SOL_SOCKET, SO_DOMAIN, &domain, &optlen)) < 0) {
		throw eSysCall("eSysCall", "getsockopt (SO_DOMAIN)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
#endif

	optlen = sizeof(type);
	if ((::getsockopt(sd, SOL_SOCKET, SO_TYPE, &type, &optlen)) < 0) {
		throw eSysCall("eSysCall", "getsockopt (SO_TYPE)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

/* Some kernels do not define this option */
#ifdef SO_PROTOCOL
	optlen = sizeof(protocol);
	if ((::getsockopt(sd, SOL_SOCKET, SO_PROTOCOL, &protocol, &optlen)) < 0) {
		throw eSysCall("eSysCall", "getsockopt (SO_PROTOCOL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
#endif

	state = STATE_TCP_ESTABLISHED;

	journal.log(LOG_INFO, "TCP: state -ESTABLISHED-");

	/* instruct rxthread to read from socket descriptor */
	rxthread.add_read_fd(sd);

	if (flags.test(FLAG_TLS_IN_USE)) {
		crofsock::tls_accept(sockfd);
	} else {
		crofsock_env::call_env(env).handle_tcp_accepted(*this);
	}
}



void
crofsock::tcp_connect(
		bool reconnect)
{
	int rc;

	if (sd > 0) {
		close();
	}

	/* start thread */
	rxthread.start();
	txthread.start();

	/* cancel potentially pending reconnect timer */
	rxthread.drop_timer(TIMER_ID_RECONNECT);

	/* we do an active connect */
	mode = MODE_CLIENT;

	/* reconnect in case of an error? */
	flags.set(FLAG_RECONNECT_ON_FAILURE, reconnect);

	/* new state */
	state = STATE_TCP_CONNECTING;

	journal.log(LOG_INFO, "TCP: state -CONNECTING-");

	/* open socket */
	if ((sd = ::socket(raddr.get_family(), type, protocol)) < 0) {
		throw eSysCall("eSysCall", "socket", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	/* make socket non-blocking */
	long sockflags;
	if ((sockflags = ::fcntl(sd, F_GETFL)) < 0) {
		throw eSysCall("eSysCall", "fcntl (F_GETCL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}
	sockflags |= O_NONBLOCK;
	if ((rc = ::fcntl(sd, F_SETFL, sockflags)) < 0) {
		throw eSysCall("eSysCall", "fcntl (F_SETCL)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	/* set REUSEADDR and TCP_NODELAY options (for TCP sockets only) */
	if ((SOCK_STREAM == type) && (IPPROTO_TCP == protocol)) {
		int optval = 1;

		/* set SO_REUSEADDR option */
		if ((rc = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0) {
			throw eSysCall("eSysCall", "setsockopt (SO_REUSEADDR)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		/* set TCP_NODELAY option */
		if ((rc = ::setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval))) < 0) {
			throw eSysCall("eSysCall", "setsockopt (TCP_NODELAY)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
	}

	/* bind to local address */
	if (laddr.get_family() != AF_UNSPEC) {
		if ((rc = ::bind(sd, laddr.ca_saddr, (socklen_t)(laddr.salen))) < 0) {
			throw eSysCall("eSysCall", "bind", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
	}

	/* connect to remote address */
	if ((rc = ::connect(sd, (const struct sockaddr*)raddr.ca_saddr, (socklen_t)raddr.salen)) < 0) {
		/* connect did not succeed, handle error */
		switch (errno) {
		case EINPROGRESS: {
			journal.log(LOG_INFO, "TCP: EINPROGRESS");
			/* register socket descriptor for write operations */
			rxthread.add_write_fd(sd);
		} break;
		case ECONNREFUSED: {
			journal.log(LOG_INFO, "TCP: ECONNREFUSED");
			close();

			crofsock_env::call_env(env).handle_tcp_connect_refused(*this);

			if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
				backoff_reconnect(false);
			}
		} break;
		default: {
			journal.log(LOG_INFO, "TCP: connect error: %d(%s)", errno, strerror(errno));
			close();

			crofsock_env::call_env(env).handle_tcp_connect_failed(*this);

			if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
				backoff_reconnect(false);
			}
		};
		}
	} else {
		/* connect succeeded */

		if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
			throw eSysCall("eSysCall", "getsockname", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
			throw eSysCall("eSysCall", "getpeername", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		state = STATE_TCP_ESTABLISHED;

		journal.log(LOG_INFO, "TCP: state -ESTABLISHED-");

		/* register socket descriptor for read operations */
		rxthread.add_read_fd(sd);

		if (flags.test(FLAG_TLS_IN_USE)) {
			crofsock::tls_connect(flags.test(FLAG_RECONNECT_ON_FAILURE));
		} else {
			crofsock_env::call_env(env).handle_tcp_connected(*this);
		}
	}
}



void
crofsock::tls_init()
{
	AcquireReadWriteLock lock(crofsock::rwlock);
	if (crofsock::tls_initialized)
		return;

	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_ERR_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	BIO_new_fp(stderr, BIO_NOCLOSE);

	crofsock::tls_initialized = true;
}



void
crofsock::tls_destroy()
{
	AcquireReadWriteLock lock(crofsock::rwlock);
	if (not crofsock::tls_initialized)
		return;

	ERR_free_strings();

	crofsock::tls_initialized = false;
}



void
crofsock::tls_init_context()
{
	tls_init();

	if (ctx) {
		tls_destroy_context();
	}

	ctx = SSL_CTX_new(TLSv1_2_method());

	// certificate
	if (!SSL_CTX_use_certificate_file(ctx, certfile.c_str(), SSL_FILETYPE_PEM)) {
		throw eLibCall("eLibCall", "SSL_CTX_use_certificate_file", __FILE__, __PRETTY_FUNCTION__, __LINE__).set_key("certfile", certfile);
	}

	// private key
	SSL_CTX_set_default_passwd_cb(ctx, &crofsock::tls_pswd_cb);
	SSL_CTX_set_default_passwd_cb_userdata(ctx, (void*)this);

	if (!SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM)) {
		throw eLibCall("eLibCall", "SSL_CTX_use_PrivateKey_file", __FILE__, __PRETTY_FUNCTION__, __LINE__).set_key("keyfile", keyfile);
	}

	// ciphers
	if ((not ciphers.empty()) && (0 == SSL_CTX_set_cipher_list(ctx, ciphers.c_str()))) {
		throw eLibCall("eLibCall", "SSL_CTX_set_cipher_list", __FILE__, __PRETTY_FUNCTION__, __LINE__).set_key("ciphers", ciphers);
	}

	// capath/cafile
	if (!SSL_CTX_load_verify_locations(ctx,
			cafile.empty() ? NULL : cafile.c_str(),
			capath.empty() ? NULL : capath.c_str())) {
		throw eLibCall("eLibCall", "SSL_CTX_load_verify_locations", __FILE__, __PRETTY_FUNCTION__, __LINE__).set_key("cafile", cafile).set_key("capath", capath);
	}

	int mode = SSL_VERIFY_NONE;
	if (verify_mode == "NONE") {
		mode = SSL_VERIFY_NONE;
	} else
	if (verify_mode == "PEER") {
		mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
	}

	SSL_CTX_set_verify(ctx, mode, NULL);

	int depth; std::istringstream( verify_depth ) >> depth;

	SSL_CTX_set_verify_depth(ctx, depth);
}



void
crofsock::tls_destroy_context()
{
	if (ctx) {
		SSL_CTX_free(ctx); ctx = NULL;
	}

	tls_destroy();
}



int
crofsock::tls_pswd_cb(
		char *buf, int size, int rwflag, void *userdata)
{
	if (userdata == NULL)
		return 0;

	crofsock& socket = *(static_cast<crofsock*>(userdata));

	if (socket.get_tls_pswd().empty()) {
		return 0;
	}

	strncpy(buf, socket.get_tls_pswd().c_str(), size);

	return strnlen(buf, size);
}



void
crofsock::tls_accept(
		int sockfd)
{
	switch (state) {
	case STATE_IDLE:
	case STATE_CLOSED:
	case STATE_TCP_ACCEPTING: {

		flags.set(FLAG_TLS_IN_USE);

		crofsock::tcp_accept(sockfd);

	} break;
	case STATE_TCP_ESTABLISHED: {

		tls_init_context();

		if ((ssl = SSL_new(ctx)) == NULL) {
			throw eLibCall("eLibCall", "SSL_new", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		if ((bio = BIO_new(BIO_s_socket())) == NULL) {
			throw eLibCall("eLibCall", "BIO_new", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		BIO_set_fd(bio, sd, BIO_NOCLOSE);

		SSL_set_bio(ssl, /*rbio*/bio, /*wbio*/bio);

		SSL_set_accept_state(ssl);

		state = STATE_TLS_ACCEPTING;

		journal.log(LOG_INFO, "TLS: start passive connection");

	} break;
	case STATE_TLS_ACCEPTING: {

		int rc = 0, err_code = 0;

		if ((rc = SSL_accept(ssl)) <= 0) {
			switch (err_code = SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				journal.log(LOG_INFO, "TLS: accept WANT READ");
			} return;
			case SSL_ERROR_WANT_WRITE: {
				journal.log(LOG_INFO, "TLS: accept WANT WRITE");
			} return;
			case SSL_ERROR_WANT_ACCEPT: {
				journal.log(LOG_INFO, "TLS: accept WANT ACCEPT");
			} return;
			case SSL_ERROR_WANT_CONNECT: {
				journal.log(LOG_INFO, "TLS: accept WANT CONNECT");
			} return;


			case SSL_ERROR_NONE: {
				journal.log(LOG_INFO, "TLS: accept failed ERROR NONE");
			} break;
			case SSL_ERROR_SSL: {
				journal.log(LOG_INFO, "TLS: accept failed ERROR SSL");
			} break;
			case SSL_ERROR_SYSCALL: {
				journal.log(LOG_INFO, "TLS: accept failed ERROR SYSCALL");
			} break;
			case SSL_ERROR_ZERO_RETURN: {
				journal.log(LOG_INFO, "TLS: accept failed ERROR ZERO RETURN");
			} break;
			default: {
				journal.log(LOG_INFO, "TLS: accept failed");
			};
			}

			tls_log_errors();

			SSL_free(ssl); ssl = NULL; bio = NULL;

			tls_destroy_context();

			crofsock::close();

			crofsock_env::call_env(env).handle_tls_accept_failed(*this);

		} else
		if (rc == 1) {

			if (not tls_verify_ok()) {
				journal.log(LOG_INFO, "TLS: accept peer verification failed");

				tls_log_errors();

				SSL_free(ssl); ssl = NULL; bio = NULL;

				tls_destroy_context();

				crofsock::close();

				crofsock_env::call_env(env).handle_tls_accept_failed(*this);

				return;
			}

			journal.log(LOG_INFO, "TLS: accept succeeded");

			state = STATE_TLS_ESTABLISHED;

			crofsock_env::call_env(env).handle_tls_accepted(*this);
		}

	} break;
	case STATE_TLS_ESTABLISHED: {

		/* do nothing */

	} break;
	default: {

		throw eRofSockError("crofsock::tls_accept() called in invalid state", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}
}



void
crofsock::tls_connect(
		bool reconnect)
{
	switch (state) {
	case STATE_IDLE:
	case STATE_CLOSED:
	case STATE_TCP_CONNECTING: {

		flags.set(FLAG_TLS_IN_USE);

		crofsock::tcp_connect(reconnect);

	} break;
	case STATE_TCP_ESTABLISHED: {

		tls_init_context();

		if ((ssl = SSL_new(ctx)) == NULL) {
			throw eLibCall("eLibCall", "SSL_new", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		if ((bio = BIO_new(BIO_s_socket())) == NULL) {
			throw eLibCall("eLibCall", "BIO_new", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}

		BIO_set_fd(bio, sd, BIO_NOCLOSE);

		SSL_set_bio(ssl, /*rbio*/bio, /*wbio*/bio);

		SSL_set_connect_state(ssl);

		state = STATE_TLS_CONNECTING;

		journal.log(LOG_INFO, "TLS: start active connection");

		crofsock::tls_connect(reconnect);

	} break;
	case STATE_TLS_CONNECTING: {

		int rc = 0, err_code = 0;

		if ((rc = SSL_connect(ssl)) <= 0) {
			switch (err_code = SSL_get_error(ssl, rc)) {
			case SSL_ERROR_WANT_READ: {
				journal.log(LOG_INFO, "TLS: connect WANT READ");
			} return;
			case SSL_ERROR_WANT_WRITE: {
				journal.log(LOG_INFO, "TLS: connect WANT WRITE");
			} return;
			case SSL_ERROR_WANT_ACCEPT: {
				journal.log(LOG_INFO, "TLS: connect WANT ACCEPT");
			} return;
			case SSL_ERROR_WANT_CONNECT: {
				journal.log(LOG_INFO, "TLS: connect WANT CONNECT");
			} return;


			case SSL_ERROR_NONE: {
				journal.log(LOG_INFO, "TLS: connect failed ERROR NONE");
			} break;
			case SSL_ERROR_SSL: {
				journal.log(LOG_INFO, "TLS: connect failed ERROR SSL");
			} break;
			case SSL_ERROR_SYSCALL: {
				journal.log(LOG_INFO, "TLS: connect failed ERROR SYSCALL");
			} break;
			case SSL_ERROR_ZERO_RETURN: {
				journal.log(LOG_INFO, "TLS: connect failed ERROR ZERO RETURN");
			} break;
			default: {
				journal.log(LOG_INFO, "TLS: connect failed");
			};
			}

			tls_log_errors();

			SSL_free(ssl); ssl = NULL; bio = NULL;

			tls_destroy_context();

			crofsock::close();

			crofsock_env::call_env(env).handle_tls_connect_failed(*this);

		} else
		if (rc == 1) {

			if (not tls_verify_ok()) {
				journal.log(LOG_INFO, "TLS: connect peer verification failed");

				tls_log_errors();

				SSL_free(ssl); ssl = NULL; bio = NULL;

				tls_destroy_context();

				crofsock::close();

				crofsock_env::call_env(env).handle_tls_connect_failed(*this);

				return;
			}

			journal.log(LOG_INFO, "TLS: connect succeeded");

			state = STATE_TLS_ESTABLISHED;

			crofsock_env::call_env(env).handle_tls_connected(*this);
		}

	} break;
	case STATE_TLS_ESTABLISHED: {

		/* do nothing */

	} break;
	default: {

		throw eRofSockError("crofsock::tls_connect() called in invalid state", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}
}



bool
crofsock::tls_verify_ok()
{
	/* strategy:
	 * - always check peer certificate in client mode
	 * - check peer certificate in server mode when explicitly enabled (mode == SSL_VERIFY_PEER)
	 */
	if ((verify_mode == "PEER") || (STATE_TLS_CONNECTING == state)) {

		/*
		 * there must be a certificate presented by the peer in mode SSL_VERIFY_PEER
		 */
		X509* cert = (X509*)NULL;
		if ((cert = SSL_get_peer_certificate(ssl)) == NULL) {

			journal.log(LOG_INFO, "TLS: no certificate presented by peer");

			tls_log_errors();

			SSL_free(ssl); ssl = NULL; bio = NULL;

			tls_destroy_context();

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
				journal.log(LOG_INFO, "TLS: peer certificate verification: ok");
			} break;
			case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unable to get issuer certificate");
			} break;
			case X509_V_ERR_UNABLE_TO_GET_CRL: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unable to get certificate CRL");
			} break;
			case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unable to decrypt certificate's signature");
			} break;
			case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unable to decrypt CRL's signature");
			} break;
			case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unable to decode issuer public key");
			} break;
			case X509_V_ERR_CERT_SIGNATURE_FAILURE: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: certificate signature failure");
			} break;
			case X509_V_ERR_CRL_SIGNATURE_FAILURE: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: CRL signature failure");
			} break;
			case X509_V_ERR_CERT_NOT_YET_VALID: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: certificate is not yet valid");
			} break;
			case X509_V_ERR_CERT_HAS_EXPIRED: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: certificate has expired");
			} break;
			case X509_V_ERR_CRL_NOT_YET_VALID: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: CRL is not yet valid");
			} break;
			case X509_V_ERR_CRL_HAS_EXPIRED: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: CRL has expired");
			} break;
			case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: format error in certificate's notBefore field");
			} break;
			case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: format error in certificate's notAfter field");
			} break;
			case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: format error in CRL's lastUpdate field");
			} break;
			case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: format error in CRL's nextUpdate field");
			} break;
			case X509_V_ERR_OUT_OF_MEM: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: out of memory");
			} break;
			case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: self signed certificate");
			} break;
			case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: self signed certificate in certificate chain");
			} break;
			case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unable to get local issuer certificate");
			} break;
			case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unable to verify the first certificate");
			} break;
			case X509_V_ERR_CERT_CHAIN_TOO_LONG: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: certificate chain too long");
			} break;
			case X509_V_ERR_CERT_REVOKED: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: certificate revoked");
			} break;
			case X509_V_ERR_INVALID_CA: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: invalid CA certificate");
			} break;
			case X509_V_ERR_PATH_LENGTH_EXCEEDED: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: path length exceeded");
			} break;
			case X509_V_ERR_INVALID_PURPOSE: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: invalid purpose");
			} break;
			case X509_V_ERR_CERT_UNTRUSTED: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: certificate untrusted");
			} break;
			case X509_V_ERR_CERT_REJECTED: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: certificate rejected");
			} break;
			case X509_V_ERR_SUBJECT_ISSUER_MISMATCH: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: subject issuer mismatch");
			} break;
			case X509_V_ERR_AKID_SKID_MISMATCH: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: authority key id and subject key id mismatch");
			} break;
			case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: authority and issuer serial number mismatch");
			} break;
			case X509_V_ERR_KEYUSAGE_NO_CERTSIGN: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: key usage does not include certificate signing");
			} break;
			case X509_V_ERR_APPLICATION_VERIFICATION: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: application verification failure");
			} break;
			default: {
				journal.log(LOG_INFO, "TLS: peer certificate verification: unknown error");
			};
			}

			return false;
		}
	}

	return true;
}



void
crofsock::tls_log_errors()
{
	BIO* ebio = BIO_new(BIO_s_mem());

	ERR_print_errors(ebio);

	while (!BIO_eof(ebio)) {
		rofl::cmemory mem(1024);
		BIO_read(ebio, mem.somem(), mem.length() - 1);
		journal.log(LOG_CRIT_ERROR, "TLS: error history %s", (const char*)mem.somem());
	}

	BIO_free(ebio);
}



void
crofsock::backoff_reconnect(
		bool reset_timeout)
{
	/* restart rxthread as it was stopped in close() */
	rxthread.start();

	if (rxthread.has_timer(TIMER_ID_RECONNECT)) {
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

	journal.log(LOG_NOTICE, "scheduled reconnect in: %d secs", reconnect_backoff_current);

	rxthread.add_timer(TIMER_ID_RECONNECT, ctimespec().expire_in(reconnect_backoff_current, 0));

	++reconnect_counter;
}



bool
crofsock::is_established() const
{
	return (STATE_TCP_ESTABLISHED <= state);
}



bool
crofsock::is_tls_encrypted() const
{
	return flags.test(FLAG_TLS_IN_USE);
}



bool
crofsock::is_passive() const
{
	return (MODE_SERVER == mode);
}



bool
crofsock::is_congested() const
{
	return flags.test(FLAG_CONGESTED);
}



bool
crofsock::is_rx_disabled() const
{
	return rx_disabled;
}



void
crofsock::rx_disable()
{
	rx_disabled = true;
	switch (state) {
	case STATE_TCP_ESTABLISHED:
	case STATE_TLS_ESTABLISHED:{
		rxthread.drop_read_fd(sd, false);
	} break;
	default: {

	};
	}
}



void
crofsock::rx_enable()
{
	rx_disabled = false;
	switch (state) {
	case STATE_TCP_ESTABLISHED:
	case STATE_TLS_ESTABLISHED: {
		rxthread.add_read_fd(sd, false);
	} break;
	default: {

	};
	}
}



void
crofsock::tx_disable()
{
	tx_disabled = true;
}



void
crofsock::tx_enable()
{
	tx_disabled = false;
}



void
crofsock::handle_timeout(
		cthread& thread, uint32_t timer_id, const std::list<unsigned int>& ttypes)
{
	switch (timer_id) {
	case TIMER_ID_RECONNECT: {
		journal.log(LOG_INFO, "TCP: reconnecting");
		if (flags.test(FLAG_TLS_IN_USE)) {
			tls_connect(true);
		} else {
			tcp_connect(true);
		}
	} break;
	default: {
		/* do nothing */
	};
	}
}



void
crofsock::send_message(
		rofl::openflow::cofmsg *msg)
{
	if (tx_disabled) {
		delete msg; return;
	}

	if (flags.test(FLAG_TX_BLOCK_QUEUEING)) {
		throw eRofQueueFull("crofsock::send_message() transmission blocked, congestion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	}

	txqueue_pending_pkts++;

	switch (state) {
	case STATE_TCP_ESTABLISHED:
	case STATE_TLS_ESTABLISHED: {

		switch (msg->get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow10::OFPT_PACKET_IN:
			case rofl::openflow10::OFPT_PACKET_OUT: {
				txqueues[QUEUE_PKT].store(msg, true);
			} break;
			case rofl::openflow10::OFPT_FLOW_MOD:
			case rofl::openflow10::OFPT_FLOW_REMOVED: {
				txqueues[QUEUE_FLOW].store(msg, true);
			} break;
			case rofl::openflow10::OFPT_ECHO_REQUEST:
			case rofl::openflow10::OFPT_ECHO_REPLY: {
				txqueues[QUEUE_OAM].store(msg, true);
			} break;
			default: {
				txqueues[QUEUE_MGMT].store(msg, true);
			};
			}
		} break;
		case rofl::openflow12::OFP_VERSION: {
			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_PACKET_IN:
			case rofl::openflow12::OFPT_PACKET_OUT: {
				txqueues[QUEUE_PKT].store(msg, true);
			} break;
			case rofl::openflow12::OFPT_FLOW_MOD:
			case rofl::openflow12::OFPT_FLOW_REMOVED:
			case rofl::openflow12::OFPT_GROUP_MOD:
			case rofl::openflow12::OFPT_PORT_MOD:
			case rofl::openflow12::OFPT_TABLE_MOD: {
				txqueues[QUEUE_FLOW].store(msg, true);
			} break;
			case rofl::openflow12::OFPT_ECHO_REQUEST:
			case rofl::openflow12::OFPT_ECHO_REPLY: {
				txqueues[QUEUE_OAM].store(msg, true);
			} break;
			default: {
				txqueues[QUEUE_MGMT].store(msg, true);
			};
			}
		} break;
		case rofl::openflow13::OFP_VERSION:
		default: {
			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_PACKET_IN:
			case rofl::openflow13::OFPT_PACKET_OUT: {
				txqueues[QUEUE_PKT].store(msg, true);
			} break;
			case rofl::openflow13::OFPT_FLOW_MOD:
			case rofl::openflow13::OFPT_FLOW_REMOVED:
			case rofl::openflow13::OFPT_GROUP_MOD:
			case rofl::openflow13::OFPT_PORT_MOD:
			case rofl::openflow13::OFPT_TABLE_MOD: {
				txqueues[QUEUE_FLOW].store(msg, true);
			} break;
			case rofl::openflow13::OFPT_ECHO_REQUEST:
			case rofl::openflow13::OFPT_ECHO_REPLY: {
				txqueues[QUEUE_OAM].store(msg, true);
			} break;
			default: {
				txqueues[QUEUE_MGMT].store(msg, true);
			};
			}
		};
		}

		if (not tx_is_running) {
			txthread.wakeup();
		}

	} break;
	default: {
		delete msg;
		throw eRofSockNotEstablished("crofsock::send_message() socket not established", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}
}



void
crofsock::handle_wakeup(
		cthread& thread)
{
	if (&thread == &txthread) {
		send_from_queue();
	}
}



void
crofsock::handle_write_event(
		cthread& thread, int fd)
{
	if (&thread == &txthread) {
		assert(fd == sd);
		flags.reset(FLAG_CONGESTED);
		txthread.drop_write_fd(sd);
		send_from_queue();
	} else
	if (&thread == &rxthread) {
		assert(fd == sd);
		handle_read_event_rxthread(thread, fd);
	}
}



void
crofsock::send_from_queue()
{
	if (state < STATE_TCP_ESTABLISHED)
		return;

	bool reschedule = false;

	tx_is_running = true;

	do {
		for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

			for (unsigned int num = 0; num < txweights[queue_id]; ++num) {

				if ((tx_disabled) || (state < STATE_TCP_ESTABLISHED)) {
					tx_is_running = false;
					return;
				}

				/* no pending fragment */
				if (not tx_fragment_pending) {
					rofl::openflow::cofmsg *msg = nullptr;

					/* fetch a new message for transmission from tx queue */
					if ((msg = txqueues[queue_id].retrieve()) == NULL)
						break;
					//txqueues[queue_id].pop();

					/* bytes of this message sent so far */
					msg_bytes_sent = 0;

					/* overall length of this message */
					txlen = msg->length();

					/* pack message into txbuffer */
					msg->pack(txbuffer.somem(), txlen);

					/* remove C++ message object from heap */
					delete msg;
				}

				/* send memory block via socket in non-blocking mode */
				int nbytes = ::send(sd, txbuffer.somem() + msg_bytes_sent, txlen - msg_bytes_sent, MSG_DONTWAIT | MSG_NOSIGNAL);

				/* error occured */
				if (nbytes < 0) {
					switch (errno) {
					case EAGAIN: /* socket would block */ {
						tx_is_running = false;
						flags.set(FLAG_CONGESTED);
						txthread.add_write_fd(sd);

						if (not flags.test(FLAG_TX_BLOCK_QUEUEING)) {
							/* block transmission of further packets */
							flags.set(FLAG_TX_BLOCK_QUEUEING);
							/* remember queue size, when congestion occured */
							txqueue_size_congestion_occured = txqueue_pending_pkts;
							/* threshold for re-enabling acceptance of packets */
							txqueue_size_tx_threshold = txqueue_pending_pkts / 2;

							crofsock_env::call_env(env).congestion_occured_indication(*this);
						}
					} return;
					case SIGPIPE:
					default: {
						tx_is_running = false;
					} return;
					}

				/* at least some bytes were sent successfully */
				} else {
					msg_bytes_sent += nbytes;

					/* short write */
					if (msg_bytes_sent < txlen) {
						tx_fragment_pending = true;

					/* packet successfully sent */
					} else {
						tx_fragment_pending = false;
						txqueue_pending_pkts--;
					}
				}

			}

			if (not txqueues[queue_id].empty()) {
				reschedule = true;
			}
		}

		if ((not flags.test(FLAG_CONGESTED)) && flags.test(FLAG_TX_BLOCK_QUEUEING)) {
			if (txqueue_pending_pkts < txqueue_size_tx_threshold) {
				flags.reset(FLAG_TX_BLOCK_QUEUEING);
				crofsock_env::call_env(env).congestion_solved_indication(*this);
			}
		}

	} while (reschedule);

	tx_is_running = false;
}



void
crofsock::handle_read_event(
		cthread& thread, int fd)
{
	if (&thread == &rxthread) {
		handle_read_event_rxthread(thread, fd);
	}
}



void
crofsock::handle_read_event_rxthread(
		cthread& thread, int fd)
{
	try {
		switch (state) {
		case STATE_LISTENING: {

			crofsock_env::call_env(env).handle_listen(*this, sd);

		} break;
		case STATE_TCP_CONNECTING: {

			int rc;
			int optval = 0;
			int optlen = sizeof(optval);
			if ((rc = getsockopt(sd, SOL_SOCKET, SO_ERROR,
								 (void*)&optval, (socklen_t*)&optlen)) < 0) {
				throw eSysCall("eSysCall", "getsockopt (SO_ERROR)", __FILE__, __PRETTY_FUNCTION__, __LINE__);
			}

			switch (optval) {
			case 0:
			case EISCONN: {
				rxthread.drop_write_fd(sd);

				if ((getsockname(sd, laddr.ca_saddr, &(laddr.salen))) < 0) {
					throw eSysCall("eSysCall", "getsockname", __FILE__, __PRETTY_FUNCTION__, __LINE__);
				}

				if ((getpeername(sd, raddr.ca_saddr, &(raddr.salen))) < 0) {
					throw eSysCall("eSysCall", "getpeername", __FILE__, __PRETTY_FUNCTION__, __LINE__);
				}

				state = STATE_TCP_ESTABLISHED;

				journal.log(LOG_INFO, "TCP: state -ESTABLISHED-");

				/* register socket descriptor for read operations */
				rxthread.add_read_fd(sd);

				if (flags.test(FLAG_TLS_IN_USE)) {
					crofsock::tls_connect(flags.test(FLAG_RECONNECT_ON_FAILURE));
				} else {
					crofsock_env::call_env(env).handle_tcp_connected(*this);
				}
			} break;
			case EINPROGRESS: {
				/* connect still pending, just wait */
				journal.log(LOG_INFO, "TCP: EINPROGRESS");
			} break;
			case ECONNREFUSED: {
				journal.log(LOG_INFO, "TCP: ECONNREFUSED");
				close();

				crofsock_env::call_env(env).handle_tcp_connect_refused(*this);

				if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
					backoff_reconnect(false);
				}
			} break;
			default: {
				journal.log(LOG_INFO, "TCP: connect error: %d(%s)", errno, strerror(errno));
				close();

				crofsock_env::call_env(env).handle_tcp_connect_failed(*this);

				if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
					backoff_reconnect(false);
				}
			};
			}

		} break;
		case STATE_TCP_ACCEPTING: {

		} break;
		case STATE_TLS_CONNECTING: {

			tls_connect(flags.test(FLAG_RECONNECT_ON_FAILURE));

		} break;
		case STATE_TLS_ACCEPTING: {

			tls_accept(fd);

		} break;
		case STATE_TCP_ESTABLISHED: {

			if (flags.test(FLAG_TLS_IN_USE))
				return;
			recv_message();

		} break;
		case STATE_TLS_ESTABLISHED: {

			if (not flags.test(FLAG_TLS_IN_USE))
				return;
			recv_message();

		} break;
		default: {
			/* do nothing */
		};
		}


	} catch (std::runtime_error& e) {

		journal.log(LOG_RUNTIME_ERROR, "crofsock::handle_read_event_rxthread() exception caught, what: ", e.what());

	} catch (...) {

	}
}



void
crofsock::recv_message()
{
	while (not rx_disabled) {

		if (not rx_fragment_pending) {
			msg_bytes_read = 0;
		}

		uint16_t msg_len = 0;

		/* how many bytes do we have to read? */
		if (msg_bytes_read < sizeof(struct openflow::ofp_header)) {
			msg_len = sizeof(struct openflow::ofp_header);
		} else {
			struct openflow::ofp_header *header = (struct openflow::ofp_header*)(rxbuffer.somem());
			msg_len = be16toh(header->length);
		}

		/* sanity check: 8 <= msg_len <= 2^16 */
		if (msg_len < sizeof(struct openflow::ofp_header)) {
			/* out-of-sync => enforce reconnect in client mode */
			goto on_error;
		}

		/* read from socket more bytes, at most "msg_len - msg_bytes_read" */
		int rc = ::recv(sd, (void*)(rxbuffer.somem() + msg_bytes_read), msg_len - msg_bytes_read, MSG_DONTWAIT);

		if (rc < 0) {
			switch (errno) {
			case EAGAIN: {
				/* do not continue and let kernel inform us, once more data is available */
				return;
			} break;
			default: {
				/* oops, error */
				goto on_error;
			};
			}
		} else
		if (rc == 0) {
			/* shutdown from peer */
			goto on_error;
		}

		msg_bytes_read += rc;

		/* minimum message length received, check completeness of message */
		if (msg_bytes_read >= sizeof(struct openflow::ofp_header)) {
			struct openflow::ofp_header *header =
					(struct openflow::ofp_header*)(rxbuffer.somem());
			uint16_t msg_len = be16toh(header->length);

			/* ok, message was received completely */
			if (msg_len == msg_bytes_read) {
				rx_fragment_pending = false;
				parse_message();
				msg_bytes_read = 0;
			} else {
				rx_fragment_pending = true;
			}
		}
	}

	return;

on_error:

	journal.log(LOG_INFO, "TCP: peer shutdown");
	close();

	crofsock_env::call_env(env).handle_closed(*this);

	if (flags.test(FLAG_RECONNECT_ON_FAILURE)) {
		backoff_reconnect(true);
	}
}



void
crofsock::parse_message()
{
	struct rofl::openflow::ofp_header* hdr =
			(struct rofl::openflow::ofp_header*)rxbuffer.somem();

	rofl::openflow::cofmsg *msg = (rofl::openflow::cofmsg*)0;
	try {
		if (rxbuffer.length() < sizeof(struct rofl::openflow::ofp_header)) {
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
			throw eBadRequestBadVersion("eBadRequestBadVersion", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}

		crofsock_env::call_env(env).handle_recv(*this, msg);

	} catch (eBadRequestBadType& e) {

		e.set_caller(__PRETTY_FUNCTION__).set_action("dropping message").set_key("xid", be32toh(hdr->xid));
		journal.log(e);

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_type(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : msg_bytes_read));

	} catch (eBadRequestBadStat& e) {

		e.set_caller(__PRETTY_FUNCTION__).set_action("dropping message").set_key("xid", be32toh(hdr->xid));
		journal.log(e);

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_stat(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : msg_bytes_read));

	} catch (eBadRequestBadVersion& e) {

		e.set_caller(__PRETTY_FUNCTION__).set_action("dropping message").set_key("xid", be32toh(hdr->xid));
		journal.log(e);

		if (msg) delete msg;

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_version(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : msg_bytes_read));

	} catch (eBadRequestBadLen& e) {

		e.set_caller(__PRETTY_FUNCTION__).set_action("dropping message").set_key("xid", be32toh(hdr->xid));
		journal.log(e);

		if (msg) delete msg;

		send_message(
				new rofl::openflow::cofmsg_error_bad_request_bad_len(
						hdr->version,
						be32toh(hdr->xid),
						rxbuffer.somem(),
						(rxbuffer.length() > 64) ? 64 : msg_bytes_read));


	} catch (rofl::exception& e) {

		e.set_caller(__PRETTY_FUNCTION__).set_action("dropping message").set_key("xid", be32toh(hdr->xid));
		journal.log(e);

		//if (msg) delete msg;

	} catch (std::runtime_error& e) {
		journal.log(LOG_RUNTIME_ERROR, "std::runtime_error: %s", e.what());

	}
}



void
crofsock::parse_of10_message(
		rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)rxbuffer.somem();

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
	case rofl::openflow10::OFPT_FEATURES_REQUEST:	{
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
		if (rxbuffer.length() < sizeof(struct rofl::openflow10::ofp_stats_request)) {
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow10::ofp_stats_request*)rxbuffer.somem())->type);
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
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
	} break;
	case rofl::openflow10::OFPT_STATS_REPLY: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow10::ofp_stats_reply)) {
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow10::ofp_stats_reply*)rxbuffer.somem())->type);
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
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
		throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}

	(*(*pmsg)).unpack(rxbuffer.somem(), msg_bytes_read);
}



void
crofsock::parse_of12_message(
		rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)rxbuffer.somem();

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
	case rofl::openflow12::OFPT_EXPERIMENTER:	{
		*pmsg = new rofl::openflow::cofmsg_experimenter();
	} break;
	case rofl::openflow12::OFPT_FEATURES_REQUEST:	{
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
		if (rxbuffer.length() < sizeof(struct rofl::openflow12::ofp_stats_request)) {
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow12::ofp_stats_request*)rxbuffer.somem())->type);
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
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
	} break;
	case rofl::openflow12::OFPT_STATS_REPLY: {
		if (rxbuffer.length() < sizeof(struct rofl::openflow12::ofp_stats_reply)) {
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow12::ofp_stats_reply*)rxbuffer.somem())->type);
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
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
		throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}

	(*(*pmsg)).unpack(rxbuffer.somem(), msg_bytes_read);
}



void
crofsock::parse_of13_message(
		rofl::openflow::cofmsg **pmsg)
{
	struct openflow::ofp_header* header = (struct openflow::ofp_header*)rxbuffer.somem();

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
	case rofl::openflow13::OFPT_EXPERIMENTER:	{
		*pmsg = new rofl::openflow::cofmsg_experimenter();
	} break;
	case rofl::openflow13::OFPT_FEATURES_REQUEST:	{
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
		if (rxbuffer.memlen() < sizeof(struct rofl::openflow13::ofp_multipart_request)) {
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow13::ofp_multipart_request*)rxbuffer.somem())->type);
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
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		};
		}
	} break;
	case rofl::openflow13::OFPT_MULTIPART_REPLY: {
		if (rxbuffer.memlen() < sizeof(struct rofl::openflow13::ofp_multipart_reply)) {
			throw eBadRequestBadLen("eBadRequestBadLen", __FILE__, __PRETTY_FUNCTION__, __LINE__);
		}
		uint16_t stats_type = be16toh(((struct rofl::openflow13::ofp_multipart_reply*)rxbuffer.somem())->type);
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
			throw eBadRequestBadStat("eBadRequestBadStat", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
		throw eBadRequestBadType("eBadRequestBadType", __FILE__, __PRETTY_FUNCTION__, __LINE__);
	};
	}

	(*(*pmsg)).unpack(rxbuffer.somem(), msg_bytes_read);
}




