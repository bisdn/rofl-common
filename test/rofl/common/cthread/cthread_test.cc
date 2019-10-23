#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cthread_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION(cthread_test);

void cthread_test::setUp() { object = new cobject(); }

void cthread_test::tearDown() { delete object; }

void cthread_test::test1() {
  unsigned int keep_running = 60;

  rofl::cthread::thread(object->thread_num)
      .add_timer(object, 0, rofl::ctimespec().expire_in(1));
  rofl::cthread::thread(object->thread_num)
      .add_timer(object, 1, rofl::ctimespec().expire_in(2));

  while ((--keep_running > 0) && (object->cnt < 10)) {
    CPPUNIT_ASSERT(not object->error);
    std::cerr << ".";
    sleep(1);
  }
  std::cerr << std::endl;
  CPPUNIT_ASSERT(keep_running > 0);
}

void cthread_test::cobject::handle_timeout(rofl::cthread &thread,
                                           uint32_t timer_id) {
  switch (timer_id) {
  case 0: {
    cnt++;
    /* reschedule timer 0 */
    thread.add_timer(this, 0, rofl::ctimespec().expire_in(1));
    /* reset timer 1, should never fire */
    thread.add_timer(this, 1, rofl::ctimespec().expire_in(2));
  } break;
  case 1: {
    error = true;
  } break;
  default: {
  };
  }
};
