#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
# define MASTER 0
# define n 4
//
// mpicc Floyd-Warshall.c -o ./Floyd -lm
//mpirun -np 4 ./Floyd

int main(int argc, char *argv[])
{
    // int row_p;
    // int col_p;
    // printf("Enter a number of P in row : ");
    // scanf("%d", &row_p);
    // printf("Enter another number P in Col: ");
    // scanf("%d", &col_p);

    MPI_Init(&argc, &argv);
    int numtasks;
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);




    int num_processors_in_d = sqrt( (double)numtasks ) ;
    int ndims = 2;
    int dims[] = {num_processors_in_d, num_processors_in_d};
    int periodic[] = {0,0};

    MPI_Comm com_cart;
    MPI_Cart_create(MPI_COMM_WORLD,
                    ndims,
                    dims,
                    periodic,
                    0,
                    &com_cart);
    
    int rank;
    MPI_Comm_rank(com_cart, &rank);

    int coords [2];
    MPI_Cart_coords (com_cart, rank, 2, coords);
    //printf("\n %d : coords %d %d ",rank, coords[0], coords[1]);

    MPI_Barrier(com_cart);
    printf("\n");


    int min = rank;
    //
    // int to_node = rank +10;
    // int row_val = to_node;
    // /////////////////////
    // int from_node = rank +20;
    // int col_val = from_node;


    int xfree_coords[] = {0, 1};
    MPI_Comm x_com;
    int yfree_coords[] = {1, 0};
    MPI_Comm y_com;

    MPI_Cart_sub(com_cart, xfree_coords, &x_com);
    MPI_Cart_sub(com_cart, yfree_coords, &y_com);

    // using this array to scatter metrix data
    int* array;
    array = (int*)malloc((n*n) * sizeof(int));

    int elemenrt_per_p = (n*n)/numtasks;
    int localArr[elemenrt_per_p];
    

    if (rank == MASTER)
    {
        // edge distances
        // int metrix [4][4] = { 
        //                       {0,            10000000,       -2,      10000000},
        //                       {6,              0,             5,      10000000},
        //                       {10000000,     10000000,        0,             4},
        //                       {10000000,       -1,         10000000,         0},

        //                     };

        int metrix [4][4] = { 
                        {0,              ,            -2,             0},
                        {4,               0,             2,             4},
                        {5,               1,             0,             2},
                        {3,              -1,             1,             0},

                    };


        int count = 0;
        for (int i = 0; i < num_processors_in_d; i++)
        {

        for (int j = 0 ; j < num_processors_in_d; j++)
        {
            int lower_x = (i-1)*n/num_processors_in_d+2;
            int lower_y = (j-1)*n/num_processors_in_d+2;

            int up_x = (i)*n/num_processors_in_d+1;
            int up_y = (j)*n/num_processors_in_d+1;
                // printf("(%d, %d) == lower(%d,%d) up(%d,%d) \n", i,j,lower_x, lower_y, up_x, up_y);
            

                
                for (int x = lower_x; x <= up_x; x++)
                {
                    for (int y = lower_y; y <= up_y; y++)
                    {
                    //   printf(" %d(%d, %d),  ",count,y,x);

                    
                    array[count] = metrix[x][y];
                    count++;

                
                    }
                }
                // printf("////  ");
            

            
        }
            
        }
        // our metrix into array to scatter among processors
        for (int i = 0; i < n*n; i++)
        {
            printf("(%d=%d), ", i, array[i]);
        }
           printf("\n"); 
           printf("=================\n"); 

    }

    MPI_Scatter(array,elemenrt_per_p,MPI_INT,localArr,elemenrt_per_p,MPI_INT,MASTER,com_cart);
    /////////////////////
    // int reslt_Arr [n*n];
    // MPI_Gather(localArr,elemenrt_per_p,MPI_INT,reslt_Arr,elemenrt_per_p,MPI_INT,MASTER,com_cart);

    // shw by processor (each line is a processors elements)
    // if (rank == MASTER)
    // {
    //     for (int  rnum = 0; rnum < n; rnum++)
    //     {

    //         for (int cnum = 0; cnum < n/num_processors_in_d; cnum++)
    //         {
    //             for (int dr = 0; dr < n/num_processors_in_d; dr++)
    //             {
    //                // printf("%d, ",reslt_Arr[ ( (rnum*n) + (cnum)*(n/num_processors_in_d)+dr   ) ]);
    //                 printf("(%d,%d,%d=%d)%d,     ",rnum,cnum,dr,( (rnum*n) + (cnum)*(n/num_processors_in_d)+dr   ), reslt_Arr[ ( (rnum*n) + (cnum)*(n/num_processors_in_d)+dr   ) ]);

    //             }
                
    //         }
    //         printf("\n");

    //     }


    // if (rank == MASTER)
    // {
    //     int co =0;
    //     for (int  rnum = 0; rnum < n; rnum++)
    //     {

    //         for (int cnum = 0; cnum < num_processors_in_d; cnum++)
    //         {
    //             for (int dr = 0; dr < n/num_processors_in_d; dr++)
    //             {
    //                 int rrow = rnum/(n/num_processors_in_d);
    //                 int cclm = rnum%(n/num_processors_in_d);
    //                 co = rnum;
    //                 if (cclm==0)
    //                 {
    //                     co = (rrow)*num_processors_in_d*elemenrt_per_p;
    //                     // printf("co%d   ",co);
    //                 }
                    
    //                 int inn =( (co*n/num_processors_in_d) + (cnum)*(elemenrt_per_p)+dr   );
    //                // printf("%d, ",reslt_Arr[ ( (rnum*n) + (cnum)*(n/num_processors_in_d)+dr   ) ]);
    //                 // printf("(%d,%d,%d=%d)%d,     ",rnum,cnum,dr,inn, reslt_Arr[ inn ]);
    //                 printf("(%d)%d,     ",inn, reslt_Arr[ inn ]);

    //             }
                
    //         }
    //         printf("\n");

    //     }
        

    //     // printf("\n"); 
    //     // for (int icv = 0; icv < (n*n); icv++)
    //     // {
    //     //     printf("%d=%d, ",icv,reslt_Arr[icv]);
    //     // }
    //     // printf("\n"); 
    //     // printf("=================\n"); 
    // }



    // checking processors local array values
    // 
    // our metrix
    // ----------------------
    // | 0     01 | 02   03 |
    // |   P0     |    P1   | 
    // | 10    11 | 12   13 |
    // |----------|---------|
    // | 20    21 | 22   23 |
    // |   P3     |    P4   | 
    // | 30    31 | 32   33 |
    // |__________|_________| 
    // 
    // indicies for each p
    // ----------------------
    // | 0    1   | 0     1 |
    // |   P0     |    P1   | 
    // | 2    3   | 2     3 |
    // |----------|---------|
    // | 0    1   | 0     1 |
    // |   P3     |    P4   | 
    // | 2    3   | 2     3 |
    // |__________|_________| 
    // p0 local array [0, 1, 2, 3]  in P0
    // p1 local array [0, 1, 2, 3]  in P1
    // p2 local array [0, 1, 2, 3]  in P2
    // p3 local array [0, 1, 2, 3]  in P3
    //*****************************************

    // if ( rank == 1)
    // {
    //     for (int i = 0; i < elemenrt_per_p; i++)
    //     {
    //         printf("%d, ",localArr[i]);
    //     }
    //    printf("\n"); 
    //  }
    /////////////////////////////////////////////////
     
    
    
    int row_val[n/num_processors_in_d];
    //row_val = (int*)malloc((n/num_processors_in_d) * sizeof(int));

    int col_val [n/num_processors_in_d];
    //col_val = (int*)malloc((n/num_processors_in_d) * sizeof(int));


    for (int i = 0; i < n; i++)
    {
        int p_index;
        for (int index = 1; index < num_processors_in_d+1; index++)
        {
            //checking p_index
            // if (rank == MASTER)
            // {
            //     printf("\n i:%d <? %lf \n",i , index*(sqrt( (double)elemenrt_per_p )) );
            // }
            if (i < index*(sqrt( (double)elemenrt_per_p ) ))
            {


                p_index = index-1;
                break;
            }
            
        }

        // row colum index
        int index_RC = i%(n/num_processors_in_d);

        // checking pindex and the row or colum element in p
        // if (rank == 0)
        // {
        //     printf("i:%d p:%d rc:%d  ,\n",i,p_index,index_RC );

        // }


        //assigning valus to send arrays (row_val and _col_val)

            for (int e = 0; e < (n/num_processors_in_d); e++)
            {
                row_val[e] = localArr[(index_RC*(n/num_processors_in_d)+e)];
                col_val[e] = localArr[(index_RC)+ ((n/num_processors_in_d)*e) ];
                


                //confirm the send array indicies (element index in each processor)
                // ----------------------
                // | 0    1   | 0     1 |
                // | 2    3   | 2     3 |
                // |----------|---------|
                // | 0    1   | 0     1 |
                // | 2    3   | 2     3 |
                // |__________|_________| 
                //
                // p  local array [0, 1, 2, 3] 
                //
                // fist row_val [0, 1] 
                // 1st col_val [ 0, 2 ]
                //
                // seconed row_val [2, 3]
                // 2nd col_val [ 1, 3]
                //************************************
                // if (rank==0)
                // {
 
                //     printf("       i%d e:%d locR:%d, locC:%d \n",i,e,(index_RC*(n/num_processors_in_d)+e), (index_RC)+ ((n/num_processors_in_d)*e)); 
                //     printf("val in localArr: %d      %d\n",localArr[(index_RC*(n/num_processors_in_d)+e)], localArr[(index_RC)+ ((n/num_processors_in_d)*e) ] );
                //     printf("val in sendArrs: %d      %d\n",row_val[e], col_val[e] );
                // }
                ////////////////////////////////////////////////////////////////
                
            } //// be careful not to cm out this !!

            // if (rank ==0 )
            // {
            //     printf("_______\n\n");
            // }



            // check to see if each prossesor assigned the corect VALUE to send arrays (row_val and  col_val) 
            // |------------------------|
            // |     0              1   | fist row_val
            // |     2              3   |  seconed row_val
            // |________________________|
            //  1st col_val    2nd col_val
            //
            //******************************

            // if (rank==0)
            // {
            //         for (int v = 0; v < (n/num_processors_in_d); v++)
            //         {
            //             printf("i%drow_val[%d] = %d, ",i,v,row_val[v]);
            //         }

            //         printf("\n---------\n");
            //         for (int v = 0; v < (n/num_processors_in_d); v++)
            //         {
            //             printf("i%dcol_val[%d] = %d, ",i,v,col_val[v]);
            //         }
            //         printf("\n---------\n");
            //         printf("\n---------\n");
            // }
            //////////////////////////////////////////////////////
                
            

            
            
        
        
        

        
        MPI_Bcast(&col_val, n/num_processors_in_d, MPI_INT, p_index, x_com);
        MPI_Bcast(&row_val , n/num_processors_in_d, MPI_INT, p_index, y_com);
        MPI_Barrier(com_cart);
        // for (int x = 0; x < n/num_processors_in_d; x++)

        // {
        //     printf("%d: %d=%d, ",rank,x,row_val[x]);
        // }

        // see row and col array after broad cast in one processor
        // if (rank==0)
        // {
        //     for (int tt  = 0; tt < n/num_processors_in_d; tt++)
        //     {
        //         printf("rv[%d]%d\n",tt ,row_val[tt]);
        //         printf("cv[%d]%d\n",tt, col_val[tt]);
        //     }
            
        // }
        
        for (int rel = 0; rel <  n/num_processors_in_d; rel++)
         {
            for (int element = 0; element< n/num_processors_in_d; element++)
            {
                // if (rank == 0)
                // {
                //     printf ( "l%d ,n%d \n",localArr[(  (rel*(n/num_processors_in_d) )  + element  )] , row_val[element]+col_val[rel]);
                //    // printf ( "l%d ,n%d \n",(  (rel*(n/num_processors_in_d) )  + element  ), element);

               
                // }
                
                
                if (localArr[(  (rel*(n/num_processors_in_d) )  + element  )] > (row_val[element]+col_val[rel]) )
                {
                    localArr[(  (rel*(n/num_processors_in_d) )  + element  )] = row_val[element]+col_val[rel];
                    // printf("\n\n\nhhhhhhhhhhaaaaaaaaaaa\n");
                }
            } 

        }
            

        
        

        MPI_Barrier(com_cart);
        //update vals?

    /////////////////////
        int reslt_Arr [n*n];
        MPI_Gather(localArr,elemenrt_per_p,MPI_INT,reslt_Arr,elemenrt_per_p,MPI_INT,MASTER,com_cart);

        // shw by processor (each line is a processors elements)
        if (rank == MASTER)
        {
            for (int  rnum = 0; rnum < n; rnum++)
            {
                printf("k= %d\n",i);

                for (int cnum = 0; cnum < n/num_processors_in_d; cnum++)
                {
                    for (int dr = 0; dr < n/num_processors_in_d; dr++)
                    {
                     printf("%d, ",reslt_Arr[ ( (rnum*n) + (cnum)*(n/num_processors_in_d)+dr   ) ]);
                     //  printf("(%d,%d,%d=%d)%d,     ",rnum,cnum,dr,( (rnum*n) + (cnum)*(n/num_processors_in_d)+dr   ), reslt_Arr[ ( (rnum*n) + (cnum)*(n/num_processors_in_d)+dr   ) ]);

                    }
                    
                }
                printf("\n");
                

            }
            printf("========\n");
        }
        

        
    }











    

    

    

    // printf("\n %d : coords %d %d r:%d c:%d",rank, coords[0], coords[1], row_val, col_val);


    MPI_Finalize();

    return EXIT_SUCCESS;

}

void show_p_Array(int numtasks, MPI_Comm com, int *array, int arraySize, int rank)
{
    MPI_Barrier(com);
    int c = 0;
    while (c < numtasks)
    {

        if (rank == c)
        {
            for (int i = 0; i < arraySize; i++)
            {

                printf("r%d:%d, ", rank, array[i]);
            }
            printf("\n\n");
        }

        MPI_Barrier(com);

        c++;
    }
}


