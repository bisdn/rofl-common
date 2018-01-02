#ifndef OFDPA_MATCHES
#define OFDPA_MATCHES 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace ofdpa {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

enum openflow_exp_ids {
	EXP_ID_BCM = 0x001018,       // Broadcom
	ONF_EXP_ID_ONF = 0x4F4E4600, // ONF Extensions
};

/* OF-DPA Experimenter Match Field types */
enum ofdpa_match_exp_type {
  OFDPA_OXM_VRF = 1,
  OFDPA_OXM_TRAFFIC_CLASS = 2,
  OFDPA_OXM_COLOR = 3,
  OFDPA_OXM_VLAN_DEI = 4,
  OFDPA_OXM_QOS_INDEX = 5,
  OFDPA_OXM_LMEP_ID = 6,
  OFDPA_OXM_MPLS_TTL = 7,
  OFDPA_OXM_MPLS_L2_PORT = 8,
  OFDPA_OXM_L3_IN_PORT = 9,
  OFDPA_OXM_OVID = 10,
  OFDPA_OXM_MPLS_DATA_FIRST_NIBBLE = 11,
  OFDPA_OXM_MPLS_ACH_CHANNEL = 12,
  OFDPA_OXM_NEXT_LABEL_IS_GAL = 13,
  OFDPA_OXM_OAM_Y1731_MDL = 14,
  OFDPA_OXM_OAM_Y1731_OPCODE = 15,
  OFDPA_OXM_COLOR_ACTIONS_INDEX = 16,
  OFDPA_OXM_PROTECTION_INDEX = 21,
  OFDPA_OXM_MPLS_TYPE = 23,
  OFDPA_OXM_ALLOW_VLAN_TRANSLATION = 24,
  OFDPA_OXM_ACTSET_OUTPUT = 43,
};

/* OXM Flow match field types for OpenFlow experimenter class. */
enum oxm_tlv_match_fields {
  OXM_TLV_EXPR_VRF =
      (openflow::OFPXMC_EXPERIMENTER << 16) | (OFDPA_OXM_VRF << 9) | 2,

  OXM_TLV_EXPR_VRF_MASK =
	  (openflow::OFPXMC_EXPERIMENTER << 16) | (OFDPA_OXM_VRF << 9) | 4 | HAS_MASK_FLAG,

  OXM_TLV_EXPR_OVID =
	  (openflow::OFPXMC_EXPERIMENTER << 16) | (OFDPA_OXM_OVID << 9) | 2,

  OXM_TLV_EXPR_ALLOW_VLAN_TRANSLATION =
      (openflow::OFPXMC_EXPERIMENTER << 16) | (OFDPA_OXM_ALLOW_VLAN_TRANSLATION << 9) | 5,

  OXM_TLV_EXPR_ACTSET_OUTPUT =
	  (openflow::OFPXMC_EXPERIMENTER << 16) | (OFDPA_OXM_ACTSET_OUTPUT << 9) | 8,
};

class coxmatch_ofb_vrf : public openflow::coxmatch_exp {
public:
  coxmatch_ofb_vrf(uint16_t vrf)
      : coxmatch_exp(ofdpa::OXM_TLV_EXPR_VRF, EXP_ID_BCM, vrf) {}

  coxmatch_ofb_vrf(uint16_t vrf, uint16_t mask)
      : coxmatch_exp(ofdpa::OXM_TLV_EXPR_VRF_MASK, EXP_ID_BCM, vrf, mask) {}

  coxmatch_ofb_vrf(const coxmatch_exp &oxm) : coxmatch_exp(oxm) {}

  virtual ~coxmatch_ofb_vrf() {}

  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofb_vrf &oxm) {
    os << dynamic_cast<const coxmatch &>(oxm);
    os << "  <coxmatch_ofb_vlan_vid >" << std::endl;
    os << "    <vlan-vid: 0x" << std::hex << (int)oxm.get_u16value() << "/0x"
       << (int)oxm.get_u16mask() << std::dec << " >" << std::endl;
    return os;
  }
};

class coxmatch_ofb_allow_vlan_translation : public openflow::coxmatch_exp {
public:
  coxmatch_ofb_allow_vlan_translation(uint8_t val)
      : coxmatch_exp(ofdpa::OXM_TLV_EXPR_ALLOW_VLAN_TRANSLATION, EXP_ID_BCM,
                     val) {}

  coxmatch_ofb_allow_vlan_translation(const coxmatch_exp &oxm)
      : coxmatch_exp(oxm) {}

  virtual ~coxmatch_ofb_allow_vlan_translation() {}

  friend std::ostream &
  operator<<(std::ostream &os, const coxmatch_ofb_allow_vlan_translation &oxm) {
    os << dynamic_cast<const coxmatch &>(oxm);
    os << "  <coxmatch_ofb_allow_vlan_translation >" << std::endl;
    os << "    <value: 0x" << std::hex << (int)oxm.get_u8value() << std::dec
       << " >" << std::endl;
    return os;
  }
};

class coxmatch_ofb_actset_output : public openflow::coxmatch_exp {

  struct broadcom_t {
    uint32_t portno;
  } __attribute__((packed));

public:
  coxmatch_ofb_actset_output(uint32_t port)
      : coxmatch_exp(ofdpa::OXM_TLV_EXPR_ACTSET_OUTPUT, ONF_EXP_ID_ONF, port) {}

  coxmatch_ofb_actset_output(const coxmatch_exp &oxm) : coxmatch_exp(oxm) {}

  virtual ~coxmatch_ofb_actset_output() {}

  friend std::ostream &operator<<(std::ostream &os,
                                  const coxmatch_ofb_actset_output &oxm) {
    os << dynamic_cast<const coxmatch &>(oxm);
    os << "<coxmatch_ofb_actset_output >" << std::endl;
    os << "    <port: 0x" << std::hex << (int)oxm.get_u32value() << std::dec
       << " >" << std::endl;
    return os;
  }
};

}; // end of namespace ofdpa
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
