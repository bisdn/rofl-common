/*
 * cofmsghellotest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsghellotest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsghellotest);

void cofmsghellotest::setUp() {}

void cofmsghellotest::tearDown() {}

void cofmsghellotest::testHello10() {
  testHello(rofl::openflow10::OFP_VERSION, rofl::openflow10::OFPT_HELLO,
            0xa1a2a3a4);
}

void cofmsghellotest::testHello12() {
  testHello(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPT_HELLO,
            0xa1a2a3a4);
}

void cofmsghellotest::testHello13() {
  testHello(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPT_HELLO,
            0xa1a2a3a4);
}

void cofmsghellotest::testHello(uint8_t version, uint8_t type, uint32_t xid) {
  rofl::openflow::cofhelloelems helloIEs;
  helloIEs.set_hello_elem_versionbitmap().add_ofp_version(version);
  rofl::openflow::cofmsg_hello msg1(version, xid, helloIEs);
  rofl::openflow::cofmsg_hello msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(
        msg2.get_helloelems().get_hello_elem_versionbitmap().has_ofp_version(
            version));

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsghellotest::testHelloParser10() {
  testHelloParser(rofl::openflow10::OFP_VERSION, rofl::openflow10::OFPT_HELLO,
                  0xa1a2a3a4);
}

void cofmsghellotest::testHelloParser12() {
  testHelloParser(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPT_HELLO,
                  0xa1a2a3a4);
}

void cofmsghellotest::testHelloParser13() {
  testHelloParser(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPT_HELLO,
                  0xa1a2a3a4);
}

void cofmsghellotest::testHelloParser(uint8_t version, uint8_t type,
                                      uint32_t xid) {
  rofl::openflow::cofhelloelems helloIEs;
  helloIEs.set_hello_elem_versionbitmap().add_ofp_version(version);

  size_t msglen =
      sizeof(struct rofl::openflow13::ofp_group_mod) + helloIEs.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow::ofp_header *hdr =
      (struct rofl::openflow::ofp_header *)(mem.somem());

  hdr->version = version;
  hdr->type = type;
  hdr->xid = htobe32(xid);
  hdr->length = htobe16(0);
  helloIEs.pack(hdr->body, helloIEs.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow::ofp_header); i++) {
    rofl::openflow::cofmsg_hello msg;
    try {
      hdr->length = htobe16(i);
      msg.unpack(mem.somem(), i);

      std::cerr << ">>> testing length values (len: " << i << ") <<< "
                << std::endl;
      std::cerr << "[FAILURE] unpack() no exception seen" << std::endl;
      std::cerr << ">>> request <<<" << std::endl << msg;
      std::cerr << ">>> memory <<<" << std::endl << mem;

      /* unpack() Must yield an axception */
      CPPUNIT_ASSERT(false);

    } catch (rofl::eBadRequestBadLen &e) {
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow::ofp_header));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow::ofp_header); i == msglen;
       i++) {
    rofl::openflow::cofmsg_hello msg;
    try {
      hdr->length = htobe16(i);
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
    rofl::openflow::cofmsg_hello msg;
    try {
      hdr->length = htobe16(i);
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
