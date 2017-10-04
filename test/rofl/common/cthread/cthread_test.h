#include "rofl/common/cthread.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cthread_test : public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(cthread_test);
  CPPUNIT_TEST(test1);
  CPPUNIT_TEST_SUITE_END();

private:
  class cobject : public rofl::cthread_timeout_event {
  public:
    /**
     *
     */
    virtual ~cobject() { thread.stop(); };

    /**
     *
     */
    cobject() : cnt(0), error(false) { thread.start(); };

  protected:
    void handle_timeout(void *userdata) override;

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
