#ifndef CAPWAP_MATCHES_H
#define CAPWAP_MATCHES_H 

#include "../../coxmatch.h"

namespace rofl {
namespace openflow {
namespace experimental {
namespace capwap {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
	/* CAPWAP related extensions */
	OFPXMT_OFX_CAPWAP_WBID			= 27,
	OFPXMT_OFX_CAPWAP_RID			= 28,
	OFPXMT_OFX_CAPWAP_FLAGS			= 29,
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
	OXM_TLV_EXPR_CAPWAP_WBID		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_CAPWAP_WBID  << 9) |  1,
	OXM_TLV_EXPR_CAPWAP_WBID_MASK	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_CAPWAP_WBID  << 9) |  2 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_CAPWAP_RID			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_CAPWAP_RID   << 9) |  1,
	OXM_TLV_EXPR_CAPWAP_RID_MASK	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_CAPWAP_RID   << 9) |  2 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_CAPWAP_FLAGS		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_CAPWAP_FLAGS << 9) |  2,
	OXM_TLV_EXPR_CAPWAP_FLAGS_MASK	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_CAPWAP_FLAGS << 9) |  4 | HAS_MASK_FLAG,
};



/** OXM_OFX_CAPWAP_WBID
 *
 */
class coxmatch_ofx_capwap_wbid : public coxmatch_exp {
public:
	coxmatch_ofx_capwap_wbid(
			uint8_t wbid) :
				coxmatch_exp(OXM_TLV_EXPR_CAPWAP_WBID, ROFL_EXP_ID, wbid)
	{};
	coxmatch_ofx_capwap_wbid(
			uint8_t wbid, uint8_t mask) :
				coxmatch_exp(OXM_TLV_EXPR_CAPWAP_WBID_MASK, ROFL_EXP_ID, wbid, mask)
	{};
	coxmatch_ofx_capwap_wbid(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_capwap_wbid()
	{};
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_capwap_wbid& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<capwap-wbid: 0x" << std::hex
						<< (int)oxm.get_u8value() << "/0x" << (int)oxm.get_u8mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/** OXM_OFX_CAPWAP_RID
 *
 */
class coxmatch_ofx_capwap_rid : public coxmatch_exp {
public:
	coxmatch_ofx_capwap_rid(
			uint8_t rid) :
				coxmatch_exp(OXM_TLV_EXPR_CAPWAP_RID, ROFL_EXP_ID, rid)
	{};
	coxmatch_ofx_capwap_rid(
			uint8_t rid, uint8_t mask) :
				coxmatch_exp(OXM_TLV_EXPR_CAPWAP_RID_MASK, ROFL_EXP_ID, rid, mask)
	{};
	coxmatch_ofx_capwap_rid(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_capwap_rid()
	{};
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_capwap_rid& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<capwap-rid: 0x" << std::hex
						<< (int)oxm.get_u8value() << "/0x" << (int)oxm.get_u8mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/** OXM_OFX_CAPWAP_FLAGS
 *
 */
class coxmatch_ofx_capwap_flags : public coxmatch_exp {
public:
	coxmatch_ofx_capwap_flags(
			uint16_t flags) :
				coxmatch_exp(OXM_TLV_EXPR_CAPWAP_FLAGS, ROFL_EXP_ID, flags)
	{};
	coxmatch_ofx_capwap_flags(
			uint16_t flags, uint16_t mask) :
				coxmatch_exp(OXM_TLV_EXPR_CAPWAP_FLAGS_MASK, ROFL_EXP_ID, flags, mask)
	{};
	coxmatch_ofx_capwap_flags(
			const coxmatch_exp& oxm) :
				coxmatch_exp(oxm)
	{};
	virtual
	~coxmatch_ofx_capwap_flags()
	{};
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_capwap_flags& oxm) {
		os << dynamic_cast<const coxmatch_exp&>(oxm);
		os << "<capwap-flags: 0x" << std::hex
						<< (int)oxm.get_u16value() << "/0x" << (int)oxm.get_u16mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



}; // end of namespace capwap
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
