#pragma once

namespace rofl {

class cthread_write_event {
public:
  cthread_write_event() {}
  virtual ~cthread_write_event() {}
  virtual void handle_write(int fd, void *userdata) = 0;
};

class cthread_read_event {
public:
  cthread_read_event() {}
  virtual ~cthread_read_event() {}
  virtual void handle_read(int fd, void *userdata) = 0;
};

class cthread_timeout_event {
public:
  cthread_timeout_event() {}
  virtual ~cthread_timeout_event() {}
  virtual void handle_timeout(void *userdata) = 0;
};

class cthread_wakeup_event {
public:
  cthread_wakeup_event() {}
  virtual ~cthread_wakeup_event() {}
  virtual void handle_wakeup(void *userdata) = 0;
};

}; // end of namespace rofl
