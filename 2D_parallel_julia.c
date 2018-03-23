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

  int tiles = sqrt(size);
  int my_tile_h = rank/tiles;
  int my_tile_w = rank%tiles;

  int width_per_process = width / tiles;
  int height_per_process = height / tiles;

  int my_initial_cell_x = width_per_process * my_tile_w;
  int my_initial_cell_y = height_per_process * my_tile_h;

  int my_end_cell_x = width_per_process * (my_tile_w + 1) - 1;
  int my_end_cell_y = height_per_process * (my_tile_h + 1) - 1;

  int total_cells = width_per_process*height_per_process;


  printf("[Process %d/%d on (%s)]: Tile = (%d/%d), cells = (%d:%d/%d:%d), my total cells:%d\n",
        rank, size, hostname, my_tile_h, my_tile_w, my_initial_cell_x, my_end_cell_x, my_initial_cell_y,
        my_end_cell_y, total_cells);

  unsigned char* data = (unsigned char*)malloc(total_cells*3*sizeof(char));

  gettimeofday(&start,NULL);
  for(int y=0; y < height_per_process; y++){
    for(int x=0; x < width_per_process; x++){
      compute_julia_pixel(x+my_initial_cell_x, y+my_initial_cell_y, width, height, 1.0, &data[y * width_per_process * 3 + x * 3]);
      //data[y * width_per_process * 3 + x * 3] = rank*20;
      //data[y * width_per_process * 3 + x * 3 + 1] = rank*20;
      //data[y * width_per_process * 3 + x * 3 + 2] = rank*20;
    }
  }
  gettimeofday(&end,NULL);

  //char buf[20];
  //sprintf(buf, "rank_%d.bmp", rank);

  //save_julia_set(buf, width_per_process, height_per_process, data);

  int go=0;
  int actual_line = 0;

  int recip = rank-1;
  if(my_tile_w==0){
    recip = rank-tiles;
  }

  // Saving
  if(rank==0){
      FILE *fp;
      fp = fopen("parallel_2D_julia.bmp", "w+");
      write_bmp_header(fp, width, height);

      int t = 0;
      for(int i=0; i < width_per_process*3; i++){
        t += fwrite(data+i, sizeof(char), 1, fp);
      }
      recip=tiles-1;
      actual_line++;
      fclose(fp);
      printf("[Process %d/%d on (%s)]: Send GO to:%d\n", rank, size, hostname, rank+1);
      MPI_Send(&go, 1, MPI_INT, (rank+1), 1, MPI_COMM_WORLD);
  }

  int send=0;
  while(actual_line<height_per_process){
      MPI_Recv(&go, 1, MPI_INT, recip, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("[Process %d/%d on (%s)]: Recv go from:%d\n", rank, size, hostname, recip);
      FILE *fp;
      fp = fopen("parallel_2D_julia.bmp", "a+");

      int t = 0;
      for(int i=0; i < width_per_process*3; i++){
        t += fwrite(data+i+(actual_line*width_per_process*3), sizeof(char), 1, fp);
      }
      actual_line++;
      fclose(fp);
      if(my_tile_w==0){
        recip = rank+tiles-1;
      }else{
        recip = rank-1;
      }

      if(my_tile_w==(tiles-1)){
        send = rank-tiles+1;
      }else{
        send = rank+1;
      }
      printf("[Process %d/%d on (%s)]: Send GO to:%d\n", rank, size, hostname, send);
      MPI_Send(&go, 1, MPI_INT, send, 1, MPI_COMM_WORLD);

  }
  if(my_tile_w==0){
    MPI_Recv(&go, 1, MPI_INT, recip, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("[Process %d/%d on (%s)]: Send GO to:%d\n", rank, size, hostname, rank+tiles);
    MPI_Send(&go, 1, MPI_INT, rank+tiles, 1, MPI_COMM_WORLD);
  }


  MPI_Barrier(MPI_COMM_WORLD);
      float time_compute = (end.tv_sec*1000000.0 + end.tv_usec - start.tv_sec*1000000.0 - start.tv_usec) / 1000000.0;
      printf("[Process %d/%d on (%s)]: Time to compute my factal:%f\n", rank, size, hostname, time_compute);
  MPI_Finalize();

  return 0;
}
