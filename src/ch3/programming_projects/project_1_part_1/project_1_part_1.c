#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINE_BUFFER_SIZE 80

int parse_input(char *line, char **args, int max, int *no_wait);
void run_cmd(char **args, int no_wait);

int main(int argc, char **argv) {
    char *args[LINE_BUFFER_SIZE/2+1];
    int should_run = 1;
    char line[LINE_BUFFER_SIZE];
    char *l;
    int no_wait;

    while (should_run) {
        printf("darbinsshell> ");
        //fflush(stdout); // The purpose of calling this function is unclear.

        /* Get a line of input */
        line[0] = '\0';
        l = fgets(line, LINE_BUFFER_SIZE, stdin);

        /* Check if fgets encountered an error */
        if (l == NULL && ferror(stdin)) {
            fprintf(stderr, "fgets error. Bye.\n");
            return 1;
        }

        /* User press control+d key. Treat it as a request to terminate. */
        if (l == NULL && feof(stdin)) {
            fprintf(stderr, "fgets got eof. Bye.\n");
            return 0;
        }

        /* Check if the input line was too long */
        if (strlen(line) == LINE_BUFFER_SIZE - 1 && line[LINE_BUFFER_SIZE - 2] != '\n') {
            fprintf(stderr, "Input line was too long.\n");
            return 2;
        }

        /* Parse the input line */
        if (parse_input(line, args, sizeof(args)/sizeof(args[0]), &no_wait)) {
            fprintf(stderr, "Input line had too many tokens.\n");
            return 1;
        }

        /* Check if the user wants to exit, otherwise try to run the given command */
        if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
            printf("Ok. Goodbye!\n");
            should_run = 0;
        } else if (args[0] != NULL) {
            run_cmd(args, no_wait);
        }
    }

    return 0;
}

/*

    Parses line into space delimited tokens and stores the result in args. The
    format of args is identical to the typical argv argument passed to main().
    The last token in args is indicated by a subsequent NULL pointer.

    Returns 0 if successful, otherwise an error occurred.

    At most max - 1 tokens are returned. If there are more than max - 1 tokens
    parsing stops, args should not be used, this is considered an error, 1 is
    returned. `no_wait` indicates the presence of an '&' character as the final
    token in line. If no_wait == 0, '&' was not present, if == 1, '&' was
    present. If '&' is present as the final token in line, the '&' is not
    returned in args.

*/
int parse_input(char *line, char **args, int max, int *no_wait) {
    int i = 0;
    char *s;

    *no_wait = 0;

    s = strtok(line, " \n");

    while (s != NULL && i < max - 1) {
        args[i] = s;
        i++;
        s = strtok(NULL, " \n");
    }

    args[i] = NULL;

    if (s != NULL && i == max - 1) {
        return 1;
    }


    if (i > 0 && args[i - 1][0] == '&') {
        *no_wait = 1;
        args[i - 1] = NULL; // This prevents the '&' from being interpreted as an argument.
    }

    return 0;
}

/*

    Creates a child process using fork(). If successful, only the parent returns
    from this function. After the fork, the child calls execvp() to execute the
    command specified by args. If no_wait == 0, the parent waits for the child
    to terminate before returning from this function. If no_wait == 1, the
    parent not wait for the child to terminate and returns immediately.

*/
void run_cmd(char **args, int no_wait) {
    pid_t pid;
    int stat_loc;

    pid = fork();

    if (pid < 0) {
        /* fork() failed. Terminate. */
        fprintf(stderr, "fork() error. Bye.\n");
        exit(1);
    } else if (pid > 0) {
        /* Parent process */
        if (no_wait == 0) {
            waitpid(pid, &stat_loc, 0);
        }
    } else {
        /* Child process */
        execvp(args[0], args); // execvp() only returns if an error occurred.
        /* execvp() error. Terminate. */
        fprintf(stderr, "Exec() error.\n");
        exit(1);
    }

}