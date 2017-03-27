/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmatch.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#include "rofl/common/openflow/coxmatch.h"

//#include "rofl/common/openflow/experimental/matches/gre_matches.h"
//#include "rofl/common/openflow/experimental/matches/gtp_matches.h"
#include "rofl/common/openflow/experimental/matches/pppoe_matches.h"

namespace rofl {
namespace openflow {

std::ostream &operator<<(std::ostream &os, const coxmatch_exp &oxm) {
  using experimental::pppoe::coxmatch_ofx_pppoe_code;
  using experimental::pppoe::coxmatch_ofx_ppp_prot;
  using experimental::pppoe::coxmatch_ofx_pppoe_sid;
  using experimental::pppoe::coxmatch_ofx_pppoe_type;

  os << "<coxmatch_exp exp_id: 0x" << std::hex << oxm.get_oxm_exp_id()
     << std::dec << " >" << std::endl;

  switch (oxm.get_oxm_exp_id()) {
  case rofl::openflow::ROFL_EXP_ID:
    os << "field=" << (int)oxm.get_oxm_field();
    switch (oxm.get_oxm_field()) {
    case experimental::pppoe::OFPXMT_OFX_PPPOE_CODE: {
      coxmatch_ofx_pppoe_code code(oxm);
      os << code << std::endl;
    } break;
    case experimental::pppoe::OFPXMT_OFX_PPPOE_TYPE: {
      coxmatch_ofx_pppoe_type type(oxm);
      os << type << std::endl;
    } break;
    case experimental::pppoe::OFPXMT_OFX_PPPOE_SID: {
      coxmatch_ofx_pppoe_sid sid(oxm);
      os << sid << std::endl;
    } break;
    case experimental::pppoe::OFPXMT_OFX_PPP_PROT: {
      coxmatch_ofx_ppp_prot prot(oxm);
      os << prot << std::endl;
    } break;
    default:
      os << "failed ";
      break;
    }
    break;
  default:
    os << dynamic_cast<const coxmatch &>(oxm) << "<value: >" << oxm.get_value()
       << "<mask: >" << oxm.get_mask();
    break;
  }
  return os;
};
}
}
