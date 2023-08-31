#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  //We will need width and length for the window
  int width;
  int length;

  //If no number is specified, default window size is 512 
  if(argc < 2) {
    length = 512;
    width = 512;
  }

  //If one number is specified, then that is the length and width
  else if(argc == 2) {
    length = atoi(argv[1]);
    width = atoi(argv[1]);
  }

  //If two numbers are specified, the first is length, the second is width
  else {
    length = atoi(argv[1]);
    width = atoi(argv[2]);
  }

  //making and allocating space for the array of pixels
  unsigned char *pixelArray;
  pixelArray = (unsigned char*) malloc((width * length * 3) * sizeof(unsigned char));
  long size = width*length*3;

  //Set up array
  for(int i = 0; i < size; i++) {
    switch(i % 3) {

    case 0 :
      pixelArray[i] = 255;
      break;
      
    case 1:
      pixelArray[i] = 105;
      break;
      
    case 2:
      pixelArray[i] = 180;
      break;
      
    }
  }
  
  FILE *fp;

  fp = fopen("ray1.png", "w");
  
  stbi_write_png("ray1.png", width, length, 3, pixelArray, width*3);
  
  fclose(fp);

  
  free(pixelArray);
  return 0;
}

