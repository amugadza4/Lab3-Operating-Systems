#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipe1[2];  // Pipe from P1 -> P2
    int pipe2[2];  // Pipe from P2 -> P1
    pid_t pid;

    // Create both pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    // -------------------- Parent Process (P1) --------------------
    if (pid > 0) {
        char inputStr[100], recvStr[200];

        close(pipe1[0]); // Close unused read end of pipe1
        close(pipe2[1]); // Close unused write end of pipe2

        printf("Other string is: howard.edu\n");
        printf("Input : ");
        scanf("%s", inputStr);

        // Send input to P2
        write(pipe1[1], inputStr, strlen(inputStr) + 1);

        // Read modified string from P2
        read(pipe2[0], recvStr, sizeof(recvStr));

        // Concatenate "gobison.org"
        strcat(recvStr, "gobison.org");

        // Print final result
        printf("Output : %s\n", recvStr);

        close(pipe1[1]);
        close(pipe2[0]);
    }

    // -------------------- Child Process (P2) --------------------
    else {
        char recvStr[100], sendStr[200], secondInput[100];

        close(pipe1[1]); // Close unused write end of pipe1
        close(pipe2[0]); // Close unused read end of pipe2

        // Read string from P1
        read(pipe1[0], recvStr, sizeof(recvStr));

        // Concatenate "howard.edu"
        strcpy(sendStr, recvStr);
        strcat(sendStr, "howard.edu");
        printf("Output : %s\n", sendStr);

        // Ask user for second input
        printf("Input : ");
        scanf("%s", secondInput);

        // Append second input
        strcat(sendStr, secondInput);

        // Send the new string back to P1
        write(pipe2[1], sendStr, strlen(sendStr) + 1);

        close(pipe1[0]);
        close(pipe2[1]);
    }

    return 0;
}
