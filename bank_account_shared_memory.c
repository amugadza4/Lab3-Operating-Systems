#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#define TRUE 1

int main() {
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    srand(time(NULL));

    // Create shared memory for 2 integers: BankAccount and Turn
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("shmget error\n");
        exit(1);
    }

    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if ((long)ShmPTR == -1) {
        printf("shmat error\n");
        exit(1);
    }

    ShmPTR[0] = 0; // BankAccount
    ShmPTR[1] = 0; // Turn

    pid = fork();

    if (pid < 0) {
        printf("fork error\n");
        exit(1);
    } 
    else if (pid == 0) {  // Child Process (Poor Student)
        for (int i = 0; i < 25; i++) {
            sleep(rand() % 6); // sleep 0–5 seconds
            int account = ShmPTR[0];

            while (ShmPTR[1] != 1) ; // wait for Turn == 1

            int balance = rand() % 51; // 0–50
            printf("Poor Student needs $%d\n", balance);

            if (balance <= account) {
                account -= balance;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", account);
            }

            ShmPTR[0] = account; // update BankAccount
            ShmPTR[1] = 0;       // set Turn to 0 (Parent’s turn)
        }
        exit(0);
    } 
    else {  // Parent Process (Dear Old Dad)
        for (int i = 0; i < 25; i++) {
            sleep(rand() % 6); // sleep 0–5 seconds
            int account = ShmPTR[0];

            while (ShmPTR[1] != 0) ; // wait for Turn == 0

            if (account <= 100) {
                int balance = rand() % 101; // 0–100
                if (balance % 2 == 0) {
                    account += balance;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
                } else {
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
            }

            ShmPTR[0] = account; // update BankAccount
            ShmPTR[1] = 1;       // set Turn to 1 (Child’s turn)
        }

        wait(NULL); // wait for child
        shmdt((void *)ShmPTR);
        shmctl(ShmID, IPC_RMID, NULL);
        printf("Parent and child processes completed.\n");
    }

    return 0;
}

