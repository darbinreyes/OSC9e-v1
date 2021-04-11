/*!
    @header Programming problem 9.40. Consumer process.
*/

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include "unix_shm.h"

/*!
  @function main
  @discussion Main function for the consumer process. Opens the shared memory
  object created by the producer procress, prints the contents, terminates.

  @param argc The usual argc to main.
  @param argv The usual argv to main.
  @result 0 if successful.
*/
int main(void) {
  int shm_fd;
  void *shm_ptr;

  /* "The consumer will then read and output the sequence of numbers from shared
     memory." */
  errno = 0;
  shm_fd = shm_open(shm_name, O_RDONLY);

  if(shm_fd == -1) { // Error.
    perror("FYI0");
    printf("Make sure the producer process running.\n");
    return 1;
  }


  errno = 0;
  shm_ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

  if (shm_ptr == MAP_FAILED) { // Error.
    perror("FYI1");
    return 1;
  }

  printf("%s\n", (char *) shm_ptr);

  /* Remove the shared mem. object. */

  /* FYI calling shm_unlink() here results in the consumer process only being
     able to read the contents of the shared memory object once. You need to
     restart the producer process for this process to work again. */

  // if (shm_unlink(shm_name) == -1) { // Error.
  //   perror("FYI shm_unlink");
  // }

  return 0;
}