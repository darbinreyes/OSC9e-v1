//#include <sys/types.h>
#include <stdio.h> // fprintf(), printf()
#include <string.h> // strlen()
#include <unistd.h> // pipe(), close(), write()
#include <errno.h> // errno

#define BUFFER_SIZE 256
#define READ_END 0
#define WRITE_END 1


int main(int argc, char **argv) {
  char cpbuf[BUFFER_SIZE];
  int fd[2]; // Pipe fd's. Parent writes, child reads.
  pid_t pid;
  char *src_name, *dest_name;
  FILE *src, *dest;
  int n, nw;

  /* Get the file names from the program arguments. */
  /* Usage: filecopy input.txt copy.txt */
  if (argc != 3) {
    fprintf(stderr, "Usage: %s input.txt copy.txt\n", argv[0]);
    return 1;
  }

  src_name = argv[1]; // Source file name.
  dest_name = argv[2]; // Destination file name.

  /* create the pipe */
  if (pipe(fd) == -1) {
    fprintf(stderr, "Pipe failed.\n");
    return 2;
  }

  /* fork a child process */
  pid = fork();

  if (pid < 0) { /* error occurred */
    fprintf(stderr, "Fork failed.\n");
    return 3;
  }

  if (pid > 0) { /* parent process */
    /* Open the source file */
    src = fopen(src_name, "r");

    if (src == NULL) {
      fprintf(stderr, "fopen failed for file name %s.\n", src_name);
      return -1;
    }

    /* Parent. close the unused end of the pipe */
    close(fd[READ_END]);

    while(!feof(src)) {
      /* man fread
        If an error occurs, or the end-of-file is reached, the return value is a short object count (or zero).
        The function fread() does not distinguish between end-of-file and error; callers must use feof(3) and ferror(3) to determine which occurred.
      */
      /* Read from the source file. */
      n = fread(cpbuf, sizeof(char), BUFFER_SIZE, src);

      if (n < BUFFER_SIZE && ferror(src)) { // Bail if an error occurs.
        fprintf(stderr, "An error occurred for file name %s.\n", src_name);
        perror("Parent");
        return -1;
      }

      /* Parent. write to the pipe */
      nw = write(fd[WRITE_END], cpbuf, n); // Note: The textbook doesn't check for errors here. I leave it that way.
      if (nw == -1) {
        fprintf(stderr, "Parent. Error writing to pipe.\n");
        perror("Parent");
        return -1;
      }
      printf("Parent wrote %d bytes.\n", nw);
    }

    /* Failed to close the file, but we already the its contents so we can still continue. Inform user. */
    if (fclose(src))
      fprintf(stderr, "fclose failed for file name %s.\n", src_name);

    /* close the write end of the pipe */
    close(fd[WRITE_END]);
  } else { /* child process */
    /* Open the destination file */
    dest = fopen(dest_name, "w");
    if (dest == NULL) {
      fprintf(stderr, "fopen failed for file name %s.\n", dest_name);
      return -1;
    }
    /* Child read from pipe */
    /* close the unused end of the pipe */
    close(fd[WRITE_END]);

    /* read from the pipe */
    while ( (n = read(fd[READ_END], cpbuf, BUFFER_SIZE)) != 0 ) {
      if(n == -1) {
        // Bail if reading from the pipe fails.
        fprintf(stderr, "Child. An error occurred reading from the pipe.\n");
        perror("Child");
        return -1;
      }
      printf("Child read %d bytes.\n", n);
      /* man fwrite
        If an error occurs, or the end-of-file is reached, the return value is a short object count (or zero).
        The function fwrite() returns a value less than nitems only if a write error has occurred.
      */
      /* Write to the destination file. */
      nw = fwrite(cpbuf, sizeof(char), n, dest);
      if (nw < n) {
        fprintf(stderr, "Error writing to file name %s.\n", dest_name);
        perror("Child");
        return -1;
      }
    }

    /* close the read end of the pipe */
    close(fd[READ_END]);

    if (fclose(dest))
      fprintf(stderr, "fclose failed for file name %s.\n", dest_name);
  }

  return 0;
}