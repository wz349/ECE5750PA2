#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "thpool.h"
#define BILLION 1000000000L

// global variable
threadpool thpool;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int   sum = 0;
int   tid = 0;
int*  max_val;
int   count;
int NTHREADS, SIZE;

// arguments for worker
typedef struct {
    int* board;
    int  col;
    int  DEPTH;
} GM;

// prototype
void extra_job();
void worker( void* varg ); 


void increment() {
	pthread_mutex_lock(&mutex);
  if ( sum < 10 ) {
    thpool_add_work( thpool, (void*)extra_job, NULL );
  }
	sum ++;
	pthread_mutex_unlock(&mutex);
}

void extra_job() {
  pthread_mutex_lock(&mutex);
  printf("extra job assigned\n");
  sum ++;
  pthread_mutex_unlock(&mutex);
}

int is_safe( int* board, int col )
{
    int i;
    /* Check this row on left side */
    for ( i = 0; i < col; i++ )
            //  Q on the left       |  Q on the top left                |    Q on the bot left
        if (board[i] == board[col] || board[i] - i == board[col] - col || board[col] + col == board[i] + i )
            return 0;
 
    return 1;
}

// keep using the same board, generate new only for new works
void solve( int col, int* board, int end )
{
    int i;
    if (col == SIZE)
    {
      // reach the end of board
      pthread_mutex_lock(&mutex);

      count++;
      pthread_mutex_unlock(&mutex);
      return; 
    }
    else if(col == end + 2) {
      // reach the end of depth
      /*
      // generate work
      int* temp_board = (int*)malloc( SIZE * sizeof(int) );
      // copy the board
      for ( i = 0; i < end + 1; i++) {
        temp_board[i] = board[i];
      }
      GM *arg = malloc(sizeof(*arg));
      arg->board = temp_board;
      arg->col   = 1;
      arg->depth = DEPTH;
      
      thpool_add_work( thpool, (void*)worker, arg);

      free(arg);
      // end
      return;
      */
     // do nothing
    }

    for (i = 0; i < SIZE; i++) {
      // try place it at the row
      board[col] = i; 
      if ( is_safe ( board, col ) ) {
        solve(col + 1, board, end);
      }
      // backtrack
      board[col] = -1;
    }
}

void worker( void* varg ) {
  GM* arg    = varg;
  int* board = arg->board;
  int col    = arg->col;
  int end    = col + arg->DEPTH;
  puts("tst3");
  solve(col, board, end);
  free(arg);
  free(board);
}

int main(int argc, char *argv[]){
  struct timespec start, end;
  double time;
	
	char* p;
	if (argc != 3){
		puts("This testfile needs excactly 2 arguments");
		exit(1);
	}
	SIZE     = strtol(argv[1], &p, 10);
	NTHREADS = strtol(argv[2], &p, 10);
  int DEPTH    = SIZE;
    // DEPTH    = strtol(argv[3], &p, 10);

	thpool = thpool_init(NTHREADS);
	
  int* temp_board;
  puts("tst1");
  clock_gettime(CLOCK_MONOTONIC, &start);
	for ( size_t i = 0; i<SIZE; i++) {
    // generate new board
		temp_board = (int*) malloc( SIZE * sizeof(int) );
    // place on ith row
    temp_board[0] = i; 
    // generate args
    GM *arg = malloc(sizeof(*arg));
    // where to free the board?
    arg->board = temp_board;
    arg->col   = 1;
    arg->DEPTH = DEPTH;

    thpool_add_work( thpool, (void*)worker, arg);
    
	}
	
	thpool_wait(thpool);
  clock_gettime(CLOCK_MONOTONIC, &end);
  time = BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
  time = time / BILLION;
  

	printf("# of solution: %d\n", count);

  printf("Elapsed time: %lf seconds\n", time);
  thpool_destroy(thpool);

	return 0;
}
