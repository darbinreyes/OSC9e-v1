/*!
  @header Programming problem 9.40. Producer process.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "unix_shm.h"


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
int print_collatz(long n, char *shm_ptr, int bw);

/*!
  @function main
  @discussion Main function for the producer process. Gets argument from the
  command line representing the starting number for the Collatz sequence.
  Creates a shared memory object and writes the Collatz sequence to it.
  @param argc The usual argc to main.
  @param argv The usual argv to main.
  @result 0 if successful.
*/
int main(int argc, char **argv) {
  long n;
  int shm_fd;
  void *shm_ptr;
  int r;

  if (get_arg(argc, argv, &n)) {
    return 1;
  }

  /* Since this process just spins after creating the shared memory object, we
     must call shm_unlink() to begin with, otherwise ftruncate() will fail.
     An error from shm_unlink() is OK. */
  if (shm_unlink(shm_name) == -1) { // @TODO Is this cleaning up correctly?
    perror("FYI shm_unlink");
  }

  /* Create shared mem. object */
  errno = 0;

  shm_fd = shm_open(shm_name, O_RDWR | O_CREAT | O_TRUNC, SHM_MODE);

  if(shm_fd == -1) { // Error.
    perror("FYI0");
    return 1;
  }

  /* Set size of shared mem. object. */

  errno = 0;

  if (ftruncate(shm_fd, SHM_SIZE) == -1) { // Error.
    perror("FYI1");
    return 1;
  }

  /* Mem. map the shared mem. object. */
  /* void *mmap(void *addr, size_t len, int prot, int flags, int fd,
     off_t offset); */
  errno = 0;

  shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

  if (shm_ptr == MAP_FAILED) { // Error.
    perror("FYI2");
    return 1;
  }

  /* "The producer will generate the numbers specified in the Collatz conjecture
  and write them to a shared memory object." */

  r = print_collatz(n, shm_ptr, 0);

  if (r != 0 && r == ERROR_SEQUENCE_TRUNCATED) {
    printf("FYI3: The sequence was truncated.\n");
  } else if (r != 0) {
    printf("FYI4: print_collatz() returned error status = %d.\n", r);
    return 1;
  }

  while (1)
    ; // This is recommended by the given README file (README not README.md).

  return 0;
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