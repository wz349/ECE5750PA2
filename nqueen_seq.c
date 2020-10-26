#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "thpool.h"
#define BILLION 1000000000L

// global variable

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

void update_score( int board[], size_t N )
{
  int score = 0;
  for ( size_t i = 0; i < N; i++ ) {
    score += abs( i - board[i]);
  }
  if (score > max_val) {
    max_val = score;
  }
  for ( size_t i = 0; i < N; i++ ) {
    max_board[i] = board[i];
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
      count++;
      update_score( board, SIZE );
      return; 
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


int main(int argc, char *argv[]){
  struct timespec start, end;
  double time;
	
	char* p;
	if (argc != 2){
		puts("This testfile needs excactly 1 arguments");
		exit(1);
	}
	SIZE     = strtol(argv[1], &p, 10);
  DEPTH    = SIZE;
  // DEPTH    = strtol(argv[3], &p, 10);
  max_board = (int*) malloc( SIZE * sizeof(int) );

  clock_gettime(CLOCK_MONOTONIC, &start);
	
  int* temp_board = (int*) malloc( SIZE * sizeof(int) );
  solve(0,temp_board,SIZE);

  clock_gettime(CLOCK_MONOTONIC, &end);

  time = BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
  time = time / BILLION;
  

	printf("# of solution: %d\n", count);
  printf("highest profit : %d with board: \n", max_val);
  for( size_t i = 0; i < SIZE; i++ ) {
    printf("%d ", max_board[i]);
  }
  printf("\n");
  printf("Elapsed time: %lf seconds\n", time);

	return 0;
}
