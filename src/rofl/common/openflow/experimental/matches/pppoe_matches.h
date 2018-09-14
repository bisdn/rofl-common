#ifndef PPPOE_MATCHES
#define PPPOE_MATCHES 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace pppoe {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
  /* PPP/PPPoE related extensions */
  OFPXMT_OFX_PPPOE_CODE = 21, /* PPPoE code */
  OFPXMT_OFX_PPPOE_TYPE = 22, /* PPPoE type */
  OFPXMT_OFX_PPPOE_SID = 23,  /* PPPoE session id */
  OFPXMT_OFX_PPP_PROT = 24,   /* PPP protocol */
};

static unsigned int const OXM_EXP_ID_SIZE =
    sizeof(uint32_t); // length of experimenter id field

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
  OXM_TLV_EXPR_PPPOE_CODE = (OFPXMC_EXPERIMENTER << 16) |
                            (OFPXMT_OFX_PPPOE_CODE << 9) |
                            (OXM_EXP_ID_SIZE + 1),
  OXM_TLV_EXPR_PPPOE_TYPE = (OFPXMC_EXPERIMENTER << 16) |
                            (OFPXMT_OFX_PPPOE_TYPE << 9) |
                            (OXM_EXP_ID_SIZE + 1),
  OXM_TLV_EXPR_PPPOE_SID = (OFPXMC_EXPERIMENTER << 16) |
                           (OFPXMT_OFX_PPPOE_SID << 9) | (OXM_EXP_ID_SIZE + 2),
  OXM_TLV_EXPR_PPP_PROT = (OFPXMC_EXPERIMENTER << 16) |
                          (OFPXMT_OFX_PPP_PROT << 9) | (OXM_EXP_ID_SIZE + 2),
};

/** OXM_OF_PPPOE_CODE
 *
 */
class coxmatch_ofx_pppoe_code : public coxmatch_exp {
public:
  coxmatch_ofx_pppoe_code(uint8_t code)
      : coxmatch_exp(OXM_TLV_EXPR_PPPOE_CODE, ROFL_EXP_ID, code){};
  coxmatch_ofx_pppoe_code(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_pppoe_code(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_pppoe_code &oxm) {
    os << "<pppoe-code: " << (int)oxm.get_u8value() << " >" << std::endl;
    return os;
  };
};

/** OXM_OF_PPPOE_TYPE
 *
 */
class coxmatch_ofx_pppoe_type : public coxmatch_exp {
public:
  coxmatch_ofx_pppoe_type(uint8_t type)
      : coxmatch_exp(OXM_TLV_EXPR_PPPOE_TYPE, ROFL_EXP_ID, type){};
  coxmatch_ofx_pppoe_type(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_pppoe_type(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  coxmatch_ofx_pppoe_type const &oxm) {
    os << "<pppoe-type: " << (int)oxm.get_u8value() << " >" << std::endl;
    return os;
  };
};

/** OXM_OF_PPPOE_SID
 *
 */
class coxmatch_ofx_pppoe_sid : public coxmatch_exp {
public:
  coxmatch_ofx_pppoe_sid(uint16_t sid)
      : coxmatch_exp(OXM_TLV_EXPR_PPPOE_SID, ROFL_EXP_ID, sid){};
  coxmatch_ofx_pppoe_sid(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_pppoe_sid(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_pppoe_sid &oxm) {
    os << "<pppoe-sid: " << (int)oxm.get_u16value() << " >" << std::endl;
    return os;
  };
};

/** OXM_OF_PPP_PROT
 *
 */
class coxmatch_ofx_ppp_prot : public coxmatch_exp {
public:
  coxmatch_ofx_ppp_prot(uint16_t prot)
      : coxmatch_exp(OXM_TLV_EXPR_PPP_PROT, ROFL_EXP_ID, prot){};
  coxmatch_ofx_ppp_prot(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_ppp_prot(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_ppp_prot &oxm) {
    os << "<ppp-prot: " << (int)oxm.get_u16value() << " >" << std::endl;
    return os;
  };
};

}; // end of namespace pppoe
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
