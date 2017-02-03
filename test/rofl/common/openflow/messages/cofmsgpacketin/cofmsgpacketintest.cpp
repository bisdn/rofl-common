/*
 * cofmsgpacketintest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgpacketintest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgpacketintest);

void cofmsgpacketintest::setUp() {}

void cofmsgpacketintest::tearDown() {}

void cofmsgpacketintest::testPacketIn10() {
  testPacketIn(rofl::openflow10::OFP_VERSION, rofl::openflow10::OFPT_PACKET_IN,
               0xa1a2a3a4);
}

void cofmsgpacketintest::testPacketIn12() {
  testPacketIn(rofl::openflow12::OFP_VERSION, rofl::openflow12::OFPT_PACKET_IN,
               0xa1a2a3a4);
}

void cofmsgpacketintest::testPacketIn13() {
  testPacketIn(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPT_PACKET_IN,
               0xa1a2a3a4);
}

void cofmsgpacketintest::testPacketIn(uint8_t version, uint8_t type,
                                      uint32_t xid) {
  uint32_t buffer_id = 0x31323334;
  uint16_t total_len = 0x4142;
  uint8_t reason = 0x51;
  uint8_t table_id = 0x61;
  uint64_t cookie = 0x7172737475767778;
  uint16_t in_port = 0x8182;
  rofl::openflow::cofmatch match(version);
  match.set_eth_type(0x0800);
  match.set_eth_dst(rofl::caddress_ll("ff:ff:ff:ff:ff:ff"));
  match.set_eth_src(rofl::caddress_ll("00:01:02:03:04:05"));
  rofl::cpacket packet(128);
  for (unsigned int i = 0; i < packet.length(); i++) {
    packet.soframe()[i] = i;
  }
  rofl::openflow::cofmsg_packet_in msg1(
      version, xid, buffer_id, total_len, reason, table_id, cookie, in_port,
      match, packet.soframe(), packet.length());
  rofl::openflow::cofmsg_packet_in msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(msg2.get_buffer_id() == buffer_id);
    CPPUNIT_ASSERT(msg2.get_total_len() == total_len);
    CPPUNIT_ASSERT(msg2.get_reason() == reason);
    if (version == rofl::openflow10::OFP_VERSION) {
      CPPUNIT_ASSERT(msg2.get_in_port() == in_port);
    }
    if (version > rofl::openflow10::OFP_VERSION) {
      CPPUNIT_ASSERT(msg2.get_table_id() == table_id);
      CPPUNIT_ASSERT(msg2.get_match().get_version() == version);
      CPPUNIT_ASSERT(msg2.get_match().get_eth_dst() == match.get_eth_dst());
      CPPUNIT_ASSERT(msg2.get_match().get_eth_src() == match.get_eth_src());
      CPPUNIT_ASSERT(msg2.get_match().get_eth_type() == match.get_eth_type());
    }
    if (version > rofl::openflow12::OFP_VERSION) {
      CPPUNIT_ASSERT(msg2.get_cookie() == cookie);
    }
    CPPUNIT_ASSERT(msg2.get_packet() == packet);

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgpacketintest::testPacketInParser10() {
  uint8_t version = rofl::openflow10::OFP_VERSION;
  uint8_t type = rofl::openflow10::OFPT_PACKET_IN;
  uint32_t xid = 0xa1a2a3a4;
  uint32_t buffer_id = 0x31323334;
  uint16_t total_len = 0x4142;
  uint8_t reason = 0x51;
  uint16_t in_port = 0x8182;
  rofl::cpacket packet(64);
  for (unsigned int i = 0; i < packet.length(); i++) {
    packet.soframe()[i] = i % 16;
  }

  size_t msglen =
      sizeof(struct rofl::openflow10::ofp_packet_in) + 2 + packet.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow10::ofp_packet_in *hdr =
      (struct rofl::openflow10::ofp_packet_in *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->buffer_id = htobe32(buffer_id);
  hdr->total_len = htobe16(total_len);
  hdr->reason = reason;
  hdr->in_port = htobe16(in_port);
  packet.pack(hdr->data, packet.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow10::ofp_packet_in);
       i++) {
    rofl::openflow::cofmsg_packet_in msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow10::ofp_packet_in));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow10::ofp_packet_in);
       i == msglen; i++) {
    rofl::openflow::cofmsg_packet_in msg;
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
    rofl::openflow::cofmsg_packet_in msg;
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

void cofmsgpacketintest::testPacketInParser12() {
  uint8_t version = rofl::openflow12::OFP_VERSION;
  uint8_t type = rofl::openflow12::OFPT_PACKET_IN;
  uint32_t xid = 0xa1a2a3a4;
  uint32_t buffer_id = 0x31323334;
  uint16_t total_len = 0x4142;
  uint8_t reason = 0x51;
  uint8_t table_id = 0x61;
  rofl::openflow::cofmatch match(version);
  match.set_eth_type(0x0800);
  match.set_eth_dst(rofl::caddress_ll("ff:ff:ff:ff:ff:ff"));
  match.set_eth_src(rofl::caddress_ll("00:01:02:03:04:05"));
  rofl::cpacket packet(64);
  for (unsigned int i = 0; i < packet.length(); i++) {
    packet.soframe()[i] = i % 16;
  }

  size_t msglen =
      sizeof(struct rofl::openflow12::ofp_packet_in) + 2 + packet.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow12::ofp_packet_in *hdr =
      (struct rofl::openflow12::ofp_packet_in *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->buffer_id = htobe32(buffer_id);
  hdr->total_len = htobe16(total_len);
  hdr->reason = reason;
  hdr->table_id = table_id;
  match.pack((uint8_t *)&(hdr->match), match.length());
  packet.pack((uint8_t *)&(hdr->match) + match.length(), packet.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow12::ofp_packet_in);
       i++) {
    rofl::openflow::cofmsg_packet_in msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow12::ofp_packet_in));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow12::ofp_packet_in);
       i == msglen; i++) {
    rofl::openflow::cofmsg_packet_in msg;
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
    rofl::openflow::cofmsg_packet_in msg;
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

void cofmsgpacketintest::testPacketInParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;
  uint8_t type = rofl::openflow13::OFPT_PACKET_IN;
  uint32_t xid = 0xa1a2a3a4;
  uint32_t buffer_id = 0x31323334;
  uint16_t total_len = 0x4142;
  uint8_t reason = 0x51;
  uint8_t table_id = 0x61;
  uint64_t cookie = 0x7172737475767778;
  rofl::openflow::cofmatch match(version);
  match.set_eth_type(0x0800);
  match.set_eth_dst(rofl::caddress_ll("ff:ff:ff:ff:ff:ff"));
  match.set_eth_src(rofl::caddress_ll("00:01:02:03:04:05"));
  rofl::cpacket packet(64);
  for (unsigned int i = 0; i < packet.length(); i++) {
    packet.soframe()[i] = i % 16;
  }

  size_t msglen =
      sizeof(struct rofl::openflow13::ofp_packet_in) + 2 + packet.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_packet_in *hdr =
      (struct rofl::openflow13::ofp_packet_in *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->buffer_id = htobe32(buffer_id);
  hdr->total_len = htobe16(total_len);
  hdr->reason = reason;
  hdr->table_id = table_id;
  hdr->cookie = htobe64(cookie);
  match.pack((uint8_t *)&(hdr->match), match.length());
  packet.pack((uint8_t *)&(hdr->match) + match.length(), packet.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_packet_in);
       i++) {
    rofl::openflow::cofmsg_packet_in msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_packet_in));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow13::ofp_packet_in);
       i == msglen; i++) {
    rofl::openflow::cofmsg_packet_in msg;
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
    rofl::openflow::cofmsg_packet_in msg;
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
