#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        exit(1);
    }

    if (pipe(pipe1) == -1) { perror("pipe1"); exit(1); }
    if (pipe(pipe2) == -1) { perror("pipe2"); exit(1); }

    // First child: grep
    pid1 = fork();
    if (pid1 == 0) {
        dup2(pipe1[0], STDIN_FILENO); // read from cat
        dup2(pipe2[1], STDOUT_FILENO); // write to sort
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        execlp("grep", "grep", argv[1], NULL);
        perror("execlp grep"); exit(1);
    }

    // Second child: sort
    pid2 = fork();
    if (pid2 == 0) {
        dup2(pipe2[0], STDIN_FILENO); // read from grep
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        execlp("sort", "sort", NULL);
        perror("execlp sort"); exit(1);
    }

    // Parent: cat
    dup2(pipe1[1], STDOUT_FILENO);
    close(pipe1[0]); close(pipe1[1]);
    close(pipe2[0]); close(pipe2[1]);
    execlp("cat", "cat", "scores", NULL);
    perror("execlp cat"); exit(1);
}
