/*
 * cofmsgportstatustest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgportstatustest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgportstatustest);

void cofmsgportstatustest::setUp() {}

void cofmsgportstatustest::tearDown() {}

void cofmsgportstatustest::testPortStatus10() {
  testPortStatus(rofl::openflow10::OFP_VERSION,
                 rofl::openflow10::OFPT_PORT_STATUS, 0xa1a2a3a4);
}

void cofmsgportstatustest::testPortStatus12() {
  testPortStatus(rofl::openflow12::OFP_VERSION,
                 rofl::openflow12::OFPT_PORT_STATUS, 0xa1a2a3a4);
}

void cofmsgportstatustest::testPortStatus13() {
  testPortStatus(rofl::openflow13::OFP_VERSION,
                 rofl::openflow13::OFPT_PORT_STATUS, 0xa1a2a3a4);
}

void cofmsgportstatustest::testPortStatus(uint8_t version, uint8_t type,
                                          uint32_t xid) {
  uint8_t reason = 0xb1;
  rofl::openflow::cofport port(version);
  port.set_hwaddr(rofl::caddress_ll("00:01:02:03:04:05"));
  rofl::openflow::cofmsg_port_status msg1(version, xid, reason, port);
  rofl::openflow::cofmsg_port_status msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(msg2.get_reason() == reason);
    CPPUNIT_ASSERT(msg2.get_port().get_version() == version);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgportstatustest::testPortStatusParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint8_t type = rofl::openflow10::OFPT_PORT_STATUS;
  uint32_t xid = 0xa1a2a3a4;
  uint8_t reason = 0xb1;
  rofl::openflow::cofport port(version);
  port.set_hwaddr(rofl::caddress_ll("00:01:02:03:04:05"));

  size_t msglen = sizeof(struct rofl::openflow10::ofp_port_status);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_port_status *hdr =
      (struct rofl::openflow10::ofp_port_status *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->reason = reason;
  port.pack((uint8_t *)&(hdr->desc), port.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow10::ofp_header);
       i++) {
    rofl::openflow::cofmsg_port_status msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_header));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow10::ofp_header);
       i == msglen; i++) {
    rofl::openflow::cofmsg_port_status msg;
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
    rofl::openflow::cofmsg_port_status msg;
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

void cofmsgportstatustest::testPortStatusParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;
  uint8_t type = rofl::openflow12::OFPT_PORT_STATUS;
  uint32_t xid = 0xa1a2a3a4;
  uint8_t reason = 0xb1;
  rofl::openflow::cofport port(version);
  port.set_hwaddr(rofl::caddress_ll("00:01:02:03:04:05"));

  size_t msglen = sizeof(struct rofl::openflow12::ofp_port_status);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_port_status *hdr =
      (struct rofl::openflow12::ofp_port_status *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->reason = reason;
  port.pack((uint8_t *)&(hdr->desc), port.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_header);
       i++) {
    rofl::openflow::cofmsg_port_status msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_header));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow12::ofp_header);
       i == msglen; i++) {
    rofl::openflow::cofmsg_port_status msg;
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
    rofl::openflow::cofmsg_port_status msg;
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

void cofmsgportstatustest::testPortStatusParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;
  uint8_t type = rofl::openflow13::OFPT_PORT_STATUS;
  uint32_t xid = 0xa1a2a3a4;
  uint8_t reason = 0xb1;
  rofl::openflow::cofport port(version);
  port.set_hwaddr(rofl::caddress_ll("00:01:02:03:04:05"));

  size_t msglen = sizeof(struct rofl::openflow13::ofp_port_status);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_port_status *hdr =
      (struct rofl::openflow13::ofp_port_status *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->reason = reason;
  port.pack((uint8_t *)&(hdr->desc), port.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_header);
       i++) {
    rofl::openflow::cofmsg_port_status msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_header));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow13::ofp_header);
       i == msglen; i++) {
    rofl::openflow::cofmsg_port_status msg;
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
    rofl::openflow::cofmsg_port_status msg;
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
