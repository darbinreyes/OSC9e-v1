#include "seatest.h" // der: access to seatest's built in functions.

/**

  der: declare function defined in test_suite_one.c.
  test_suite_one.c defines five functions
  The first four functions correspond to individual test cases.
  The last function defined is test_fixture_one() the function declared here
  - this function calls each test via a call to run_test().
  The calls to run_test() are sandwiched between calls to
  test_fixture_start()/test_fixture_end()

**/
void test_fixture_one( void );

void all_tests( void ) // der: a function to execute all tests. This is an argument seatest_testrunner().
{
  test_fixture_one();
  // add new test fixtures here.
}

/**
  der: Main function for running tests. See below for alternate mechanisms.
**/
int main( int argc, char** argv )
{
  return seatest_testrunner(argc, argv, all_tests, NULL, NULL);
}