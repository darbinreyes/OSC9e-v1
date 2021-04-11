#include <stdio.h> // printf
#include <unistd.h> // fork() , sleep()

int main(void) {
  pid_t pid;

  printf("Parent process is about to call fork().\n");

  pid = fork();

  if (pid < 0) {
    printf("fork() failed.\n");
  } else if (pid == 0) {
    printf("This is the child process. Bye.\n");
    return 0;
  } else {
    printf("This is the parent process. Child's pid = %d. Going to sleep.\n", pid);
    sleep(10);
    printf("\nParent process awakened.\n");
  }


  return 0;
}