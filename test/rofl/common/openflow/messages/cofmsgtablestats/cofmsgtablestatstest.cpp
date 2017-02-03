/*
 * cofmsgtablestatstest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgtablestatstest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgtablestatstest);

void cofmsgtablestatstest::setUp() {}

void cofmsgtablestatstest::tearDown() {}

void cofmsgtablestatstest::testRequest10() {
  testRequest(rofl::openflow10::OFP_VERSION,
              rofl::openflow10::OFPT_STATS_REQUEST, 0xa1a2a3a4,
              rofl::openflow10::OFPST_TABLE, 0xb1b2);
}

void cofmsgtablestatstest::testRequest12() {
  testRequest(rofl::openflow12::OFP_VERSION,
              rofl::openflow12::OFPT_STATS_REQUEST, 0xa1a2a3a4,
              rofl::openflow12::OFPST_TABLE, 0xb1b2);
}

void cofmsgtablestatstest::testRequest13() {
  testRequest(rofl::openflow13::OFP_VERSION,
              rofl::openflow13::OFPT_MULTIPART_REQUEST, 0xa1a2a3a4,
              rofl::openflow13::OFPMP_TABLE, 0xb1b2);
}

void cofmsgtablestatstest::testRequest(uint8_t version, uint8_t type,
                                       uint32_t xid, uint16_t stats_type,
                                       uint16_t stats_flags) {
  rofl::openflow::cofmsg_table_stats_request msg1(version, xid, stats_flags);
  rofl::openflow::cofmsg_table_stats_request msg2;
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

void cofmsgtablestatstest::testReply10() {
  testReply(rofl::openflow10::OFP_VERSION, rofl::openflow10::OFPT_STATS_REPLY,
            0xa1a2a3a4, rofl::openflow10::OFPST_TABLE, 0xb1b2);
}

void cofmsgtablestatstest::testReply12() {
  testReply(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPT_STATS_REPLY,
            0xa1a2a3a4, rofl::openflow12::OFPST_TABLE, 0xb1b2);
}

void cofmsgtablestatstest::testReply13() {
  testReply(rofl::openflow13::OFP_VERSION,
            rofl::openflow13::OFPT_MULTIPART_REPLY, 0xa1a2a3a4,
            rofl::openflow13::OFPMP_TABLE, 0xb1b2);
}

void cofmsgtablestatstest::testReply(uint8_t version, uint8_t type,
                                     uint32_t xid, uint16_t stats_type,
                                     uint16_t stats_flags) {
  rofl::openflow::cofmsg_table_stats_reply msg1(version, xid, stats_flags);
  rofl::openflow::cofmsg_table_stats_reply msg2;
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
    CPPUNIT_ASSERT(msg2.get_table_stats_array().get_version() == version);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgtablestatstest::testRequestParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;

  size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_request);
  size_t memlen = msglen + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_stats_request *stats =
      (struct rofl::openflow10::ofp_stats_request *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow10::OFPT_STATS_REQUEST;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow10::OFPST_TABLE);
  stats->flags = htobe16(0xb1b2);

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_table_stats_request msg;
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
    rofl::openflow::cofmsg_table_stats_request msg;
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
    rofl::openflow::cofmsg_table_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsgtablestatstest::testRequestParser12() {
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
  stats->type = htobe16(rofl::openflow12::OFPST_TABLE);
  stats->flags = htobe16(0xb1b2);

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_table_stats_request msg;
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
    rofl::openflow::cofmsg_table_stats_request msg;
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
    rofl::openflow::cofmsg_table_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsgtablestatstest::testRequestParser13() {
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
  stats->type = htobe16(rofl::openflow13::OFPMP_TABLE);
  stats->flags = htobe16(0xb1b2);

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_table_stats_request msg;
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
    rofl::openflow::cofmsg_table_stats_request msg;
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
    rofl::openflow::cofmsg_table_stats_request msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsgtablestatstest::testReplyParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint64_t lookup_count = 0x6162636465666768;
  uint32_t max_entries = 0xc1c2c3c4;

  coftablestatsarray tables(version);
  for (unsigned int i = 0; i < 4; i++) {
    tables.set_table_stats(i).set_lookup_count(lookup_count);
    tables.set_table_stats(i).set_max_entries(max_entries);
  }

  size_t msglen = sizeof(struct rofl::openflow10::ofp_stats_reply);
  size_t memlen = msglen + tables.length() + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_stats_reply *stats =
      (struct rofl::openflow10::ofp_stats_reply *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow10::OFPT_STATS_REPLY;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow10::OFPST_TABLE);
  stats->flags = htobe16(0xb1b2);
  tables.pack(stats->body, tables.length());

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_table_stats_reply msg;
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
    rofl::openflow::cofmsg_table_stats_reply msg;
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
    rofl::openflow::cofmsg_table_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsgtablestatstest::testReplyParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;
  uint64_t match = 0x4142434445464748;
  uint64_t lookup_count = 0x6162636465666768;
  uint32_t max_entries = 0xc1c2c3c4;

  coftablestatsarray tables(version);
  for (unsigned int i = 0; i < 4; i++) {
    tables.set_table_stats(i).set_match(match);
    tables.set_table_stats(i).set_lookup_count(lookup_count);
    tables.set_table_stats(i).set_max_entries(max_entries);
  }

  size_t msglen = sizeof(struct rofl::openflow12::ofp_stats_reply);
  size_t memlen = msglen + tables.length() + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_stats_reply *stats =
      (struct rofl::openflow12::ofp_stats_reply *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow12::OFPT_STATS_REPLY;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow12::OFPST_TABLE);
  stats->flags = htobe16(0xb1b2);
  tables.pack(stats->body, tables.length());

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_table_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(i < msglen);
    }
  }

  for (unsigned int i = msglen; i == msglen; i++) {
    rofl::openflow::cofmsg_table_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_table_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}

void cofmsgtablestatstest::testReplyParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;
  uint64_t lookup_count = 0x6162636465666768;
  uint32_t active_count = 0xc1c2c3c4;

  coftablestatsarray tables(version);
  for (unsigned int i = 0; i < 4; i++) {
    tables.set_table_stats(i).set_lookup_count(lookup_count);
    tables.set_table_stats(i).set_active_count(active_count);
  }

  size_t msglen = sizeof(struct rofl::openflow13::ofp_multipart_reply);
  size_t memlen = msglen + tables.length() + /*test overhead*/ 4;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_multipart_reply *stats =
      (struct rofl::openflow13::ofp_multipart_reply *)(mem.somem());

  stats->header.version = version;
  stats->header.type = rofl::openflow13::OFPT_MULTIPART_REPLY;
  stats->header.xid = htobe32(0xa0a1a2a3);
  stats->header.length = htobe16(0);
  stats->type = htobe16(rofl::openflow13::OFPMP_TABLE);
  stats->flags = htobe16(0xb1b2);
  tables.pack(stats->body, tables.length());

  for (unsigned int i = 1; i < msglen; i++) {
    rofl::openflow::cofmsg_table_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(i < msglen);
    }
  }

  for (unsigned int i = msglen; i == msglen; i++) {
    rofl::openflow::cofmsg_table_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_table_stats_reply msg;
    try {
      stats->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {
      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> reply <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      CPPUNIT_ASSERT(false);
    }
  }
}
