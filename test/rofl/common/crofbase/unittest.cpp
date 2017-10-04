/*
 * radmsgtest.cpp
 *
 *  Created on: Apr 26, 2015
 *      Author: andi
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <stdlib.h>
#include <glog/logging.h>

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);

  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry =
      CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  bool wasSuccessful = runner.run("", false);

  int rc = (wasSuccessful) ? EXIT_SUCCESS : EXIT_FAILURE;
  return rc;
}
