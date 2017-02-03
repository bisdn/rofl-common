/*
 * cofmsgmetermodtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmsgmetermodtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION(cofmsgmetermodtest);

void cofmsgmetermodtest::setUp() {}

void cofmsgmetermodtest::tearDown() {}

void cofmsgmetermodtest::testMeterMod13() {
  testMeterMod(rofl::openflow13::OFP_VERSION, rofl::openflow13::OFPT_METER_MOD,
               0xa1a2a3a4);
}

void cofmsgmetermodtest::testMeterMod(uint8_t version, uint8_t type,
                                      uint32_t xid) {
  uint16_t command = 0x3132;
  uint16_t flags = 0x4142;
  uint32_t meter_id = 0x51525354;
  rofl::openflow::cofmeter_bands meter_bands(version);

  rofl::openflow::cofmsg_meter_mod msg1(version, xid, command, flags, meter_id,
                                        meter_bands);
  rofl::openflow::cofmsg_meter_mod msg2;
  rofl::cmemory mem(msg1.length());

  try {
    msg1.pack(mem.somem(), mem.length());
    msg2.unpack(mem.somem(), mem.length());

    CPPUNIT_ASSERT(msg2.get_version() == version);
    CPPUNIT_ASSERT(msg2.get_type() == type);
    CPPUNIT_ASSERT(msg2.get_length() == msg1.length());
    CPPUNIT_ASSERT(msg2.get_xid() == xid);
    CPPUNIT_ASSERT(msg2.get_command() == command);
    CPPUNIT_ASSERT(msg2.get_flags() == flags);
    CPPUNIT_ASSERT(msg2.get_meter_id() == meter_id);
    CPPUNIT_ASSERT(msg2.get_meter_bands().get_num_of_mbs() ==
                   meter_bands.get_num_of_mbs());
    CPPUNIT_ASSERT(msg2.get_meter_bands().get_version() ==
                   meter_bands.get_version());

  } catch (...) {
    std::cerr << ">>> request <<<" << std::endl << msg1;
    std::cerr << ">>> memory <<<" << std::endl << mem;
    std::cerr << ">>> clone <<<" << std::endl << msg2;
    throw;
  }
}

void cofmsgmetermodtest::testMeterModParser13() {
  uint8_t version = rofl::openflow13::OFP_VERSION;
  uint8_t type = rofl::openflow13::OFPT_METER_MOD;
  uint32_t xid = 0xa1a2a3a4;
  uint16_t command = 0x3132;
  uint16_t flags = 0x4142;
  uint32_t meter_id = 0x51525354;
  rofl::openflow::cofmeter_bands meter_bands(version);
  uint32_t burst_size = 0x61626364;
  uint32_t rate = 0x71727374;
  meter_bands.add_meter_band_drop(0).set_burst_size(burst_size);
  meter_bands.set_meter_band_drop(0).set_rate(rate);

  size_t msglen =
      sizeof(struct rofl::openflow13::ofp_meter_mod) + meter_bands.length();
  size_t memlen = 2 * msglen /*test overhead*/;

  rofl::cmemory mem(memlen);
  struct rofl::openflow13::ofp_meter_mod *hdr =
      (struct rofl::openflow13::ofp_meter_mod *)(mem.somem());

  hdr->header.version = version;
  hdr->header.type = type;
  hdr->header.xid = htobe32(xid);
  hdr->header.length = htobe16(0);
  hdr->command = htobe16(command);
  hdr->flags = htobe16(flags);
  hdr->meter_id = htobe32(meter_id);
  meter_bands.pack((uint8_t *)(hdr + 1), meter_bands.length());

  for (unsigned int i = 1; i < sizeof(struct rofl::openflow13::ofp_meter_mod);
       i++) {
    rofl::openflow::cofmsg_meter_mod msg;
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
      CPPUNIT_ASSERT(i < sizeof(struct rofl::openflow13::ofp_meter_mod));
    }
  }

  for (unsigned int i = sizeof(struct rofl::openflow13::ofp_meter_mod);
       i == msglen; i++) {
    rofl::openflow::cofmsg_meter_mod msg;
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
    rofl::openflow::cofmsg_meter_mod msg;
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
