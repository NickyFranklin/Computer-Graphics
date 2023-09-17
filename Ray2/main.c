#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


typedef struct {
  float pos[3];
  float dir[3];
} Ray;

typedef struct {
  unsigned char r, g, b;
} Pixel;

//Going to try to map 3d to 2d by dividing the distance to the object by z and then multiply x and y by z
Ray getRay(float x, float y, float z, Ray ray) {
  for(int i = 0; i < 3; i++) {
    ray.pos[i] = 0;
  }
  ray.dir[0] = x;
  ray.dir[1] = y;
  ray.dir[2] = z;
  return ray;
}

void normalize(float *x, float *y, float *z) {
  
  float norm = sqrt( ((*x) * (*x)) + ((*y) * (*y)) + ((*z) * (*z)) );
  *x = (*x)/norm;
  *y = (*y)/norm;
  *z = (*z)/norm;
}

//This assumes a 2 by 2 image grid
void TwoDto3D (float* x, float* y, float* z, float* pixelX, float* pixelY) {
  //current 2d coord / total 2d * total 3d
  *x = (*pixelX - 256) / 512 * 2;
  *y = ((*pixelY - 256) / 256) * -1;
  //lost scaling information
  *z = -2;
}

//this assumes a 2 by 2 image grid
void ThreeDto2D (float* x, float* y, float* z, float* pixelX, float* pixelY) {
  //current (3d coord * image plane / z)/ total 3d coord * total 2d coord
  float scaling = -2 / *z;
  *pixelX = (((*x/2) * scaling) * 512 + 256);
  *pixelY = (512 - (((*y/2) * scaling) *  512 + 256));
}

void printRay(Ray ray) {
  printf("RayPosition %.6f %.6f %.6f\n", ray.pos[0], ray.pos[1], ray.pos[2]);
  printf("RayDirection %.6f %.6f %.6f\n", ray.dir[0], ray.dir[1], ray.dir[2]);
}

//The coordinate functions only work with 512 pixels. Will add another parameter to the
//function later to allow any resolution
//As is self explanatory, they give me the coordinates of a pixel given a value
/*
long yCoord(int pixel) {
  pixel = pixel/3;
  return pixel / 512;
}

long xCoord(int pixel) {
  pixel = pixel/3;
  return pixel % 512;
}
*/

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
  Pixel *pixelArr;
  pixelArr = (Pixel *) malloc((width * length) * sizeof(Pixel));
  long size = width*length;
  Ray testRay;
  testRay = getRay(-1.0, -1.0, -2.0, testRay);
  printRay(testRay);
  normalize(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2]);
  printRay(testRay);

  float pixelX = 0.5;
  float pixelY = 0.5;
  //float x = 0;
  //float y = 0;
  //float z = 0;
  TwoDto3D(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2], &pixelX, &pixelY);
  normalize(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2]);
  printRay(testRay);
  
  //image plane is 2x2 for the assignment
  //float xInc = (float) 2 /(float) length;
  //float yInc = (float) 2 / (float) width;
  //printf("%.6f %.6f\n", xInc, yInc);
  
  /*
  for(int i = 0; i < size; i++) {
    pixelArr[i].r = 0;
    pixelArr[i].g = 0;
    pixelArr[i].b = 0;
    
  }
  */
  
  
  
}

