/*
 * cflow.cc
 *
 *  Created on: 15.08.2014
 *      Author: andreas
 */

#include "cflowentry.h"

using namespace rofl::examples::ethswctld;

cflowentry::cflowentry(cflowentry_env *flowenv, const rofl::cdptid &dptid,
                       const rofl::caddress_ll &src,
                       const rofl::caddress_ll &dst, uint32_t port_no)
    : env(flowenv), dptid(dptid), port_no(port_no), src(src), dst(dst),
      entry_timeout(CFLOWENTRY_DEFAULT_TIMEOUT) {
  flow_mod_add();
  thread.add_timer(this, CFLOWENTRY_ENTRY_EXPIRED,
                   rofl::ctimespec().expire_in(entry_timeout));
  std::cerr << "[cflowentry] created" << std::endl << *this;
  thread.start("cflowentry");
}

cflowentry::~cflowentry() {
  std::cerr << "[cflowentry] deleted" << std::endl << *this;
  flow_mod_delete();
  thread.drop_timer(this, rofl::cthread::ALL_TIMERS);
}

void cflowentry::handle_timeout(void *userdata) {
  int timer_id = (long)userdata;
  switch (timer_id) {
  case CFLOWENTRY_ENTRY_EXPIRED: {
    env->flow_timer_expired(*this);
  } break;
  }
}

void cflowentry::set_out_port_no(uint32_t port_no) {
  if (port_no != this->port_no) {
    this->port_no = port_no;
    flow_mod_modify();
  }

  thread.add_timer(this, CFLOWENTRY_ENTRY_EXPIRED,
                   rofl::ctimespec().expire_in(entry_timeout));
}

void cflowentry::flow_mod_add() {
  try {
    rofl::crofdpt &dpt = env->set_dpt(dptid);

    rofl::openflow::cofflowmod fe(dpt.get_version());
    rofl::cindex index(0);

    fe.set_command(rofl::openflow::OFPFC_ADD);
    fe.set_table_id(0);
    fe.set_hard_timeout(entry_timeout);
    fe.set_match().set_eth_src(src);
    fe.set_match().set_eth_dst(dst);

    switch (dpt.get_version()) {
    case rofl::openflow10::OFP_VERSION: {
      fe.set_actions().add_action_output(index++).set_port_no(port_no);
    } break;
    default: {
      fe.set_instructions()
          .set_inst_apply_actions()
          .set_actions()
          .add_action_output(index++)
          .set_port_no(port_no);
    };
    }

    dpt.send_flow_mod_message(rofl::cauxid(0), fe);

  } catch (rofl::eRofConnNotConnected &e) {
    // control connection not connected
  } catch (rofl::eRofQueueFull &e) {
    // control channel congested
  }
}

void cflowentry::flow_mod_modify() {
  try {
    rofl::crofdpt &dpt = env->set_dpt(dptid);

    rofl::openflow::cofflowmod fe(dpt.get_version());
    rofl::cindex index(0);

    fe.set_command(rofl::openflow::OFPFC_MODIFY_STRICT);
    fe.set_table_id(0);
    fe.set_hard_timeout(entry_timeout);
    fe.set_match().set_eth_src(src);
    fe.set_match().set_eth_dst(dst);

    switch (dpt.get_version()) {
    case rofl::openflow10::OFP_VERSION: {
      fe.set_actions().add_action_output(index++).set_port_no(port_no);
    } break;
    default: {
      fe.set_instructions()
          .set_inst_apply_actions()
          .set_actions()
          .add_action_output(index++)
          .set_port_no(port_no);
    };
    }

    dpt.send_flow_mod_message(rofl::cauxid(0), fe);

  } catch (rofl::eRofConnNotConnected &e) {
    // control connection not connected
  } catch (rofl::eRofQueueFull &e) {
    // control channel congested
  } catch (...) {
    // ...
  }
}

void cflowentry::flow_mod_delete() {
  try {
    rofl::crofdpt &dpt = env->set_dpt(dptid);

    rofl::openflow::cofflowmod fe(dpt.get_version());
    rofl::cindex index(0);

    fe.set_command(rofl::openflow::OFPFC_DELETE_STRICT);
    fe.set_table_id(0);
    fe.set_hard_timeout(entry_timeout);
    fe.set_match().set_eth_src(src);
    fe.set_match().set_eth_dst(dst);

    dpt.send_flow_mod_message(rofl::cauxid(0), fe);

  } catch (rofl::eRofConnNotConnected &e) {
    // control connection not connected
  } catch (rofl::eRofQueueFull &e) {
    // control channel congested
  }
}
