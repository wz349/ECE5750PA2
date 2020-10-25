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
int   max_val = -1;
int*  max_board;
int   count;
int   NTHREADS, SIZE, DEPTH;

// arguments for worker
typedef struct {
    int* board;
    int  col;
    int  DEPTH;
} GM;

// prototype
void worker( void* varg ); 
int calc_profit( int* board, size_t N)
{
  int profit = 0;
  for ( size_t i = 0; i < N; i++ ) {
    profit += abs( i - board[i]);
  }
  return profit;
}

void update_score( int board[], size_t N )
{
  int score = 0;
  for ( size_t i = 0; i < N; i++ ) {
    score += abs( i - board[i]);
  }
  if (score > max_val) {
    max_val = score;
    for ( size_t i = 0; i < N; i++ ) {
    max_board[i] = board[i];
    }
  }
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
      update_score( board, SIZE );
      pthread_mutex_unlock(&mutex);
      
      return; 
    }
    else if(col == end + 1) {

      // generate work
      int* temp_board = (int*)malloc( SIZE * sizeof(int) );
      // copy the board
      for ( i = 0; i < end + 1; i++) {
        temp_board[i] = board[i];
      }
      GM *arg = malloc(sizeof(*arg));
      arg->board = temp_board;
      arg->col   = col;
      arg->DEPTH = DEPTH;
      
      thpool_add_work( thpool, (void*)worker, arg);

      // end
      return;

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
  /*
  printf("a worker is assigned with board: ");
  for ( size_t i = 0; i < col; i++) {
    printf("%d ", board[i]);
  }
  printf(" to go to %d col \n",end);
  */
  solve(col, board, end);
  free(arg);
  free(board);
}

int main(int argc, char *argv[]){
  struct timespec start, end;
  double time;
	
	char* p;
	if (argc != 4){
		puts("This testfile needs excactly 3 arguments");
		exit(1);
	}
	SIZE     = strtol(argv[1], &p, 10);
	NTHREADS = strtol(argv[2], &p, 10);
  DEPTH    = strtol(argv[3], &p, 10);
    // DEPTH    = strtol(argv[3], &p, 10);

	thpool = thpool_init(NTHREADS);
	max_board = (int*) malloc( SIZE * sizeof(int) );
  int* temp_board;
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
  printf("highest profit : %d with board: \n", max_val);
  for( size_t i = 0; i < SIZE; i++ ) {
    printf("%d ", max_board[i]);
  }
  printf("\n");
  printf("recalculate the board's profit is %d\n ",calc_profit(max_board,SIZE));
  printf("Elapsed time: %lf seconds\n", time);
  thpool_destroy(thpool);

	return 0;
}
