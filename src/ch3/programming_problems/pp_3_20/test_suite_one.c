#include "seatest.h"
#include "prog_prob_3_20.h"
#include "stack.h"

/* Given test cases.

*/

/* My test cases.


*/


void test_0(void) // Verify initial state.
{
  char const * const pid_state = test_get_pid_state_arr();
  int num_stack_entries;
  int const * const free_pid_stack = test_get_free_pid_stack();

  allocate_map();

  // Verify that the pid_state array is initialized to "all free".
  for (int i = 0; i < NUM_PIDS; i++) {
    assert_int_equal(PID_STATE_IS_FREE, pid_state[i]);
  }


  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS, num_stack_entries); // Stack should be full.

  for (int i = 0; i < NUM_PIDS; i++) {
    // Check that the PID value is inside the valid bounds.
    assert_true(free_pid_stack[i] >= MIN_PID);
    assert_true(free_pid_stack[i] <= MAX_PID);
    // Stack should have MIN_PID at the bottom and MAX_PID at the top, in order.
    assert_int_equal(i + MIN_PID, free_pid_stack[i]);
  }

}

void test_1(void) { // Allocate 1 PID.
  int num_stack_entries;
  int pid;

  allocate_map();

  pid = allocate_pid();
  // Check that the PID value is inside the valid bounds.
  assert_true(pid >= MIN_PID);
  assert_true(pid <= MAX_PID);
  assert_int_equal(MAX_PID, pid); // MAX_PID should be at the top of the stack.
  num_stack_entries = test_get_stack_next_free();
  // The number of free PIDS should have decreased by 1.
  assert_int_equal(NUM_PIDS - 1, num_stack_entries);
}

void test_2(void) { // Allocate 2 PIDs.
  int num_stack_entries;
  int pid;

  allocate_map();

  pid = allocate_pid();
  // Check that the PID value is inside the valid bounds.
  assert_true(pid >= MIN_PID);
  assert_true(pid <= MAX_PID);
  assert_int_equal(MAX_PID, pid); // MAX_PID should be at the top of the stack.
  num_stack_entries = test_get_stack_next_free();
  // The number of free PIDS should have decreased by 1.
  assert_int_equal(NUM_PIDS - 1, num_stack_entries);

  pid = allocate_pid();
  assert_true(pid >= MIN_PID);
  assert_true(pid <= MAX_PID);
  assert_int_equal(MAX_PID - 1, pid);
  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS - 2, num_stack_entries);
}

void test_3(void) { // Test allocate all.
  int num_stack_entries;
  int pid;

  allocate_map();

  for (int i = 0; i < NUM_PIDS; i++) {
    pid = allocate_pid();
    // Check that the PID value is inside the valid bounds.
    assert_true(pid >= MIN_PID);
    assert_true(pid <= MAX_PID);
  }

  num_stack_entries = test_get_stack_next_free();
  // The number of free PIDS should have decreased by 1.
  assert_int_equal(0, num_stack_entries);
}

void test_4(void) { // Verify expected behavior when there are no free PIDs.
  int num_stack_entries;
  int pid;

  allocate_map();

  for (int i = 0; i < NUM_PIDS; i++) {
    pid = allocate_pid();
    // Check that the PID value is inside the valid bounds.
    assert_true(pid >= MIN_PID);
    assert_true(pid <= MAX_PID);
  }

  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(0, num_stack_entries);

  pid = allocate_pid();
  assert_int_equal(-1, pid);
  assert_int_equal(0, num_stack_entries);
}

void test_5(void) { // Verify expected behavior when invalid an PID is released.
  int num_stack_entries;

  allocate_map();

  release_pid(299);
  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS, num_stack_entries);

  release_pid(5001);
  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS, num_stack_entries);
}

void test_6(void) { // Test allocate, release, release.
  int num_stack_entries;
  int pid;

  allocate_map();

  pid = allocate_pid();

  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS - 1, num_stack_entries);

  release_pid(pid);
  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS, num_stack_entries);

  release_pid(pid);
  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS, num_stack_entries);
}

void test_7(void) { // Test allocate all, release all.
  int num_stack_entries;
  int pid;

  allocate_map();

  for (int i = 0; i < NUM_PIDS; i++) {
    allocate_pid();
  }

  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(0, num_stack_entries);

  for (int i = 0; i < NUM_PIDS; i++) {
    release_pid(i + MIN_PID);
  }
  num_stack_entries = test_get_stack_next_free();
  assert_int_equal(NUM_PIDS, num_stack_entries);
}

void test_fixture_one( void )
{
  test_fixture_start();               // starts a fixture
  run_test(test_0);
  run_test(test_1);
  run_test(test_2);
  run_test(test_3);
  run_test(test_4);
  run_test(test_5);
  run_test(test_6);
  run_test(test_7);
  test_fixture_end();                 // ends a fixture
}