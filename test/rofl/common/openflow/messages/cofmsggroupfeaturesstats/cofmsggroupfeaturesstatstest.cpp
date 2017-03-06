/*
 * cofmsggroupfeaturesstatstest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsggroupfeaturesstatstest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsggroupfeaturesstatstest);

void cofmsggroupfeaturesstatstest::setUp() {}

void cofmsggroupfeaturesstatstest::tearDown() {}

void cofmsggroupfeaturesstatstest::testRequest12() {
  testRequest(rofl::openflow12::OFP_VERSION,
              rofl::openflow12::OFPT_STATS_REQUEST, 0xa1a2a3a4,
              rofl::openflow12::OFPST_GROUP_FEATURES, 0xb1b2);
}

void cofmsggroupfeaturesstatstest::testRequest13() {
  testRequest(rofl::openflow13::OFP_VERSION,
              rofl::openflow13::OFPT_MULTIPART_REQUEST, 0xa1a2a3a4,
              rofl::openflow13::OFPMP_GROUP_FEATURES, 0xb1b2);
}

void cofmsggroupfeaturesstatstest::testRequest(uint8_t version, uint8_t type,
                                               uint32_t xid,
                                               uint16_t stats_type,
                                               uint16_t stats_flags) {
  rofl::openflow::cofmsg_group_features_stats_request msg1(version, xid,
                                                           stats_flags);
  rofl::openflow::cofmsg_group_features_stats_request msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(msg2.get_stats_type() == stats_type);
    CPPUNIT_ASSERT(msg2.get_stats_flags() == stats_flags);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsggroupfeaturesstatstest::testReply12() {
  testReply(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPT_STATS_REPLY,
            0xa1a2a3a4, rofl::openflow12::OFPST_GROUP_FEATURES, 0xb1b2);
}

void cofmsggroupfeaturesstatstest::testReply13() {
  testReply(rofl::openflow13::OFP_VERSION,
            rofl::openflow13::OFPT_MULTIPART_REPLY, 0xa1a2a3a4,
            rofl::openflow13::OFPMP_GROUP_FEATURES, 0xb1b2);
}

void cofmsggroupfeaturesstatstest::testReply(uint8_t version, uint8_t type,
                                             uint32_t xid, uint16_t stats_type,
                                             uint16_t stats_flags) {
  rofl::openflow::cofmsg_group_features_stats_reply msg1(version, xid,
                                                         stats_flags);
  rofl::openflow::cofmsg_group_features_stats_reply msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(msg2.get_stats_type() == stats_type);
    CPPUNIT_ASSERT(msg2.get_stats_flags() == stats_flags);
    CPPUNIT_ASSERT(msg2.get_group_features_stats().get_version() == version);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsggroupfeaturesstatstest::testRequestParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;

  size_t msglen = sizeof(struct rofl::openflow12::ofp_stats_request);
  size_t memlen = msglen + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_stats_request *stats =
      (struct rofl::openflow12::ofp_stats_request *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow12::OFPT_STATS_REQUEST;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow12::OFPST_GROUP_FEATURES);
  stats->flags = htobe16(0xb1b2);

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(i < msglen);
    }
  }

  for (unsigned int i = msglen; i == msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_group_features_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsggroupfeaturesstatstest::testRequestParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;

  size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_request);
  size_t memlen = msglen + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_multipart_request *stats =
      (struct rofl::openflow13::ofp_multipart_request *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow13::OFPT_MULTIPART_REQUEST;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow13::OFPMP_GROUP_FEATURES);
  stats->flags = htobe16(0xb1b2);

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(i < msglen);
    }
  }

  for (unsigned int i = msglen; i == msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_group_features_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsggroupfeaturesstatstest::testReplyParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;

  cofgroup_features_stats_reply groupfeatures(version);

  size_t msglen =
      sizeof(struct rofl::openflow12::ofp_stats_reply) + groupfeatures.length();
  size_t memlen = msglen + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_stats_reply *stats =
      (struct rofl::openflow12::ofp_stats_reply *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow12::OFPT_STATS_REPLY;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow12::OFPST_GROUP_FEATURES);
  stats->flags = htobe16(0xb1b2);
  groupfeatures.pack(stats->body, groupfeatures.length());

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(i < msglen);
    }
  }

  for (unsigned int i = msglen; i == msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_group_features_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsggroupfeaturesstatstest::testReplyParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;

  cofgroup_features_stats_reply groupfeatures(version);

  size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_reply) +
                  groupfeatures.length();
  size_t memlen = msglen + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_multipart_reply *stats =
      (struct rofl::openflow13::ofp_multipart_reply *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow13::OFPT_MULTIPART_REPLY;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow13::OFPMP_GROUP_FEATURES);
  stats->flags = htobe16(0xb1b2);
  groupfeatures.pack(stats->body, groupfeatures.length());

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(i < msglen);
    }
  }

  for (unsigned int i = msglen; i == msglen; i++) {
    rofl::openflow::cofmsg_group_features_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_group_features_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}
