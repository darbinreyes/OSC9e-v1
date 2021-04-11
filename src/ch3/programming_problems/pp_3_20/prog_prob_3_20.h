#ifndef _PROG_PROB_3_20_H_
#define _PROG_PROB_3_20_H_

#define MIN_PID 300
#define MAX_PID 5000
#define NUM_PIDS (MAX_PID - MIN_PID + 1)

int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);

/*

  Test related functions.

*/

// Normally these defines would be private to prog_prob_3_20.c but are exposed
// here for testing purposes

#define PID_STATE_IS_FREE 0
#define PID_STATE_IS_IN_USE 1

char const * const test_get_pid_state_arr(void);

#endif