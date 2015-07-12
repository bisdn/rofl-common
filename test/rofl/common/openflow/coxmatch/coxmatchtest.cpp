/*
 * coxmatch_test.cpp
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
	CPPUNIT_ASSERT(oxm.get_oxm_type() == ((rofl::openflow::OFPXMC_OPENFLOW_BASIC << 16) | (rofl::openflow::OFPXMT_OFB_ETH_TYPE << 9)));
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
	CPPUNIT_ASSERT(oxm.get_oxm_type() == ((rofl::openflow::OFPXMC_OPENFLOW_BASIC << 16) | (rofl::openflow::OFPXMT_OFB_ETH_DST << 9)));
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
	CPPUNIT_ASSERT(oxm.get_oxm_type() == ((rofl::openflow::OFPXMC_OPENFLOW_BASIC << 16) | (rofl::openflow::OFPXMT_OFB_ETH_DST << 9)));
	CPPUNIT_ASSERT(oxm.get_u48value_as_lladdr() == lladdr);
	CPPUNIT_ASSERT(oxm.get_u48mask_as_lladdr() == mask);
	CPPUNIT_ASSERT(oxm.get_u48masked_value_as_lladdr() == (lladdr & mask));
}


