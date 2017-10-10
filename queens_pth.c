/* The Eight Queens Puzzle is a classic strategy game problem that 
 * consist of a chessboard and eight chess queens. Following the chess 
 * game’s rules, the objective is to situate the queens on the board 
 * in such a way that all of them are safe, this means that no queen 
 * can attack each other. The puzzle was originally described by the 
 * chess composer Max Bezzel and extended by Franz Nauck to be a 
 * N-Queens Puzzle, with N queens on a chessboard of N×N squares.
 * 
 * This is a parallel implementation using pthreads, recursion and 
 * backtracing. It calculates the time it takes to solve the problem 
 * given N number of queens. Finally, it shows the total number of 
 * solutions. To compile it may be necessary to add the option 
 * -D_BSD_SOURCE to be able to use the timing functions.
 * 
 * Compilation
 *	gcc -D_BSD_SOURCE -Wall -lpthread -o queens_pth queens_pth.c
 * 
 * Execution 
 *	./queens_pth [number_of_queens] [number_of_threads]
 * 
 *
 * File: queens_pth.c			Author: Manases Galindo
 * Date: 09.02.2017
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_QUEENS 8			/* default number of queens	*/
#define NUM_THREAD 8			/* default number of threads	*/
#define ITS_SAFE   0			/* queen on a safe position	*/
#define NOT_SAFE   1			/* or not			*/


/* Shared global variables.				  	  	*/
int *solutions,				/* number of solutions by thd	*/
    **queen_on,				/* track positions of the queen */
    nq,					/* number of queens		*/
    nthreads;				/* number of threads		*/
    

void *start_thread (void *);    
void nqueens (int, int);		/* find total solutions		*/
int is_safe (int, int, int, int);	/* is queen in a safe position?	*/  


int main (int argc, char **argv)
{
  pthread_t *thr_ids;			/* array of thread ids		*/
  int i, j,				/* loop variables		*/
      *thr_num,				/* array of thread numbers	*/
      total;				/* total number of solutions	*/
  struct timeval tval_before,		/* timing variables		*/
	 tval_after, tval_result;

  switch (argc)				/* check command line arguments	*/
  {
    case 1:
      /* Number of queens not specified, using the default value	*/
      nq = NUM_QUEENS;
      /* Number of threads not specified, using the default value	*/
      nthreads = NUM_THREAD;
      break;

    case 2:
      /* Number of threads not specified, using the default value	*/
      nthreads = NUM_THREAD;
      /* Using the specified number of queens				*/
      nq = atoi (argv[1]);
      if (nq < 1)
      {
	fprintf (stderr, "Error: wrong number of queens.\n"
		 "Usage:\n"
		 "  %s [number_of_queens] [number_of_threads]\n"
		 "number_of_queens should be > 0\n"
		 "Using default number of queens (%d).\n",
		 argv[0], NUM_QUEENS);
	nq = NUM_QUEENS;
      }
      break;
      
    case 3:
      /* Using the specified number of threads				*/
      nthreads = atoi(argv[2]);
      /* Using the specified number of queens				*/
      nq = atoi (argv[1]);
      if ((nq < 1) || (nthreads < 1) || (nthreads > nq))
      {
	fprintf (stderr, "Error: wrong number of queens or threads.\n"
		 "Usage:\n"
		 "  %s [number_of_queens] [number_of_threads]\n"
		 "number_of_queens  should be > 0\n"
		 "number_of_threads should be > 0\n"
		 "number_of_threads can't be bigger than number_of_queens\n"
		 "Using default number of queens and threads (%d).\n",
		 argv[0], NUM_QUEENS);
	nq = NUM_QUEENS;
	nthreads = NUM_THREAD;
      }
      break;

    default:
      fprintf (stderr, "Error: wrong number of parameters.\n"
	       "Usage:\n"
	       "  %s [number_of_queens] [number_of_threads]\n",
	       argv[0]);
      exit (EXIT_FAILURE);
  }
  
  /* allocate memory for all dynamic data structures and validate them 	*/
  thr_num   = (int *) malloc (nthreads * sizeof (int));
  thr_ids   = (pthread_t *) malloc (nthreads * sizeof (pthread_t));
  queen_on  = (int **) malloc (nq * sizeof (int *));
  solutions = (int *) malloc (nq * sizeof (int));
  
  if ((thr_num == NULL) || (thr_ids == NULL) ||
      (queen_on == NULL) || (solutions == NULL)) 
  {
    fprintf (stderr, "File: %s, line %d: Can't allocate memory.",
	     __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }
  
  for (i = 0; i < nq; i++)
  {
    queen_on[i] = (int *) malloc(nq * sizeof (int));
    if (queen_on[i] == NULL)
    {
      fprintf (stderr, "File: %s, line %d: Can't allocate memory.",
	       __FILE__, __LINE__);
      exit (EXIT_FAILURE);
    }
    for (j = 0; j < nq; j++)
    {
      queen_on[i][j] = 0;
    }
  }

  /* Get start time and solve the nqueens			 	*/
  gettimeofday(&tval_before, NULL);
  
  /* Create the threads	and let them do their work		  	*/
  for (i = 0; i < nthreads; i++) 
  {
    thr_num[i] = i;			/* Thread number	        */
    pthread_create (&thr_ids[i], NULL, start_thread, &thr_num[i]); 
  }
  
  /* Sum all solutions by thread to get the total		  	*/
  total = 0;
  for (i = 0; i < nthreads; i++) 
  {
    pthread_join (thr_ids[i], NULL);
    total += solutions[i];
  }
  
  /* calculate and show the elapsed time			  	*/
  gettimeofday(&tval_after, NULL);
  timersub(&tval_after, &tval_before, &tval_result);
  printf("\nElapsed time: %ld.%06ld", (long int)tval_result.tv_sec, 
       (long int)tval_result.tv_usec);
  printf ("\nThere are %d solutions for %d queens.\n\n", total, nq);

  /* Deallocate any memory or resources associated			*/
  for (i = 0; i < nq; i++)
  {
    free (queen_on[i]);
  }
  free (queen_on); 
  free (solutions);
  free (thr_num);
  free (thr_ids);

  return EXIT_SUCCESS;
}


/* start_thread runs as a peer thread and will execute the nqueens
 * function concurrently to find the number of possible solutions
 *
 * Input:		arg		pointer to current thread number
 * Return value:	none
 *
 */
void *start_thread (void *arg)
{
  int thr_index;
    
  /* Get the index number of current thread				*/
  thr_index = *( ( int* )arg );
  /* Set start number of solutions for current thread			*/
  solutions[thr_index] = 0;
  /* Release the Kraken!						*/
  nqueens (0, thr_index);

  pthread_exit (EXIT_SUCCESS);		/* Terminate the thread		*/
}


/* nqueens calculates the total number of solutions using recursion 
 * and backtracing.
 *
 * Input:		col		column of the board
 *			thr_index	number of current thread
 * Return value:	none
 *
 */
void nqueens (int col, int thr_index)
{
  int i, j,				/* loop variables		*/  
      start, end;			/* position variables		*/
  
  /* start/end point to cover all rows					*/
  start = (col > 0) ? 0 : thr_index * (nq / nthreads);
  end	= ((col > 0) || (thr_index == nthreads -1)) ? 
	  nq - 1 : (thr_index + 1) * (nq / nthreads) - 1;

  if (col == nq)			/* tried N queens permutations  */
  {      
    solutions[thr_index]++;		/* peer found one solution 	*/
  }

  /* Backtracking - try next column on recursive call for current thd	*/
  for (i = start; i <= end; i++) 
  {
    for (j = 0; j < col && is_safe(i, j, col, thr_index); j++);
    if (j < col) 
    {
      continue;
    }
    queen_on[thr_index][col] = i;
    nqueens (col + 1, thr_index);
  }
}


/* is_safe determines if a queen does not attack other
 *
 * Input:		i, j		board coordinates
 *			col		column of the board
 *			thr_index	number of current thread
 * Return value:	ITS_SAFE	Queen without problems
 *			NOT_SAFE	Queen under attack!
 *
 */
int is_safe (int i, int j, int col, int thr_index)
{
  if (queen_on[thr_index][j] == i)
  {
    return ITS_SAFE;
  }
  if (abs(queen_on[thr_index][j] - i) == col - j)
  {
    return ITS_SAFE;
  }

  return NOT_SAFE;
}
