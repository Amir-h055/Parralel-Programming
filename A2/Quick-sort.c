#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MASTER  0

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main (int argc, char *argv[])
{
int chunk = 128; //n/p
MPI_Init(&argc,&argv);
int numtasks;
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
// MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

 int ndims = 3;
 int dims[] = {2,2,2};
 int periodic[] = {0,0,0};

MPI_Comm com_cart;
MPI_Cart_create(MPI_COMM_WORLD,
                     ndims,
                     dims,
                     periodic,
                     0,
                     &com_cart);
int rank;
MPI_Comm_rank(com_cart, &rank);
int coords [3];
MPI_Cart_coords (com_cart, rank, 3, coords);
printf("\n %d   coords %d %d %d",rank, coords[0], coords[1], coords[2]);
MPI_Barrier(com_cart);

// for (coords[0] = 0; coords[0] < dims[0]; coords[0]++)
// {
//     printf("%2d |", coords[0]);
//     for (coords[1]=0; coords[1] < dims[1]; coords[1]++)
//     {
//         int rank;
//         MPI_Cart_rank(com_cart, coords, &rank);
//         printf(" %2d", rank);
//     }
//     printf("\n");
// }

// populate each array in each p with random num(instead of master giving equal chunk to each p)
int ary [chunk];
srandom(time(NULL)+rank);
for (int i = 0; i < chunk; i++)
{
    ary[i] = rand()%100+1; // random num from 1 to 1000
}

// debug line : show values in each p
// for (int i = 0; i < chunk; i++)
// {
//     printf("(%d: %d) ",i,ary[i]);
// }


int pivot;
if (rank == MASTER)
{
    pivot = rand()%chunk;
    // printf("\n%d\n",pivot);
    pivot = ary[pivot];
    printf("\nPivot: %d\n",pivot);

}

MPI_Bcast(&pivot, 1 , MPI_INT, MASTER, com_cart);

// if (rank != MASTER)
// {
//     printf("\nPivot: %d\n",pivot);


// }


int* max;
max = (int*)malloc(10 * sizeof(int));
int* min; 
min = (int*)malloc(10 * sizeof(int));
int maxSize=0;
int minSize=0;
// max [0]=0;
// max [1]=1;
// max [2]=2;
// max [3]=3;
// max = (int*)realloc(max, 20 * sizeof(int));
// max [4]=4;
//printf("\n0:%d, 1:%d, 2:%d, 3:%d, 4:%d\n",max[0], max[1], max[2], max[3], max[4]);

// split data based on pivot
for (int i = 0; i < chunk; i++)
{
    if (ary[i] > pivot)
    {
        if ( (maxSize+1) >= (((maxSize/10)+1)*10) )
        {
            //printf("*****************maxSize: %d\n", maxSize);
            max = (int*)realloc(max, ((((maxSize+1)/10)+1)*10) * sizeof(int));
        }
        //printf("maxSize: %d,  %d\n", maxSize, (((maxSize/10)+1)*10) );
        
// printf("\nhere2\n");
        max[maxSize] = ary[i];
        maxSize = maxSize+1;

    }

    else if ( ary[i] <= pivot )
    {
        if ( (minSize+1) >= (((minSize/10)+1)*10) )
        {
            min = (int*)realloc(min, ((((minSize+1)/10)+1)*10 )* sizeof(int));
        }
        

        min[minSize] = ary[i];
        minSize++;
    }
    
    
}


// oh well
int maxx [maxSize] ;
for (int i = 0; i < maxSize; i++)
{
    maxx[i] = max[i];
}
int minn [minSize] ;
for (int i = 0; i < minSize; i++)
{
    minn[i] = min[i];
}
int* buff;
int buffSize;

//send max to signifcant bit of (1*)  and rec min forom them in 1d 
if (coords[0]==0)
{
    //int size[2] = {minSize, maxSize};
    int recSize;
    int dest_rank;
    int src_rank;
    MPI_Cart_shift (com_cart, 0, 1, &src_rank, &dest_rank);
    //printf("\nrank:%d  dest:%d  src:%d\n",rank, dest_rank, src_rank);
    MPI_Sendrecv(&maxSize,1, MPI_INT, dest_rank, 10,
                &recSize, 1,  MPI_INT, dest_rank, 11,
                com_cart, MPI_STATUS_IGNORE);
    //printf("\nrank:%d  dest:%d  src:%d recSize:%d \n",rank, dest_rank, src_rank,recSize);
    int recArr [recSize]; //= (int*)malloc(recSize * sizeof(int));
   // MPI_Send(maxx, maxSize, MPI_INT, dest_rank, 37, com_cart);

    MPI_Sendrecv(maxx, maxSize, MPI_INT, dest_rank, 13,
                &recArr, recSize,  MPI_INT, dest_rank, 14,
                com_cart, MPI_STATUS_IGNORE);

    //free(ary);
    //free(max);
    buffSize = minSize + recSize;
    printf("\n---r:%d  bufs%d  mins%d  recs%d",rank, buffSize ,minSize, recSize);
    buff = (int*)malloc((buffSize) * sizeof(int));
    for (int i = 0; i < minSize; i++)
    {
        buff[i] = min[i];
    }
    for (int i = 0; i < recSize; i++)
    {
        buff[minSize+i] = recArr[i];
    }


}

// rec max from significamt bit of (0*) nad send min to them in 1 dimention
if (coords[0]==1)
{
    //int size[2] = {minSize, maxSize};
    int recSize;
    int dest_rank;
    int src_rank;
    MPI_Cart_shift (com_cart, 0, 1, &src_rank, &dest_rank);
    //printf("\nrank:%d  dest:%d  src:%d\n",rank, dest_rank, src_rank);
    //printf("%d :minSize: %d",rank ,minSize);
    MPI_Sendrecv(&minSize,1, MPI_INT, src_rank, 11,
                &recSize, 1,  MPI_INT, src_rank, 10,
                com_cart, MPI_STATUS_IGNORE);

    int recArr[recSize]; //(int*)malloc(recSize * sizeof(int));          
    MPI_Sendrecv(&minn, minSize, MPI_INT, src_rank, 14,
                &recArr, recSize,  MPI_INT, src_rank, 13,
                com_cart, MPI_STATUS_IGNORE);
   // MPI_Recv(recArr, recSize, MPI_INT, src_rank, 37, com_cart, MPI_STATUS_IGNORE);


    buffSize = maxSize + recSize;
    printf("\n+++r:%d  buffs%d  maxs%d  recs%d",rank, buffSize ,maxSize, recSize);
    buff = (int*)malloc((buffSize) * sizeof(int));
    for (int i = 0; i < maxSize; i++)
    {
        buff[i] = max[i];
    }
    for (int i = 0; i < recSize; i++)
    {
        buff[maxSize+i] = recArr[i];
    }
   
}
//printf("\nsize:%ld\n", sizeof(*max)/sizeof(int));

free(max);
free(min);


//************************************************ Second Dim *************************
MPI_Barrier(com_cart);
printf("\nwaitting to srat d2");
MPI_Barrier(com_cart);
int Bxfree_coords[]= {1,0,1};
int Bx2free_coords[]= {0,1,1};
MPI_Comm Bx_com;
MPI_Comm Bx2_com;
int row_test[2];
MPI_Cart_sub(com_cart , Bxfree_coords, &Bx_com);
MPI_Cart_sub(com_cart , Bx2free_coords, &Bx2_com);
// if (coords[1] == 0 )
// {
// row_test[0] = coords[0];
// ro
// }
// else
// {
// row_test = -1;
// }
if (coords[0]==0 & coords[1]==0 & coords[2]==0)
{
    pivot = rand()%buffSize;
    printf("\nrank %d  | %d\n",rank, pivot);
    pivot = buff[pivot];
    printf("\n rank %d  real Pivot: %d\n",rank, pivot);

}

if (coords[0]==1 & coords[1]==0 & coords[2]==0)
{
    pivot = rand()%buffSize;
     printf("\nrank %d:| %d\n",rank, pivot);
    pivot = buff[pivot];
    //DEBUG
     printf("\n rank %d  real Pivot: %d\n",rank, pivot);
    // for (int i = 0; i < buffSize; i++)
    // {
    //     printf("%d:%d, ",i,buff[i]);
    // }
    

}

MPI_Barrier(com_cart);
// broad cast the pivot on each sub comm of the 
MPI_Bcast(&pivot, 1 , MPI_INT, MASTER, Bx2_com);


printf("\nProcess %d > coords = (%d,%d,%d), nPivot = %d\n",
rank, coords[0], coords[1],coords[2],pivot );
//MPI_Bcast(&row_test, 1, MPI_INT, 0, row_comm);
// printf("Process %d > coords = (%d,%d,%d), row_test = %d\n",
// rank, coords[0], coords[1],coords[2], row_test);
//MPI_Bcast();

// split data based on pivot
minSize  = 0;
maxSize = 0;
min = (int*)malloc(10 * sizeof(int));
max = (int*)malloc(10 * sizeof(int));

for (int i = 0; i < buffSize; i++)
{
    if (buff[i] > pivot)
    {
        if ( (maxSize+1) >= (((maxSize/10)+1)*10) )
        {
            //printf("*****************maxSize: %d\n", maxSize);
            max = (int*)realloc(max, ((((maxSize+1)/10)+1)*10) * sizeof(int));
        }
        //printf("maxSize: %d,  %d\n", maxSize, (((maxSize/10)+1)*10) );
        
// printf("\nhere2\n");
        max[maxSize] = buff[i];
        maxSize = maxSize+1;

    }

    else if ( buff[i] <= pivot )
    {
        if ( (minSize+1) >= (((minSize/10)+1)*10) )
        {
            min = (int*)realloc(min, ((((minSize+1)/10)+1)*10 )* sizeof(int));
        }
        

        min[minSize] = buff[i];
        minSize++;
    }

}

free(buff);

//free(maxx);
//free(minn);

// oh well
int maxx2 [maxSize] ;
for (int i = 0; i < maxSize; i++)
{
    maxx2[i] = max[i];
}
int minn2 [minSize] ;
for (int i = 0; i < minSize; i++)
{
    minn2[i] = min[i];
}
MPI_Barrier(com_cart);
//send max to seconed signifcant bit of (1*)  and rec min forom them in 2d 
if (coords[1]==0)
{
    //int size[2] = {minSize, maxSize};
    int recSize;
    int dest_rank;
    int src_rank;
    MPI_Cart_shift (com_cart, 1, 1, &src_rank, &dest_rank);
    //printf("\nrank:%d  dest:%d  src:%d\n",rank, dest_rank, src_rank);
    MPI_Sendrecv(&maxSize,1, MPI_INT, dest_rank, 15,
                &recSize, 1,  MPI_INT, dest_rank, 16,
                com_cart, MPI_STATUS_IGNORE);
    //printf("\nrank:%d  dest:%d  src:%d recSize:%d \n",rank, dest_rank, src_rank,recSize);
    int recArr [recSize]; //= (int*)malloc(recSize * sizeof(int));
   // MPI_Send(maxx, maxSize, MPI_INT, dest_rank, 37, com_cart);

    MPI_Sendrecv(maxx2, maxSize, MPI_INT, dest_rank, 17,
                &recArr, recSize,  MPI_INT, dest_rank, 18,
                com_cart, MPI_STATUS_IGNORE);

    //free(ary);
    //free(max);
    buffSize = minSize + recSize;
    printf("\n---r:%d  bufs%d  mins%d  recs%d",rank, buffSize ,minSize, recSize);
    buff = (int*)malloc((buffSize) * sizeof(int));
    for (int i = 0; i < minSize; i++)
    {
        buff[i] = min[i];
    }
    for (int i = 0; i < recSize; i++)
    {
        buff[minSize+i] = recArr[i];
    }

}


// rec max from seconed significamt bit of (0*) nad send min to them in 2 dimention
if (coords[1]==1)
{
    //int size[2] = {minSize, maxSize};
    int recSize;
    int dest_rank;
    int src_rank;
    MPI_Cart_shift (com_cart, 1, 1, &src_rank, &dest_rank);
    //printf("\nrank:%d  dest:%d  src:%d\n",rank, dest_rank, src_rank);
    //printf("%d :minSize: %d",rank ,minSize);
    MPI_Sendrecv(&minSize,1, MPI_INT, src_rank, 16,
                &recSize, 1,  MPI_INT, src_rank, 15,
                com_cart, MPI_STATUS_IGNORE);

    int recArr[recSize]; //(int*)malloc(recSize * sizeof(int));          
    MPI_Sendrecv(&minn2, minSize, MPI_INT, src_rank, 18,
                &recArr, recSize,  MPI_INT, src_rank, 17,
                com_cart, MPI_STATUS_IGNORE);
   // MPI_Recv(recArr, recSize, MPI_INT, src_rank, 37, com_cart, MPI_STATUS_IGNORE);


    buffSize = maxSize + recSize;
    printf("\n+++r:%d  buffs%d  maxs%d  recs%d",rank, buffSize ,maxSize, recSize);
    buff = (int*)malloc((buffSize) * sizeof(int));
    for (int i = 0; i < maxSize; i++)
    {
        buff[i] = max[i];
    }
    for (int i = 0; i < recSize; i++)
    {
        buff[maxSize+i] = recArr[i];
    }
   
}
//printf("\nsize:%ld\n", sizeof(*max)/sizeof(int));

free(max);
free(min);



//************************************************ third Dim *************************

MPI_Barrier(com_cart);
printf("\nwaitting to srat d3");
MPI_Barrier(com_cart);
int Cyfree_coords[]= {0,0,1};
MPI_Comm Cy_com;
//int row_test[2];
MPI_Cart_sub(com_cart , Cyfree_coords, &Cy_com);

// if (coords[0]==0 & coords[1]==0 & coords[2]==0)
// {
//     pivot = rand()%buffSize;
//     printf("\nrank %d  | %d\n",rank, pivot);
//     pivot = buff[pivot];
//     printf("\n rank %d  real Pivot: %d\n",rank, pivot);

// }
MPI_Barrier(com_cart);
printf("1ok%d\n",rank);
MPI_Barrier(com_cart);
if (coords[2]==0)
{

    int r = rand()%buffSize;
     //printf("\nrank %d:| %d\n",rank, pivot);
    pivot = buff[r];
    //DEBUG
     printf("\n rank %d random:%d real Pivot: %d\n",rank,r, pivot);
    // for (int i = 0; i < buffSize; i++)
    // {
    //     printf("%d:%d, ",i,buff[i]);
    // }
    

 }

     //MPI_Barrier(com_cart);
    printf(" 2ok%d\n",rank);
    MPI_Barrier(com_cart);




// broad cast the pivot on each sub comm of the 
MPI_Bcast(&pivot, 1 , MPI_INT, MASTER, Cy_com);


printf("\ndim 3 Process %d > coords = (%d,%d,%d), Pivot = %d  buffsize: %d\n",
rank, coords[0], coords[1],coords[2],pivot, buffSize );

//MPI_Bcast(&row_test, 1, MPI_INT, 0, row_comm);
// printf("Process %d > coords = (%d,%d,%d), row_test = %d\n",
// rank, coords[0], coords[1],coords[2], row_test);
//MPI_Bcast();

// split data based on pivot
minSize  = 0;
maxSize = 0;
min = (int*)malloc(10 * sizeof(int));
max = (int*)malloc(10 * sizeof(int));
MPI_Barrier(com_cart);
 
for (int i = 0; i < buffSize; i++)
{
    if (buff[i] > pivot)
    {
        if ( (maxSize+1) >= (((maxSize/10)+1)*10) )
        {
            //printf("*****************maxSize: %d\n", maxSize);
            max = (int*)realloc(max, ((((maxSize+1)/10)+1)*10) * sizeof(int));
        }
        //printf("maxSize: %d,  %d\n", maxSize, (((maxSize/10)+1)*10) );
        
// printf("\nhere2\n");
        max[maxSize] = buff[i];
        maxSize = maxSize+1;

    }

    else if ( buff[i] <= pivot )
    {
        if ( (minSize+1) >= (((minSize/10)+1)*10) )
        {
            min = (int*)realloc(min, ((((minSize+1)/10)+1)*10 )* sizeof(int));
        }
        

        min[minSize] = buff[i];
        minSize++;
    }

}

free(buff);   


// oh well
int maxx3 [maxSize] ;
for (int i = 0; i < maxSize; i++)
{
    maxx3[i] = max[i];
}
int minn3 [minSize] ;
for (int i = 0; i < minSize; i++)
{
    minn3[i] = min[i];
}





//send max to third signifcant bit of (1*)  and rec min forom them in 3d 
if (coords[2]==0)
{
    //int size[2] = {minSize, maxSize};
    int recSize;
    int dest_rank;
    int src_rank;
    MPI_Cart_shift (com_cart, 2, 1, &src_rank, &dest_rank);
    //printf("\n>>>>rank:%d  dest:%d  src:%d\n",rank, dest_rank, src_rank);
    MPI_Sendrecv(&maxSize,1, MPI_INT, dest_rank, 19,
                &recSize, 1,  MPI_INT, dest_rank, 20,
                com_cart, MPI_STATUS_IGNORE);
    //printf("\nrank:%d  dest:%d  src:%d recSize:%d \n",rank, dest_rank, src_rank,recSize);
    int recArr [recSize]; //= (int*)malloc(recSize * sizeof(int));
   // MPI_Send(maxx, maxSize, MPI_INT, dest_rank, 37, com_cart);

    MPI_Sendrecv(maxx3, maxSize, MPI_INT, dest_rank, 21,
                &recArr, recSize,  MPI_INT, dest_rank, 22,
                com_cart, MPI_STATUS_IGNORE);

    //free(ary);
    //free(max);
    buffSize = minSize + recSize;
    printf("\n---r:%d  bufs%d  mins%d  recs%d",rank, buffSize ,minSize, recSize);
    buff = (int*)malloc((buffSize) * sizeof(int));
    for (int i = 0; i < minSize; i++)
    {
        buff[i] = min[i];
        
    }
    for (int i = 0; i < recSize; i++)
    {
        buff[minSize+i] = recArr[i];

            

    }

}




// rec max from third significamt bit of (0*) nad send min to them in 3 dimention
if (coords[2]==1)
{
    //int size[2] = {minSize, maxSize};
    int recSize;
    int dest_rank;
    int src_rank;
    MPI_Cart_shift (com_cart, 2, 1, &src_rank, &dest_rank);
    //printf("\n<<<<<rank:%d  dest:%d  src:%d\n",rank, dest_rank, src_rank);
    //printf("%d :minSize: %d",rank ,minSize);

    


    MPI_Sendrecv(&minSize,1, MPI_INT, src_rank, 20,
                &recSize, 1,  MPI_INT, src_rank, 19,
                com_cart, MPI_STATUS_IGNORE);

    int recArr[recSize]; //(int*)malloc(recSize * sizeof(int));          
    MPI_Sendrecv(&minn3, minSize, MPI_INT, src_rank, 22,
                &recArr, recSize,  MPI_INT, src_rank, 21,
                com_cart, MPI_STATUS_IGNORE);
   // MPI_Recv(recArr, recSize, MPI_INT, src_rank, 37, com_cart, MPI_STATUS_IGNORE);


    buffSize = maxSize + recSize;
    printf("\n+++r:%d  buffs%d  maxs%d  recs%d",rank, buffSize ,maxSize, recSize);
    buff = (int*)malloc((buffSize) * sizeof(int));
    for (int i = 0; i < maxSize; i++)
    {
        buff[i] = max[i];

        
    }
    for (int i = 0; i < recSize; i++)
    {
        buff[maxSize+i] = recArr[i];

    }
   
}

// for (int i = 0; i < buffSize; i++)
// {
//     if (buff[i]>100)
//     {
//         printf("\n######################3hhhhhhhhhhhhhhh************ r: %d  ..%d  %d \n",rank, buff[i],i);
//         for (int a = 0; a < buffSize; a++)
//         {
//             printf("%d,%d:%d, ",rank,a, buff[a]);
//         }
//         printf("\n");
        
//     }
    
// }


//printf("\nsize:%ld\n", sizeof(*max)/sizeof(int));

//free(max);
//free(min);


qsort(buff,buffSize, sizeof(int), cmpfunc);
MPI_Barrier(com_cart);

 int c = 0;
while (c<numtasks)
{
    
    if ( rank ==c)
    {
    for (int i = 0; i < buffSize; i++)
    {

    printf("r%d:%d, ",rank, buff[i]);
    
    }
printf("\n\n"); 


    }

    MPI_Barrier(com_cart);

    c++;

}


free(max);
free(min);

if (rank ==0)
{
//int* bufferSizes = (int*)calloc(numtasks, sizeof(int));
int bufferSizes [numtasks];
 MPI_Gather(&buffSize, 1, MPI_INT, bufferSizes, 1, MPI_INT, MASTER, com_cart);
 //int placement = (int*)calloc(numtasks, sizeof(int));
 int placement [numtasks];
 placement[0] = 0;
 int sum_size = bufferSizes[0];

 for (int i = 0; i < numtasks; i++)
 {
    printf("bs%d:%d\n",i,bufferSizes[i]);
 }
 
 for (int i = 1; i < numtasks; i++)
 {
    
    placement[i] = bufferSizes[i-1] + placement[i-1];
    sum_size = bufferSizes[i]+sum_size;
 }


 
printf("sum size%d\n",sum_size);

 int* buffer = (int*)calloc(sum_size, sizeof(int));

 

 MPI_Gatherv(&buff, 1, MPI_INT, buffer, bufferSizes, placement, MPI_INT, MASTER, com_cart);
  for (int i = 0; i < numtasks; i++)
 {
     printf("p%d:%d\n",i,placement[i]);
 }
 
 for (int i = 0; i < buffSize; i++)
 {
    buffer[i] = buff[i];
 }

 for (int i = 0; i < numtasks*chunk; i++)
 {
    printf("%d:%d ,", i, buffer[i]);
 }

 
 free(buffer);
}
else
{
 MPI_Gather(&buffSize, 1, MPI_INT, NULL, 0, MPI_INT, MASTER, com_cart);

 MPI_Gatherv(buff, buffSize, MPI_INT, NULL, NULL, NULL, MPI_INT, MASTER, com_cart);
}




    MPI_Finalize();
 
    return EXIT_SUCCESS;
}




