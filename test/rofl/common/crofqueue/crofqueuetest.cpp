/*
 * crofqueuetest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "crofqueuetest.hpp"

using namespace rofl::openflow;

CPPUNIT_TEST_SUITE_REGISTRATION( crofqueuetest );

void
crofqueuetest::setUp()
{}



void
crofqueuetest::tearDown()
{}



void
crofqueuetest::test1()
{
	uint32_t xid = 0xa1a2a3a4;
	rofl::crofqueue queue;

	queue.set_queue_max_size(128);
	CPPUNIT_ASSERT(queue.get_queue_max_size() == 128);
	CPPUNIT_ASSERT(queue.capacity() == 128);

	for (unsigned int i = 0; i < 256; i++) {
		rofl::openflow::cofmsg* msg =
				new rofl::openflow::cofmsg(
						rofl::openflow13::OFP_VERSION,
						rofl::openflow13::OFPT_HELLO,
						xid++);
		if (i < 128) {
			std::cerr << "index i=" << i
									<< " capacity=" << queue.capacity()
									<< " size=" << queue.size()
									<< std::endl;
			CPPUNIT_ASSERT(queue.capacity() == (128 - i));
			queue.store(msg);
		} else {
			try {
				queue.store(msg);
				CPPUNIT_ASSERT(false);
			} catch (rofl::eRofQueueFull& e) {
				std::cerr << "index i=" << i
							<< " capacity=" << queue.capacity()
							<< " size=" << queue.size()
							<< std::endl;
				CPPUNIT_ASSERT(queue.capacity() == 0);
				CPPUNIT_ASSERT(queue.size() == 128);
				delete msg;
			}
		}
	}

	queue.clear();

	CPPUNIT_ASSERT(queue.size() == 0);
	CPPUNIT_ASSERT(queue.empty());
}



void
crofqueuetest::test2()
{
	uint32_t xid = 0xa1a2a3a4;
	rofl::crofqueue queue;

	queue.set_queue_max_size(128);
	CPPUNIT_ASSERT(queue.get_queue_max_size() == 128);
	CPPUNIT_ASSERT(queue.capacity() == 128);

	for (unsigned int i = 0; i < 256; i++) {
		rofl::openflow::cofmsg* msg =
				new rofl::openflow::cofmsg(
						rofl::openflow13::OFP_VERSION,
						rofl::openflow13::OFPT_HELLO,
						xid++);
		if (i < 128) {
			std::cerr << "index i=" << i
									<< " capacity=" << queue.capacity()
									<< " size=" << queue.size()
									<< std::endl;
			CPPUNIT_ASSERT(queue.capacity() == (128 - i));
			queue.store(msg);
		} else {
			try {
				std::cerr << "index i=" << i
							<< " capacity=" << queue.capacity()
							<< " size=" << queue.size()
							<< std::endl;
				CPPUNIT_ASSERT(queue.capacity() == 0);
				CPPUNIT_ASSERT(queue.size() == i);
				queue.store(msg, true);

			} catch (rofl::eRofQueueFull& e) {
				CPPUNIT_ASSERT(false);
			}
		}
	}

	queue.clear();

	CPPUNIT_ASSERT(queue.size() == 0);
	CPPUNIT_ASSERT(queue.empty());
}


