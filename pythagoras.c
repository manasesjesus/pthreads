/* The Pythagoras' theorem states the relation among the three sides of 
 * a right-angled triangle, where the sum of the areas of the two 
 * squares on the sides (a, b) equals the area of the square on the 
 * hypotenuse (c), and it can be represented with the equation 
 * a^2 + b^2 = c^2
 * 
 * This program uses two threads; each one to calculate the areas of the 
 * two squares on the sides. Initially, the hypotenuse value is set to 
 * zero. When one thread has made its calculation, it sums it to the 
 * hypotenuse, therefore it has to be treated as a critical section. 
 * It uses mutex to protect the shared data. To compile it may be 
 * necessary to add the option -lm to link the math.h library.
 * 
 * Compilation
 *	gcc -lm -Wall -lpthread -o pythagoras pythagoras.c
 * 
 * Execution 
 *	./pythagoras <side_a> <side_b>
 * 
 *
 * File: pythagoras.c			Author: Manases Galindo
 * Date: 09.02.2017
 */



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREADS 2			/* default number of threads	*/

/* Shared global variables. All threads can access them.	  	*/
float hypotenuse;
pthread_mutex_t mutexsum;


void *square_side (void *);


int main (int argc, char **argv)
{
  int i;				/* loop variable		*/
  float sides[2];			/* right-angled triangle sides	*/
  pthread_t *thr_ids;			/* array of thread ids		*/

  switch (argc)				/* check command line arguments	*/
  {
    case 3:
      /* Get the values of the right-angled triangle sides		*/
      sides[0] = atof (argv[1]);
      sides[1] = atof (argv[2]);
      if ((sides[0] < 1) || (sides[1] < 1))
      {
	fprintf (stderr, "Error: wrong values for triangle sides.\n"
		 "Usage:\n"
		 "  %s <side_a> <side_b>\n"
		 "values of sizes should be > 0\n",
		 argv[0]);
	exit (EXIT_FAILURE);
      }
      break;

    default:
      fprintf (stderr, "Error: wrong number of parameters.\n"
	       "Usage:\n"
	       "  %s <side_a> <side_b>\n",
	       argv[0]);
      exit (EXIT_FAILURE);
  }
  
  /* allocate memory for all dynamic data structures			*/
  thr_ids = (pthread_t *) malloc (NUM_THREADS * sizeof (pthread_t));
  
  /* Validate that memory was successfully allocated			*/
  if (thr_ids == NULL)
  {
    fprintf (stderr, "File: %s, line %d: Can't allocate memory.",
	     __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }
  
  printf ("\nPythagoras' theorem | a^2 + b^2 = c^2 \n"); 
  hypotenuse = 0;
  
  /* Initialize the mutex to protect share data (hypotenuse)		*/
  pthread_mutex_init (&mutexsum, NULL);
  
  /* Create the threads	and calculate the squares on the sides		*/
  pthread_create (&thr_ids[0], NULL, square_side, &sides[0]);
  pthread_create (&thr_ids[1], NULL, square_side, &sides[1]);
  
  /* Using join to syncronize the threads				*/
  for (i = 0; i < NUM_THREADS; i++)		
  {
    pthread_join (thr_ids[i], NULL);
  }
  
  printf ("Hypotenuse is %.2f\n", sqrt(hypotenuse)); 
  
  /* Deallocate any memory or resources associated			*/
  pthread_mutex_destroy (&mutexsum);
  free (thr_ids);
  
  return EXIT_SUCCESS;
}


/* square_side runs as a thread and calculates the areas of the 
 * square on the side, then sums the value to the hypotenuse.
 * It uses a mutex to protect the hypotenuse and avoid a race
 * conditiong within the threads.
 *
 * Input:		arg		pointer to triangle side value
 * Return value:	none
 *
 */
void *square_side (void *arg)
{
  float side;

  /* Get the value of the triangle side	and print the square 		*/
  side = *( ( float* )arg );
  printf ("%.2f^2 = %.2f\n", side, side * side); 
  
  /* Mutex lock/unlock to safely update the value of hypotenuse		*/
  pthread_mutex_lock (&mutexsum);
  hypotenuse += side * side;
  pthread_mutex_unlock (&mutexsum);
  
  pthread_exit (EXIT_SUCCESS);		/* Terminate the thread		*/
}
