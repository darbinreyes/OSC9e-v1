/*

  Programming problem 3.21.

*/

#include <unistd.h> // fork()
#include <stdio.h> // printf()
#include <assert.h>
#include <stdlib.h> // strtoul()
#include <limits.h> // ULONG_MAX
#include <errno.h> // For errors from strtoul().

void print_collatz(unsigned long int n);

int main(int argc, char **argv) {
  pid_t pid, cpid;
  int stat_loc;
  unsigned long int n;


  printf("Parent process is about to fork().\n");

  pid = fork();

  if (pid < 0) {
    printf("fork() failed.\n");
  } else if (pid == 0) { // Child
    printf("This is the child process.\n");

    if (argc != 2 || argv[1][0] == '-') {
      printf("Usage: Supply one integer argument greater than 0, e.g. \"a.out 8\". The argument is converted to unsigned long using strtoul().\n");
      return 1;
    }

    errno = 0; // strtoul() sets errno.

    n = strtoul(argv[1], NULL, 10);
    if (n == ULONG_MAX && errno == ERANGE) {
      printf("Failed to convert argument \'%s\' to unsigned long. Overflow.\n", argv[1]);
      perror("Fuck");
      return 2;
    }

    if(n == 0 && errno == EINVAL) {
      printf("Failed to convert argument \'%s\' to unsigned long. No conversion.\n", argv[1]);
      perror("Fuck");
      return 3;
    }

    if(n == 0) {
      printf("Argument must be greater than 0.\n");
      return 4;
    }

    print_collatz(n);

  } else { // Parent
    printf("Parent process is about to wait().\n");
    cpid = wait(&stat_loc);
    printf("Parent process returned from wait().\n");
    assert(pid == cpid); // Check the wait() returns the child's PID.
    if (WIFEXITED(stat_loc)) {
      printf("Child process terminated with status value = %d.\n", WEXITSTATUS(stat_loc));
    } else {
      printf("Child process terminated abnormally.\n");
    }
  }

  return 0;
}


// Since the point of this is to learn about OS concepts and not C programming:
// FYI: I will not check for overflow.
// FYI: The command line parameter validation will be very basic.
// FYI: I will assume the Collatz conjecture is in fact true. If it is false, then there will exist values of n for which this program will never terminate.
void print_collatz(unsigned long int n) {
  assert(n > 0);

  if (n == 1) {
    // Done.
    printf("1.\n");
    return;
  } else {
    printf("%lu, ", n);
  }

  if ((n & 0x01) == 0) {
    // n is even
    n >>= 1; // n = n/2
  } else {
    // n is od
    n = 3*n + 1;
  }

  print_collatz(n);

}

