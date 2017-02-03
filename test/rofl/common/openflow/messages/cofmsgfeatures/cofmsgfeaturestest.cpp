/*
 * cofmsgfeaturestest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgfeaturestest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgfeaturestest);

void cofmsgfeaturestest::setUp() {}

void cofmsgfeaturestest::tearDown() {}

void cofmsgfeaturestest::testRequest10() {
  testRequest(rofl::openflow10::OFP_VERSION,
              rofl::openflow10::OFPT_FEATURES_REQUEST, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testRequest12() {
  testRequest(rofl::openflow12::OFP_VERSION,
              rofl::openflow12::OFPT_FEATURES_REQUEST, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testRequest13() {
  testRequest(rofl::openflow13::OFP_VERSION,
              rofl::openflow13::OFPT_FEATURES_REQUEST, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testRequest(uint8_t version, uint8_t type,
                                     uint32_t xid) {
  rofl::openflow::cofmsg_features_request msg1(version, xid);
  rofl::openflow::cofmsg_features_request msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgfeaturestest::testReply10() {
  testReply(rofl::openflow10::OFP_VERSION,
            rofl::openflow10::OFPT_FEATURES_REPLY, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testReply12() {
  testReply(rofl::openflow12::OFP_VERSION,
            rofl::openflow12::OFPT_FEATURES_REPLY, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testReply13() {
  testReply(rofl::openflow13::OFP_VERSION,
            rofl::openflow13::OFPT_FEATURES_REPLY, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testReply(uint8_t version, uint8_t type,
                                   uint32_t xid) {
  uint64_t dpid = 0xc1c2c3c4c5c6c7c8;
  uint32_t n_buffers = 0xd1d2d3d4;
  uint8_t n_tables = 0xff;
  uint8_t auxid = 0xee;
  uint32_t capabilities = 0xe1e2e3e4;
  uint32_t actions_bitmap = 0xf1f2f3f4;
  rofl::openflow::cofports ports(version);
  for (unsigned int i = 0; i < 4; i++) {
    ports.add_port(i).set_hwaddr(rofl::caddress_ll("01:02:03:04:05:06"));
  }
  rofl::openflow::cofmsg_features_reply msg1(version, xid, dpid, n_buffers,
                                             n_tables, auxid, capabilities,
                                             actions_bitmap, ports);
  rofl::openflow::cofmsg_features_reply msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(msg2.get_dpid() == dpid);
    CPPUNIT_ASSERT(msg2.get_n_buffers() == n_buffers);
    CPPUNIT_ASSERT(msg2.get_n_tables() == n_tables);
    if (version >= rofl::openflow13::OFP_VERSION)
      CPPUNIT_ASSERT(msg2.get_auxid() == auxid);
    CPPUNIT_ASSERT(msg2.get_capabilities() == capabilities);
    if (version < rofl::openflow13::OFP_VERSION)
      CPPUNIT_ASSERT(msg2.get_ports().size() == ports.size());
    else
      CPPUNIT_ASSERT(msg2.get_ports().size() == 0);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgfeaturestest::testRequestParser10() {
  testRequestParser(rofl::openflow10::OFP_VERSION,
                    rofl::openflow10::OFPT_FEATURES_REQUEST, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testRequestParser12() {
  testRequestParser(rofl::openflow12::OFP_VERSION,
                    rofl::openflow12::OFPT_FEATURES_REQUEST, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testRequestParser13() {
  testRequestParser(rofl::openflow13::OFP_VERSION,
                    rofl::openflow13::OFPT_FEATURES_REQUEST, 0xa1a2a3a4);
}

void cofmsgfeaturestest::testRequestParser(uint8_t version, uint8_t type,
                                           uint32_t xid) {
  size_t msglen = sizeof(struct rofl::openflow::ofp_header);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow::ofp_header *hdr =
      (struct rofl::openflow::ofp_header *)(mem.somem());

  hdr->version = version;
  hdr->type = type;
  hdr->xid = htobe32(xid);
  hdr->length = htobe16(0);

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow::ofp_header); i++) {
    rofl::openflow::cofmsg_features_request msg;
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
    rofl::openflow::cofmsg_features_request msg;
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
    rofl::openflow::cofmsg_features_request msg;
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

void cofmsgfeaturestest::testReplyParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint8_t type = rofl::openflow10::OFPT_FEATURES_REPLY;
  uint32_t xid = 0xa1a2a3a4;
  uint64_t dpid = 0xc1c2c3c4c5c6c7c8;
  uint32_t n_buffers = 0xd1d2d3d4;
  uint8_t n_tables = 0xff;
  uint32_t capabilities = 0xe1e2e3e4;
  rofl::openflow::cofports ports(version);
  for (unsigned int i = 0; i < 4; i++) {
    ports.add_port(i).set_hwaddr(rofl::caddress_ll("01:02:03:04:05:06"));
  }
  size_t msglen =
      sizeof(struct rofl::openflow10::ofp_switch_features) + ports.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_switch_features *hdr =
      (struct rofl::openflow10::ofp_switch_features *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->datapath_id = htobe64(dpid);
  hdr->n_buffers = htobe32(n_buffers);
  hdr->n_tables = n_tables;
  hdr->capabilities = htobe32(capabilities);
  ports.pack((uint8_t *)hdr->ports, ports.length());

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow10::ofp_switch_features); i++) {
    rofl::openflow::cofmsg_features_reply msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_switch_features));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow10::ofp_switch_features);
       i == msglen; i++) {
    rofl::openflow::cofmsg_features_reply msg;
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
    rofl::openflow::cofmsg_features_reply msg;
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

void cofmsgfeaturestest::testReplyParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;
  uint8_t type = rofl::openflow12::OFPT_FEATURES_REPLY;
  uint32_t xid = 0xa1a2a3a4;
  uint64_t dpid = 0xc1c2c3c4c5c6c7c8;
  uint32_t n_buffers = 0xd1d2d3d4;
  uint8_t n_tables = 0xff;
  uint32_t capabilities = 0xe1e2e3e4;
  rofl::openflow::cofports ports(version);
  for (unsigned int i = 0; i < 4; i++) {
    ports.add_port(i).set_hwaddr(rofl::caddress_ll("01:02:03:04:05:06"));
  }
  size_t msglen =
      sizeof(struct rofl::openflow12::ofp_switch_features) + ports.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_switch_features *hdr =
      (struct rofl::openflow12::ofp_switch_features *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->datapath_id = htobe64(dpid);
  hdr->n_buffers = htobe32(n_buffers);
  hdr->n_tables = n_tables;
  hdr->capabilities = htobe32(capabilities);
  ports.pack((uint8_t *)hdr->ports, ports.length());

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow12::ofp_switch_features); i++) {
    rofl::openflow::cofmsg_features_reply msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_switch_features));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow12::ofp_switch_features);
       i == msglen; i++) {
    rofl::openflow::cofmsg_features_reply msg;
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
    rofl::openflow::cofmsg_features_reply msg;
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

void cofmsgfeaturestest::testReplyParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;
  uint8_t type = rofl::openflow13::OFPT_FEATURES_REPLY;
  uint32_t xid = 0xa1a2a3a4;
  uint64_t dpid = 0xc1c2c3c4c5c6c7c8;
  uint32_t n_buffers = 0xd1d2d3d4;
  uint8_t n_tables = 0xff;
  uint8_t auxid = 0xee;
  uint32_t capabilities = 0xe1e2e3e4;
  size_t msglen = sizeof(struct rofl::openflow13::ofp_switch_features);
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_switch_features *hdr =
      (struct rofl::openflow13::ofp_switch_features *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->datapath_id = htobe64(dpid);
  hdr->n_buffers = htobe32(n_buffers);
  hdr->n_tables = n_tables;
  hdr->auxiliary_id = auxid;
  hdr->capabilities = htobe32(capabilities);

  for (unsigned int i = 1;
       i < sizeof(struct rofl::openflow13::ofp_switch_features); i++) {
    rofl::openflow::cofmsg_features_reply msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_switch_features));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow13::ofp_switch_features);
       i == msglen; i++) {
    rofl::openflow::cofmsg_features_reply msg;
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
    rofl::openflow::cofmsg_features_reply msg;
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
