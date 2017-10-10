/* The Eight Queens Puzzle is a classic strategy game problem that 
 * consist of a chessboard and eight chess queens. Following the chess 
 * game’s rules, the objective is to situate the queens on  the board 
 * in such a way that all of them are safe, this means that no queen 
 * can attack each other. The puzzle was originally described by the 
 * chess composer Max Bezzel and extended by Franz Nauck to be a 
 * N-Queens Puzzle, with N queens on a chessboard of  N×N squares.
 * 
 * This is a secuential implementation using recursion and backtracing.
 * It calculates the time it takes to solve the problem given N number
 * of queens. Finally, it shows the total number of solutions and 
 * prints to stdout (e.g. the screen) one of them. To compile it may be 
 * necessary to add the option -D_BSD_SOURCE to be able to use the 
 * timing functions.
 * 
 * 
 * Compilation
 *	gcc -D_BSD_SOURCE -o queens queens.c
 * 
 * Execution 
 *	./queens [number_of_queens]
 * 
 *
 * File: queens.c			Author: Manases Galindo
 * Date: 09.02.2017
 */



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define NUM_QUEENS 8			/* default number of queens	*/
#define ITS_SAFE   0			/* queen on a safe position	*/
#define NOT_SAFE   1			/* or not			*/


/* Shared global variables.				  	  	*/
int solutions,		  		/* total number of solutions	*/
    *queen_on,				/* track positions of the queen */
    nq;					/* number of queens		*/		
char **board;				/* NxN chess board		*/
char show = 1;				/* flag to save the board	*/


void nqueens (int);	  		/* find total solutions		*/
int is_safe (int, int, int);		/* is queen in a safe position?	*/
void pboard (void);			/* show a solution on stdout	*/


int main (int argc, char **argv)
{
  int i;				/* loop variable		*/
  struct timeval tval_before,		/* timing variables		*/
	 tval_after, tval_result;

  switch (argc)				/* check command line arguments	*/
  {
    case 1:
      /* Number of queens not specified, using the default value	*/
      nq = NUM_QUEENS;
      break;

    case 2:
      /* Using the specified number of queens				*/
      nq = atoi (argv[1]);
      if (nq < 1)
      {
	fprintf (stderr, "Error: wrong number of queens.\n"
		 "Usage:\n"
		 "  %s [number_of_queens]\n"
		 "number_of_queens should be > 0\n"
		 "Using default number of queens (%d).\n",
		 argv[0], NUM_QUEENS);
	nq = NUM_QUEENS;
      }
      break;

    default:
      fprintf (stderr, "Error: wrong number of parameters.\n"
	       "Usage:\n"
	       "  %s [number_of_queens]\n",
	       argv[0]);
      exit (EXIT_FAILURE);
  }

  /* allocate memory for all dynamic data structures and validate them 	*/
  queen_on = (int *) malloc(nq * sizeof (int));
  board = (char **) malloc(nq * sizeof (char *));
  
  if ((queen_on == NULL) || (board == NULL))
  {
    fprintf (stderr, "File: %s, line %d: Can't allocate memory.",
	     __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }

  for (i = 0; i < nq; i++) 
  {
    board[i] = (char *) malloc(nq * sizeof (char));
    if (board[i] == NULL)
    {
      fprintf (stderr, "File: %s, line %d: Can't allocate memory.",
	       __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }
  }

  /* Get start time and solve the nqueens			 	*/
  solutions = 0;
  gettimeofday(&tval_before, NULL);
  nqueens (0);
  /* calculate and show the elapsed time			  	*/
  gettimeofday(&tval_after, NULL);
  timersub(&tval_after, &tval_before, &tval_result);
  printf("\nElapsed time: %ld.%06ld", (long int)tval_result.tv_sec, 
       (long int)tval_result.tv_usec);

  printf ("\nThere are %d solutions for %d queens. Here's one of them:\n\n",
	 solutions, nq);

  pboard ();				/* show one solution		*/
  
  /* Deallocate any memory or resources associated			*/
  free (queen_on);
  for (i = 0; i < nq; i++) 
  {
    free (board[i]);
  }
  free (board);

  return EXIT_SUCCESS;
}


/* nqueens calculates the total number of solutions using recursion 
 * and backtracing.
 *
 * Input:		col		column of the board
 * Return value:	none
 *
 */
void nqueens (int col)
{
  int i, j;				/* loop variables		*/

  if (col == nq)			/* tried N queens permutations  */
  {
    solutions++;			/* one solution found		*/
    if (show-- > 0)
    {
      for (i = 0; i < nq; i++) 
      {
	for (j = 0; j < nq; j++)
	{
	  /* check if the queen is situated on (i,j) position on board */
	  board[i][j] = (j == queen_on[i] ? 'Q' : ((i + j) & 1) ? '_' : '_');
	}
      }
    }
    return;
  }

  /* Backtracking - try next column on recursive call */
  for (i = 0; i < nq; i++)
  {
    for (j = 0; j < col && is_safe(i, j, col); j++);
    if (j < col) {
      continue;
    }
    queen_on[col] = i;
    nqueens (col + 1);
  }
}


/* is_safe determines if a queen does not attack other
 *
 * Input:		i, j		board coordinates
 *			col		column of the board
 * Return value:	ITS_SAFE	Queen without problems
 *			NOT_SAFE	Queen under attack!
 *
 */
int is_safe (int i, int j, int col)
{
  if (queen_on[j] == i)
  {
    return ITS_SAFE;
  }
  if (abs (queen_on[j] - i) == col - j)
  {
    return ITS_SAFE;
  }

  return NOT_SAFE;
}


/* pboard show a NxN board with one possible solution. All queens
 * are situated in a safe way.
 *
 * Input:		none
 * Return value:	ITS_SAFE	Queen without problems
 *			NOT_SAFE	Queen under attack!
 *
 */
void pboard (void)
{
  int i, j;

  for (i = 0; i < nq; i++)
  {
    for (j = 0; j < nq; j++)
    {
      printf (board[i][j] == 'Q' ? " Q " : " + ");
    }
    printf ("\n");
  }
}
