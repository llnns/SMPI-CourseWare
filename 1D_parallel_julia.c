#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "common.h"


int main(int argc, char** argv){

  // MPI START
  int size, rank;
  struct timeval start, end;
  char hostname[256];
  int hostname_len;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(hostname,&hostname_len);

  // Checking the parameter for Julia
  if(argc!=2){
    printf("Error: One parameter is required, n positive integer, the height of final image.\n");
    return 1;
  }
  int n = atoi(argv[1]);
  if(n <= 0){
    printf("Error: Invalid n value\n");
    return 1;
  }

  //printf("N value: %s\n", argv[1]);

  // Julia program
  int width = 2*n;
  int height = n;

  int rows_per_process = height / size;

  int rest = height % size;

  int extra = rest;
  if(rank<extra){
    extra=0;
    rows_per_process+=1;
  }

  int my_row = rows_per_process * rank + extra;
  int my_end = rows_per_process * (rank) + extra + rows_per_process - 1;

  printf("[Process %d/%d on (%s)]: Start on row %d, to row %d my total rows:%d\n", rank, size, hostname, my_row, my_end, rows_per_process);

  unsigned char* data = (unsigned char*)malloc(rows_per_process*(2*n)*3*sizeof(char));

  gettimeofday(&start,NULL);
  for(int y=0; y < rows_per_process; y++){
    for(int x=0; x < width; x++){
      compute_julia_pixel(x, y+my_row, width, height, 1.0, &data[y * width * 3 + x * 3]);
    }
  }
  gettimeofday(&end,NULL);

  int go=0;
  // Saving
  if(rank==0){
      FILE *fp;
      fp = fopen("parallel_1D_julia.bmp", "w+");
      write_bmp_header(fp, width, height);

      int t = 0;
      for(int i=0; i < width*rows_per_process*3; i++){
        t += fwrite(data+i, sizeof(char), 1, fp);
      }

      fclose(fp);
      printf("[Process %d/%d on (%s)]: Send GO to:%d\n", rank, size, hostname, rank+1);
      MPI_Send(&go, 1, MPI_INT, (rank+1), 1, MPI_COMM_WORLD);
      MPI_Recv(&go, 1, MPI_INT, size-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("[Process %d/%d on (%s)]: Recv go from:%d\n", rank, size, hostname, size-1);
  }else{
      MPI_Recv(&go, 1, MPI_INT, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("[Process %d/%d on (%s)]: Recv go from:%d\n", rank, size, hostname, rank-1);
      FILE *fp;
      fp = fopen("parallel_1D_julia.bmp", "a+");

      int t = 0;
      for(int i=0; i < width*rows_per_process*3; i++){
        t += fwrite(data+i, sizeof(char), 1, fp);
      }

      fclose(fp);
      printf("[Process %d/%d on (%s)]: Send GO to:%d\n", rank, size, hostname, (rank+1)%size);
      MPI_Send(&go, 1, MPI_INT, (rank+1)%size, 1, MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);
      float time_compute = (end.tv_sec*1000000.0 + end.tv_usec - start.tv_sec*1000000.0 - start.tv_usec) / 1000000.0;
      printf("[Process %d/%d on (%s)]: Time to compute my factal:%f\n", rank, size, hostname, time_compute);
  MPI_Finalize();

  return 0;
}
