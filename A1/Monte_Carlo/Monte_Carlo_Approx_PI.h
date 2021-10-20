
#pragma once



#define DARTS 50000     /* number of throws at dartboard */
#define ROUNDS 100      /* number of times "darts" is iterated */
#define MASTER 0        /* task ID of master task */

void srandom (unsigned seed);
double dboard(int darts);

