#ifndef WLAN_MATCHES_H
#define WLAN_MATCHES_H

#include "../../coxmatch.h"

namespace rofl {
namespace openflow {
namespace experimental {
namespace wlan {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
  /* IEEE 802.11 related extensions */
  OFPXMT_OFX_WLAN_FC = 30,
  OFPXMT_OFX_WLAN_TYPE = 31,
  OFPXMT_OFX_WLAN_SUBTYPE = 32,
  OFPXMT_OFX_WLAN_DIRECTION = 33,
  OFPXMT_OFX_WLAN_ADDRESS_1 = 34,
  OFPXMT_OFX_WLAN_ADDRESS_2 = 35,
  OFPXMT_OFX_WLAN_ADDRESS_3 = 36,
};

static unsigned int const OXM_EXP_ID_SIZE = sizeof(uint32_t); // length of experimenter id field

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
  OXM_TLV_EXPR_WLAN_FC =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_FC << 9) | (OXM_EXP_ID_SIZE + 2),
  OXM_TLV_EXPR_WLAN_FC_MASK = (OFPXMC_EXPERIMENTER << 16) |
                              (OFPXMT_OFX_WLAN_FC << 9) | (OXM_EXP_ID_SIZE + 4) | HAS_MASK_FLAG,
  OXM_TLV_EXPR_WLAN_TYPE =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_TYPE << 9) | (OXM_EXP_ID_SIZE + 1),
  OXM_TLV_EXPR_WLAN_TYPE_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                (OFPXMT_OFX_WLAN_TYPE << 9) | (OXM_EXP_ID_SIZE + 2) | HAS_MASK_FLAG,
  OXM_TLV_EXPR_WLAN_SUBTYPE =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_SUBTYPE << 9) | (OXM_EXP_ID_SIZE + 1),
  OXM_TLV_EXPR_WLAN_SUBTYPE_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                   (OFPXMT_OFX_WLAN_SUBTYPE << 9) | (OXM_EXP_ID_SIZE + 2) |
                                   HAS_MASK_FLAG,
  OXM_TLV_EXPR_WLAN_DIRECTION =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_DIRECTION << 9) | (OXM_EXP_ID_SIZE + 1),
  OXM_TLV_EXPR_WLAN_DIRECTION_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                     (OFPXMT_OFX_WLAN_DIRECTION << 9) | (OXM_EXP_ID_SIZE + 2) |
                                     HAS_MASK_FLAG,
  OXM_TLV_EXPR_WLAN_ADDRESS_1 =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_1 << 9) | (OXM_EXP_ID_SIZE + 6),
  OXM_TLV_EXPR_WLAN_ADDRESS_1_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                     (OFPXMT_OFX_WLAN_ADDRESS_1 << 9) | (OXM_EXP_ID_SIZE + 12) |
                                     HAS_MASK_FLAG,
  OXM_TLV_EXPR_WLAN_ADDRESS_2 =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_2 << 9) | (OXM_EXP_ID_SIZE + 6),
  OXM_TLV_EXPR_WLAN_ADDRESS_2_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                     (OFPXMT_OFX_WLAN_ADDRESS_2 << 9) | (OXM_EXP_ID_SIZE + 12) |
                                     HAS_MASK_FLAG,
  OXM_TLV_EXPR_WLAN_ADDRESS_3 =
      (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_3 << 9) | (OXM_EXP_ID_SIZE + 6),
  OXM_TLV_EXPR_WLAN_ADDRESS_3_MASK = (OFPXMC_EXPERIMENTER << 16) |
                                     (OFPXMT_OFX_WLAN_ADDRESS_3 << 9) | (OXM_EXP_ID_SIZE + 12) |
                                     HAS_MASK_FLAG,
};

/** OXM_OFX_WLAN_FC
 *
 */
class coxmatch_ofx_wlan_fc : public coxmatch_exp {
public:
  coxmatch_ofx_wlan_fc(uint16_t fc)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_FC, ROFL_EXP_ID, fc){};
  coxmatch_ofx_wlan_fc(uint16_t fc, uint16_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_FC_MASK, ROFL_EXP_ID, fc, mask){};
  coxmatch_ofx_wlan_fc(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_wlan_fc(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_wlan_fc &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<wlan-fc: 0x" << std::hex << (int)oxm.get_u16value() << "/0x"
       << (int)oxm.get_u16mask() << std::dec << " >" << std::endl;
    return os;
  };
};

/** OXM_OFX_WLAN_TYPE
 *
 */
class coxmatch_ofx_wlan_type : public coxmatch_exp {
public:
  coxmatch_ofx_wlan_type(uint8_t type)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_TYPE, ROFL_EXP_ID, type){};
  coxmatch_ofx_wlan_type(uint8_t type, uint8_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_TYPE_MASK, ROFL_EXP_ID, type, mask){};
  coxmatch_ofx_wlan_type(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_wlan_type(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_wlan_type &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<wlan-type: 0x" << std::hex << (int)oxm.get_u8value() << "/0x"
       << (int)oxm.get_u8mask() << std::dec << " >" << std::endl;
    return os;
  };
};

/** OXM_OFX_WLAN_SUBTYPE
 *
 */
class coxmatch_ofx_wlan_subtype : public coxmatch_exp {
public:
  coxmatch_ofx_wlan_subtype(uint8_t subtype)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_SUBTYPE, ROFL_EXP_ID, subtype){};
  coxmatch_ofx_wlan_subtype(uint8_t subtype, uint8_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_SUBTYPE_MASK, ROFL_EXP_ID, subtype,
                     mask){};
  coxmatch_ofx_wlan_subtype(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_wlan_subtype(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_wlan_subtype &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<wlan-subtype: 0x" << std::hex << (int)oxm.get_u8value() << "/0x"
       << (int)oxm.get_u8mask() << std::dec << " >" << std::endl;
    return os;
  };
};

/** OXM_OFX_WLAN_DIRECTION
 *
 */
class coxmatch_ofx_wlan_direction : public coxmatch_exp {
public:
  coxmatch_ofx_wlan_direction(uint8_t direction)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_DIRECTION, ROFL_EXP_ID, direction){};
  coxmatch_ofx_wlan_direction(uint8_t direction, uint8_t mask)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_DIRECTION_MASK, ROFL_EXP_ID, direction,
                     mask){};
  coxmatch_ofx_wlan_direction(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_wlan_direction(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_wlan_direction &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<wlan-direction: 0x" << std::hex << (int)oxm.get_u8value() << "/0x"
       << (int)oxm.get_u8mask() << std::dec << " >" << std::endl;
    return os;
  };
};

/** OXM_OFX_WLAN_ADDRESS_1
 *
 */
class coxmatch_ofx_wlan_address_1 : public coxmatch_exp {
public:
  coxmatch_ofx_wlan_address_1(const rofl::caddress_ll &maddr)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_ADDRESS_1, ROFL_EXP_ID, maddr){};
  coxmatch_ofx_wlan_address_1(const rofl::caddress_ll &maddr,
                              const rofl::caddress_ll &mask)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_ADDRESS_1_MASK, ROFL_EXP_ID, maddr,
                     mask){};
  coxmatch_ofx_wlan_address_1(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_wlan_address_1(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_wlan_address_1 &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<wlan-address-1 value: >" << std::endl;
    { os << oxm.get_u48value(); }
    os << "<wlan-address-1 mask: >" << std::endl;
    { os << oxm.get_u48mask(); }
    return os;
  };
};

/** OXM_OFX_WLAN_ADDRESS_2
 *
 */
class coxmatch_ofx_wlan_address_2 : public coxmatch_exp {
public:
  coxmatch_ofx_wlan_address_2(const rofl::caddress_ll &maddr)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_ADDRESS_2, ROFL_EXP_ID, maddr){};
  coxmatch_ofx_wlan_address_2(const rofl::caddress_ll &maddr,
                              const rofl::caddress_ll &mask)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_ADDRESS_2_MASK, ROFL_EXP_ID, maddr,
                     mask){};
  coxmatch_ofx_wlan_address_2(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_wlan_address_2(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_wlan_address_2 &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<wlan-address-2 value: >" << std::endl;
    { os << oxm.get_u48value(); }
    os << "<wlan-address-2 mask: >" << std::endl;
    { os << oxm.get_u48mask(); }
    return os;
  };
};

/** OXM_OFX_WLAN_ADDRESS_3
 *
 */
class coxmatch_ofx_wlan_address_3 : public coxmatch_exp {
public:
  coxmatch_ofx_wlan_address_3(const rofl::caddress_ll &maddr)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_ADDRESS_3, ROFL_EXP_ID, maddr){};
  coxmatch_ofx_wlan_address_3(const rofl::caddress_ll &maddr,
                              const rofl::caddress_ll &mask)
      : coxmatch_exp(OXM_TLV_EXPR_WLAN_ADDRESS_3_MASK, ROFL_EXP_ID, maddr,
                     mask){};
  coxmatch_ofx_wlan_address_3(const coxmatch_exp &oxm) : coxmatch_exp(oxm){};
  virtual ~coxmatch_ofx_wlan_address_3(){};
  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofx_wlan_address_3 &oxm) {
    os << dynamic_cast<const coxmatch_exp &>(oxm);
    os << "<wlan-address-3 value: >" << std::endl;
    { os << oxm.get_u48value(); }
    os << "<wlan-address-3 mask: >" << std::endl;
    { os << oxm.get_u48mask(); }
    return os;
  };
};

}; // end of namespace wlan
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
