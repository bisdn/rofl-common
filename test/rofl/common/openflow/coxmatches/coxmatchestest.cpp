/*
 * coxmatchtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coxmatchestest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( coxmatchestest );

void
coxmatchestest::setUp()
{
}



void
coxmatchestest::tearDown()
{
}



void
coxmatchestest::testPack()
{
	using rofl::openflow::coxmatch_8;
	using rofl::openflow::coxmatch_16;
	using rofl::openflow::coxmatch_48;
	rofl::openflow::coxmatches matches;

	matches.add_ofb_eth_dst(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"), rofl::cmacaddr("ff:ff:ff:ff:ff:ff"));
	matches.add_ofb_eth_src(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"), rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
	matches.add_ofb_vlan_vid(0x3132, 0x4142);
	matches.add_ofb_ip_dscp(0xa1);
	matches.add_ofb_udp_src(0x1112);

	// MPLS

	rofl::cmemory mem(matches.length());
	matches.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(51);
	// ETH_DST
	test[0] = 0x80;
	test[1] = 0x00;
	test[2] = 0x07; // (ETH_DST == 3) << 1, has-mask == 1
	test[3] = 0x0c;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xff;
	test[11] = 0xff;
	test[12] = 0xff;
	test[13] = 0xff;
	test[14] = 0xff;
	test[15] = 0xff;

	// ETH_SRC_MASK
	test[16] = 0x80;
	test[17] = 0x00;
	test[18] = 0x09; // (ETH_SRC == 4) << 1, has-mask == 1
	test[19] = 0x0c;
	test[20] = 0xc1;
	test[21] = 0xc2;
	test[22] = 0xc3;
	test[23] = 0xc4;
	test[24] = 0xc5;
	test[25] = 0xc6;
	test[26] = 0xd1;
	test[27] = 0xd2;
	test[28] = 0xd3;
	test[29] = 0xd4;
	test[30] = 0xd5;
	test[31] = 0xd6;
	// VLAN_VID
	test[32] = 0x80;
	test[33] = 0x00;
	test[34] = 0x0d; // (VLAN_VID == 6) << 1, has-mask == 1
	test[35] = 0x04;
	test[36] = 0x31;
	test[37] = 0x32;
	test[38] = 0x41;
	test[39] = 0x42;
	// IP_DSCP
	test[40] = 0x80;
	test[41] = 0x00;
	test[42] = 0x10; // (IP_DSCP == 8) << 1, has-mask == 0
	test[43] = 0x01;
	test[44] = 0xa1;
	// UDP_SRC
	test[45] = 0x80;
	test[46] = 0x00;
	test[47] = 0x1e; // (UDP_SRC == 15) << 1, has-mask == 0
	test[48] = 0x02;
	test[49] = 0x11;
	test[50] = 0x12;

	std::cerr << "matches:" << std::endl << matches;
	std::cerr << "mem:" << std::endl << mem;
	std::cerr << "test:" << std::endl << test;

	CPPUNIT_ASSERT(matches.get_matches().size() == 5);
	CPPUNIT_ASSERT(matches.length() == 51);
	CPPUNIT_ASSERT(mem == test);
}



void
coxmatchestest::testUnPack()
{
	rofl::openflow::coxmatches matches;

	rofl::cmemory test(45);
	// ETH_DST
	test[0] = 0x80;
	test[1] = 0x00;
	test[2] = 0x06; // (ETH_DST == 3) << 1, has-mask == 0
	test[3] = 0x06;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	// ETH_SRC_MASK
	test[10] = 0x80;
	test[11] = 0x00;
	test[12] = 0x09; // (ETH_SRC == 4) << 1, has-mask == 1
	test[13] = 0x0c;
	test[14] = 0xc1;
	test[15] = 0xc2;
	test[16] = 0xc3;
	test[17] = 0xc4;
	test[18] = 0xc5;
	test[19] = 0xc6;
	test[20] = 0xd1;
	test[21] = 0xd2;
	test[22] = 0xd3;
	test[23] = 0xd4;
	test[24] = 0xd5;
	test[25] = 0xd6;
	// VLAN_VID
	test[26] = 0x80;
	test[27] = 0x00;
	test[28] = 0x0d; // (VLAN_VID == 6) << 1, has-mask == 1
	test[29] = 0x04;
	test[30] = 0x31;
	test[31] = 0x32;
	test[32] = 0x41;
	test[33] = 0x42;
	// IP_DSCP
	test[34] = 0x80;
	test[35] = 0x00;
	test[36] = 0x10; // (IP_DSCP == 8) << 1, has-mask == 0
	test[37] = 0x01;
	test[38] = 0xa1;
	// UDP_SRC
	test[39] = 0x80;
	test[40] = 0x00;
	test[41] = 0x1e; // (UDP_SRC == 15) << 1, has-mask == 0
	test[42] = 0x02;
	test[43] = 0x11;
	test[44] = 0x12;

	matches.unpack(test.somem(), test.memlen());

	std::cerr << "test:" << std::endl << test;
	std::cerr << "matches:" << std::endl << matches;

	CPPUNIT_ASSERT(matches.get_matches().size() == 5);
	CPPUNIT_ASSERT(matches.has_ofb_eth_dst());
	CPPUNIT_ASSERT(matches.has_ofb_eth_src());
	CPPUNIT_ASSERT(matches.has_ofb_vlan_vid());
	CPPUNIT_ASSERT(matches.has_ofb_ip_dscp());
	CPPUNIT_ASSERT(matches.has_ofb_udp_src());
	CPPUNIT_ASSERT(matches.length() == 45);
}



void
coxmatchestest::testAddMatch()
{
	using rofl::openflow::coxmatch_48;
	rofl::openflow::coxmatches matches;

	matches.add_ofb_eth_dst(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	try {
		CPPUNIT_ASSERT(matches.get_ofb_eth_dst().get_u48value_as_lladdr() == rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	} catch (rofl::openflow::eOxmNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);
	CPPUNIT_ASSERT(matches.add_ofb_eth_dst().get_u48value_as_lladdr() != rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
}



void
coxmatchestest::testDropMatch()
{
	using rofl::openflow::coxmatch_48;
	rofl::openflow::coxmatches matches;

	CPPUNIT_ASSERT(matches.get_matches().size() == 0);
}



void
coxmatchestest::testSetMatch()
{
	using rofl::openflow::coxmatch_48;
	rofl::openflow::coxmatches matches;


	rofl::cmemory test(16);
	test[0] = 0x80;
	test[1] = 0x00;
	test[2] = 0x07;
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

	matches.unpack(test.somem(), test.memlen());

	std::cerr << "matches: " << std::endl << matches;

	CPPUNIT_ASSERT(matches.length() == 16);

	rofl::cmemory mem(matches.length());
	matches.pack(mem.somem(), mem.memlen());

	CPPUNIT_ASSERT(test == mem);

	CPPUNIT_ASSERT(matches.get_matches().size() == 1);

	try {

	} catch (rofl::openflow::eOxmNotFound& e) {
		CPPUNIT_ASSERT(false);
	}
}



void
coxmatchestest::testGetMatch()
{
	rofl::openflow::coxmatches matches;

	CPPUNIT_ASSERT(matches.get_matches().size() == 0);
}



void
coxmatchestest::testHasMatch()
{
	rofl::openflow::coxmatches matches;


	CPPUNIT_ASSERT(matches.get_matches().size() == 0);
}



void
coxmatchestest::testStrictMatching()
{
	using rofl::openflow::coxmatch_8;
	// TODO
	return;
//
//	uint16_t exact_hits = 0, wildcard_hits = 0, missed = 0;
//
//	rofl::openflow::coxmatches left;
//
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48value(rofl::cmacaddr("a1:a2:a3:a4:a5:a6"));
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48mask(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_SRC).set_u48value(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_VID).set_u16value(0x3132);
//	static_cast<coxmatch_8>(left.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)).set_u8value(0x3);
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE).set_u16value(0x0800);
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32value(rofl::caddress_in4("10.1.1.0"));
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32mask(rofl::caddress_in4("255.255.255.0"));
//	static_cast<coxmatch_8>(left.set_match(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)).set_u8value(17);
//	left.set_match(rofl::openflow::OXM_TLV_BASIC_UDP_DST).set_u16value(80);
//
//	rofl::openflow::coxmatches right;
//
//	// right is all wildcard, left does not contain entire namespace
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	// right is all wildcard, left is part of it, but strict checking means => right does not contain left
//	CPPUNIT_ASSERT(not right.contains(left, true));
//
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48value(rofl::cmacaddr("a1:a2:a3:a4:a5:a6"));
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK).set_u48mask(rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
//	//std::cerr << "left:" << std::endl << left;
//	//std::cerr << "right:" << std::endl << right;
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(left.is_part_of(right, exact_hits, wildcard_hits, missed));
//	//std::cerr << "exact: " << exact_hits << " wildcard: " << wildcard_hits << " missed: " << missed << std::endl;
//	CPPUNIT_ASSERT(not right.contains(left, true));
//	CPPUNIT_ASSERT(not right.is_part_of(left, exact_hits, wildcard_hits, missed));
//	//std::cerr << "exact: " << exact_hits << " wildcard: " << wildcard_hits << " missed: " << missed << std::endl;
//
//
//
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_SRC).set_u48value(rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(right.contains(left, true));
//
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_VID).set_u16value(0x3132);
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(right.contains(left, true));
//
//	static_cast<coxmatch_8>(right.set_match(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP)).set_u8value(0x3);
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(right.contains(left, true));
//
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE).set_u16value(0x0800);
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(right.contains(left, true));
//
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32value(rofl::caddress_in4("10.1.1.0"));
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK).set_u32mask(rofl::caddress_in4("255.255.255.0"));
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(right.contains(left, true));
//
//	static_cast<coxmatch_8>(right.set_match(rofl::openflow::OXM_TLV_BASIC_IP_PROTO)).set_u8value(17);
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(right.contains(left, true));
//
//	right.set_match(rofl::openflow::OXM_TLV_BASIC_UDP_DST).set_u16value(80);
//	CPPUNIT_ASSERT(not left.contains(right, true));
//	CPPUNIT_ASSERT(right.contains(left, true));


}



void
coxmatchestest::testNonStrictMatching()
{
	// TODO
}



void
coxmatchestest::testOxmVlanVidUnpack()
{
	rofl::openflow::coxmatches matches;

	rofl::cmemory mem(6);
	mem[0] = 0x80;
	mem[1] = 0x00;
	mem[2] = 0x0c;
	mem[3] = 0x02;
	mem[4] = 0x11;
	mem[5] = 0x12;

	matches.unpack(mem.somem(), mem.memlen());

	//std::cerr << "testOxmVlanVidUnpack:" << std::endl;
	//std::cerr << "mem:" << std::endl << mem;
	//std::cerr << "matches:" << std::endl << matches;
}



void
coxmatchestest::testExp()
{
	uint8_t  u8value   = 0xa1;
	uint8_t  u8mask    = 0xa8;
	uint16_t u16value  = 0xb1b2;
	uint16_t u16mask   = 0xb8b9;
	uint32_t u32value  = 0xc1c2c3c4;
	uint32_t u32mask   = 0xc8c9cacb;
	rofl::caddress_ll u48value("d1:d2:d3:d4:d5:d6");
	rofl::caddress_ll u48mask ("d8:d9:da:db:dc:dd");
	uint64_t u64value  = 0xe0e1e2e3e4e5e6e7;
	uint64_t u64mask   = 0xe8e9eaebecedeeef;

	uint32_t exp_id    = 0x40414243;
	uint16_t oxm_class = 0xffff;
	uint8_t  oxm_field = 0x52;
	uint32_t oxm_id    = (((uint32_t)oxm_class) << 16) | (((uint32_t)oxm_field) << 9);

	rofl::openflow::coxmatches oxms;
	rofl::openflow::coxmatches clone;
	rofl::cmemory mem;

	oxms.add_ofb_mpls_tc(u8value);
	oxms.set_ofb_eth_type().set_u16value(u16value);
	oxms.add_ofb_ipv4_src().set_u32value(u32value).set_u32mask(u32mask);
	oxms.add_ofb_eth_src(u48value, u48mask);
	oxms.add_ofb_eth_dst().set_u48value(u48value).set_u48mask(u48mask);
	oxms.add_ofb_metadata(u64value).set_u64mask(u64mask);
	oxms.add_ofb_tcp_src(u16value);
	oxms.add_exp_match(exp_id, oxm_id).set_u64value(u64value);

	mem.resize(oxms.length());
	oxms.pack(mem.somem(), mem.memlen());
	clone.unpack(mem.somem(), mem.memlen());

	std::cerr << ">>>oxms<<< " << std::endl << oxms;
	std::cerr << ">>>mem<<< " << std::endl << mem;
	std::cerr << ">>>clone<<< " << std::endl << clone;

	(void)u8mask;
	(void)u16mask;
	(void)u32mask;
	(void)u64mask;
}


