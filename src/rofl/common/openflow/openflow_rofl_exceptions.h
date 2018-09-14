/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OPENFLOW_ROFL_EXCEPTIONS_H
#define OPENFLOW_ROFL_EXCEPTIONS_H 1

#include <assert.h>
#include <iostream>

#include "rofl/common/exception.hpp"

namespace rofl {

class eBadVersion : public rofl::exception {
public:
  eBadVersion(const std::string &__arg = std::string("eBadVersion"),
              const std::string &__file = std::string(""),
              const std::string &__func = std::string(""), int __line = 0)
      : rofl::exception(__arg, __file, __func, __line){};
};

class eOpenFlowBase : public rofl::exception {
public:
  eOpenFlowBase(const std::string &__arg = std::string("eOpenFlowBase"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : rofl::exception(__arg, __file, __func, __line){};
};

/* exceptions for hello according to OpenFlow */
class eHelloBase : public eOpenFlowBase {
public:
  eHelloBase(const std::string &__arg = std::string("eHelloBase"),
             const std::string &__file = std::string(""),
             const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eHelloIncompatible : public eHelloBase { // OFPHFC_INCOMPATIBLE
public:
  eHelloIncompatible(
      const std::string &__arg = std::string("eHelloIncompatible"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eHelloBase(__arg, __file, __func, __line){};
};
class eHelloEperm : public eHelloBase { // OFPHFC_EPERM
public:
  eHelloEperm(const std::string &__arg = std::string("eHelloEperm"),
              const std::string &__file = std::string(""),
              const std::string &__func = std::string(""), int __line = 0)
      : eHelloBase(__arg, __file, __func, __line){};
};

/* exceptions for requests according to OpenFlow */
class eBadRequestBase : public eOpenFlowBase {
public:
  eBadRequestBase(const std::string &__arg = std::string("eBadRequestBase"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eBadRequestBadVersion : public eBadRequestBase { // OFPBRC_BAD_VERSION
public:
  eBadRequestBadVersion(
      const std::string &__arg = std::string("eBadRequestBadVersion"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadType : public eBadRequestBase { // OFPBRC_BAD_TYPE
public:
  eBadRequestBadType(
      const std::string &__arg = std::string("eBadRequestBadType"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadStat : public eBadRequestBase { // OFPBRC_BAD_STAT
public:
  eBadRequestBadStat(
      const std::string &__arg = std::string("eBadRequestBadStat"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadExperimenter
    : public eBadRequestBase { // OFPBRC_BAD_EXPERIMENTER
public:
  eBadRequestBadExperimenter(
      const std::string &__arg = std::string("eBadRequestBadExperimenter"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadExpType : public eBadRequestBase { // OFPBRC_BAD_EXP_TYPE
public:
  eBadRequestBadExpType(
      const std::string &__arg = std::string("eBadRequestBadExpType"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestEperm : public eBadRequestBase { // OFPBRC_EPERM
public:
  eBadRequestEperm(const std::string &__arg = std::string("eBadRequestEperm"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadLen : public eBadRequestBase { // OFPBRC_BAD_LEN
public:
  eBadRequestBadLen(const std::string &__arg = std::string("eBadRequestBadLen"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBufferEmpty : public eBadRequestBase { // OFPBRC_BUFFER_EMPTY
public:
  eBadRequestBufferEmpty(
      const std::string &__arg = std::string("eBadRequestBufferEmpty"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBufferUnknown
    : public eBadRequestBase { // OFPBRC_BUFFER_UNKNOWN
public:
  eBadRequestBufferUnknown(
      const std::string &__arg = std::string("eBadRequestBufferUnknown"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadTableId : public eBadRequestBase { // OFPBRC_BAD_TABLE_ID
public:
  eBadRequestBadTableId(
      const std::string &__arg = std::string("eBadRequestBadTableId"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestIsSlave : public eBadRequestBase { // OFPBRC_IS_SLAVE
public:
  eBadRequestIsSlave(
      const std::string &__arg = std::string("eBadRequestIsSlave"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadPort : public eBadRequestBase { // OFPBRC_BAD_PORT
public:
  eBadRequestBadPort(
      const std::string &__arg = std::string("eBadRequestBadPort"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestBadPacket : public eBadRequestBase { // OFPBRC_BAD_PACKET
public:
  eBadRequestBadPacket(
      const std::string &__arg = std::string("eBadRequestBadPacket"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};
class eBadRequestMultipartBufferOverflow
    : public eBadRequestBase { // OFPBRC_MULTIPART_BUFFER_OVERFLOW
public:
  eBadRequestMultipartBufferOverflow(
      const std::string &__arg =
          std::string("eBadRequestMultipartBufferOverflow"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadRequestBase(__arg, __file, __func, __line){};
};

/* exceptions for action-containing requests according to OpenFlow */
class eBadActionBase : public eOpenFlowBase {
public:
  eBadActionBase(const std::string &__arg = std::string("eBadActionBase"),
                 const std::string &__file = std::string(""),
                 const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eBadActionBadType : public eBadActionBase { // OFPBAC_BAD_TYPE
public:
  eBadActionBadType(const std::string &__arg = std::string("eBadActionBadType"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadLen : public eBadActionBase { // OFPBAC_BAD_LEN
public:
  eBadActionBadLen(const std::string &__arg = std::string("eBadActionBadLen"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadExperimenter
    : public eBadActionBase { // OFPBAC_BAD_EXPERIMENTER
public:
  eBadActionBadExperimenter(
      const std::string &__arg = std::string("eBadActionBadExperimenter"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadExperimenterType
    : public eBadActionBase { // OFPBAC_BAD_EXPERIMENTER_TYPE
public:
  eBadActionBadExperimenterType(
      const std::string &__arg = std::string("eBadActionBadExperimenterType"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadOutPort : public eBadActionBase { // OFPBAC_BAD_OUT_PORT
public:
  eBadActionBadOutPort(
      const std::string &__arg = std::string("eBadActionBadOutPort"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadArgument : public eBadActionBase { // OFPBAC_BAD_ARGUMENT
public:
  eBadActionBadArgument(
      const std::string &__arg = std::string("eBadActionBadArgument"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionEperm : public eBadActionBase { // OFPBAC_EPERM
public:
  eBadActionEperm(const std::string &__arg = std::string("eBadActionEperm"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionTooMany : public eBadActionBase { // OFPBAC_TOO_MANY
public:
  eBadActionTooMany(const std::string &__arg = std::string("eBadActionTooMany"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadQueue : public eBadActionBase { // OFPBAC_BAD_QUEUE
public:
  eBadActionBadQueue(
      const std::string &__arg = std::string("eBadActionBadQueue"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadOutGroup : public eBadActionBase { // OFPBAC_BAD_OUT_GROUP
public:
  eBadActionBadOutGroup(
      const std::string &__arg = std::string("eBadActionBadOutGroup"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionMatchInconsistent
    : public eBadActionBase { // OFPBAC_MATCH_INCONSISTENT
public:
  eBadActionMatchInconsistent(
      const std::string &__arg = std::string("eBadActionMatchInconsistent"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionUnsupportedOrder
    : public eBadActionBase { // OFPBAC_UNSUPPORTED_ORDER
public:
  eBadActionUnsupportedOrder(
      const std::string &__arg = std::string("eBadActionUnsupportedOrder"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadTag : public eBadActionBase { // OFPBAC_BAD_TAG
public:
  eBadActionBadTag(const std::string &__arg = std::string("eBadActionBadTag"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadSetType : public eBadActionBase { // OFPBAC_BAD_SET_TYPE
public:
  eBadActionBadSetType(
      const std::string &__arg = std::string("eBadActionBadSetType"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadSetLen : public eBadActionBase { // OFPBAC_BAD_SET_LEN
public:
  eBadActionBadSetLen(
      const std::string &__arg = std::string("eBadActionBadSetLen"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};
class eBadActionBadSetArgument
    : public eBadActionBase { // OFPBAC_BAD_SET_ARGUMENT
public:
  eBadActionBadSetArgument(
      const std::string &__arg = std::string("eBadActionBadSetArgument"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadActionBase(__arg, __file, __func, __line){};
};

/* exceptions for action-containing requests according to OpenFlow */
class eBadInstBase : public eOpenFlowBase {
public:
  eBadInstBase(const std::string &__arg = std::string("eBadInstBase"),
               const std::string &__file = std::string(""),
               const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eBadInstUnknownInst : public eBadInstBase { // OFPBIC_UNKNOWN_INST
public:
  eBadInstUnknownInst(
      const std::string &__arg = std::string("eBadInstUnknownInst"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstUnsupInst : public eBadInstBase { // OFPBIC_UNSUP_INST
public:
  eBadInstUnsupInst(const std::string &__arg = std::string("eBadInstUnsupInst"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstBadTableId : public eBadInstBase { // OFPBIC_BAD_TABLE_ID
public:
  eBadInstBadTableId(
      const std::string &__arg = std::string("eBadInstBadTableId"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstUnsupMetadata : public eBadInstBase { // OFPBIC_UNSUP_METADATA
public:
  eBadInstUnsupMetadata(
      const std::string &__arg = std::string("eBadInstUnsupMetadata"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstUnsupMetadataMask
    : public eBadInstBase { // OFPBIC_UNSUP_METADATA_MASK
public:
  eBadInstUnsupMetadataMask(
      const std::string &__arg = std::string("eBadInstUnsupMetadataMask"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstBadExperimenter : public eBadInstBase { // OFPBIC_BAD_EXPERIMENTER
public:
  eBadInstBadExperimenter(
      const std::string &__arg = std::string("eBadInstBadExperimenter"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstBadExpType : public eBadInstBase { // OFPBIC_BAD_EXP_TYPE
public:
  eBadInstBadExpType(
      const std::string &__arg = std::string("eBadInstBadExpType"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstBadLen : public eBadInstBase { // OFPBIC_BAD_LEN
public:
  eBadInstBadLen(const std::string &__arg = std::string("eBadInstBadLen"),
                 const std::string &__file = std::string(""),
                 const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};
class eBadInstEperm : public eBadInstBase { // OFPBIC_EPERM
public:
  eBadInstEperm(const std::string &__arg = std::string("eBadInstEperm"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eBadInstBase(__arg, __file, __func, __line){};
};

/* exceptions for action-containing requests according to OpenFlow */
class eBadMatchBase : public eOpenFlowBase {
public:
  eBadMatchBase(const std::string &__arg = std::string("eBadMatchBase"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eBadMatchBadType : public eBadMatchBase { // OFPBMC_BAD_TYPE
public:
  eBadMatchBadType(const std::string &__arg = std::string("eBadMatchBadType"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadLen : public eBadMatchBase { // OFPBMC_BAD_LEN
public:
  eBadMatchBadLen(const std::string &__arg = std::string("eBadMatchBadLen"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadTag : public eBadMatchBase { // OFPBMC_BAD_TAG
public:
  eBadMatchBadTag(const std::string &__arg = std::string("eBadMatchBadTag"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadDlAddrMask : public eBadMatchBase { // OFPBMC_BAD_DL_ADDR_MASK
public:
  eBadMatchBadDlAddrMask(
      const std::string &__arg = std::string("eBadMatchBadDlAddrMask"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadNwAddrMask : public eBadMatchBase { // OFPBMC_BAD_NW_ADDR_MASK
public:
  eBadMatchBadNwAddrMask(
      const std::string &__arg = std::string("eBadMatchBadNwAddrMask"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadWildcards : public eBadMatchBase { // OFPBMC_BAD_WILDCARDS
public:
  eBadMatchBadWildcards(
      const std::string &__arg = std::string("eBadMatchBadWildcards"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadField : public eBadMatchBase { // OFPBMC_BAD_FIELD
public:
  eBadMatchBadField(const std::string &__arg = std::string("eBadMatchBadField"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadValue : public eBadMatchBase { // OFPBMC_BAD_VALUE
public:
  eBadMatchBadValue(const std::string &__arg = std::string("eBadMatchBadValue"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadMask : public eBadMatchBase { // OFPBMC_BAD_MASK
public:
  eBadMatchBadMask(const std::string &__arg = std::string("eBadMatchBadMask"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchBadPrereq : public eBadMatchBase { // OFPBMC_BAD_PREREQ
public:
  eBadMatchBadPrereq(
      const std::string &__arg = std::string("eBadMatchBadPrereq"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchDupField : public eBadMatchBase { // OFPBMC_DUP_FIELD
public:
  eBadMatchDupField(const std::string &__arg = std::string("eBadMatchDupField"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};
class eBadMatchEperm : public eBadMatchBase { // OFPBMC_EPERM
public:
  eBadMatchEperm(const std::string &__arg = std::string("eBadMatchEperm"),
                 const std::string &__file = std::string(""),
                 const std::string &__func = std::string(""), int __line = 0)
      : eBadMatchBase(__arg, __file, __func, __line){};
};

/* exceptions for flow-mod tables and flow-mod entries according to OpenFlow */
class eFlowModBase : public eOpenFlowBase {
public:
  eFlowModBase(const std::string &__arg = std::string("eFlowModBase"),
               const std::string &__file = std::string(""),
               const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eFlowModUnknown : public eFlowModBase { // OFPFMFC_UNKNOWN
public:
  eFlowModUnknown(const std::string &__arg = std::string("eFlowModUnknown"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};
class eFlowModTableFull : public eFlowModBase { // OFPFMFC_TABLE_FULL
public:
  eFlowModTableFull(const std::string &__arg = std::string("eFlowModTableFull"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};
class eFlowModBadTableId : public eFlowModBase { // OFPFMFC_BAD_TABLE_ID
public:
  eFlowModBadTableId(
      const std::string &__arg = std::string("eFlowModBadTableId"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};
class eFlowModOverlap : public eFlowModBase { // OFPFMFC_OVERLAP
public:
  eFlowModOverlap(const std::string &__arg = std::string("eFlowModOverlap"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};
class eFlowModEperm : public eFlowModBase { // OFPFMFC_EPERM
public:
  eFlowModEperm(const std::string &__arg = std::string("eFlowModEperm"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};
class eFlowModBadTimeout : public eFlowModBase { // OFPFMFC_BAD_TIMEOUT
public:
  eFlowModBadTimeout(
      const std::string &__arg = std::string("eFlowModBadTimeout"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};
class eFlowModBadCommand : public eFlowModBase { // OFPFMFC_BAD_COMMAND
public:
  eFlowModBadCommand(
      const std::string &__arg = std::string("eFlowModBadCommand"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};
class eFlowModBadFlags : public eFlowModBase { // OFPFMFC_BAD_FLAGS
public:
  eFlowModBadFlags(const std::string &__arg = std::string("eFlowModBadFlags"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eFlowModBase(__arg, __file, __func, __line){};
};

/* exceptions for group table and group entries according to OpenFlow */
class eGroupModBase : public eOpenFlowBase {
public:
  eGroupModBase(const std::string &__arg = std::string("eGroupModBase"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eGroupModExists : public eGroupModBase { // OFPGMFC_GROUP_EXISTS
public:
  eGroupModExists(const std::string &__arg = std::string("eGroupModExists"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModInvalGroup : public eGroupModBase { // OFPGMFC_INVALID_GROUP
public:
  eGroupModInvalGroup(
      const std::string &__arg = std::string("eGroupModInvalGroup"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModWeightUnsupported
    : public eGroupModBase { // OFPGMFC_WEIGHT_UNSUPPORTED
public:
  eGroupModWeightUnsupported(
      const std::string &__arg = std::string("eGroupModWeightUnsupported"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModOutOfGroups : public eGroupModBase { // OFPGMFC_OUT_OF_GROUPS
public:
  eGroupModOutOfGroups(
      const std::string &__arg = std::string("eGroupModOutOfGroups"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModOutOfBuckets : public eGroupModBase { // OFPGMFC_OUT_OF_BUCKETS
public:
  eGroupModOutOfBuckets(
      const std::string &__arg = std::string("eGroupModOutOfBuckets"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModChainingUnsupported
    : public eGroupModBase { // OFPGMFC_CHAINING_UNSUPPORTED
public:
  eGroupModChainingUnsupported(
      const std::string &__arg = std::string("eGroupModChainingUnsupported"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModWatchUnsupported
    : public eGroupModBase { // OFPGMFC_WATCH_UNSUPPORTED
public:
  eGroupModWatchUnsupported(
      const std::string &__arg = std::string("eGroupModWatchUnsupported"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModLoop : public eGroupModBase { // OFPGMFC_LOOP
public:
  eGroupModLoop(const std::string &__arg = std::string("eGroupModLoop"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModUnknownGroup : public eGroupModBase { // OFPGMFC_UNKNOWN_GROUP
public:
  eGroupModUnknownGroup(
      const std::string &__arg = std::string("eGroupModUnknownGroup"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModChainedGroup : public eGroupModBase { // OFPGMFC_CHAINED_GROUP
public:
  eGroupModChainedGroup(
      const std::string &__arg = std::string("eGroupModChainedGroup"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModBadType : public eGroupModBase { // OFPGMFC_BAD_TYPE
public:
  eGroupModBadType(const std::string &__arg = std::string("eGroupModBadType"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModBadCommand : public eGroupModBase { // OFPGMFC_BAD_COMMAND
public:
  eGroupModBadCommand(
      const std::string &__arg = std::string("eGroupModBadType"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModBadBucket : public eGroupModBase { // OFPGMFC_BAD_BUCKET
public:
  eGroupModBadBucket(
      const std::string &__arg = std::string("eGroupModBadBucket"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModBadWatch : public eGroupModBase { // OFPGMFC_BAD_WATCH
public:
  eGroupModBadWatch(const std::string &__arg = std::string("eGroupModBadWatch"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};
class eGroupModEperm : public eGroupModBase { // OFPGMFC_EPERM
public:
  eGroupModEperm(const std::string &__arg = std::string("eGroupModEperm"),
                 const std::string &__file = std::string(""),
                 const std::string &__func = std::string(""), int __line = 0)
      : eGroupModBase(__arg, __file, __func, __line){};
};

/* exceptions for port-mod commands according to OpenFlow */
class ePortModBase : public eOpenFlowBase {
public:
  ePortModBase(const std::string &__arg = std::string("ePortModBase"),
               const std::string &__file = std::string(""),
               const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class ePortModBadPort : public ePortModBase { // OFPPMFC_BAD_PORT
public:
  ePortModBadPort(const std::string &__arg = std::string("ePortModBadPort"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : ePortModBase(__arg, __file, __func, __line){};
};
class ePortModBadHwAddr : public ePortModBase { // OFPPMFC_BAD_HW_ADDR
public:
  ePortModBadHwAddr(const std::string &__arg = std::string("ePortModBadHwAddr"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : ePortModBase(__arg, __file, __func, __line){};
};
class ePortModBadConfig : public ePortModBase { // OFPPMFC_BAD_CONFIG
public:
  ePortModBadConfig(const std::string &__arg = std::string("ePortModBadConfig"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : ePortModBase(__arg, __file, __func, __line){};
};
class ePortModBadAdvertise : public ePortModBase { // OFPPMFC_BAD_ADVERTISE
public:
  ePortModBadAdvertise(
      const std::string &__arg = std::string("ePortModBadAdvertise"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : ePortModBase(__arg, __file, __func, __line){};
};
class ePortModEperm : public ePortModBase { // OFPPMFC_EPERM
public:
  ePortModEperm(const std::string &__arg = std::string("ePortModEperm"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : ePortModBase(__arg, __file, __func, __line){};
};

/* exceptions for table-mod commands according to OpenFlow */
class eTableModBase : public eOpenFlowBase {
public:
  eTableModBase(const std::string &__arg = std::string("eTableModBase"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eTableModBadTable : public eTableModBase { // OFPTMFC_BAD_TABLE
public:
  eTableModBadTable(const std::string &__arg = std::string("eTableModBadTable"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eTableModBase(__arg, __file, __func, __line){};
};
class eTableModBadConfig : public eTableModBase { // OFPTMFC_BAD_CONFIG
public:
  eTableModBadConfig(
      const std::string &__arg = std::string("eTableModBadConfig"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eTableModBase(__arg, __file, __func, __line){};
};
class eTableModEperm : public eTableModBase { // OFPTMFC_EPERM
public:
  eTableModEperm(const std::string &__arg = std::string("eTableModEperm"),
                 const std::string &__file = std::string(""),
                 const std::string &__func = std::string(""), int __line = 0)
      : eTableModBase(__arg, __file, __func, __line){};
};

/* exceptions for queue-op commands according to OpenFlow */
class eQueueOpBase : public eOpenFlowBase {
public:
  eQueueOpBase(const std::string &__arg = std::string("eQueueOpBase"),
               const std::string &__file = std::string(""),
               const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eQueueOpBadPort : public eQueueOpBase { // OFPQOFC_BAD_PORT
public:
  eQueueOpBadPort(const std::string &__arg = std::string("eQueueOpBadPort"),
                  const std::string &__file = std::string(""),
                  const std::string &__func = std::string(""), int __line = 0)
      : eQueueOpBase(__arg, __file, __func, __line){};
};
class eQueueOpBadQueue : public eQueueOpBase { // OFPQOFC_BAD_QUEUE
public:
  eQueueOpBadQueue(const std::string &__arg = std::string("eQueueOpBadQueue"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eQueueOpBase(__arg, __file, __func, __line){};
};
class eQueueOpEperm : public eQueueOpBase { // OFPQOFC_EPERM
public:
  eQueueOpEperm(const std::string &__arg = std::string("eQueueOpEperm"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eQueueOpBase(__arg, __file, __func, __line){};
};

/* exceptions for switch-config commands according to OpenFlow */
class eSwitchConfigBase : public eOpenFlowBase {
public:
  eSwitchConfigBase(const std::string &__arg = std::string("eSwitchConfigBase"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eSwitchConfigBadFlags : public eSwitchConfigBase { // OFPSCFC_BAD_FLAGS
public:
  eSwitchConfigBadFlags(
      const std::string &__arg = std::string("eSwitchConfigBadFlags"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eSwitchConfigBase(__arg, __file, __func, __line){};
};
class eSwitchConfigBadLen : public eSwitchConfigBase { // OFPSCFC_BAD_LEN
public:
  eSwitchConfigBadLen(
      const std::string &__arg = std::string("eSwitchConfigBadLen"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eSwitchConfigBase(__arg, __file, __func, __line){};
};
class eSwitchConfigEPerm : public eSwitchConfigBase { // OFPSCFC_EPERM
public:
  eSwitchConfigEPerm(
      const std::string &__arg = std::string("eSwitchConfigEPerm"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eSwitchConfigBase(__arg, __file, __func, __line){};
};

/* exceptions for role-request commands according to OpenFlow */
class eRoleRequestBase : public eOpenFlowBase {
public:
  eRoleRequestBase(const std::string &__arg = std::string("eRoleRequestBase"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eRoleRequestStale : public eRoleRequestBase { // OFPRRFC_STALE
public:
  eRoleRequestStale(const std::string &__arg = std::string("eRoleRequestStale"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eRoleRequestBase(__arg, __file, __func, __line){};
};
class eRoleRequestUnsupported : public eRoleRequestBase { // OFPRRFC_UNSUPPORTED
public:
  eRoleRequestUnsupported(
      const std::string &__arg = std::string("eRoleRequestUnsupported"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eRoleRequestBase(__arg, __file, __func, __line){};
};
class eRoleRequestBadRole : public eRoleRequestBase { // OFPRRFC_BAD_ROLE
public:
  eRoleRequestBadRole(
      const std::string &__arg = std::string("eRoleRequestBadRole"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eRoleRequestBase(__arg, __file, __func, __line){};
};

/* exceptions for meter-mod commands according to OpenFlow */
class eMeterModBase : public eOpenFlowBase {
public:
  eMeterModBase(const std::string &__arg = std::string("eMeterModBase"),
                const std::string &__file = std::string(""),
                const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eMeterModUnknown : public eMeterModBase { // OFPMMFC_UNKNOWN
public:
  eMeterModUnknown(const std::string &__arg = std::string("eMeterModUnknown"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModMeterExists : public eMeterModBase { // OFPMMFC_METER_EXISTS
public:
  eMeterModMeterExists(
      const std::string &__arg = std::string("eMeterModMeterExists"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModInvalidMeter : public eMeterModBase { // OFPMMFC_INVALID_METER
public:
  eMeterModInvalidMeter(
      const std::string &__arg = std::string("eMeterModInvalidMeter"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModUnknownMeter : public eMeterModBase { // OFPMMFC_UNKNOWN_METER
public:
  eMeterModUnknownMeter(
      const std::string &__arg = std::string("eMeterModUnknownMeter"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModBadCommand : public eMeterModBase { // OFPMMFC_BAD_COMMAND
public:
  eMeterModBadCommand(
      const std::string &__arg = std::string("eMeterModBadCommand"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModBadFlags : public eMeterModBase { // OFPMMFC_BAD_FLAGS
public:
  eMeterModBadFlags(const std::string &__arg = std::string("eMeterModBadFlags"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModBadRate : public eMeterModBase { // OFPMMFC_BAD_RATE
public:
  eMeterModBadRate(const std::string &__arg = std::string("eMeterModBadRate"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModBadBurst : public eMeterModBase { // OFPMMFC_BAD_BURST
public:
  eMeterModBadBurst(const std::string &__arg = std::string("eMeterModBadBurst"),
                    const std::string &__file = std::string(""),
                    const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModBadBand : public eMeterModBase { // OFPMMFC_BAD_BAND
public:
  eMeterModBadBand(const std::string &__arg = std::string("eMeterModBadBand"),
                   const std::string &__file = std::string(""),
                   const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModBadBandValue : public eMeterModBase { // OFPMMFC_BAD_BAND_VALUE
public:
  eMeterModBadBandValue(
      const std::string &__arg = std::string("eMeterModBadBandValue"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModOutOfMeters : public eMeterModBase { // OFPMMFC_OUT_OF_METERS
public:
  eMeterModOutOfMeters(
      const std::string &__arg = std::string("eMeterModOutOfMeters"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};
class eMeterModOutOfBands : public eMeterModBase { // OFPMMFC_OUT_OF_BANDS
public:
  eMeterModOutOfBands(
      const std::string &__arg = std::string("eMeterModOutOfBands"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eMeterModBase(__arg, __file, __func, __line){};
};

/* exceptions for table-features commands according to OpenFlow */
class eTableFeaturesReqBase : public eOpenFlowBase {
public:
  eTableFeaturesReqBase(
      const std::string &__arg = std::string("eTableFeaturesReqBase"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eOpenFlowBase(__arg, __file, __func, __line){};
};
class eTableFeaturesReqBadTable
    : public eTableFeaturesReqBase { // OFPTFFC_BAD_TABLE
public:
  eTableFeaturesReqBadTable(
      const std::string &__arg = std::string("eTableFeaturesReqBadTable"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eTableFeaturesReqBase(__arg, __file, __func, __line){};
};
class eTableFeaturesReqBadMetaData
    : public eTableFeaturesReqBase { // OFPTFFC_BAD_METADATA
public:
  eTableFeaturesReqBadMetaData(
      const std::string &__arg = std::string("eTableFeaturesReqBadMetaData"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eTableFeaturesReqBase(__arg, __file, __func, __line){};
};
class eTableFeaturesReqBadType
    : public eTableFeaturesReqBase { // OFPTFFC_BAD_TYPE
public:
  eTableFeaturesReqBadType(
      const std::string &__arg = std::string("eTableFeaturesReqBadType"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eTableFeaturesReqBase(__arg, __file, __func, __line){};
};
class eTableFeaturesReqBadLen
    : public eTableFeaturesReqBase { // OFPTFFC_BAD_LEN
public:
  eTableFeaturesReqBadLen(
      const std::string &__arg = std::string("eTableFeaturesReqBadLen"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eTableFeaturesReqBase(__arg, __file, __func, __line){};
};
class eTableFeaturesReqBadArgument
    : public eTableFeaturesReqBase { // OFPTFFC_BAD_ARGUMENT
public:
  eTableFeaturesReqBadArgument(
      const std::string &__arg = std::string("eTableFeaturesReqBadArgument"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eTableFeaturesReqBase(__arg, __file, __func, __line){};
};
class eTableFeaturesReqEperm : public eTableFeaturesReqBase { // OFPTFFC_EPERM
public:
  eTableFeaturesReqEperm(
      const std::string &__arg = std::string("eTableFeaturesReqEperm"),
      const std::string &__file = std::string(""),
      const std::string &__func = std::string(""), int __line = 0)
      : eTableFeaturesReqBase(__arg, __file, __func, __line){};
};

}; // namespace rofl

#endif
