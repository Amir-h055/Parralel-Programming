#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "../Monte_Carlo/Monte_Carlo_Approx_PI.h"

 

int main (int argc, char *argv[])
{
    double startTime, endtime, takentime;
    double homepi ;
    int processId, numChildren, numParents;

    double avepi, pisum, pi, fpi = 0.00;
    const double PI_GOAL = 3.1415926535897932;

    MPI_Comm childComm;
    
    /* Obtain number of tasks and task ID */
    MPI_Init(&argc,&argv);
    startTime = MPI_Wtime();

    MPI_Comm_size(MPI_COMM_WORLD, &numParents);

    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    printf ("Parent task id is: %d. Total tasks in parent's world are: %d \n", processId, numParents );

    if ( argc != 2 )
        printf( "usage: %s <number of workers>\n", argv[0] );
    else
        numChildren = atoi( argv[1] ); //Given by user in command line e.g: 'mpirun -np 1 master 4' -> numChildren=4
    
    int spawn_error;
    MPI_Comm_spawn( "./dynamicChild", argv, numChildren, MPI_INFO_NULL, 0, MPI_COMM_SELF, &childComm, MPI_ERRCODES_IGNORE );

    int sent_dart = 0;
    int chunk = 1000;
    int continue_work = 1;
    MPI_Bcast(&continue_work, 1 , MPI_INT, MPI_ROOT, childComm);
    int pi_n = 0;

    sent_dart = 1000 * numChildren*numParents;

    printf("\n Total number of %d darts has been sent to all Child Processesors for each master ",sent_dart);
    int rec_dart = 0;
    
    while (sent_dart < DARTS)
    {
        MPI_Status stat;
        MPI_Recv(&pisum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 37, childComm, &stat);
        rec_dart = rec_dart+1000;
        pi = pi + pisum;
        //printf("pi:%10.8f",pi);
        pi_n = pi_n + 1;

        fpi = pi/pi_n;
        printf("\n Child Process (%d) Finished its 1000, sent darts so far: %d ",stat.MPI_SOURCE, sent_dart);
        printf("\n \tclaculated value of Pi at this moment : %10.8f \n",fpi);


        if ( fabs(PI_GOAL-fpi) < 0.0001)  // check for coverges , if you want you can comment this out to check it without converges checking
        {
            printf("\n %lf",fabs(PI_GOAL-fpi));
            printf("\n ***** We reached the good accuracy now we start to terminating after we get all already sent darts back\n ");
            //sent_dart = DARTS;
            continue_work = 0;
            MPI_Send(&continue_work, 1, MPI_INT, stat.MPI_SOURCE, 37, childComm);

            break;
        }
       
        
        MPI_Send(&continue_work, 1, MPI_INT, stat.MPI_SOURCE, 37, childComm);
        sent_dart = sent_dart+1000;
    }

    continue_work = 0;

    while (rec_dart!=sent_dart)
    {
        printf("\n Start terminating after reciving back all the already sent darts\n");
        MPI_Status stat;
        MPI_Recv(&pisum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 37, childComm, &stat);
        rec_dart = rec_dart+1000;
        pi = pi + pisum;
        //printf("pi:%10.8f",pi);
        pi_n = pi_n + 1;

        fpi = pi/pi_n;
        printf("\n Child Process (%d) Finished its 1000, sent darts so far: %d ",stat.MPI_SOURCE, sent_dart);
        printf("\n \tclaculated value of Pi at this moment : %10.8f",fpi);

        MPI_Send(&continue_work, 1, MPI_INT, stat.MPI_SOURCE, 37, childComm);
        printf("\n terminate child %d ",stat.MPI_SOURCE);
    }
    
    
    //MPI_Bcast(&continue_work, 1 , MPI_INT, MPI_ROOT, childComm);

    pi = pi/pi_n;
    printf("\n ********************************** \n\npi_n:%d, pi:%10.8f",pi_n,pi);

    

    endtime = MPI_Wtime();
    takentime = endtime - startTime;
    printf("\ntime in Dynamic: %lf\n\n**********************************\n",takentime);
    MPI_Finalize();
    return 0;
}
    

