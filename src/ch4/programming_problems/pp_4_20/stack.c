#include <assert.h>
#include "prog_prob_4_20.h"



/*

  Standard stack implementation.

*/

static int free_pid_stack[NUM_PIDS];
static int  stack_next_free = 0;

void stack_push(int pid) {
  assert(stack_next_free < NUM_PIDS); // assert that the stack is not full.
  free_pid_stack[stack_next_free] = pid;
  stack_next_free++;
}

int stack_pop(void) {
  assert(stack_next_free > 0); // assert that the stack is not empty.
  stack_next_free--;
  return free_pid_stack[stack_next_free];
}

int stack_is_empty(void) {
  return (stack_next_free == 0);
}

void stack_init(void) {
  stack_next_free = 0;
}