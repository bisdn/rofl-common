/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *  Revised on: 28.09.2015
 *      Author: andreas
 */

#ifndef CROFCHAN_H_
#define CROFCHAN_H_

#include <atomic>
#include <bitset>
#include <inttypes.h>
#include <list>
#include <map>

#include "rofl/common/cauxid.h"
#include "rofl/common/crandom.h"
#include "rofl/common/crofconn.h"
#include "rofl/common/cthread.hpp"
#include "rofl/common/exception.hpp"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/openflow/messages/cofmsg.h"

namespace rofl {

class eRofChanBase : public exception {
public:
  eRofChanBase(const std::string &__arg) : exception(__arg){};
};
class eRofChanNotFound : public eRofChanBase {
public:
  eRofChanNotFound(const std::string &__arg) : eRofChanBase(__arg){};
};
class eRofChanExists : public eRofChanBase {
public:
  eRofChanExists(const std::string &__arg) : eRofChanBase(__arg){};
};
class eRofChanInval : public eRofChanBase {
public:
  eRofChanInval(const std::string &__arg) : eRofChanBase(__arg){};
};
class eRofChanNotConnected : public eRofChanBase {
public:
  eRofChanNotConnected(const std::string &__arg) : eRofChanBase(__arg){};
};
class eRofChanExhausted : public eRofChanBase {
public:
  eRofChanExhausted(const std::string &__arg) : eRofChanBase(__arg){};
};

class crofchan; // forward declaration

/**
 * @interface crofconn_env
 * @ingroup common_devel_workflow
 * @brief Environment expected by a rofl::crofconn instance.
 */
class crofchan_env {
  friend class crofchan;

public:
  static crofchan_env &call_env(crofchan_env *env) {
    AcquireReadLock lock(crofchan_env::channel_envs_lock);
    if (crofchan_env::channel_envs.find(env) ==
        crofchan_env::channel_envs.end()) {
      throw eRofChanNotFound(
          "crofchan_env::call_env() crofchan_env instance not found")
          .set_func(__PRETTY_FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    return *(env);
  };

public:
  virtual ~crofchan_env() {
    AcquireReadWriteLock lock(crofchan_env::channel_envs_lock);
    crofchan_env::channel_envs.erase(this);
  };
  crofchan_env() {
    AcquireReadWriteLock lock(crofchan_env::channel_envs_lock);
    crofchan_env::channel_envs.insert(this);
  };

protected:
  virtual void handle_established(crofchan &chan, uint8_t ofp_version) = 0;

  virtual void handle_closed(crofchan &chan) = 0;

  virtual void handle_established(crofchan &chan, crofconn &conn,
                                  uint8_t ofp_version) = 0;

  virtual void handle_connect_refused(crofchan &chan, crofconn &conn) = 0;

  virtual void handle_connect_failed(crofchan &chan, crofconn &conn) = 0;

  virtual void handle_accept_failed(crofchan &chan, crofconn &conn) = 0;

  virtual void handle_negotiation_failed(crofchan &chan, crofconn &conn) = 0;

  virtual void handle_closed(crofchan &chan, crofconn &conn) = 0;

  virtual void handle_recv(crofchan &chan, crofconn &conn,
                           rofl::openflow::cofmsg *msg) = 0;

  virtual void congestion_occured_indication(crofchan &chan,
                                             crofconn &conn) = 0;

  virtual void congestion_solved_indication(crofchan &chan, crofconn &conn) = 0;

  virtual void handle_transaction_timeout(crofchan &chan, crofconn &conn,
                                          uint32_t xid, uint8_t type,
                                          uint16_t sub_type = 0){};

private:
  static std::set<crofchan_env *> channel_envs;
  static crwlock channel_envs_lock;
};

/**
 * @ingroup common_devel_workflow
 * @brief	An OpenFlow control channel grouping multiple control
 * connections
 */
class crofchan : public cthread_env, public rofl::crofconn_env {
  enum crofchan_timer_t {
    TIMER_ID_ROFCONN_DESTROY = 1,
  };

public:
  /**
   *
   */
  virtual ~crofchan() {
    /* stop management thread */
    thread.stop();
    /* drop connections scheduled for removal */
    __drop_conns_deletion();
    /* drop active connections */
    __drop_conns();
  };

  /**
   *
   */
  crofchan(crofchan_env *env)
      : env(env), thread(this), last_auxid(0),
        ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN) {
    thread.start("crofchan");
  };

public:
  /**
   *
   */
  void send_message(const cauxid &auxid, rofl::openflow::cofmsg *msg);

  /**
   *
   */
  void send_message(const cauxid &auxid, rofl::openflow::cofmsg *msg,
                    const ctimespec &ts);

public:
  /**
   *
   */
  bool is_established() const;

  /**
   *
   */
  uint8_t get_version() const { return ofp_version; };

public:
  /**
   *
   */
  size_t size() const {
    AcquireReadLock rwlock(conns_rwlock);
    return conns.size();
  };

  /**
   *
   */
  std::list<cauxid> get_conn_ids() const { return keys(); };

  /**
   *
   */
  std::list<cauxid> keys() const {
    AcquireReadLock rwlock(conns_rwlock);
    std::list<cauxid> ids;
    for (auto it : conns) {
      ids.push_back(it.first);
    }
    return ids;
  };

  /**
   *
   */
  void close() { clear(); };

  /**
   *
   */
  void clear() {
    AcquireReadWriteLock rwlock(conns_rwlock);
    for (auto it : conns) {
      /* redirect environment */
      it.second->set_env(nullptr);
      /* close connection */
      it.second->close();
      AcquireReadWriteLock lock(conns_deletion_rwlock);
      conns_deletion.insert(it.second);
    }
    conns.clear();
    if (not thread.has_timer(TIMER_ID_ROFCONN_DESTROY)) {
      thread.add_timer(TIMER_ID_ROFCONN_DESTROY, ctimespec().expire_in(8));
    }
  };

  /**
   *
   */
  crofconn &add_conn() {
    AcquireReadWriteLock rwlock(conns_rwlock);
    unsigned int cnt = 256;
    while ((cnt--) && (conns.find(last_auxid) != conns.end())) {
      last_auxid = (last_auxid == 255) ? 0 : last_auxid + 1;
    }
    if (cnt == 0) {
      throw eRofChanExhausted("crofchan::add_conn() cauxid namespace exhausted")
          .set_func(__PRETTY_FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    (conns[last_auxid] = new crofconn(this))->set_auxid(cauxid(last_auxid));
    return *(conns[last_auxid]);
  };

  /**
   *
   */
  crofconn &add_conn(const cauxid &auxid) {
    AcquireReadWriteLock rwlock(conns_rwlock);
    if (conns.find(auxid) != conns.end()) {
      delete conns[auxid];
    }
    (conns[auxid] = new crofconn(this))->set_auxid(auxid);
    return *(conns[auxid]);
  };

  /**
   *
   */
  crofconn &add_conn(crofconn *conn) {
    if (nullptr == conn) {
      throw eRofChanInval("crofchan::add_conn() null pointer")
          .set_func(__PRETTY_FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__);
    }
    AcquireReadWriteLock rwlock(conns_rwlock);
    cauxid auxid(conn->get_auxid());
    if (conns.find(auxid) != conns.end()) {

      crofchan_env::call_env(env).handle_closed(*this, *conns[auxid]);

      AcquireReadWriteLock lock(conns_deletion_rwlock);
      conns[auxid]->set_env(nullptr);
      conns_deletion.insert(conns[auxid]);
      if (not thread.has_timer(TIMER_ID_ROFCONN_DESTROY)) {
        thread.add_timer(TIMER_ID_ROFCONN_DESTROY, ctimespec().expire_in(8));
      }
    }
    (conns[auxid] = conn)->set_env(this);
    if (auxid == cauxid(0)) {
      ofp_version = conn->get_version();
    }
    return *(conns[auxid]);
  };

  /**
   *
   */
  crofconn &set_conn(const cauxid &auxid) {
    AcquireReadWriteLock rwlock(conns_rwlock);
    if (conns.find(auxid) == conns.end()) {
      (conns[auxid] = new crofconn(this))->set_auxid(auxid);
    }
    return *(conns[auxid]);
  };

  /**
   *
   */
  const crofconn &get_conn(const cauxid &auxid) const {
    AcquireReadLock rwlock(conns_rwlock);
    if (conns.find(auxid) == conns.end()) {
      throw eRofChanNotFound("crofchan::get_conn() auxid not found")
          .set_func(__PRETTY_FUNCTION__)
          .set_file(__FILE__)
          .set_line(__LINE__)
          .set_key("auxid", auxid.get_id());
    }
    return *(conns.at(auxid));
  };

  /**
   *
   */
  bool drop_conn(const cauxid &auxid) {
    AcquireReadWriteLock rwlock(conns_rwlock);
    if (auxid.get_id() == 0) {
      for (auto it : conns) {
        /* redirect environment */
        it.second->set_env(nullptr);
        /* close connection */
        it.second->close();
        {
          AcquireReadWriteLock rwlock(conns_deletion_rwlock);
          /* add pointer to crofconn instance on heap to conns_deletion */
          conns_deletion.insert(it.second);
        }
      }
      /* mark all auxids as free */
      conns.clear();
    } else {
      if (conns.find(auxid) == conns.end()) {
        return false;
      }
      /* redirect environment */
      conns[auxid]->set_env(nullptr);
      /* close connection */
      conns[auxid]->close();
      {
        AcquireReadWriteLock rwlock(conns_deletion_rwlock);
        /* add pointer to crofconn instance on heap to conns_deletion */
        conns_deletion.insert(conns[auxid]);
      }
      /* mark its auxid as free */
      conns.erase(auxid);
    }
    /* trigger management thread for doing the clean-up work */
    if (not thread.has_timer(TIMER_ID_ROFCONN_DESTROY)) {
      thread.add_timer(TIMER_ID_ROFCONN_DESTROY, ctimespec().expire_in(8));
    }
    return true;
  };

  /**
   *
   */
  bool has_conn(const cauxid &auxid) const {
    AcquireReadLock rwlock(conns_rwlock);
    return (not(conns.find(auxid) == conns.end()));
  };

private:
  /**
   *
   */
  bool __drop_conn(const cauxid &auxid) {
    AcquireReadWriteLock rwlock(conns_rwlock);
    if (auxid.get_id() == 0) {
      for (auto it : conns) {
        delete it.second;
      }
      conns.clear();
    } else {
      if (conns.find(auxid) == conns.end()) {
        return false;
      }
      delete conns[auxid];
      conns.erase(auxid);
    }
    return true;
  };

  /**
   *
   */
  void __drop_conns() {
    AcquireReadWriteLock rwlock(conns_rwlock);
    for (auto it : conns) {
      // std::cerr << "__drop_conns: deleting conn (" <<
      // (int)it.second->get_auxid().get_id() << ") 0x" << it.second <<
      // std::endl;
      delete it.second;
    }
    conns.clear();
  };

  /**
   *
   */
  void __drop_conns_deletion() {
    AcquireReadWriteLock lock(conns_deletion_rwlock);
    for (auto conn : conns_deletion) {
      // std::cerr << "__drop_conns_deletion: deleting conn (" <<
      // (int)conn->get_auxid().get_id() << ") 0x" << conn << std::endl;
      delete conn;
    }
    conns_deletion.clear();
  };

public:
  friend std::ostream &operator<<(std::ostream &os, crofchan const &chan) {
    AcquireReadLock rwlock(chan.conns_rwlock);
    os << "<crofchan established:" << chan.is_established()
       << " ofp-version: " << (int)chan.ofp_version << " >" << std::endl;

    for (std::map<cauxid, crofconn *>::const_iterator it = chan.conns.begin();
         it != chan.conns.end(); ++it) {
      os << *(it->second);
    }
    return os;
  };

  std::string str() const {
    AcquireReadLock rwlock(conns_rwlock);
    std::stringstream ss;
    ss << "OFP version: " << (int)get_version() << " ";
    if (conns.empty() || (conns.find(rofl::cauxid(0)) == conns.end()) ||
        (not(conns.at(rofl::cauxid(0))->is_established()))) {
      ss << " state: -disconnected- ";
    } else {
      ss << " state: -established- ";
    }
    ss << "auxids: ";
    for (std::map<cauxid, crofconn *>::const_iterator it = conns.begin();
         it != conns.end(); ++it) {
      ss << "{" << (int)it->first.get_id() << ":"
         << (it->second->is_established() ? "-established-" : "-disconnected-")
         << "} ";
    }
    return ss.str();
  };

private:
  virtual void handle_established(crofconn &conn, uint8_t ofp_version) {
    if (conn.get_auxid() == cauxid(0)) {
      this->ofp_version = ofp_version;
      crofchan_env::call_env(env).handle_established(*this, ofp_version);
    }
    crofchan_env::call_env(env).handle_established(*this, conn, ofp_version);
  };

  virtual void handle_closed(crofconn &conn) {
    if (conn.get_auxid().get_id() == 0) {
      { /* acquire rwlock */
        AcquireReadLock rwlock(conns_rwlock);

        std::list<cauxid> to_be_removed;
        for (auto it : conns) {
          it.second->close();
          /* if connection is passive, schedule its deletion */
          if (it.second->is_passive()) {
            AcquireReadWriteLock lock(conns_deletion_rwlock);
            it.second->set_env(nullptr);
            conns_deletion.insert(it.second);
            to_be_removed.push_back(it.first);
          }
        }
        if (not to_be_removed.empty()) {
          if (not thread.has_timer(TIMER_ID_ROFCONN_DESTROY)) {
            thread.add_timer(TIMER_ID_ROFCONN_DESTROY,
                             ctimespec().expire_in(8));
          }
        }
        for (auto auxid : to_be_removed) {
          conns.erase(auxid);
        }
      } /* release rwlock */
      crofchan_env::call_env(env).handle_closed(*this);
    } else {
      /* if connection is passive, schedule its deletion */
      if (conn.is_passive()) {
        {
          AcquireReadWriteLock lock(conns_rwlock);
          if (conns.find(conn.get_auxid()) == conns.end()) {
            return;
          }
          conns.erase(conn.get_auxid());
        }
        conn.set_env(nullptr);
        {
          AcquireReadWriteLock lock(conns_deletion_rwlock);
          conns_deletion.insert(&conn);
        }
        if (not thread.has_timer(TIMER_ID_ROFCONN_DESTROY)) {
          thread.add_timer(TIMER_ID_ROFCONN_DESTROY, ctimespec().expire_in(8));
        }
      }
      crofchan_env::call_env(env).handle_closed(*this, conn);
    }
  };

  virtual void handle_connect_refused(crofconn &conn) {
    crofchan_env::call_env(env).handle_connect_refused(*this, conn);
  };

  virtual void handle_connect_failed(crofconn &conn) {
    crofchan_env::call_env(env).handle_connect_failed(*this, conn);
  };

  virtual void handle_accept_failed(crofconn &conn) {
    crofchan_env::call_env(env).handle_accept_failed(*this, conn);
  };

  virtual void handle_negotiation_failed(crofconn &conn) {
    crofchan_env::call_env(env).handle_negotiation_failed(*this, conn);
  };

  virtual void handle_recv(crofconn &conn, rofl::openflow::cofmsg *msg) {
    crofchan_env::call_env(env).handle_recv(*this, conn, msg);
  };

  virtual void congestion_occured_indication(crofconn &conn) {
    crofchan_env::call_env(env).congestion_occured_indication(*this, conn);
  };

  virtual void congestion_solved_indication(crofconn &conn) {
    crofchan_env::call_env(env).congestion_solved_indication(*this, conn);
  };

  virtual void handle_transaction_timeout(crofconn &conn, uint32_t xid,
                                          uint8_t type, uint16_t sub_type = 0) {
    crofchan_env::call_env(env).handle_transaction_timeout(*this, conn, xid,
                                                           type, sub_type);
  };

private:
  virtual void handle_wakeup(cthread &thread){};

  virtual void handle_timeout(cthread &thread, uint32_t timer_id) {
    switch (timer_id) {
    case TIMER_ID_ROFCONN_DESTROY: {
      __drop_conns_deletion();
    } break;
    default: {};
    }
  };

  virtual void handle_read_event(cthread &thread, int fd){};

  virtual void handle_write_event(cthread &thread, int fd){};

private:
  // owner of this crofchan instance
  crofchan_env *env;

  // management thread
  cthread thread;

  // main and auxiliary connections
  std::map<cauxid, crofconn *> conns;

  // connections set rwlock
  mutable crwlock conns_rwlock;

  // last auxid assigned
  uint8_t last_auxid;

  // OFP version negotiated
  std::atomic_uint_fast8_t ofp_version;

  // state related flags
  std::bitset<32> flags;

  // connections scheduled for deletion
  std::set<crofconn *> conns_deletion;

  // ... and the associated lock
  mutable rofl::crwlock conns_deletion_rwlock;
};

}; /* namespace rofl */

#endif /* CROFCHAN_H_ */
