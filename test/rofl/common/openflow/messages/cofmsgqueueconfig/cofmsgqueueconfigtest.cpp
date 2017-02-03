/*
 * cofmsgqueueconfigtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgqueueconfigtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgqueueconfigtest);

void cofmsgqueueconfigtest::setUp() {}

void cofmsgqueueconfigtest::tearDown() {}

void cofmsgqueueconfigtest::testRequest10() {
  testRequest(rofl::openflow10::OFP_VERSION,
              rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testRequest12() {
  testRequest(rofl::openflow12::OFP_VERSION,
              rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testRequest13() {
  testRequest(rofl::openflow13::OFP_VERSION,
              rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testRequest(uint8_t version, uint8_t type,
                                        uint32_t xid) {
  uint32_t portno = 0x61626364;
  rofl::openflow::cofmsg_queue_get_config_request msg1(version, xid, portno);
  rofl::openflow::cofmsg_queue_get_config_request msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    if (rofl::openflow10::OFP_VERSION == version)
      CPPUNIT_ASSERT(msg2.get_portno() == (portno & 0x0000ffff));
    else
      CPPUNIT_ASSERT(msg2.get_portno() == portno);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgqueueconfigtest::testReply10() {
  testReply(rofl::openflow10::OFP_VERSION,
            rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testReply12() {
  testReply(rofl::openflow12::OFP_VERSION,
            rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testReply13() {
  testReply(rofl::openflow13::OFP_VERSION,
            rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REPLY, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testReply(uint8_t version, uint8_t type,
                                      uint32_t xid) {
  uint32_t portno = 0x61626364;
  rofl::openflow::cofpacket_queues queues(version);
  rofl::openflow::cofmsg_queue_get_config_reply msg1(version, xid, portno,
                                                     queues);
  rofl::openflow::cofmsg_queue_get_config_reply msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    if (rofl::openflow10::OFP_VERSION == version)
      CPPUNIT_ASSERT(msg2.get_portno() == (portno & 0x0000ffff));
    else
      CPPUNIT_ASSERT(msg2.get_portno() == portno);
    CPPUNIT_ASSERT(msg2.get_queues().get_version() == version);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgqueueconfigtest::testRequestParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint8_t type = rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST;
  uint32_t xid = 0xa1a2a3a4;
  uint16_t portno = 0x6162;
  size_t msglen = sizeof(struct rofl::openflow10::ofp_queue_get_config_request);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_queue_get_config_request *hdr =
      (struct rofl::openflow10::ofp_queue_get_config_request *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->port = htobe16(portno);

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow10::ofp_queue_get_config_request); i++) {
    rofl::openflow::cofmsg_queue_get_config_request msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(
          i < sizeof(struct rofl::openflow10::ofp_queue_get_config_request));
    }
  }

  for (unsigned int i =
           sizeof(struct rofl::openflow10::ofp_queue_get_config_request);
       i == msglen; i++) {
    rofl::openflow::cofmsg_queue_get_config_request msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_queue_get_config_request msg;
    try {
      hdr->header.length = htobe16(i);
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

void cofmsgqueueconfigtest::testRequestParser12() {
  testRequestParser(rofl::openflow12::OFP_VERSION,
                    rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST,
                    0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testRequestParser13() {
  testRequestParser(rofl::openflow13::OFP_VERSION,
                    rofl::openflow13::OFPT_QUEUE_GET_CONFIG_REQUEST,
                    0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testRequestParser(uint8_t version, uint8_t type,
                                              uint32_t xid) {
  uint32_t portno = 0x61626364;
  size_t msglen = sizeof(struct rofl::openflow13::ofp_queue_get_config_request);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_queue_get_config_request *hdr =
      (struct rofl::openflow13::ofp_queue_get_config_request *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->port = htobe32(portno);

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow13::ofp_queue_get_config_request); i++) {
    rofl::openflow::cofmsg_queue_get_config_request msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(
          i < sizeof(struct rofl::openflow13::ofp_queue_get_config_request));
    }
  }

  for (unsigned int i =
           sizeof(struct rofl::openflow13::ofp_queue_get_config_request);
       i == msglen; i++) {
    rofl::openflow::cofmsg_queue_get_config_request msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_queue_get_config_request msg;
    try {
      hdr->header.length = htobe16(i);
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

void cofmsgqueueconfigtest::testReplyParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint8_t type = rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY;
  uint32_t xid = 0xa1a2a3a4;
  uint16_t portno = 0x6162;
  rofl::openflow::cofpacket_queues queues(version);
  queues.add_pqueue(0, 0x71727374);
  size_t msglen = sizeof(struct rofl::openflow10::ofp_queue_get_config_reply) +
                  queues.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_queue_get_config_reply *hdr =
      (struct rofl::openflow10::ofp_queue_get_config_reply *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->port = htobe32(portno);

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow10::ofp_queue_get_config_reply); i++) {
    rofl::openflow::cofmsg_queue_get_config_reply msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(
          i < sizeof(struct rofl::openflow10::ofp_queue_get_config_reply));
    }
  }

  for (unsigned int i =
           sizeof(struct rofl::openflow10::ofp_queue_get_config_reply);
       i == msglen; i++) {
    rofl::openflow::cofmsg_queue_get_config_reply msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_queue_get_config_reply msg;
    try {
      hdr->header.length = htobe16(i);
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

void cofmsgqueueconfigtest::testReplyParser12() {
  testReplyParser(rofl::openflow12::OFP_VERSION,
                  rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testReplyParser13() {
  testReplyParser(rofl::openflow12::OFP_VERSION,
                  rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY, 0xa1a2a3a4);
}

void cofmsgqueueconfigtest::testReplyParser(uint8_t version, uint8_t type,
                                            uint32_t xid) {
  uint32_t portno = 0x61626364;
  rofl::openflow::cofpacket_queues queues(version);
  queues.add_pqueue(0, 0x71727374);
  size_t msglen = sizeof(struct rofl::openflow13::ofp_queue_get_config_reply) +
                  queues.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_queue_get_config_reply *hdr =
      (struct rofl::openflow13::ofp_queue_get_config_reply *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->port = htobe32(portno);

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow13::ofp_queue_get_config_reply); i++) {
    rofl::openflow::cofmsg_queue_get_config_reply msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(
          i < sizeof(struct rofl::openflow13::ofp_queue_get_config_reply));
    }
  }

  for (unsigned int i =
           sizeof(struct rofl::openflow13::ofp_queue_get_config_reply);
       i == msglen; i++) {
    rofl::openflow::cofmsg_queue_get_config_reply msg;
    try {
      hdr->header.length = htobe16(i);
      msg.unpack(mem.somem(), i);

    } catch (rofl::eBadRequestBadLen &e) {

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an exception */
      CPPUNIT_ASSERT(false);
    }
  }

  for (unsigned int i = msglen + 1; i < memlen; i++) {
    rofl::openflow::cofmsg_queue_get_config_reply msg;
    try {
      hdr->header.length = htobe16(i);
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
