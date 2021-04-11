/*

    Chapter 3, programming project 1, part 2.

    A basic shell implementation with support for command history.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define LINE_BUFFER_SIZE 80 // Given by problem.
#define HISTORY_SIZE 3 // Given by problem.

int parse_input(char *line, char **args, int max, int *no_wait);
void run_cmd(char **args, int no_wait);
int parse_input2(char **args, int *is_exit_cmd, int *is_hist_cmd, \
                 int *is_bangbang_cmd, int *is_bang_cmd);
void print_history (void);
int get_history_mr_cmd(char *line);
int get_history_Nth_cmd(int N, char *line);
void add_history(char *line);

/* Array of input line buffers. */
static char history_lines[HISTORY_SIZE][LINE_BUFFER_SIZE];
/*

    Circular array, history_ptrs[history_mri] always points to the most recent
    command e.g.:

    history_ptrs[history_mri], most recent
    history_ptrs[history_mri - 1], second most recent
    history_ptrs[history_mri - 2], third most recent
    ...
    etc.

*/
static char *history_ptrs[HISTORY_SIZE];
static int history_mri = 0; /* mri = most recent command index */
/*

    Monotonic total command count. This program does not handle an overflow of
    this variable.

*/
static int history_count = 0;

static char *args[LINE_BUFFER_SIZE/2+1]; // Given by problem.
static char line[LINE_BUFFER_SIZE];
static char line_tokenized[LINE_BUFFER_SIZE];

int main(int argc, char **argv) {
    int should_run = 1;
    char *l;
    int no_wait;
    int is_line_from_history = 0;
    int N;
    /* Input flags.*/
    int is_exit_cmd, is_hist_cmd, is_bangbang_cmd, is_bang_cmd;

    while (should_run) {

        if (is_line_from_history) {
            /*

                This command was fetched from the command history, skip fetching
                input from user.

            */
            is_line_from_history = 0;
        } else {
            printf("darbinsshell> ");

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
            if (strlen(line) == LINE_BUFFER_SIZE - 1 && \
                line[LINE_BUFFER_SIZE - 2] != '\n') {

                fprintf(stderr, "Input line was too long.\n");
                return 2;
            }
        }

        strcpy(line_tokenized, line);

        /* Parse the input line */
        if (parse_input(line_tokenized, args, sizeof(args)/sizeof(args[0]),    \
            &no_wait)) {

            fprintf(stderr, "Input line had too many tokens.\n");
            return 1;
        }

        if (!parse_input2(args, &is_exit_cmd, &is_hist_cmd, &is_bangbang_cmd,  \
            &is_bang_cmd)) {
            continue;
        }

        if (!is_bangbang_cmd && !is_bang_cmd) {
            /*

                Bash does not include !! or !N  in the history, instead we save
                a copy of the fetched command. Also, bash places the history
                command in command history before displaying the history. Adding
                the history here mimics bash.

            */
            add_history(line);
        }

        /*

            Check if the user wants to exit, otherwise try to run the given
            command.

        */
        if (is_exit_cmd) {
            printf("Ok. Goodbye!\n");
            /*

                I don't see the point of this, but it is given by the problem
                statement.

            */
            should_run = 0;
            exit(0);
        } else if (is_hist_cmd) {
            print_history();
        } else if (is_bangbang_cmd) {
            is_line_from_history = get_history_mr_cmd(line);
        } else if (is_bang_cmd) {
            N = atoi(&args[0][1]);
            is_line_from_history = get_history_Nth_cmd(N, line);
        } else  {
            run_cmd(args, no_wait);
        }


    }

    return 0;
}

/*

    Second step of input parsing. Returns several boolean flags. Returns 0 if
    `args` is empty, none of the flags are set. Otherwise returns 1, flags are
    set accordingly.

*/
int parse_input2(char **args, int *is_exit_cmd, int *is_hist_cmd,              \
    int *is_bangbang_cmd, int *is_bang_cmd) {

    *is_exit_cmd = 0;
    *is_hist_cmd = 0;
    *is_bang_cmd = 0;
    *is_bangbang_cmd = 0;

    if (args[0] == NULL)
        return 0;
    /*

        Check if the user wants to exit, otherwise try to run the given command.

    */
    if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
        *is_exit_cmd = 1;
    }

    if (args[0] != NULL && strcmp(args[0], "history") == 0) {
        *is_hist_cmd = 1;
    }

    if (args[0] != NULL && strcmp(args[0], "!!") == 0) {
        *is_bangbang_cmd = 1;
    }

    if (args[0] != NULL && args[0][0] == '!' && isdigit(args[0][1])) {
        *is_bang_cmd = 1;
    }

    return 1;
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
        /* This prevents the '&' from being interpreted as an argument. */
        args[i - 1] = NULL;
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

void print_history (void) {
    int i, hi; // hi = history index.

    /*
        i < history_count ensures correct printing before the
        history is full.
    */
    for (i = 0; i < HISTORY_SIZE && i < history_count; i++) {
        hi = history_mri - i;
        /*

            This accounts for the fact that we are iterating over the circular
            history array with decreasing indexes.

        */
        if (hi < 0)
            hi += HISTORY_SIZE;
        printf("  %d   %s\n", history_count - i, history_ptrs[hi]);
    }
}

/*

    Returns 1 if most recent command exists and copies it to `line`.
    Returns 0 Otherwise.

*/
int get_history_mr_cmd(char *line) {
    if (history_count == 0) {
        printf("No such command. History is empty.\n");
        return 0;
    }

    /* Echo most recent command. */
    printf("%s", history_ptrs[history_mri]);

    /* Return most recent command. */
    strcpy(line, history_ptrs[history_mri]);

    return 1;
}

/*

    Copies to `line` a command from the history identified by `N`.

    Returns 1 if the command exists. Returns 0 Otherwise.

*/
int get_history_Nth_cmd(int N, char *line) {
    int i, hi; // hi = history index.

    if (history_count == 0) {
        printf("No such command. History is empty.\n");
        return 0;
    }

    /* ******* Scratch work *******
        assert history_count > 0.

        When history is not full, history_count < HISTORY_SIZE
        if history_count == 1
            N == 1 is the only valid value.
        if history_count == 2
            N == 2, 1 OK
        if history_count == 3, history is full, code below works.

        Thus,

        if history_count < HISTORY_SIZE
            valid N means
            N >= 1 && N <= history_count

            1 > 1, false
            1 <= 1 - 3 // 1 <= -2, false
            N = 1  is OK
            i = 1 - 1 = 0.
            hi = 0 - 0 = 0.

            0 > 1, false
            0 <= 1 - 3 // 0 <= -2, false

            N = 2, history_count = 2
            2 > 2, false
            2 <= 2 - 3 // 2 <= -1, false
            i = 2 - 2 = 0.
            hi = 1 - 0 = 1

    ************** */

    // When history is non-empty and not full, this validates N.
    if (history_count < HISTORY_SIZE) {
        if (N < 1 || N > history_count) {
            printf("No such command.\n");
            return 0;
        }
    }

    // When history is full, this validates N.
    if (N > history_count || N <= history_count - HISTORY_SIZE) {
        printf("No such command.\n");
        return 0;
    }

    /* ******* Scratch work *******
        assert N <= history_count && N > history_count - HISTORY_SIZE
        Use N to identify the command in command history.

        N = most recent command.

        if N == history_count
            return history_ptrs[history_mri]

        if history_count - N == 0
            return history_ptrs[history_mri]

        // case N = 11

        history_count - N = 12 - 11 = 1

        // case N = 10

        history_count - N = 12 - 10 = 2


    ************** */

    i = history_count - N;
    hi = history_mri - i;

    if (hi < 0)
        hi += HISTORY_SIZE;

    /* Echo command. */
    printf("%s", history_ptrs[hi]);

    /* Return command. */
    strcpy(line, history_ptrs[hi]);

    return 1;
}

void add_history(char *line) {
    if (history_count < HISTORY_SIZE)  {
        /* History is not full yet. */
        strcpy(history_lines[history_count], line); // Save input line.
        /*
            Assuming a history of size 3.
            1st command is stored in 0
            2nd command is stored in 1
            3rd command is stored in 2

        */
        history_ptrs[history_count] = history_lines[history_count];
        history_mri = history_count;
    } else {
        /* History is stored in a circular array. */
        /*

            Assuming a history of size 3.

            1st command is stored in 0
            2nd command is stored in 1
            3rd command is stored in 2
            history_count >= HISTORY_SIZE
            4th command is stored in 0
            5th command is stored in 1
            etc.

        */
        history_mri = (history_mri + 1) % HISTORY_SIZE;
        strcpy(history_lines[history_mri], line);
    }

    history_count++;

}