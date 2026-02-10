#define _GNU_SOURCE  // Required for sched_setaffinity
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <time.h>
#include <sched.h>   // Required for CPU affinity

#define LIMIT 200000000

void perform_heavy_task(long start, long end) {
    double result = 0;
    for (long i = start; i < end; i++) {
        result += sqrt(i);
    }
    (void)result;
}

int main() {
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    long chunk_size = LIMIT / num_cores;

    printf("Spawning %ld processes and pinning each to its own core...\n", num_cores);

    for (int i = 0; i < num_cores; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // --- CORE PINNING LOGIC ---
            cpu_set_t mask;
            CPU_ZERO(&mask);        // Clear the CPU set
            CPU_SET(i, &mask);      // Add core 'i' to the set

            // Apply affinity to the current process (pid 0)
            if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
                perror("sched_setaffinity failed");
                exit(1);
            }

            // Confirm which core the process is actually running on
            printf("Child %d pinned to Core %d (Actual: %d)\n", i, i, sched_getcpu());

            perform_heavy_task(i * chunk_size, (i + 1) * chunk_size);
            exit(0);
        }
    }

    for (int i = 0; i < num_cores; i++) wait(NULL);
    printf("All tasks complete.\n");

    return 0;
}
