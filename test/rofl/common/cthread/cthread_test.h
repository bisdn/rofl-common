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
    virtual ~cobject() { rofl::cthread::thread(thread_num).drop(this); };

    /**
     *
     */
    cobject()
        : thread_num(rofl::cthread::get_mgt_thread_num_from_pool()), cnt(0),
          error(false){};

  protected:
    virtual void handle_wakeup(rofl::cthread &thread){};
    virtual void handle_timeout(rofl::cthread &thread, uint32_t timer_id);
    virtual void handle_read_event(rofl::cthread &thread, int fd){};
    virtual void handle_write_event(rofl::cthread &thread, int fd){};

  public:
    uint32_t thread_num;

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
