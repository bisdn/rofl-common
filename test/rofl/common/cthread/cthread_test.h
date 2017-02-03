#include "rofl/common/cthread.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cthread_test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(cthread_test);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST_SUITE_END();

private:
  class cobject : public rofl::cthread_env {
  public:
    /**
     *
     */
    virtual ~cobject() { thread.stop(); };

    /**
     *
     */
    cobject() : thread(this), cnt(0), error(false) { thread.start(); };

  protected:
    virtual void handle_wakeup(rofl::cthread &thread){};
    virtual void handle_timeout(rofl::cthread &thread, uint32_t timer_id);
    virtual void handle_read_event(rofl::cthread &thread, int fd){};
    virtual void handle_write_event(rofl::cthread &thread, int fd){};

  public:
    rofl::cthread thread;

    unsigned int cnt;

    bool error;
  };

private:
  cobject *object;

public:
  void setUp();
  void tearDown();

  void test1();
};
