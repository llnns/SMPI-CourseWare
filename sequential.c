#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int main(int argc, char** argv){
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

  for(int y=0; y < height; y++){
    for(int x=0; x < width; x++){
      compute_julia_pixel(x, y, width, height, 1.0, &data[y * width * 3 + x * 3]);
    }
  }

  printf("D:%d\n", data[100]);

  // Saving
  FILE *fp;
  fp = fopen("julia_sequential.bmp", "w+");
  write_bmp_header(fp, width, height);

  int t = 0;
  for(int i=0; i < width*height*3; i++){
    t += fwrite(data+i, sizeof(char), 1, fp);
  }

  fclose(fp);
  return 0;
}
