/*
 ============================================================================
 Name        : Sequencial.c
 Author      : Amir
 Version     :
 Copyright   : Your copyright notice
 Description : Calculate Pi 
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "../Monte_Carlo/Monte_Carlo_Approx_PI.h"
int main(int argc, const char* argv[])
{
    
    clock_t startTime, endtime;
    double takentime ;

    double  pi,
        avepi = 0.;

    startTime = clock ( );
    //printf("Sequential Program of computing of pi (3.1415926535).\n");

    srandom((unsigned int)time(NULL));

    double   i;
    for (i = 0; i < ROUNDS; i++) {

        pi = dboard(DARTS);
        avepi = ((avepi * i) + pi) / (i + 1.0);

        // printf("    After %.0Lf throws, average value of pi = %10.8f\n", (long double)(DARTS * (i + 1)), avepi);
    }
    printf("\t                           Final result: %.10f\n", avepi);
    printf("Sequential computing of of finding Real value of PI(3.1415926535897)  \n");
    

endtime = clock ( );
//printf("%f",endtime);
takentime =  (double)(endtime - startTime)/CLOCKS_PER_SEC;
printf(" Time in Sequential: %lf \n", takentime);

    return (0);
}