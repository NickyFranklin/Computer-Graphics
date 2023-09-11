#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


struct Ray {
  float position[3];
  float direction[3];
};
typedef struct Ray Ray;

//Going to try to map 3d to 2d by dividing the distance to the object by z and then multiply x and y by z
Ray getRay(float x, float y, float z) {
  
}

void normalize(float *x, float *y, float *z) {
  
  float norm = sqrt( ((*x) * (*x)) + ((*y) * (*y)) + ((*z) * (*z)) );
  *x = (*x)/norm;
  *y = (*y)/norm;
  *z = (*z)/norm;
}

void 2Dto3D (float* x, float* y, float* z, float* pixelX, float* pixelY) {
  
}

//The coordinate functions only work with 512 pixels. Will add another parameter to the
//function later to allow any resolution
//As is self explanatory, they give me the coordinates of a pixel given a value
long yCoord(int pixel) {
  pixel = pixel/3;
  return pixel / 512;
}

long xCoord(int pixel) {
  pixel = pixel/3;
  return pixel % 512;
}

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

  

}

