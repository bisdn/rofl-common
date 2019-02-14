#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <stdlib.h>

int main(__attribute__((unused)) int argc,
         __attribute__((unused)) char **argv) {
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry =
      CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  bool wasSuccessful = runner.run("", false);

  int rc = (wasSuccessful) ? EXIT_SUCCESS : EXIT_FAILURE;
  return rc;
}
