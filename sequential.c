#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "common.h"

int main(int argc, char** argv){

  struct timeval start, end;

  // Checking the parameter
  if(argc!=2){
    printf("Error: One parameter is required, n positive integer, the height of final image.\n");
    return 1;
  }
  int n = atoi(argv[1]);
  if(n <= 0){
    printf("Error: Invalid n value\n");
    return 1;
  }

  printf("N value: %s\n", argv[1]);

  // Julia program
  int width = 2*n;
  int height = n;

  unsigned char* data = (unsigned char*)malloc(n*(2*n)*3*sizeof(char));

  gettimeofday(&start,NULL);
  for(int y=0; y < height; y++){
    for(int x=0; x < width; x++){
      compute_julia_pixel(x, y, width, height, 1.0, &data[y * width * 3 + x * 3]);
    }
  }
  gettimeofday(&end,NULL);

  float time_compute = (end.tv_sec*1000000.0 + end.tv_usec - start.tv_sec*1000000.0 - start.tv_usec) / 1000000.0;
  printf("Time:%f\n", time_compute);
  // Saving
  save_julia_set("julia_sequential.bmp", width, height, data);

  return 0;
}
