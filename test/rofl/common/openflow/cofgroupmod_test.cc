#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofgroupmod_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofgroupmod_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofgroupmod_test::setUp()
{
}



void
cofgroupmod_test::tearDown()
{
}


void
cofgroupmod_test::test()
{
	rofl::openflow::cofgroupmod groupmod;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == groupmod.get_version());

	groupmod.set_version(rofl::openflow13::OFP_VERSION);

	uint16_t command 		= 0xb1b2;
	uint8_t type            = 0xc1;
	uint32_t group_id       = 0xa1a2a3a4;
	rofl::openflow::cofbuckets buckets(groupmod.get_version());

	groupmod.set_command(command);
	groupmod.set_type(type);
	groupmod.set_group_id(group_id);
	groupmod.set_buckets().add_bucket(0).set_weight(0xa1);
	groupmod.set_buckets().add_bucket(1).set_weight(0xa2);

	std::cerr << "[13] groupmod:" << std::endl << groupmod;

	rofl::cmemory packed(groupmod.length());

	groupmod.pack(packed.somem(), packed.memlen());

	std::cerr << "[13] packed:" << std::endl << packed;

	rofl::openflow::cofgroupmod clone(rofl::openflow13::OFP_VERSION);

	clone.unpack(packed.somem(), packed.memlen());

	std::cerr << "[13] clone:" << std::endl << clone;

	CPPUNIT_ASSERT(clone.get_command() 		== command);
	CPPUNIT_ASSERT(clone.get_type() 	    == type);
	CPPUNIT_ASSERT(clone.get_group_id()     == group_id);
	CPPUNIT_ASSERT(clone.get_buckets().get_bucket(0).get_weight() == 0xa1);
	CPPUNIT_ASSERT(clone.get_buckets().get_bucket(1).get_weight() == 0xa2);
}






