/**********************************************************************
 * FILE: mpi_pi_reduce.c
 * OTHER FILES: dboard.c
 * DESCRIPTION:  
 *   MPI pi Calculation Example - C Version 
 *   Collective Communication example:  
 *   This program calculates pi using a "dartboard" algorithm.  See
 *   Fox et al.(1988) Solving Problems on Concurrent Processors, vol.1
 *   page 207.  All processes contribute to the calculation, with the
 *   master averaging the values for pi. This version uses mpc_reduce to 
 *   collect results
 * AUTHOR: Blaise Barney. Adapted from Ros Leibensperger, Cornell Theory
 *   Center. Converted to MPI: George L. Gusciora, MHPCC (1/95) 
 * LAST REVISED: 06/13/13 Blaise Barney
**********************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../Monte_Carlo/Monte_Carlo_Approx_PI.h"




int main (int argc, char *argv[])
{
double startTime, endtime, takentime ;
double	homepi,         /* value of pi calculated by current task */
	pisum,	        /* sum of tasks' pi values */
	pi,	        /* average of pi after "darts" is thrown */
	avepi;	        /* average pi value for all iterations */
int	taskid,	        /* task ID - also used as seed number */
	numtasks,       /* number of tasks */
	rc,             /* return code */
	i;
MPI_Status status;

/* Obtain number of tasks and task ID */
MPI_Init(&argc,&argv);
startTime = MPI_Wtime ( );
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
printf ("MPI task %d has started...\n", taskid);

    if (taskid == MASTER) {
        printf("Parallel computing of pi (3.1415926535), using %d tasks.\n", numtasks);
    }

    printf("MPI task %d has started...\n", taskid);


srandom (taskid);

avepi = 0;
for (i = 0; i < ROUNDS; i++) {
   /* All tasks calculate pi using dartboard algorithm */
   homepi = dboard(DARTS);

   /* Use MPI_Reduce to sum values of homepi across all tasks 
    * Master will store the accumulated value in pisum 
    * - homepi is the send buffer
    * - pisum is the receive buffer (used by the receiving task only)
    * - the size of the message is sizeof(double)
    * - MASTER is the task that will receive the result of the reduction
    *   operation
    * - MPI_SUM is a pre-defined reduction function (double-precision
    *   floating-point vector addition).  Must be declared extern.
    * - MPI_COMM_WORLD is the group of tasks that will participate.
    */

   rc = MPI_Reduce(&homepi, &pisum, 1, MPI_DOUBLE, MPI_SUM,
                   MASTER, MPI_COMM_WORLD);

   /* Master computes average for this iteration and all iterations */
   if (taskid == MASTER) {
      pi = pisum/numtasks;
      avepi = ((avepi * i) + pi)/(i + 1); 
      printf("   After %8d throws, average value of pi = %10.8f\n",
              (DARTS * (i + 1)),avepi);
   }    
} 
if (taskid == MASTER) 
   printf ("\n Parallel Program of finding Real value of PI(3.1415926535897)  \n");

endtime = MPI_Wtime ( );
takentime =  endtime - startTime;
    MPI_Finalize();

  if ( taskid == MASTER)
  {
   printf(" Parallel Program time:  %lf \n start: %lf \n end: %lf\n", takentime, startTime,endtime);
  }

    return (0);
}




