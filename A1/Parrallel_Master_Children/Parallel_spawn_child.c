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
// double startTime, endtime, takentime ;

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
// startTime = MPI_Wtime ( );

MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

MPI_Comm commParent;  //Parent Communicator

MPI_Comm_get_parent(&commParent);

printf ("MPI task %d has started...\n", taskid);
//printf("\n numtask %d\n",numtasks);

/* Set seed for random number generator equal to task ID */
srandom (taskid);


avepi = 0;
for (i = 0; i < ROUNDS; i++) {
   /* All tasks calculate pi using dartboard algorithm */

  // printf("  *************  Round:      %d  ********\t", i);
   homepi += dboard(DARTS/numtasks);

   //printf("homepi is : %lf \n", homepi);
}
// printf("\n ompepi %lf\n",homepi);
double x =(homepi/(double)ROUNDS);
// printf("\n dvideby round: %lf \n",x );


//homepi=homepi/(double)ROUNDS;
   rc = MPI_Reduce(&x, NULL/*&pisum*/, 1, MPI_DOUBLE, MPI_SUM,
                   MASTER, commParent);

 

// endtime = MPI_Wtime ( );
//  takentime =  endtime - startTime;
MPI_Finalize();


return 0;
}


