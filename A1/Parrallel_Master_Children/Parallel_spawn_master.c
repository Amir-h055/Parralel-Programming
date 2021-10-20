#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../Monte_Carlo/Monte_Carlo_Approx_PI.h"

 

int main (int argc, char *argv[])
{
    double homepi ;
    int processId, numChildren, numParents;

    double avepi, pisum, pi;
    double startTime, endtime, takentime ;

    MPI_Comm childComm;
    
    /* Obtain number of tasks and task ID */
    MPI_Init(&argc,&argv);
    startTime = MPI_Wtime ( );

    MPI_Comm_size(MPI_COMM_WORLD, &numParents);

    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    printf ("Parent task id is: %d. Total tasks in parent's world are: %d \n", processId, numParents );

    if ( argc != 2 )
        printf( "usage: %s <number of workers>\n", argv[0] );
    else
        numChildren = atoi( argv[1] ); //Given by user in command line e.g: 'mpirun -np 1 master 4' -> numChildren=4
    
    int spawn_error;
    MPI_Comm_spawn( "./Parallel_spawn_child", argv, numChildren, MPI_INFO_NULL, 0, MPI_COMM_SELF, &childComm, MPI_ERRCODES_IGNORE );

    MPI_Reduce(&numChildren, &pisum, 1, MPI_DOUBLE, MPI_SUM, MPI_ROOT ,childComm);  //Specifies child communicator as comm....USE MPI_ROOT (VERY IMPORTANT)
	 
// if (processId==MASTER)
//     {
    // printf("\nMaster value of pisum %lf \n", pisum);
    // printf("\n P %d: PI: %lf\n\n",processId,pisum);
    printf("\n");
    printf("\n num of parents %d",numParents);
    printf("\n num of children %d ", numChildren);
     pi = pisum/numChildren;
     printf("\nStatic Parallel MASTER_CHILDEN : %lf\n",pi);

    endtime = MPI_Wtime ( );
 takentime =  endtime - startTime;
     printf("\nTaken time : %lf\n\n**********************************\n",takentime);

    MPI_Finalize();



    

}