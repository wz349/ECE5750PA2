/* C/C++ program to solve N Queen Problem using
backtracking */
#include <sys/queue.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

// use of 1 D array to represent the board, board[i] represent col (i-1) the value of board[i] 
// represent the y location of the Queen
// Coordinates: 
//    0  1  2  3  4  5
//   ___________________
// 0 |__|__|__|__|__|__|
// 1 |__|__|__|__|__|__|
// 2 |__|__|__|__|__|__|
// 3 |__|__|__|__|__|__|
// 4 |__|__|__|__|__|__|
// 5 |__|__|__|__|__|__|

typedef struct { 
  int* board;
  int  score;
} sol_t;


int calculate_score( int board[], size_t N )
{
  int score = 0;
  for ( size_t i = 0; i < N; i++ ) {
    score += abs( i - board[i]);
  }
  return score;
}
 
// isSafe 1-D Ver
int isSafe(int* board, int col, size_t N )
{
    int i;
    /* Check this row on left side */
    for (i = 0; i < col; i++)
            //  Q on the left       |  Q on the top left                |    Q on the bot left
        if (board[i] == board[col] || board[i] - i == board[col] - col || board[col] + col == board[i] + i )
            return 0;
 
    return 1;
}
 
/* A recursive utility function to solve N
Queen problem */
int solveNQUtil(int* board, int col, size_t N, list_sol_t* solList)
{
    /* base case: If all queens are placed
    then return true */
    if (col == N)
    {   
        // add sol to the solution list
        printSolution(board);
        return 1;
    }
 
    /* Consider this column and try placing
    this queen in all rows one by one */
    int res = 0;
    for (int i = 0; i < N; i++)
    {
        /* Check if queen can be placed on
        board[i][col] */
        board[col] = i;
        if ( isSafe(board, col, N) )
        {
            res = solveNQUtil(board, col + 1, N, solList) || res;

            board[col] = 0; // BACKTRACK
 
        }
    }

    return res;
}
 
/* This function solves the N Queen problem using
Backtracking. It mainly uses solveNQUtil() to
solve the problem. It returns false if queens
cannot be placed, otherwise return true and
prints placement of queens in the form of 1s.
Please note that there may be more than one
solutions, this function prints one of the
feasible solutions.*/
void solveNQ(size_t N, list_sol_t* solList)
{   
  int* board = (int*) malloc( N * sizeof(int) );

  if (solveNQUtil(board, 0, N, solList) == 0)
  {
      printf("Solution does not exist");
      return ;
  }
  return ;
}
 
// driver program to test above function
int main( int argc, char **argv )
{ 
  int n,p;

  if (argc != 2) {
        printf("Usage: bksb n\nAborting...\n");
        exit(0);
  }

  n = atoi(argv[1]);
  p = atoi(argv[2]);

  list_sol_t solList;
  list_sol_construct( &solList );

  solveNQ( n, &solList );

  list_sol_destruct( &solList );
  return 0;
}