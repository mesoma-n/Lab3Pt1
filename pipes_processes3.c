#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        return 1;
    }

    int pipefd1[2]; // Pipe between cat and grep
    int pipefd2[2]; // Pipe between grep and sort

    pid_t pid1, pid2;

    char *search_term = argv[1];  // Argument passed to grep

    // Create the first pipe
    if (pipe(pipefd1) == -1) {
        perror("Pipe1 failed");
        exit(1);
    }

    // Fork the first process for "cat scores"
    pid1 = fork();

    if (pid1 < 0) {
        perror("Fork failed for cat");
        exit(1);
    }

    if (pid1 == 0) {
        // In child process P1 (cat scores)
        // Replace standard output with pipe1 write end
        dup2(pipefd1[1], STDOUT_FILENO);

        // Close both ends of the pipe1 (after redirecting)
        close(pipefd1[0]);
        close(pipefd1[1]);

        // Execute "cat scores"
        execlp("cat", "cat", "scores", NULL);

        // If execlp fails
        perror("execlp failed for cat");
        exit(1);
    }

    // Create the second pipe
    if (pipe(pipefd2) == -1) {
        perror("Pipe2 failed");
        exit(1);
    }

    // Fork the second process for "grep <search_term>"
    pid2 = fork();

    if (pid2 < 0) {
        perror("Fork failed for grep");
        exit(1);
    }

    if (pid2 == 0) {
        // In child process P2 (grep <search_term>)
        // Replace standard input with pipe1 read end
        dup2(pipefd1[0], STDIN_FILENO);
        // Replace standard output with pipe2 write end
        dup2(pipefd2[1], STDOUT_FILENO);

        // Close all pipe ends (after redirecting)
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        // Execute "grep <search_term>"
        execlp("grep", "grep", search_term, NULL);

        // If execlp fails
        perror("execlp failed for grep");
        exit(1);
    }

    // In parent process (P3 for "sort")
    // Replace standard input with pipe2 read end
    dup2(pipefd2[0], STDIN_FILENO);

    // Close all unused pipe ends
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    // Execute "sort"
    execlp("sort", "sort", NULL);

    // If execlp fails
    perror("execlp failed for sort");
    exit(1);

    return 0;
}