/*!
    @header Programming problem 3.22.
    @discussion See README.md.
*/

#include <unistd.h> // fork()
#include <stdio.h> // printf()
#include <assert.h>
#include <stdlib.h> // strtoul()
#include <limits.h> // ULONG_MAX
#include <errno.h> // For errors from strtoul().

/* Share memory includes */
#include <sys/mman.h> // shm_open()
#include <fcntl.h>

const char * const shm_name = "collatz";
#define SHM_SIZE 4096
#define SHM_MODE 0666 // File mode in the usual UNIX format. See `man 2 chmod`.

/*!
    @defined MAX_SEQUENCE_STR_SIZE
    @discussion -1 to account for the terminating '\0'.
*/
#define MAX_SEQUENCE_STR_SIZE (SHM_SIZE - 1)

/*!
    @defined ERROR_SEQUENCE_TRUNCATED
    @discussion The Collatz sequence (as a string) had to be truncated because
    the size of the string exceeded the size of the shared memory region. See
    MAX_SEQUENCE_STR_SIZE.

*/
#define ERROR_SEQUENCE_TRUNCATED 2

int get_arg(int argc, char **argv, long *n);
int child_p(long n);

int main(int argc, char **argv) {
  long n;
  pid_t pid, cpid;
  int stat_loc;
  int shm_fd;
  void *shm_ptr;

  if (get_arg(argc, argv, &n)) {
      return 1;
  }

  /* Do the work specified for the parent process:
     1. Establish the shared memory object.
     2. Create the child process and wait for it to terminate.
     3. Output the contents of shared memory.
     4. Removed the shared memory object.
     5. Terminate. */

  /* Create shared mem. object */
  errno = 0;
  shm_fd = shm_open(shm_name, O_RDONLY | O_CREAT | O_EXCL, SHM_MODE);

  if(shm_fd == -1) { // Error.
    perror("FYI");
    return 1;
  }

  /* Set size of shared mem. object. */
  errno = 0;
  if (ftruncate(shm_fd, SHM_SIZE) == -1) { // Error.
    perror("FYI");
    return 1;
  }

  /* Mem. map the shared mem. object. */
  /* void *mmap(void *addr, size_t len, int prot, int flags, int fd,
     off_t offset); */
  errno = 0;
  shm_ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

  if (shm_ptr == MAP_FAILED) { // Error.
    perror("FYI");
    return 1;
  }

  printf("Parent process is about to fork().\n");

  pid = fork();

  if (pid < 0) {
    printf("fork() failed.\n");
    return 1;
  }

  if (pid == 0) { // Child process
    return child_p(n);
  }

  // Parent process

  printf("Parent process is about to wait().\n");

  errno = 0;
  cpid = wait(&stat_loc);

  printf("Parent process returned from wait().\n");

  if (cpid == -1) {
    perror("FYI");
    return 1;
  }

  if (pid != cpid) { // Sanity check. wait() returns the child's PID.
    assert(0);
    return 1;
  }

  if (!WIFEXITED(stat_loc)) {
    printf("Child process terminated abnormally.\n");
    // Assume the collatz sequence was not written to the shared mem. object.
    //return 1;
  } else if (WEXITSTATUS(stat_loc) != 0 && WEXITSTATUS(stat_loc) != ERROR_SEQUENCE_TRUNCATED) {
    printf("Child process terminated without generating the sequence exit status = %d.\n", WEXITSTATUS(stat_loc));
    //return 4;
  } else if (WEXITSTATUS(stat_loc) == ERROR_SEQUENCE_TRUNCATED) {
    printf("Sequence was truncated.\n");
    printf("%s\n", (char *) shm_ptr);
  } else {
    printf("%s", (char *) shm_ptr);
  }

  /* Remove the shared mem. object. */
  if (shm_unlink(shm_name) == -1) { // Error.
    return 3;
  }

  printf("Parent process: successful.\n");
  return 0; // Success.
}

/*!
    @defined USAGE_STR
    @discussion The string to print when invalid input is given.
*/
#define USAGE_STR "Usage: Supply one integer argument greater than 0,"\
                  " e.g. \"a.out 8\".\n"

/*!
    @function get_arg
    @discussion Gets the one and only command line argument to the program, the
    starting number for the Collatz sequence.

    @param argc The usual arg to main()
    @param argv The usual arg to main()
    @param n If successful, a validated argument from the command line.
    Untouched otherwise.

    @result 0 if successful.
*/
int get_arg(int argc, char **argv, long *n) {
  long t;
  char *endptr;

  /* Get the command line argument. It should be a single positive integer >= 1.
     */

  assert(argc > 0 && argv != NULL && n != NULL);

  if (argc != 2) {
    printf("%s", USAGE_STR);
    return 1;
  }

  if(argv[1][0] == '\0') {
    printf("Empty string.\n");
    printf("%s", USAGE_STR);
    return 1;
  }

  errno = 0; // strtol() sets errno.

  t = strtol(argv[1], &endptr, 10);

  if (endptr == argv[1]) {
    printf("No digits at all.\n");
    printf("%s", USAGE_STR);
    return 1;
  }

  if (errno != 0) {
    printf("Failed to convert argument \'%s\' to long.\n", argv[1]);
    perror("FYI");
    return 1;
  }

  if(t <= 0) {
    printf("Argument must be greater than 0.\n");
    printf("%s", USAGE_STR);
    return 1;
  }

  *n = t;

  return 0;
}

int print_collatz(long n, char *shm_ptr, int bw);

/*!
    @function child_p

    @discussion Function that does the work specified for child process:
    generates the Collatz sequence. The sequence is written to the shared memory
    object.

    @param n The starting number for the Collatz sequence.

    @result 0 if successful. ERROR_SEQUENCE_TRUNCATED if the sequence was
    generated but truncated. Otherwise assume no sequence was generated.
*/
int child_p(long n) {
  int shm_fd;
  void *shm_ptr;

  printf("This is the child process.\n");

  /* Open the shared memory region for so we can write the Collatz sequence to
     it. The parent process should have created it already. */

  /* Open the shared mem. object */
  errno = 0;
  shm_fd = shm_open(shm_name, O_RDWR);

  if(shm_fd == -1) { // Error.
    perror("FYI");
    return 1;
  }

  /* Mem. map the shared mem. object. */
  /* void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset); */
  errno = 0;
  shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

  if (shm_ptr == MAP_FAILED) {
    perror("FYI");
    return 1;
  }

  /* Write to the share mem. object. */
  return print_collatz(n, shm_ptr, 0);
}

/*!
    @discussion This is a recursive implementation for printing the sequence
    corresponding to the Collatz conjecture.

    @param n  The starting value for computing the next number in the Collatz
              sequence.
    @param shm_ptr  Pointer to the shared memory region.
    @param bw Number of bytes written to the shared memory region so far.

    @result 0 if successful. ERROR_SEQUENCE_TRUNCATED if the sequence was
    generated but truncated. Otherwise assume no sequence was generated.
*/
int print_collatz(long n, char *shm_ptr, int bw) {
  char tmp_str[64]; /* The value 64 is an arbitrary upper bound for a long. */

  if (n <= 0) { // Should not occur. The Collatz sequence ends at 1.
    assert(0);
    return 1;
  }

  if (n == 1) { // n == 1 is the base case. Done.
    /* `man sprintf` These functions return the number of characters printed
       (not including the trailing '\0' used to end output to strings) */
    bw += sprintf(tmp_str, "1.\n");
    if (bw < MAX_SEQUENCE_STR_SIZE) {
      shm_ptr += sprintf(shm_ptr, "1.\n");
      return 0;
    } else {
      return ERROR_SEQUENCE_TRUNCATED; // Sequence truncated.
    }

  } else {
    bw += sprintf(tmp_str, "%ld, ", n);

    if (bw < MAX_SEQUENCE_STR_SIZE) {
      shm_ptr += sprintf(shm_ptr, "%ld, ", n);
    } else {
      return ERROR_SEQUENCE_TRUNCATED; // Sequence truncated.
    }
  }

  if ((n & 0x01) == 0) {
    // n is even
    n >>= 1; // n = n/2
  } else {
    // n is odd
    n = 3 * n + 1;
  }

  return print_collatz(n, shm_ptr, bw);

}