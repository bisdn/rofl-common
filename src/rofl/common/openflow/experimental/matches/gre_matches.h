/*
 * gre_matches.h
 *
 *  Created on: 23.09.2014
 *      Author: andreas
 */

#ifndef GRE_MATCHES_H_
#define GRE_MATCHES_H_

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace gre {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
  /* GRE related extensions */
  OFPXMT_OFX_GRE_VERSION = 37,   /* GRE version */
  OFPXMT_OFX_GRE_PROT_TYPE = 38, /* GRE protocol type */
  OFPXMT_OFX_GRE_KEY = 39,       /* GRE key */
};

static unsigned int const OXM_EXP_ID_SIZE =
    sizeof(uint32_t); // length of experimenter id field

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
  OXM_TLV_EXPR_GRE_VERSION = (OFPXMC_EXPERIMENTER << 16) |
                             (OFPXMT_OFX_GRE_VERSION << 9) |
                             (OXM_EXP_ID_SIZE + 2),
  OXM_TLV_EXPR_GRE_VERSION_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                  (OFPXMT_OFX_GRE_VERSION << 9) |
                                  (OXM_EXP_ID_SIZE + 4) | HAS_MASK_FLAG,
  OXM_TLV_EXPR_GRE_PROT_TYPE = (OFPXMC_EXPERIMENTER << 16) |
                               (OFPXMT_OFX_GRE_PROT_TYPE << 9) |
                               (OXM_EXP_ID_SIZE + 2),
  OXM_TLV_EXPR_GRE_PROT_TYPE_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                    (OFPXMT_OFX_GRE_PROT_TYPE << 9) |
                                    (OXM_EXP_ID_SIZE + 4) | HAS_MASK_FLAG,
  OXM_TLV_EXPR_GRE_KEY = (OFPXMC_EXPERIMENTER << 16) |
                         (OFPXMT_OFX_GRE_KEY << 9) | (OXM_EXP_ID_SIZE + 4),
  OXM_TLV_EXPR_GRE_KEY_MASK = (OFPXMC_EXPERIMENTER << 16) |
                              (OFPXMT_OFX_GRE_KEY << 9) |
                              (OXM_EXP_ID_SIZE + 8) | HAS_MASK_FLAG,
};

/** OXM_OFX_GRE_VERSION
 *
 */
class coxmatch_ofx_gre_version : public coxmatch_exp {
public:
  coxmatch_ofx_gre_version(uint16_t version)
      : coxmatch_exp(OXM_TLV_EXPR_GRE_VERSION, ROFL_EXP_ID, version){};
  coxmatch_ofx_gre_version(uint16_t version, uint16_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_GRE_VERSION_MASK, ROFL_EXP_ID, version,
                     mask){};
  coxmatch_ofx_gre_version(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_gre_version(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_gre_version &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<gre-version: " << (int)oxm.get_u16value() << "/"
       << (int)oxm.get_u16mask() << " >" << std::endl;
    return os;
  };
};

/** OXM_OFX_GRE_PROT_TYPE
 *
 */
class coxmatch_ofx_gre_prot_type : public coxmatch_exp {
public:
  coxmatch_ofx_gre_prot_type(uint16_t prot_type)
      : coxmatch_exp(OXM_TLV_EXPR_GRE_PROT_TYPE, ROFL_EXP_ID, prot_type){};
  coxmatch_ofx_gre_prot_type(uint16_t prot_type, uint16_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_GRE_PROT_TYPE_MASK, ROFL_EXP_ID, prot_type,
                     mask){};
  coxmatch_ofx_gre_prot_type(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_gre_prot_type(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_gre_prot_type &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<gre-prot-type: " << (int)oxm.get_u16value() << "/"
       << (int)oxm.get_u16mask() << " >" << std::endl;
    return os;
  };
};

/** OXM_OFX_GRE_KEY
 *
 */
class coxmatch_ofx_gre_key : public coxmatch_exp {
public:
  coxmatch_ofx_gre_key(uint32_t key)
      : coxmatch_exp(OXM_TLV_EXPR_GRE_KEY, ROFL_EXP_ID, key){};
  coxmatch_ofx_gre_key(uint32_t key, uint32_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_GRE_KEY_MASK, ROFL_EXP_ID, key, mask){};
  coxmatch_ofx_gre_key(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_gre_key(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_gre_key &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<gre-key: " << (int)oxm.get_u32value() << "/"
       << (int)oxm.get_u32mask() << " >" << std::endl;
    return os;
  };
};

}; // end of namespace gre
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* GRE_MATCHES_H_ */
