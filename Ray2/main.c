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

typedef struct {
  float pos[3];
  float radius;
} Sphere;

float dotProduct(float x1, float y1, float z1, float x2, float y2, float z2) {
  return (x1 * x2) + (y1 * y2) + (z1 * z2);
}

void subVector(float x1, float y1, float z1, float x2, float y2, float z2, float* resultX,
		float* resultY, float* resultZ) {
  *resultX = x1 - x2;
  *resultY = y1 - y2;
  *resultZ = z1 - z2;
}

//This assumes a 2 by 2 image grid
void TwoDto3D (float* x, float* y, float* z, float* pixelX, float* pixelY) {
  //current 2d coord / total 2d * total 3d
  *x = ((*pixelX + 0.5) - 256) / 512 * 2;
  *y = (((*pixelY + 0.5) - 256) / 256) * -1;
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

float Sphere_intersectT(Ray ray, Sphere sphere) {
  float centerX;
  float centerY;
  float centerZ;
  subVector(ray.pos[0], ray.pos[1], ray.pos[2], sphere.pos[0], sphere.pos[1], sphere.pos[2], &centerX,
	    &centerY, &centerZ);
  float discriminant;
  float rayDirCentSquare = dotProduct(ray.dir[0], ray.dir[1], ray.dir[2], centerX, centerY, centerZ);
  rayDirCentSquare = rayDirCentSquare * rayDirCentSquare;
  float dirSquare = dotProduct(ray.dir[0], ray.dir[1], ray.dir[2], ray.dir[0], ray.dir[1], ray.dir[2]);
  float radiusSquare = sphere.radius * sphere.radius;
  float centerSquareRad = dotProduct(centerX, centerY, centerZ, centerX, centerY, centerZ);
  centerSquareRad = centerSquareRad - radiusSquare;
  discriminant = (rayDirCentSquare) - (dirSquare * centerSquareRad);
  if(discriminant <= 0) {
    return -1;
  }

  discriminant = sqrt(discriminant);
  float outer = dotProduct(-1 * ray.dir[0], -1 * ray.dir[1], -1 * ray.dir[2],
			   centerX, centerY, centerZ);
  float t = (outer - discriminant) / dirSquare;
  float otherT = (outer - discriminant) / dirSquare;
  if(otherT < t && otherT > 0) {
    t = otherT;
  }
  return t;
}

void getPixelIntersect(float t, float x, float y, float z, float* pixelX, float* pixelY) {
  x = x * t;
  y = y * t;
  z = z * t;
  ThreeDto2D(&x, &y, &z, pixelX, pixelY);
}

/*
float dotProduct(float x1, float y1, float z1, float x2, float y2, float z2) {
  return (x1 * x2) + (y1 * y2) + (z1 * z2);
}

void subVector(float x1, float y1, float z1, float x2, float y2, float z2, float* resultX,
		float* resultY, float* resultZ) {
  *resultX = x1 - x2;
  *resultY = y1 - y2;
  *resultZ = z1 - z2;
}
*/
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

/*
//This assumes a 2 by 2 image grid
void TwoDto3D (float* x, float* y, float* z, float* pixelX, float* pixelY) {
  //current 2d coord / total 2d * total 3d
  *x = ((*pixelX + 0.5) - 256) / 512 * 2;
  *y = (((*pixelY + 0.5) - 256) / 256) * -1;
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
*/
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
  Ray *rayArr;
  pixelArr = (Pixel *) malloc((width * length) * sizeof(Pixel));
  rayArr = (Ray *) malloc((width * length) * sizeof(Ray));
  Pixel zeroPixel;
  zeroPixel.r = 0;
  zeroPixel.g = 0;
  zeroPixel.b = 0;
  Ray zeroRay;
  zeroRay = getRay(0.0, 0.0, 0.0, zeroRay);
  for(int i = 0; i < width * length; i++) {
    pixelArr[i] = zeroPixel;
    rayArr[i] = zeroRay;
  }
  
  
  long size = width*length;
  /*
  Ray testRay;
  testRay = getRay(-1.0, -1.0, -2.0, testRay);
  printRay(testRay);
  normalize(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2]);
  printRay(testRay);
  */
  float pixelX = 0;
  float pixelY = 0;
  /*
  TwoDto3D(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2], &pixelX, &pixelY);
  normalize(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2]);
  printRay(testRay);

  pixelX = 511;
  pixelY = 511;
  TwoDto3D(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2], &pixelX, &pixelY);
  normalize(&testRay.dir[0], &testRay.dir[1], &testRay.dir[2]);
  printRay(testRay);
  */
  
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
  Sphere sphere;
  sphere.pos[0] = 2;
  sphere.pos[1] = 2;
  sphere.pos[2] = -16;
  sphere.radius = 5.3547;

  
  for(int y = 0; y < length; y++) {
    for(int x = 0; x < width; x++) {
      //printf("x: %d , y: %d\n", x, y);
      int i  = x + (y * 512);
      pixelX = (float) x;
      pixelY = (float) y;
      TwoDto3D(&rayArr[i].dir[0], &rayArr[i].dir[1], &rayArr[i].dir[2], &pixelX, &pixelY);
      normalize(&rayArr[i].dir[0], &rayArr[i].dir[1], &rayArr[i].dir[2]);
      if(x == 0 && y == 511) {
	printf("Bottom left pixel\n");
	printRay(rayArr[i]);
      }
      if(x == 511 && y == 0) {
	printf("Top right pixel\n");
	printRay(rayArr[i]);
      }
      if(x == 255 && y == 257) {
	printf("Middle pixel\n");
	printRay(rayArr[i]);
      }
      float t = Sphere_intersectT(rayArr[i], sphere);
      if(t > 0) {
	pixelArr[i].r = 255;
	pixelArr[i].g = 255;
	pixelArr[i].b = 255;
      }
      else {
	pixelArr[i].r = 128;
	pixelArr[i].g = 0;
	pixelArr[i].b = 0;
      }
    }
  }

  FILE *fp;
  fp = fopen("sphere.png", "w");
  stbi_write_png("sphere.png", 512, 512, 3, pixelArr, 512*3);
  fclose(fp);
  return 0;
}

