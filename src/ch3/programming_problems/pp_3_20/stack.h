#ifndef _STACK_H_ // Include guard.
#define _STACK_H_

void stack_push(int pid);
int stack_pop(void);
int stack_is_empty(void);
void stack_init(void);

/*

  Test related functions.

*/

int const * const test_get_free_pid_stack(void);

int test_get_stack_next_free(void);

#endif