#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>


#define SIZE 4

int accept_matrix(int arr[][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            scanf("%d", &arr[i][j]);
        }
    }
}

void display_matrix(int arr[][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    int i, j, k, a1[SIZE][SIZE], a2[SIZE][SIZE];

    // create shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(int[SIZE][SIZE]), IPC_CREAT | 0666);
    if(shmid < 0)
    {
        printf("shmget error\n");
        exit(1);
    }

    // attach shared memory
    int (*p)[SIZE] = (int (*)[SIZE])shmat(shmid, NULL, 0);
    printf("Enter elements of first matrix:\n");
    accept_matrix(a1);
    printf("Enter elements of second matrix:\n");
    accept_matrix(a2);
    printf("First ");
    display_matrix(a1);
    printf("Second ");
    display_matrix(a2);


    // each child process will calculate one row of resultant matrix and exit, hence only 5 processes will be created
    for(i = 0; i < SIZE; i++)
    {
        int pid = fork();
        if(pid == 0)
        {
            printf("Child Process %d calculating row %d\n", getpid(), i);
            for(j = 0; j < SIZE; j++)
            {
                p[i][j] = 0;
                for(k = 0; k < SIZE; k++)
                {
                    p[i][j] += (a1[i][k]) * (a2[k][j]);
                }
            }
            printf("Child Process %d finished calculating row %d\n", getpid(), i);
            sleep(i*5);
            // detach shared memory
            shmdt(p);
            exit(0);
        }
    }

    // parent process waits for all child processes to complete
    for(i = 0; i < SIZE; i++)
        wait(NULL);

    printf("Resultant\n");
    display_matrix(p);
    // detach shared memory
    shmdt(p);
    // destroy shared memory
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
