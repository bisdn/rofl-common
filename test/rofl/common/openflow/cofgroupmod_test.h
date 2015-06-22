#include "rofl/common/openflow/cofgroupmod.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofgroupmod_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofgroupmod_test );
	CPPUNIT_TEST( test );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void test();
};

