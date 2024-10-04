#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep <term>". In this implementation,
 * the parent doesn't wait for the child to finish, and so the command prompt
 * may reappear before the child terminates.
 */

int main(int argc, char **argv)
{
    int pipefd[2];  // Pipe file descriptors
    int pid;

    // Default search term is "Lakers" if no argument is provided
    char *search_term = "Lakers";

    // If a command-line argument is provided, use that as the search term
    if (argc == 2) {
        search_term = argv[1];
    }

    // Create arguments for cat and grep
    char *cat_args[] = {"cat", "scores", NULL};
    char grep_command[256];
    snprintf(grep_command, sizeof(grep_command), "grep %s", search_term);
    char *grep_args[] = {"grep", search_term, NULL};

    // Create a pipe (fds go in pipefd[0] and pipefd[1])
    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        exit(1);
    }

    // Fork a new process
    pid = fork();

    if (pid < 0) {
        // Error handling for fork failure
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
    // Child process - handles "grep <search_term>"

        // Replace standard input with the read end of the pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2 failed for child");
            exit(1);
        }

        // Close the write end of the pipe, since it's not used by the child
        close(pipefd[1]);

        // Execute grep with the provided or default argument
        execvp("grep", grep_args);

        // If execvp fails, handle the error
        perror("execvp failed for grep");
        exit(1);
    }
    else {
    // Parent process - handles "cat scores"

        // Replace standard output with the write end of the pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2 failed for parent");
            exit(1);
        }

        // Close the read end of the pipe, since it's not used by the parent
        close(pipefd[0]);

        // Execute cat
        execvp("cat", cat_args);

        // If execvp fails, handle the error
        perror("execvp failed for cat");
        exit(1);
    }

    return 0;
}
