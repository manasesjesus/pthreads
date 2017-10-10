/* An “advanced” implementation of the classic “Hello, World” program 
 * using the main Pthreads functions to create, identify and synchronize 
 * the threads. 
 * 
 * It creates four threads by default, and optionally the number of 
 * threads to be created can be passed as a command-line parameter when 
 * executing the program. 
 * 
 * Compilation
 *	gcc -Wall -lpthread -o holamigos holamigos.c
 * 
 * Execution 
 *	./holamigos [number_of_threads]
 * 
 *
 * File: holamigos.c			Author: Manases Galindo
 * Date: 09.02.2017
 */



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4			/* default number of threads	*/

/* Shared global variables. All threads can access them.	  	*/
int nthreads;				/* number of threads		*/


void *say_hello (void *);


int main (int argc, char **argv)
{
  int i,				/* loop variable		*/
      *thr_num;				/* array of thread numbers	*/
  pthread_t *thr_ids;			/* array of thread ids		*/

  switch (argc)				/* check command line arguments	*/
  {
    case 1:
      /* Number of threads not specified, using the default value	*/
      nthreads = NUM_THREADS;
      break;

    case 2:
      /* Using the specified number of threads				*/
      nthreads = atoi (argv[1]);
      if (nthreads < 1)
      {
	fprintf (stderr, "Error: wrong number of threads.\n"
		 "Usage:\n"
		 "  %s [number_of_threads]\n"
		 "number_of_threads should be > 0\n"
		 "Using default number of threads (%d).\n",
		 argv[0], NUM_THREADS);
	nthreads = NUM_THREADS;
      }
      break;

    default:
      fprintf (stderr, "Error: wrong number of parameters.\n"
	       "Usage:\n"
	       "  %s [number_of_threads]\n",
	       argv[0]);
      exit (EXIT_FAILURE);
  }
  
  /* allocate memory for all dynamic data structures			*/
  thr_ids = (pthread_t *) malloc (nthreads * sizeof (pthread_t));
  thr_num = (int *) malloc (nthreads * sizeof (int));

  /* Validate that memory was successfully allocated			*/
  if ((thr_num == NULL) || (thr_ids == NULL))
  {
    fprintf (stderr, "File: %s, line %d: Can't allocate memory.",
	     __FILE__, __LINE__);
    exit (EXIT_FAILURE);
  }
  
  printf ("\nHola amigos! I'm the main thread\n"); 
  for (i = 0; i < nthreads; i++)	/* Create the threads		*/
  {
    thr_num[i] = i + 1;			/* Thread number	        */
    pthread_create (&thr_ids[i], NULL, say_hello, &thr_num[i]);
  }

  /* Using join to syncronize the threads				*/
  for (i = 0; i < nthreads; i++)		
  {
    pthread_join (thr_ids[i], NULL);
  }
  
  /* Deallocate any memory or resources associated			*/
  free (thr_num);
  free (thr_ids);

  return EXIT_SUCCESS;
}


/* say_hello runs as a thread and will print a random hello message
 * to stdout (e.g. the screen)
 *
 * Input:		arg		pointer to current thread number
 * Return value:	none
 *
 */
void *say_hello (void *arg)
{
  int thr_index,
      rdm_message;

  /* Get the index number of current thread				*/
  thr_index = *( ( int* )arg );
  
  /* Select a random message to be displayed by the thread		*/
  srand (time(NULL));
  rdm_message = rand() % 4;
  switch (rdm_message) 
  {
    case 0:
      printf ("Hola amigos! I'm thread %d of %d. My ID is %d \n", 
	     thr_index, nthreads, (int) pthread_self());
      break;
      
    case 1:
      printf ("Aloha honua! I'm thread %d of %d. My ID is %d \n", 
	     thr_index, nthreads, (int) pthread_self());
      break;
      
    case 2:
      printf ("Hello peers! I'm thread %d of %d. My ID is %d \n", 
	     thr_index, nthreads, (int) pthread_self());
      break;
      
    case 3:
      printf ("Hallo Leute! I'm thread %d of %d. My ID is %d \n", 
	     thr_index, nthreads, (int) pthread_self());
      break;
    default:
      /* Default case, for the compiler not to complain			*/
      printf ("Konichiwa! I have no father. I was created by The Force!");
      break;
  }
  
  pthread_exit (EXIT_SUCCESS);		/* Terminate the thread		*/
}
