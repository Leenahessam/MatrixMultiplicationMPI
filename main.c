#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"


int **mat1, **mat2,**matRes, row1, col1, row2, col2, data, i, j,h,k, extra;
void free_2d_matrix (int **mat)
{
  free (mat[0]);
  free (mat);
}
int **matAlloc (int row, int col)
{
    int* x;
    int** mat;
    int count = 0;
    x = malloc(sizeof(int)*row *col);
    mat = malloc(sizeof(int*)*row);
    for(count = 0; count<row; count++)
       mat[count] = &x[count * col];
return mat;
}
int main(int argc , char * argv[])
{
    int my_rank;
	int p;
	int source;
	int tag = 0;
	MPI_Status status;
    int dest;


	MPI_Init( &argc , &argv );

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	MPI_Comm_size(MPI_COMM_WORLD, &p);

  //slaves
  if( my_rank != 0)
  {
        int x;
        dest = 0;
        source = 0;
        int slave_row;
        int size_matRes = slave_row*col2;
        MPI_Recv(&col1, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&slave_row, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&col2, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&row2, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        mat1 = matAlloc(slave_row,col1);
        mat2 = matAlloc(row2,col2);
        MPI_Recv(&size_matRes, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        matRes = matAlloc(slave_row,col2);
        MPI_Recv(&(mat1[0][0]), col1*slave_row, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&(mat2[0][0]), row2*col2 , MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        for (i = 0; i< slave_row; i++)
            for (j=0; j< col2; j++)
                matRes[i][j] = 0;

        for (i = 0; i <slave_row; i++)
            for (j = 0; j <col2; j++)
                for (k = 0; k <row2; k++)
                    matRes[i][j] += (mat1[i][k] * mat2[k][j]);

        //sending back to the master
        MPI_Send(&size_matRes, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
        MPI_Send(&matRes[0][0],slave_row*col2, MPI_INT, source, tag, MPI_COMM_WORLD);
    }

  //master
  else
  {
     printf("Welcome to Matrix multiplication program! ");
     printf("\n--------------------------------------------------\n");
     printf("Please enter dimensions of the first matrix: ");
            scanf("%d %d", &row1, &col1);
            mat1 = matAlloc(row1,col1);
            for(i = 0; i < row1; i++){
               for(j = 0 ; j < col1; j++){
                 scanf("%d", &data);
                 mat1[i][j] = data;
              }}

            printf("\nnow enter dimensions of the second matrix: ");
            scanf("%d %d", &row2, &col2);
            mat2 = matAlloc(row2,col2);
            for(i = 0; i < row2; i++){
              for(j = 0 ; j < col2; j++){
                 scanf("%d", &data);
                 mat2[i][j] = data;
             }}
       if (col1 != row2)
            printf("This is invalid ");
       else {
    // dividing missions on the given cores (excluding the master)
            int slave_row;
            int remaining_rows;
            int size_matRes;
           /* if (row1 < p-1){
               p = row1; //extra processes will do nothing
               slave_row =1;
            }*/
            //else
            slave_row = row1/(p-1);
            remaining_rows = row1%(p-1);
            size_matRes = slave_row*col2;
    //---------------------------------------------------------------------------------------
    // first possibility p = no. of rows for each slave
         int k = 0;
         for( dest = 1; dest < p ; dest++)
           {
            //sending row of first matrix and matrix 2
            MPI_Send(&col1, 1, MPI_INT, dest, tag, MPI_COMM_WORLD); //number of cols for each slave
            MPI_Send(&slave_row, 1, MPI_INT, dest, tag, MPI_COMM_WORLD); //number of rows for each slave
            MPI_Send(&col2, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&row2, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&size_matRes, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            MPI_Send(&mat1[k][0],slave_row*col1,MPI_INT, dest,tag, MPI_COMM_WORLD);
            MPI_Send(&mat2[0][0],row2*col2,MPI_INT, dest,tag, MPI_COMM_WORLD);
            k+=slave_row;
            }
            //receiving the product matrix
            k = 0;
            matRes = matAlloc(row1,col2);
            for( dest = 1; dest < p; dest++)
            {
                MPI_Recv(&size_matRes, 1, MPI_INT, dest, tag, MPI_COMM_WORLD,&status);
                MPI_Recv(&matRes[k][0], slave_row*col2, MPI_INT, dest, tag, MPI_COMM_WORLD,&status);
                k+=slave_row;
            }
            if (remaining_rows != 0){    //second possibility
             for(i = 0; i < remaining_rows ; i++){
                for (j = 0; j <col2; j++) {
                 for (h = 0; h <row2; h++){
                    matRes[k][j] += (mat1[k][h] * mat2[h][j]);
                 }}
                 k++;
        }
        }
         printf("Result matrix %d x %d is \n", row1,col2);
        for (i=0; i<row1; i++){
            for(j=0; j<col2; j++){
                printf("%d ", matRes[i][j]);
            }
            printf("\n");
        }

	}

}
    MPI_Finalize();
    free_2d_matrix(mat1);
    free_2d_matrix(mat2);
    /* shutdown MPI */

	return 0;

}
