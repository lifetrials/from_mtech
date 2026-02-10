#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <time.h>

#define LIMIT 200000000 // Total work units

void perform_heavy_task(long start, long end) {
    double result = 0;
    for (long i = start; i < end; i++) {
        result += sqrt(i);
    }
    (void)result; 
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {
    // 1. Detect the number of available CPU cores
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores < 1) num_cores = 1;

    printf("Detected %ld CPU cores. Starting parallel task...\n", num_cores);

    long chunk_size = LIMIT / num_cores;
    double start_time = get_time();

    // 2. Loop to fork one child process per core
    for (int i = 0; i < num_cores; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } 
        else if (pid == 0) {
            // Each child process works on its own range
            long start = i * chunk_size;
            long end = (i == num_cores - 1) ? LIMIT : (i + 1) * chunk_size;
            
            perform_heavy_task(start, end);
            exit(0); // Child must exit to avoid forking its own children
        }
    }

    // 3. Parent waits for ALL children to finish
    for (int i = 0; i < num_cores; i++) {
        wait(NULL);
    }

    double end_time = get_time();
    printf("Total Parallel Time (%ld processes): %.4f seconds\n", num_cores, end_time - start_time);

    return 0;
}
