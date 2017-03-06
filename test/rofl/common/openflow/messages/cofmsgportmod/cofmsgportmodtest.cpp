/*
 * cofmsgportmodtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgportmodtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgportmodtest);

void cofmsgportmodtest::setUp() {}

void cofmsgportmodtest::tearDown() {}

void cofmsgportmodtest::testPortMod10() {
  testPortMod(rofl::openflow10::OFP_VERSION, rofl::openflow10::OFPT_PORT_MOD,
              0xa1a2a3a4);
}

void cofmsgportmodtest::testPortMod12() {
  testPortMod(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPT_PORT_MOD,
              0xa1a2a3a4);
}

void cofmsgportmodtest::testPortMod13() {
  testPortMod(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPT_PORT_MOD,
              0xa1a2a3a4);
}

void cofmsgportmodtest::testPortMod(uint8_t version, uint8_t type,
                                    uint32_t xid) {
  uint32_t port_no = 0x41424344;
  rofl::caddress_ll lladdr("51:52:53:54:55:56");
  uint32_t config = 0x61626364;
  uint32_t mask = 0x71727374;
  uint32_t advertise = 0x81828384;
  rofl::openflow::cofmsg_port_mod msg1(version, xid, port_no, lladdr, config,
                                       mask, advertise);
  rofl::openflow::cofmsg_port_mod msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    if (version == rofl::openflow10::OFP_VERSION)
      CPPUNIT_ASSERT(msg2.get_portno() == (port_no & 0x0000ffff));
    else
      CPPUNIT_ASSERT(msg2.get_portno() == port_no);
    CPPUNIT_ASSERT(msg2.get_lladdr() == lladdr);
    CPPUNIT_ASSERT(msg2.get_config() == config);
    CPPUNIT_ASSERT(msg2.get_mask() == mask);
    CPPUNIT_ASSERT(msg2.get_advertise() == advertise);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgportmodtest::testPortModParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint8_t type = rofl::openflow10::OFPT_PORT_MOD;
  uint32_t xid = 0xa1a2a3a4;
  uint16_t port_no = 0x4142;
  rofl::caddress_ll lladdr("51:52:53:54:55:56");
  uint32_t config = 0x61626364;
  uint32_t mask = 0x71727374;
  uint32_t advertise = 0x81828384;

  size_t msglen = sizeof(struct rofl::openflow10::ofp_port_mod);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_port_mod *hdr =
      (struct rofl::openflow10::ofp_port_mod *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->port_no = htobe16(port_no);
  lladdr.pack(hdr->hw_addr, lladdr.length());
  hdr->config = htobe32(config);
  hdr->mask = htobe32(mask);
  hdr->advertise = htobe32(advertise);

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow10::ofp_port_mod);
       i++) {
    rofl::openflow::cofmsg_port_mod msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_port_mod));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow10::ofp_port_mod);
       i == msglen; i++) {
    rofl::openflow::cofmsg_port_mod msg;
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
    rofl::openflow::cofmsg_port_mod msg;
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

void cofmsgportmodtest::testPortModParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;
  uint8_t type = rofl::openflow12::OFPT_PORT_MOD;
  uint32_t xid = 0xa1a2a3a4;
  uint32_t port_no = 0x41424344;
  rofl::caddress_ll lladdr("51:52:53:54:55:56");
  uint32_t config = 0x61626364;
  uint32_t mask = 0x71727374;
  uint32_t advertise = 0x81828384;

  size_t msglen = sizeof(struct rofl::openflow12::ofp_port_mod);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_port_mod *hdr =
      (struct rofl::openflow12::ofp_port_mod *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->port_no = htobe32(port_no);
  lladdr.pack(hdr->hw_addr, lladdr.length());
  hdr->config = htobe32(config);
  hdr->mask = htobe32(mask);
  hdr->advertise = htobe32(advertise);

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_port_mod);
       i++) {
    rofl::openflow::cofmsg_port_mod msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_port_mod));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow12::ofp_port_mod);
       i == msglen; i++) {
    rofl::openflow::cofmsg_port_mod msg;
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
    rofl::openflow::cofmsg_port_mod msg;
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

void cofmsgportmodtest::testPortModParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;
  uint8_t type = rofl::openflow13::OFPT_PORT_MOD;
  uint32_t xid = 0xa1a2a3a4;
  uint32_t port_no = 0x41424344;
  rofl::caddress_ll lladdr("51:52:53:54:55:56");
  uint32_t config = 0x61626364;
  uint32_t mask = 0x71727374;
  uint32_t advertise = 0x81828384;

  size_t msglen = sizeof(struct rofl::openflow13::ofp_port_mod);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_port_mod *hdr =
      (struct rofl::openflow13::ofp_port_mod *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->port_no = htobe32(port_no);
  lladdr.pack(hdr->hw_addr, lladdr.length());
  hdr->config = htobe32(config);
  hdr->mask = htobe32(mask);
  hdr->advertise = htobe32(advertise);

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_port_mod);
       i++) {
    rofl::openflow::cofmsg_port_mod msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_port_mod));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow13::ofp_port_mod);
       i == msglen; i++) {
    rofl::openflow::cofmsg_port_mod msg;
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
    rofl::openflow::cofmsg_port_mod msg;
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
