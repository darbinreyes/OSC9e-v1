//#include <sys/types.h>
#include <stdio.h> // fprintf(), printf()
#include <string.h> // strlen()
#include <unistd.h> // pipe(), close(), write()
#include <ctype.h> // toupper() etc.

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

void toggle_str_case(char *s);

int main(void) {
  char write_msg[BUFFER_SIZE] = "Edsger Dijkstra";
  char read_msg[BUFFER_SIZE];

  int p2c_fd[2]; // Parent(write) to child(read) fd.
  int c2p_fd[2]; // child(write) to parent(read) fd.
  pid_t pid;

  /* create the pipe */
  if (pipe(p2c_fd) == -1) {
    fprintf(stderr, "Pipe failed");
    return 1;
  }

  /* create the 2nd pipe */
  if (pipe(c2p_fd) == -1) {
    fprintf(stderr, "Pipe 2 failed");
    return 2;
  }

  /* fork a child process */
  pid = fork();

  if (pid < 0) { /* error occurred */
    fprintf(stderr, "Fork failed");
    return 3;
  }

  if (pid > 0) { /* parent process */
    /* Parent write */

    /* close the unused end of the pipe */
    close(p2c_fd[READ_END]);

    /* write to the pipe */
    write(p2c_fd[WRITE_END], write_msg, strlen(write_msg) + 1);

    /* close the write end of the pipe */
    close(p2c_fd[WRITE_END]);
    /* Parent read */

    /* close the unused end of the pipe */
    close(c2p_fd[WRITE_END]);

    /* read from the pipe */
    read(c2p_fd[READ_END], read_msg, BUFFER_SIZE);
    printf("Parent read. %s\n", read_msg);

    /* close the read end of the pipe */
    close(c2p_fd[READ_END]);
  } else { /* child process */
    /* Child read */

    /* close the unused end of the pipe */
    close(p2c_fd[WRITE_END]);

    /* read from the pipe */
    read(p2c_fd[READ_END], read_msg, BUFFER_SIZE);
    printf("Child read. %s\n", read_msg);

    /* close the read end of the pipe */
    close(p2c_fd[READ_END]);

    /* Child write */

    /* close the unused end of the pipe */
    close(c2p_fd[READ_END]);

    /* reverse casing of string */
    toggle_str_case(read_msg);

    /* write to the pipe */
    write(c2p_fd[WRITE_END], read_msg, strlen(read_msg) + 1);

    /* close the write end of the pipe */
    close(c2p_fd[WRITE_END]);
  }

  return 0;
}

void toggle_str_case(char *s) {
  char c;

  while ( (c = *s) != '\0') {
    if(isupper(c)) {
      *s = tolower(c);
    } else if(islower(c)) {
      *s = toupper(c);
    }

    s++;
  }
}