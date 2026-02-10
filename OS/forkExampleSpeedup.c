#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <time.h>

#define LIMIT 100000000 // Large number for a heavy task

// A CPU-heavy function to simulate work
void perform_heavy_task(long start, long end) {
    double result = 0;
    for (long i = start; i < end; i++) {
        result += sqrt(i);
    }
    // Just to prevent compiler from optimizing the loop away
    (void)result; 
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {
    double start_time, end_time;

    // --- Part 1: Single-process Execution ---
    printf("Starting sequential (single-process) task...\n");
    start_time = get_time();
    perform_heavy_task(0, LIMIT);
    end_time = get_time();
    printf("Sequential Time: %.4f seconds\n\n", end_time - start_time);

    // --- Part 2: Multi-process Execution (using fork) ---
    printf("Starting parallel (multi-process) task with fork()...\n");
    start_time = get_time();

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } 
    else if (pid == 0) {
        // Child Process: Handles the first half
        perform_heavy_task(0, LIMIT / 2);
        exit(0); 
    } 
    else {
        // Parent Process: Handles the second half
        perform_heavy_task(LIMIT / 2, LIMIT);
        
        // Wait for child to finish
        wait(NULL); 
        end_time = get_time();
        printf("Parallel Time: %.4f seconds\n", end_time - start_time);
    }

    return 0;
}
