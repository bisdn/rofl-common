#ifndef GTP_MATCHES_H
#define GTP_MATCHES_H 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace gtp {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
  /* GTP related extensions */
  OFPXMT_OFX_GTP_MSGTYPE = 25, /* GTP message type */
  OFPXMT_OFX_GTP_TEID = 26,    /* GTP tunnel endpoint identifier */
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
  OXM_TLV_EXPR_GTP_MSGTYPE =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GTP_MSGTYPE << 9) | 1,
  OXM_TLV_EXPR_GTP_MSGTYPE_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                  (OFPXMT_OFX_GTP_MSGTYPE << 9) | 2 |
                                  HAS_MASK_FLAG,
  OXM_TLV_EXPR_GTP_TEID =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GTP_TEID << 9) | 4,
  OXM_TLV_EXPR_GTP_TEID_MASK = (OFPXMC_EXPERIMENTER << 16) |
                               (OFPXMT_OFX_GTP_TEID << 9) | 8 | HAS_MASK_FLAG,
};

/** OXM_OFX_GTP_TEID
 *
 */
class coxmatch_ofx_gtp_teid : public coxmatch_exp {
public:
  coxmatch_ofx_gtp_teid(uint32_t teid)
      : coxmatch_exp(OXM_TLV_EXPR_GTP_TEID, ROFL_EXP_ID) {
    set_u32value(teid);
  };
  coxmatch_ofx_gtp_teid(uint32_t teid, uint32_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_GTP_TEID_MASK, ROFL_EXP_ID) {
    set_u32value(teid);
    set_u32mask(mask);
  };
  coxmatch_ofx_gtp_teid(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_gtp_teid(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_gtp_teid &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<gtp-teid: " << (int)oxm.get_u32value() << "/"
       << (int)oxm.get_u32mask() << " >" << std::endl;
    return os;
  };
};

/** OXM_OFX_GTP_MSG_TYPE
 *
 */
class coxmatch_ofx_gtp_msg_type : public coxmatch_exp {
public:
  coxmatch_ofx_gtp_msg_type(uint8_t msg_type)
      : coxmatch_exp(OXM_TLV_EXPR_GTP_MSGTYPE, ROFL_EXP_ID, msg_type){};
  coxmatch_ofx_gtp_msg_type(uint8_t msg_type, uint8_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_GTP_MSGTYPE_MASK, ROFL_EXP_ID, msg_type,
                     mask){};
  coxmatch_ofx_gtp_msg_type(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_gtp_msg_type(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_gtp_msg_type &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<gtp-msg-type: " << (int)oxm.get_u8value() << " >" << std::endl;
    return os;
  };
};

}; // end of namespace gtp
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
