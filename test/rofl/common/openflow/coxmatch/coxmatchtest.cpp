/*
 * coxmatchtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coxmatchtest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( coxmatchtest );

void
coxmatchtest::setUp()
{
	oxm_id_exp = 0xffffa3a4;
	exp_id = 0x01020304;
}


void
coxmatchtest::tearDown()
{

}



void
coxmatchtest::test()
{
	uint32_t oxm_id = rofl::openflow::OXM_TLV_BASIC_ETH_TYPE;

	coxmatch oxmatch(oxm_id);

	rofl::cmemory mem(oxmatch.length());
	oxmatch.pack(mem.somem(), mem.length());
	coxmatch oxm;
	oxm.unpack(mem.somem(), mem.length());

	std::cerr << oxmatch << std::endl;
	std::cerr << oxm << std::endl;
	std::cerr << mem << std::endl;

	CPPUNIT_ASSERT(oxm.length() == sizeof(struct rofl::openflow::ofp_oxm_hdr));
	CPPUNIT_ASSERT(oxm.get_oxm_class() == rofl::openflow::OFPXMC_OPENFLOW_BASIC);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == rofl::openflow::OFPXMT_OFB_ETH_TYPE);
	CPPUNIT_ASSERT(oxm.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(oxm.get_oxm_type() == (uint32_t)((rofl::openflow::OFPXMC_OPENFLOW_BASIC << 16) | (rofl::openflow::OFPXMT_OFB_ETH_TYPE << 9)));
}



void
coxmatchtest::test_eth_dst()
{
	rofl::caddress_ll lladdr("a1:a2:a3:a4:a5:a6");
	coxmatch_ofb_eth_dst oxmatch(lladdr);

	rofl::cmemory mem(oxmatch.length());
	oxmatch.pack(mem.somem(), mem.length());
	coxmatch_ofb_eth_dst oxm;
	oxm.unpack(mem.somem(), mem.length());

	std::cerr << oxmatch << std::endl;
	std::cerr << oxm << std::endl;
	std::cerr << mem << std::endl;

	CPPUNIT_ASSERT(oxm.length() == sizeof(struct rofl::openflow::ofp_oxm_hdr) + OFP_ETH_ALEN);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == rofl::openflow::OFPXMC_OPENFLOW_BASIC);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == rofl::openflow::OFPXMT_OFB_ETH_DST);
	CPPUNIT_ASSERT(oxm.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(oxm.get_oxm_type() == (uint32_t)((rofl::openflow::OFPXMC_OPENFLOW_BASIC << 16) | (rofl::openflow::OFPXMT_OFB_ETH_DST << 9)));
	CPPUNIT_ASSERT(oxm.get_u48value_as_lladdr() == lladdr);
	CPPUNIT_ASSERT(oxm.get_u48masked_value_as_lladdr() == lladdr);
}



void
coxmatchtest::test_eth_dst_mask()
{
	rofl::caddress_ll lladdr("a1:a2:a3:a4:a5:a6");
	rofl::caddress_ll mask("b1:b2:b3:b4:b5:b6");
	coxmatch_ofb_eth_dst oxmatch(lladdr, mask);

	rofl::cmemory mem(oxmatch.length());
	oxmatch.pack(mem.somem(), mem.length());
	coxmatch_ofb_eth_dst oxm;
	oxm.unpack(mem.somem(), mem.length());

	std::cerr << oxmatch << std::endl;
	std::cerr << oxm << std::endl;
	std::cerr << mem << std::endl;

	CPPUNIT_ASSERT(oxm.length() == sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2 * OFP_ETH_ALEN);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == rofl::openflow::OFPXMC_OPENFLOW_BASIC);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == rofl::openflow::OFPXMT_OFB_ETH_DST);
	CPPUNIT_ASSERT(oxm.get_oxm_hasmask() == true);
	CPPUNIT_ASSERT(oxm.get_oxm_type() == (uint32_t)((rofl::openflow::OFPXMC_OPENFLOW_BASIC << 16) | (rofl::openflow::OFPXMT_OFB_ETH_DST << 9)));
	CPPUNIT_ASSERT(oxm.get_u48value_as_lladdr() == lladdr);
	CPPUNIT_ASSERT(oxm.get_u48mask_as_lladdr() == mask);
	CPPUNIT_ASSERT(oxm.get_u48masked_value_as_lladdr() == (lladdr & mask));
}



void
coxmatchtest::test1Byte()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch_8 oxm(oxm_id, (uint8_t)0xb1);

	CPPUNIT_ASSERT(oxm.length() == 5);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(5);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0x01;
	test[4] = 0xb1;

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>test<<< " << std::endl << test;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u8value() == 0xb1);
	CPPUNIT_ASSERT(oxm.get_u8mask() == 0xff);
	CPPUNIT_ASSERT(oxm.get_u8masked_value() == 0xb1);

	oxm.set_u8value(0xd1);

	CPPUNIT_ASSERT(oxm.get_u8value() == 0xd1);

}



void
coxmatchtest::test1ByteHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch_8 oxm(oxm_id, (uint8_t)0xb1, (uint8_t)0xc1);

	CPPUNIT_ASSERT(oxm.length() == 6);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(6);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0x02;
	test[4] = 0xb1;
	test[5] = 0xc1;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u8value() == 0xb1);
	CPPUNIT_ASSERT(oxm.get_u8mask() == 0xc1);
	CPPUNIT_ASSERT(oxm.get_u8masked_value() == (0xb1 & 0xc1));

	oxm.set_u8value(0xd1);
	oxm.set_u8mask(0xe1);

	CPPUNIT_ASSERT(oxm.get_u8value() == 0xd1);
	CPPUNIT_ASSERT(oxm.get_u8mask() == 0xe1);
	CPPUNIT_ASSERT(oxm.get_u8masked_value() == (0xd1 & 0xe1));
}



void
coxmatchtest::test2Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch_16 oxm(oxm_id, (uint16_t)0xb1b2);

	CPPUNIT_ASSERT(oxm.length() == 6);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(6);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0x02;
	test[4] = 0xb1;
	test[5] = 0xb2;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u16value() == 0xb1b2);
	CPPUNIT_ASSERT(oxm.get_u16mask() == 0xffff);
	CPPUNIT_ASSERT(oxm.get_u16masked_value() == 0xb1b2);

	oxm.set_u16value(0xd1d2);

	CPPUNIT_ASSERT(oxm.get_u16value() == 0xd1d2);
}



void
coxmatchtest::test2BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch_16 oxm(oxm_id, (uint16_t)0xb1b2, (uint16_t)0xc1c2);

	CPPUNIT_ASSERT(oxm.length() == 8);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(8);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0x04;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xc1;
	test[7] = 0xc2;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u16value() == 0xb1b2);
	CPPUNIT_ASSERT(oxm.get_u16mask() == 0xc1c2);
	CPPUNIT_ASSERT(oxm.get_u16masked_value() == (0xb1b2 & 0xc1c2));

	oxm.set_u16value(0xd1d2);
	oxm.set_u16mask(0xe1e2);

	CPPUNIT_ASSERT(oxm.get_u16value() == 0xd1d2);
	CPPUNIT_ASSERT(oxm.get_u16mask() == 0xe1e2);
	CPPUNIT_ASSERT(oxm.get_u16masked_value() == (0xd1d2 & 0xe1e2));
}



void
coxmatchtest::test4Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch_32 oxm(oxm_id, (uint32_t)0xb1b2b3b4);

	CPPUNIT_ASSERT(oxm.length() == 8);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(8);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0x04;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u32value() == 0xb1b2b3b4);
	CPPUNIT_ASSERT(oxm.get_u32mask() == 0xffffffff);
	CPPUNIT_ASSERT(oxm.get_u32masked_value() == 0xb1b2b3b4);

	oxm.set_u32value(0xd1d2d3d4);

	CPPUNIT_ASSERT(oxm.get_u32value() == 0xd1d2d3d4);
}



void
coxmatchtest::test4BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch_32 oxm(oxm_id, (uint32_t)0xb1b2b3b4, (uint32_t)0xc1c2c3c4);

	CPPUNIT_ASSERT(oxm.length() == 12);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(12);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0x08;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xc1;
	test[9] = 0xc2;
	test[10] = 0xc3;
	test[11] = 0xc4;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u32value() == 0xb1b2b3b4);
	CPPUNIT_ASSERT(oxm.get_u32mask() == 0xc1c2c3c4);
	CPPUNIT_ASSERT(oxm.get_u32masked_value() == (0xb1b2b3b4 & 0xc1c2c3c4));

	oxm.set_u32value(0xd1d2d3d4);
	oxm.set_u32mask(0xe1e2e3e4);

	CPPUNIT_ASSERT(oxm.get_u32value() == 0xd1d2d3d4);
	CPPUNIT_ASSERT(oxm.get_u32mask() == 0xe1e2e3e4);
	CPPUNIT_ASSERT(oxm.get_u32masked_value() == (0xd1d2d3d4 & 0xe1e2e3e4));
}



void
coxmatchtest::test6Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch_48 oxm(oxm_id, rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	CPPUNIT_ASSERT(oxm.length() == 10);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(10);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0x06;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u48value_as_lladdr() == rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	CPPUNIT_ASSERT(oxm.get_u48mask_as_lladdr() == rofl::cmacaddr("ff:ff:ff:ff:ff:ff"));
	CPPUNIT_ASSERT(oxm.get_u48masked_value_as_lladdr() == rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	oxm.set_u48value(rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));

	CPPUNIT_ASSERT(oxm.get_u48value_as_lladdr() == rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
}



void
coxmatchtest::test6BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch_48 oxm(oxm_id, rofl::cmacaddr("b1:b2:b3:b4:b5:b6"), rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));

	CPPUNIT_ASSERT(oxm.length() == 16);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(16);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0x0c;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xc1;
	test[11] = 0xc2;
	test[12] = 0xc3;
	test[13] = 0xc4;
	test[14] = 0xc5;
	test[15] = 0xc6;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u48value_as_lladdr() == rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	CPPUNIT_ASSERT(oxm.get_u48mask_as_lladdr() == rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));
	CPPUNIT_ASSERT(oxm.get_u48masked_value_as_lladdr() == (rofl::cmacaddr("b1:b2:b3:b4:b5:b6") & rofl::cmacaddr("c1:c2:c3:c4:c5:c6")));

	oxm.set_u48value(rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
	oxm.set_u48mask(rofl::cmacaddr("e1:e2:e3:e4:e5:e6"));

	CPPUNIT_ASSERT(oxm.get_u48value_as_lladdr() == rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
	CPPUNIT_ASSERT(oxm.get_u48mask_as_lladdr() == rofl::cmacaddr("e1:e2:e3:e4:e5:e6"));
	CPPUNIT_ASSERT(oxm.get_u48masked_value_as_lladdr() == (rofl::cmacaddr("d1:d2:d3:d4:d5:d6") & rofl::cmacaddr("e1:e2:e3:e4:e5:e6")));
}



void
coxmatchtest::test8Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch_64 oxm(oxm_id, (uint64_t)0xb1b2b3b4b5b6b7b8);

	CPPUNIT_ASSERT(oxm.length() == 12);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(12);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0x08;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xb7;
	test[11] = 0xb8;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u64value() == 0xb1b2b3b4b5b6b7b8);
	CPPUNIT_ASSERT(oxm.get_u64mask() == 0xffffffffffffffff);
	CPPUNIT_ASSERT(oxm.get_u64masked_value() == 0xb1b2b3b4b5b6b7b8);

	oxm.set_u64value(0xd1d2d3d4d5d6d7d8);

	CPPUNIT_ASSERT(oxm.get_u64value() == 0xd1d2d3d4d5d6d7d8);
}



void
coxmatchtest::test8BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch_64 oxm(oxm_id, (uint64_t)0xb1b2b3b4b5b6b7b8, (uint64_t)0xc1c2c3c4c5c6c7c8);

	CPPUNIT_ASSERT(oxm.length() == 20);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(20);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0x10;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xb7;
	test[11] = 0xb8;
	test[12] = 0xc1;
	test[13] = 0xc2;
	test[14] = 0xc3;
	test[15] = 0xc4;
	test[16] = 0xc5;
	test[17] = 0xc6;
	test[18] = 0xc7;
	test[19] = 0xc8;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u64value() == 0xb1b2b3b4b5b6b7b8);
	CPPUNIT_ASSERT(oxm.get_u64mask() == 0xc1c2c3c4c5c6c7c8);
	CPPUNIT_ASSERT(oxm.get_u64masked_value() == (0xb1b2b3b4b5b6b7b8 & 0xc1c2c3c4c5c6c7c8));

	oxm.set_u64value(0xd1d2d3d4d5d6d7d8);
	oxm.set_u64mask(0xe1e2e3e4e5e6e7e8);

	CPPUNIT_ASSERT(oxm.get_u64value() == 0xd1d2d3d4d5d6d7d8);
	CPPUNIT_ASSERT(oxm.get_u64mask() == 0xe1e2e3e4e5e6e7e8);
	CPPUNIT_ASSERT(oxm.get_u64masked_value() == (0xd1d2d3d4d5d6d7d8 & 0xe1e2e3e4e5e6e7e8));
}



void
coxmatchtest::testOxmIPv4Src()
{
	rofl::caddress_in4 addr("192.168.1.2");
	rofl::caddress_in4 mask("255.255.240.0");
	rofl::openflow::coxmatch_ofb_ipv4_src oxm(addr, mask);
	//std::cerr << oxm;
}



void
coxmatchtest::testOxmIPv4Dst()
{
	// TODO
}



void
coxmatchtest::testOxmIPv6Src()
{
	rofl::caddress_in6 addr("fe80:a1a2:a3a4:a5a6:a7a8:a9aa:abac:adae");
	rofl::caddress_in6 mask("e0e1:e2e3:e4e5:e6e7:e8e9:eaeb:eced:eeef");
	/*
	 * check value/mask
	 */
	{
		rofl::openflow::coxmatch_ofb_ipv6_src oxm(addr, mask);
		//std::cerr << std::endl << oxm;
	}
	/*
	 * check value only
	 */
	{
		rofl::openflow::coxmatch_ofb_ipv6_src oxm(addr);
		//std::cerr << std::endl << oxm;
	}
}



void
coxmatchtest::testOxmIPv6Dst()
{
	// TODO
	//exit(0);
}



void
coxmatchtest::testExp8()
{
	uint16_t oxm_class = 0xffff;
	uint8_t  oxm_field = 0x74; // must be < 128 (0x80)
	uint32_t exp_id = 0xe1e2e3e4;
	uint32_t oxm_id = (((uint32_t)oxm_class) << 16) | (((uint16_t)oxm_field) << 9);
	uint8_t value = 0xa1;
	uint8_t mask  = 0xb2;

	rofl::openflow::coxmatch_exp oxm(oxm_id, exp_id);
	rofl::openflow::coxmatch_exp clone;
	rofl::cmemory mem;

	oxm.set_u8value(value);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u8value() == value);
	CPPUNIT_ASSERT(clone.get_u8mask() == 0xff);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + sizeof(uint8_t));

	oxm.set_u8mask(mask);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u8value() == value);
	CPPUNIT_ASSERT(clone.get_u8mask() == mask);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == true);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + 2*sizeof(uint8_t));
}



void
coxmatchtest::testExp16()
{
	uint16_t oxm_class = 0xffff;
	uint8_t  oxm_field = 0x74; // must be < 128 (0x80)
	uint32_t exp_id = 0xe1e2e3e4;
	uint32_t oxm_id = (((uint32_t)oxm_class) << 16) | (((uint16_t)oxm_field) << 9);
	uint16_t value = 0xa1a2;
	uint16_t mask  = 0xb2b3;

	rofl::openflow::coxmatch_exp oxm(oxm_id, exp_id);
	rofl::openflow::coxmatch_exp clone;
	rofl::cmemory mem;

	oxm.set_u16value(value);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u16value() == value);
	CPPUNIT_ASSERT(clone.get_u16mask() == 0xffff);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + sizeof(uint16_t));

	oxm.set_u16mask(mask);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u16value() == value);
	CPPUNIT_ASSERT(clone.get_u16mask() == mask);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == true);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + 2*sizeof(uint16_t));
}



void
coxmatchtest::testExp32()
{
	uint16_t oxm_class = 0xffff;
	uint8_t  oxm_field = 0x74; // must be < 128 (0x80)
	uint32_t exp_id = 0xe1e2e3e4;
	uint32_t oxm_id = (((uint32_t)oxm_class) << 16) | (((uint16_t)oxm_field) << 9);
	uint32_t value = 0xa1a2a3a4;
	uint32_t mask  = 0xb2b3b4b5;

	rofl::openflow::coxmatch_exp oxm(oxm_id, exp_id);
	rofl::openflow::coxmatch_exp clone;
	rofl::cmemory mem;

	oxm.set_u32value(value);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u32value() == value);
	CPPUNIT_ASSERT(clone.get_u32mask() == 0xffffffff);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + sizeof(uint32_t));

	oxm.set_u32mask(mask);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u32value() == value);
	CPPUNIT_ASSERT(clone.get_u32mask() == mask);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == true);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + 2*sizeof(uint32_t));
}



void
coxmatchtest::testExp48()
{
	uint16_t oxm_class = 0xffff;
	uint8_t  oxm_field = 0x74; // must be < 128 (0x80)
	uint32_t exp_id = 0xe1e2e3e4;
	uint32_t oxm_id = (((uint32_t)oxm_class) << 16) | (((uint16_t)oxm_field) << 9);
	rofl::caddress_ll value("a1:a2:a3:a4:a5:a6");
	rofl::caddress_ll mask ("b2:b3:b4:b5:b6:b7");

	rofl::openflow::coxmatch_exp oxm(oxm_id, exp_id);
	rofl::openflow::coxmatch_exp clone;
	rofl::cmemory mem;

	oxm.set_u48value(value);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u48value() == value);
	CPPUNIT_ASSERT(clone.get_u48mask() == rofl::caddress_ll("ff:ff:ff:ff:ff:ff"));
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + 6*sizeof(uint8_t));

	oxm.set_u48mask(mask);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u48value() == value);
	CPPUNIT_ASSERT(clone.get_u48mask() == mask);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == true);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + 2*6*sizeof(uint8_t));
}



void
coxmatchtest::testExp64()
{
	uint16_t oxm_class = 0xffff;
	uint8_t  oxm_field = 0x74; // must be < 128 (0x80)
	uint32_t exp_id = 0xe1e2e3e4;
	uint32_t oxm_id = (((uint32_t)oxm_class) << 16) | (((uint16_t)oxm_field) << 9);
	uint64_t value = 0xa1a2a3a4a5a6a7a8;
	uint64_t mask  = 0xb2b3b4b5b6b7b8b9;

	rofl::openflow::coxmatch_exp oxm(oxm_id, exp_id);
	rofl::openflow::coxmatch_exp clone;
	rofl::cmemory mem;

	oxm.set_u64value(value);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u64value() == value);
	CPPUNIT_ASSERT(clone.get_u64mask() == 0xffffffffffffffff);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + sizeof(uint64_t));

	oxm.set_u64mask(mask);

	mem.resize(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxm<<< " << std::endl << oxm;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	std::cerr << "oxm_id: 0x" << std::hex << (unsigned int)oxm_id << std::endl;
	std::cerr << "clone.get_oxm_type(): 0x" << std::hex << (unsigned int)clone.get_oxm_type() << std::endl;
	std::cerr << "clone.get_oxm_id(): 0x" << std::hex << (unsigned int)clone.get_oxm_id() << std::endl;

	CPPUNIT_ASSERT(clone.get_u64value() == value);
	CPPUNIT_ASSERT(clone.get_u64mask() == mask);
	CPPUNIT_ASSERT(clone.get_oxm_class() == oxm_class);
	CPPUNIT_ASSERT(clone.get_oxm_field() == oxm_field);
	CPPUNIT_ASSERT(clone.get_oxm_hasmask() == true);
	CPPUNIT_ASSERT(OXM_ROFL_TYPE(clone.get_oxm_id()) == oxm_id);
	CPPUNIT_ASSERT(clone.get_oxm_type() == oxm_id);
	CPPUNIT_ASSERT(clone.length() == oxm.length());
	CPPUNIT_ASSERT(clone.get_oxm_length() == sizeof(uint32_t) + 2*sizeof(uint64_t));
}


