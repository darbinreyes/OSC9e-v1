/*

  Programming Problem 3.20.

*/
#include <assert.h>
#include "stack.h"
#include "prog_prob_4_20.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

static char pid_state[NUM_PIDS];

/*

    Creates and initializes a data structure for representing PIDs.

    @retval 0  If successful.
    @retval -1 An error occurred. (This value is currently not used).

*/
int allocate_map(void) {

    // All of this can be done at compile time so we need not worry about its time complexity.
    stack_init();

    for (int i = 0; i < NUM_PIDS; i++) {
        pid_state[i] = PID_STATE_IS_FREE;
        stack_push(i + MIN_PID);
    }

    return 0; // Successful.
}

/*

    Allocates and returns a PID.

    @return  If successful, an integer satisfying pid >= MIN_PID && pid <= MAX_PID.
    @retval -1 Failure. All PID are currently in use.

*/
int allocate_pid(void) {
    int pid;

    if (stack_is_empty()) {
      return -1; // No free PID is available.
    }

    pid = stack_pop(); // Get a free PID from the stack.

    assert(pid >= MIN_PID && pid <= MAX_PID); // Assert that this PID's range is valid.

    assert(pid_state[pid - MIN_PID] == PID_STATE_IS_FREE); // assert that the PID from the stack is indeed free.

    pid_state[pid - MIN_PID] = PID_STATE_IS_IN_USE; // Mark this PID as in use.

    return pid;
}

/*

  Releases a PID.

  Does nothing if:
  pid is out of the valid range. pid >= MIN_PID && pid <= MAX_PID.
  pid is not marked as in use.

*/
void release_pid(int pid) {
    if (!(pid >= MIN_PID && pid <= MAX_PID)) { // PID out of range.
        return;
    }

    // A PID that is being released should be currently in use. Return if it is
    // not.
    if(pid_state[pid - MIN_PID] != PID_STATE_IS_IN_USE) {
        return;
    }

    assert(pid_state[pid - MIN_PID] == PID_STATE_IS_IN_USE);

    pid_state[pid - MIN_PID] = PID_STATE_IS_FREE; // Mark this PID as free.

    stack_push(pid); // Push the PID onto the free stack.
}

long get_rand_sleep_ns(void) {
    float frn;
    float frd;


    frn = (float) rand();
    frd = (float) RAND_MAX;


    frn = 10000000.0 * (frn/frd);

    return (long) frn;
}

void *runner(void *param) {
    int pid;
    struct timespec rqtp; // /Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/usr/include/sys/_types/_timespec.h

    rqtp.tv_nsec = get_rand_sleep_ns();
    pid = allocate_pid();

    printf("Herro. Got pid = %d. Sleeping for %ld ns.\n", pid, rqtp.tv_nsec);

    nanosleep(&rqtp, NULL);

    release_pid(pid);

    return NULL;
}

#define N_THREADS 100

int main(void) {
    /*

        Create N threads that call allocate_pid(), sleep, release_pid().

    */
    pthread_t tid[N_THREADS];
    pthread_attr_t attr[N_THREADS];
    int i;

    sranddev(); // Set a seed for rand().
    assert(allocate_map() == 0);

    for (i = 0; i < N_THREADS; i++) {
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], runner, NULL);
    }

    for (i = 0; i < N_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("Done.\n");

    return 0;
}


