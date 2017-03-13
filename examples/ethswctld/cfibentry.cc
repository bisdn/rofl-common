/*
 * cfibentry.cc
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#include "cfibentry.h"

using namespace rofl::examples::ethswctld;
/*static*/ const time_t cfibentry::CFIBENTRY_DEFAULT_TIMEOUT = 60;

cfibentry::cfibentry(cfibentry_env *fibenv, const rofl::caddress_ll &hwaddr,
                     uint32_t port_no)
    : env(fibenv), port_no(port_no), hwaddr(hwaddr),
      entry_timeout(CFIBENTRY_DEFAULT_TIMEOUT), thread(this) {
  thread.add_timer(TIMER_ID_ENTRY_EXPIRED,
                   rofl::ctimespec().expire_in(entry_timeout));
  std::cerr << "[cfibentry] created" << std::endl << *this;
  thread.start("cfibentry");
}

cfibentry::~cfibentry() {
  std::cerr << "[cfibentry] deleted" << std::endl << *this;
}

void cfibentry::handle_timeout(cthread &thread, uint32_t timer_id) {
  switch (timer_id) {
  case TIMER_ID_ENTRY_EXPIRED: {
    env->fib_timer_expired(hwaddr);
  }
    return;
  }
}

void cfibentry::set_port_no(uint32_t port_no) {
  if (port_no != this->port_no) {
    this->port_no = port_no;
    env->fib_port_update(*this);
  }

  thread.add_timer(TIMER_ID_ENTRY_EXPIRED,
                   rofl::ctimespec().expire_in(entry_timeout));
}
