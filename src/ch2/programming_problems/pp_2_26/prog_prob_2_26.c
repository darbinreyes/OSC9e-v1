/*

A program that copies the contents of one file to a destination file.

Using the POSIX API.

Include all necessary error checking, including that the source file exists.

Use the dtrace command to trace system calls.

*/

/*
******* Using dtrace

sudo dtrace -n 'syscall:::entry /execname == "a.out"/ { @[probefunc] = count(); }'

The above command counts all system calls made by this program.

It is unclear so far whether or not I can use dtrace to print syscalls while
this program is running. The above just provides a summary after this program
has terminated.

Problem SOLVED. Here is the dtrace command:

sudo dtrace -n 'syscall:::entry /execname == "a.out"/ { printf("%s", probefunc); }'

The key change is in using printf() instead of an aggregation ("@"). By default,
aggregation types are printed AFTER dtrace exits.

*/

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h> // read, write
#include <fcntl.h> // open, O_RDONLY
#include <stdio.h> // perror(), to print detailed error messa
#include <sys/stat.h> // See man 2 chmod.

int main(void) {
  ssize_t w_nbytes;
  ssize_t r_nbytes;

  int src_file_fd;
  #define INPUT_BUFFER_SIZE 256
  char src_file_path[INPUT_BUFFER_SIZE];

  int dest_file_fd;
  char dest_file_path[INPUT_BUFFER_SIZE];

  unsigned char cp_buf[INPUT_BUFFER_SIZE];
  mode_t dest_file_mode = S_IRUSR | S_IWUSR;

  /*

    KNR pg. 138.

    "three files are open, with file descriptors 0, 1, and 2, called the standard
    input, the standard output, and the standard error

  */

  #define STD_IN_FD  0
  #define STD_OUT_FD 1
  #define STD_ERR_FD 2

  /*
    Acquire input file name.
      Write prompt to screen.
      Accept input.
  */
  #define SRC_FILE_PROMPT "\nEnter the source path: "
  w_nbytes = write(STD_OUT_FD, SRC_FILE_PROMPT, sizeof(SRC_FILE_PROMPT));

  // FYI I will not check errors on write to stdout or read from stdin.

  r_nbytes = read(STD_IN_FD, src_file_path, INPUT_BUFFER_SIZE - 1); // -1, to save a place for the string's NULL terminator.

  // FYI I will not check that the file paths are too long.

  // Remove new line from input
  if (r_nbytes > 0 && src_file_path[r_nbytes - 1] == '\n')
    src_file_path[r_nbytes - 1] = '\0';

  src_file_path[r_nbytes] = '\0';

  src_file_fd = open(src_file_path, O_RDONLY);

  if (src_file_fd == -1) {
    // Error
    #define OPEN_ERR_MSG "\nopen() error. Bye.\n"
    w_nbytes = write(STD_OUT_FD, OPEN_ERR_MSG, sizeof(OPEN_ERR_MSG));
    perror("");
    return -1;
  }

  /*

    Acquire output file name.
      Write prompt to screen
      Accept input
  */
  #define DEST_FILE_PROMPT "\nEnter the destination path: "
  w_nbytes = write(STD_OUT_FD, DEST_FILE_PROMPT, sizeof(DEST_FILE_PROMPT));

  r_nbytes = read(STD_IN_FD, dest_file_path, INPUT_BUFFER_SIZE - 1);

  // Remove new line from input
  if (r_nbytes > 0 && dest_file_path[r_nbytes - 1] == '\n')
    dest_file_path[r_nbytes - 1] = '\0';

  dest_file_path[r_nbytes] = '\0';

  /*

    Open destination file for write only, create it if it doesn't exist, throw
    error if the file already exists.

  */
  dest_file_fd = open(dest_file_path, O_WRONLY|O_CREAT|O_EXCL, dest_file_mode);

  if (dest_file_fd == -1) {
    // Error
    w_nbytes = write(STD_OUT_FD, OPEN_ERR_MSG, sizeof(OPEN_ERR_MSG));
    perror("");
    return -1;
  }
  /*
    Open the input file
      abort if not exists
  */

  // Done above.

  /*

  Create output file
    abort if exists

  */

  // Done above.

  /*

  Loop
    Read from input file
    Write to ouput file
  Until read fails

  */
  while( (r_nbytes = read(src_file_fd, cp_buf, INPUT_BUFFER_SIZE)) != 0) {
    if (r_nbytes == -1) {
      // Error
      break;
    }

    w_nbytes = write(dest_file_fd, cp_buf, r_nbytes);

    if (w_nbytes == -1) {
      // Error
      break;
    }
  }

  if (r_nbytes == -1) {
    // Error
    #define READ_ERR_MSG "\nread() error. Bye.\n"
    w_nbytes = write(STD_OUT_FD, READ_ERR_MSG, sizeof(READ_ERR_MSG));
    perror("");
    return -1;
  }

  if (w_nbytes == -1) {
    // Error
    #define WRITE_ERR_MSG "\nwrite() error. Bye.\n"
    w_nbytes = write(STD_OUT_FD, WRITE_ERR_MSG, sizeof(WRITE_ERR_MSG));
    perror("");
    return -1;
  }

  /*

  Close output file
  ******* Close input file

  Write complete message.
  Terminate normally.
  */
  close(src_file_fd);
  close(dest_file_fd);

  // FYI I am not checking for errors from close().

  #define DONE_MSG "\nCopy successful. Bye.\n"
  w_nbytes = write(STD_OUT_FD, DONE_MSG, sizeof(DONE_MSG));
  return 0;
}