/*
 * cofmsgexperimentertest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgexperimentertest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgexperimentertest);

void cofmsgexperimentertest::setUp() {}

void cofmsgexperimentertest::tearDown() {}

void cofmsgexperimentertest::testExperimenter10() {
  testExperimenter(rofl::openflow10::OFP_VERSION, rofl::openflow10::OFPT_VENDOR,
                   0xa1a2a3a4);
}

void cofmsgexperimentertest::testExperimenter12() {
  testExperimenter(rofl::openflow12::OFP_VERSION,
                   rofl::openflow12::OFPT_EXPERIMENTER, 0xa1a2a3a4);
}

void cofmsgexperimentertest::testExperimenter13() {
  testExperimenter(rofl::openflow13::OFP_VERSION,
                   rofl::openflow13::OFPT_EXPERIMENTER, 0xa1a2a3a4);
}

void cofmsgexperimentertest::testExperimenter(uint8_t version, uint8_t type,
                                              uint32_t xid) {
  uint32_t exp_id = 0xc1c2c3c4;
  uint32_t exp_type = 0xd1d2d3d4;
  rofl::cmemory body(64);
  for (unsigned int i = 0; i < body.length(); i++) {
    body[i] = i;
  }

  rofl::openflow::cofmsg_experimenter msg1(version, xid, exp_id, exp_type,
                                           body.somem(), body.length());
  rofl::openflow::cofmsg_experimenter msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(msg2.get_exp_id() == exp_id);
    if (rofl::openflow10::OFP_VERSION != version)
      CPPUNIT_ASSERT(msg2.get_exp_type() == exp_type);
    CPPUNIT_ASSERT(msg2.get_body() == body);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgexperimentertest::testExperimenterParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint8_t type = rofl::openflow10::OFPT_VENDOR;
  uint32_t xid = 0xa1a2a3a4;
  rofl::cmemory body(64);
  size_t msglen =
      sizeof(struct rofl::openflow10::ofp_vendor_header) + body.length();
  size_t memlen = 2 * msglen /*test overhead*/;
  uint32_t exp_id = 0xc1c2c3c4;

  for (unsigned int i = 0; i < body.length(); i++) {
    body[i] = i;
  }

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_vendor_header *hdr =
      (struct rofl::openflow10::ofp_vendor_header *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->vendor = htobe32(exp_id);
  body.pack(hdr->body, body.length());

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow10::ofp_vendor_header); i++) {
    rofl::openflow::cofmsg_experimenter msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_vendor_header));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow10::ofp_vendor_header);
       i == msglen; i++) {
    rofl::openflow::cofmsg_experimenter msg;
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
    rofl::openflow::cofmsg_experimenter msg;
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

void cofmsgexperimentertest::testExperimenterParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;
  uint8_t type = rofl::openflow12::OFPT_EXPERIMENTER;
  uint32_t xid = 0xa1a2a3a4;
  rofl::cmemory body(64);
  size_t msglen =
      sizeof(struct rofl::openflow12::ofp_experimenter_header) + body.length();
  size_t memlen = 2 * msglen /*test overhead*/;
  uint32_t exp_id = 0xc1c2c3c4;
  uint32_t exp_type = 0xd1d2d3d4;

  for (unsigned int i = 0; i < body.length(); i++) {
    body[i] = i;
  }

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_experimenter_header *hdr =
      (struct rofl::openflow12::ofp_experimenter_header *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->experimenter = htobe32(exp_id);
  hdr->exp_type = htobe32(exp_type);
  body.pack(hdr->body, body.length());

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow12::ofp_experimenter_header); i++) {
    rofl::openflow::cofmsg_experimenter msg;
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
      CPPUNIT_ASSERT(i <
                     sizeof(struct rofl::openflow12::ofp_experimenter_header));
    }
  }

  for (unsigned int i =
           sizeof(struct rofl::openflow12::ofp_experimenter_header);
       i == msglen; i++) {
    rofl::openflow::cofmsg_experimenter msg;
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
    rofl::openflow::cofmsg_experimenter msg;
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

void cofmsgexperimentertest::testExperimenterParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;
  uint8_t type = rofl::openflow13::OFPT_EXPERIMENTER;
  uint32_t xid = 0xa1a2a3a4;
  rofl::cmemory body(64);
  size_t msglen =
      sizeof(struct rofl::openflow13::ofp_experimenter_header) + body.length();
  size_t memlen = 2 * msglen /*test overhead*/;
  uint32_t exp_id = 0xc1c2c3c4;
  uint32_t exp_type = 0xd1d2d3d4;

  for (unsigned int i = 0; i < body.length(); i++) {
    body[i] = i;
  }

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_experimenter_header *hdr =
      (struct rofl::openflow13::ofp_experimenter_header *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->experimenter = htobe32(exp_id);
  hdr->exp_type = htobe32(exp_type);
  body.pack(hdr->body, body.length());

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow13::ofp_experimenter_header); i++) {
    rofl::openflow::cofmsg_experimenter msg;
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
      CPPUNIT_ASSERT(i <
                     sizeof(struct rofl::openflow13::ofp_experimenter_header));
    }
  }

  for (unsigned int i =
           sizeof(struct rofl::openflow13::ofp_experimenter_header);
       i == msglen; i++) {
    rofl::openflow::cofmsg_experimenter msg;
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
    rofl::openflow::cofmsg_experimenter msg;
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
