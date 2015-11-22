#include "rofl/common/csegment.hpp"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class csegment_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( csegment_test );
	CPPUNIT_TEST( testStoreAndMerge );
	CPPUNIT_TEST( testRetrieveAndDetach );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testStoreAndMerge();
	void testRetrieveAndDetach();
};

