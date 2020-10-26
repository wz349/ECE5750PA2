#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>

int NTHREADS, SIZE;
int ** hist = NULL;
int * count = NULL;

static void oof(void)
{
    fprintf(stderr, "Out of memory.\n");
    exit(1);
}

void solve(int col, int tid)
{
    /* If NTHREADS does not divide SIZE, you
     * will not cover all rows.
     * Also make sure SIZE >= NTHREADS, otherwise start is always 0.
     */
    int start = (col > 0) ? 0 : tid * (SIZE/NTHREADS);
    int end = (col > 0 || tid == NTHREADS-1) ? SIZE-1 : (tid+1) * (SIZE/NTHREADS) - 1;
    int i, j;
    if (col == SIZE)
    {
        count[tid]++;
    }

    #define attack(i, j) (hist[tid][j] == i || abs(hist[tid][j] - i) == col - j)
    for (i = start; i <= end; i++) {
        for (j = 0; j < col && !attack(i, j); j++);
        if (j < col) continue;

        hist[tid][col] = i;
        solve(col + 1, tid);
    }
}

void *worker(void *arg)
{
    int tid = (int)arg;
    count[tid] = 0;
    solve(0, tid);
}


int main(int argc, char* argv[])
{
    pthread_t* threads;
    int rc, i, j, sum;

    // checking whether user has provided the needed arguments
    if(argc != 3)
    {
        printf("Usage: %s <number_of_queens> <number_of_threads>\n", argv[0]);
        exit(1);
    }


    // passing the provided arguments to the SIZE and NTHREADS 
    // variable, initializing matrices, and allocating space 
    // for the threads
    SIZE = atoi(argv[1]);
    NTHREADS = atoi(argv[2]);
    threads = (pthread_t*)malloc(NTHREADS * sizeof(pthread_t));
    hist = malloc(SIZE * sizeof(int*));
    count = malloc(SIZE * sizeof(int));
    if (hist == NULL || count == NULL) oof();
    for (i = 0; i < SIZE; i++)
    {
        hist[i] = malloc(SIZE * sizeof(int));
        if (hist[i] == NULL) oof();
        for (j = 0; j < SIZE; j++)
        {
            hist[i][j] = 0;
        }
    }

    // declaring the needed variables for calculating the running time
    struct timespec begin, end;
    double time_spent;

    // starting the run time
    clock_gettime(CLOCK_MONOTONIC, &begin);

    for(i = 0; i < NTHREADS; i++) {
        rc = pthread_create(&threads[i], NULL, worker, (void *)i);
        assert(rc == 0); // checking whether thread creation was successful
    }

    sum = 0;
    for(i = 0; i < NTHREADS; i++) {
        rc = pthread_join(threads[i], NULL);
        assert(rc == 0); // checking whether thread join was successful
        sum += count[i];
    }


    // ending the run time
    clock_gettime(CLOCK_MONOTONIC, &end);

    // calculating time spent during the calculation and printing it
    time_spent = end.tv_sec - begin.tv_sec;
    time_spent += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    printf("Elapsed time: %.2lf seconds.\n", time_spent);

    printf("\nNumber of solutions: %d\n", sum);

    for (i = 0; i < SIZE; i++)
    {
        free(hist[i]);
    }
    free(hist); free(count);
    return 0;

}