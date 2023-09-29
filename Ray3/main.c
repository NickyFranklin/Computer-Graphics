#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//These structs are to help simplify working later
typedef struct {
  float pos[3];
  float dir[3];
} Ray;

typedef struct {
  unsigned char r, g, b;
} Pixel;

typedef struct {
  float color[3];
  int reflective;
} Material;

typedef struct {
  float v[3][3];
  Material material;
} Triangle;

typedef struct {
  float t;
  Material material;
  float pos[3];
  //This is the vector from the center of the sphere to the intersection point
  float surfaceNorm[3];
} RayHit;

typedef struct {
  float pos[3];
  float radius;
  Material material;
} Sphere;

float lightPos[3] = {3, 5, -15};

float dotProduct(float x1, float y1, float z1, float x2, float y2, float z2) {
  return (x1 * x2) + (y1 * y2) + (z1 * z2);
}

//Subtracts vectors from eachother
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

void normalize(float *x, float *y, float *z) {
  
  float norm = sqrt( ((*x) * (*x)) + ((*y) * (*y)) + ((*z) * (*z)) );
  *x = (*x)/norm;
  *y = (*y)/norm;
  *z = (*z)/norm;
}

//This determines the T value for the sphere intersection
RayHit Sphere_intersectT(Ray ray, Sphere sphere) {
  //This calculates (e-c) in the sphere equation
  RayHit rayHit;
  //Ray rayToIntersect;
  float centerX;
  float centerY;
  float centerZ;
  rayHit.t = -1;
  for(int i = 0; i < 3; i++) {
    rayHit.pos[i] = sphere.pos[i];
  }
  rayHit.material.reflective = sphere.material.reflective;
  rayHit.material = sphere.material;
  
  //This may be an issue as I believe this is negative but it might be fine
  subVector(ray.pos[0], ray.pos[1], ray.pos[2], sphere.pos[0], sphere.pos[1], sphere.pos[2], &centerX,
	    &centerY, &centerZ);

  float discriminant;

  //This calculates ((d * (e-c))^2)
  float rayDirCentSquare = dotProduct(ray.dir[0], ray.dir[1], ray.dir[2], centerX, centerY, centerZ);
  rayDirCentSquare = rayDirCentSquare * rayDirCentSquare;

  //This calculates d*d
  float dirSquare = dotProduct(ray.dir[0], ray.dir[1], ray.dir[2], ray.dir[0], ray.dir[1], ray.dir[2]);

  //This calculates R^2
  float radiusSquare = sphere.radius * sphere.radius;

  //This calculates ((e-c)*(e-c) - R^2)
  float centerSquareRad = dotProduct(centerX, centerY, centerZ, centerX, centerY, centerZ);
  centerSquareRad = centerSquareRad - radiusSquare;

  //Calculates if the stuff under the square root sign will be over 0 or not
  //If it isnt, it just returns -1
  discriminant = (rayDirCentSquare) - (dirSquare * centerSquareRad);
  if(discriminant <= 0) {
    return rayHit;
  }
  
  discriminant = sqrt(discriminant);
  //Calculates the rest of the equation
  float outer = dotProduct(-1 * ray.dir[0], -1 * ray.dir[1], -1 * ray.dir[2],
			   centerX, centerY, centerZ);
  //Calculates both sides of the intersection
  float t = (outer - discriminant) / dirSquare;
  float otherT = (outer - discriminant) / dirSquare;
  //Whichever side is closer gets returned
  if(otherT < t && otherT > 0) {
    t = otherT;
  }
  rayHit.t = t;

  for(int j = 0; j < 3; j++) {
    ray.pos[j] = ray.dir[j] * t;
    //rayToIntersect.pos[j] = sphere.pos[j];
  }
  
  subVector(ray.pos[0], ray.pos[1], ray.pos[2],
	    sphere.pos[0], sphere.pos[1], sphere.pos[2],
	    &rayHit.surfaceNorm[0], &rayHit.surfaceNorm[1], &rayHit.surfaceNorm[2]);
  normalize(&rayHit.surfaceNorm[0], &rayHit.surfaceNorm[1], &rayHit.surfaceNorm[2]);
  
  
  return rayHit;
}

RayHit TriangleIntersection(Ray ray, Triangle triangle) {
  //This first section is going to be just declaring variables for the triangle intersection code
  RayHit rayHit;
  float x1 = triangle.v[0][0];
  float x2 = triangle.v[1][0];
  float x3 = triangle.v[2][0];
  float y1 = triangle.v[0][1];
  float y2 = triangle.v[1][1];
  float y3 = triangle.v[2][1];
  float z1 = triangle.v[0][2];
  float z2 = triangle.v[1][2];
  float z3 = triangle.v[2][2];
  float A = x1 - x2;
  float B = y1 - y2;
  float C = z1 - z2;
  float D = x1 - x3;
  float E = y1 - y3;
  float F = z1 - z3;
  float G = ray.dir[0];
  float H = ray.dir[1];
  float I = ray.dir[2];
  float J = x1 - ray.pos[0];
  float K = y1 - ray.pos[1];
  float L = z1 - ray.pos[2];
  float M = A*(E*I - H*F) + B*(G*F - D*I) + C*(D*H - E*G);
  float beta = (J*(E*I - H*F) + K*(G*F - D*I) + L(D*H - E*G)) / M;
  float gamma = (I*(A*K - J*B) + H*(J*C - A*L) + G(B*L - K*C)) / M;
  float t = (-(F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C))) / M;

  //Might want to fill out the rest of ray hit just in case
  if(t < 0) {
    rayHit.t = -1;
    return rayHit;
  }

  if(gamma < 0 || gamma > 1) {
    rayHit.t = -1;
    return rayHit;
  }

  if(beta < 0 || beta > (1-gamma)) {
    rayHit.t = -1;
    return rayHit;
  }
  
  //Now that we know the rayHit, we need to find light info


  
  return rayHit;
}

//this is never used lol
void getPixelIntersect(float t, float x, float y, float z, float* pixelX, float* pixelY) {
  x = x * t;
  y = y * t;
  z = z * t;
  ThreeDto2D(&x, &y, &z, pixelX, pixelY);
}

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

void printRay(Ray ray) {
  printf("RayPosition %.6f %.6f %.6f\n", ray.pos[0], ray.pos[1], ray.pos[2]);
  printf("RayDirection %.6f %.6f %.6f\n", ray.dir[0], ray.dir[1], ray.dir[2]);
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
  float pixelX = 0;
  float pixelY = 0;
  
  Sphere sphereArr[3];
  int sphereArrSize = 3;
  for(int i = 0; i < sphereArrSize; i++) {
    if(i == 0) {
      sphereArr[i].pos[0] = 0;
      sphereArr[i].pos[1] = 0;
      sphereArr[i].pos[2] = -16;
      sphereArr[i].radius = 2;
      sphereArr[i].material.color[0] = 0;
      sphereArr[i].material.color[1] = 1;
      sphereArr[i].material.color[2] = 0;
      sphereArr[i].material.reflective = 0;
    }
    
    if(i == 1) {
      sphereArr[i].pos[0] = 3;
      sphereArr[i].pos[1] = -1;
      sphereArr[i].pos[2] = -14;
      sphereArr[i].radius = 1;
      sphereArr[i].material.color[0] = 0;
      sphereArr[i].material.color[1] = 0;
      sphereArr[i].material.color[2] = 1;
      sphereArr[i].material.reflective = 0;
    }

    if(i == 2) {
      sphereArr[i].pos[0] = -3;
      sphereArr[i].pos[1] = -1;
      sphereArr[i].pos[2] = -14;
      sphereArr[i].radius = 1;
      sphereArr[i].material.color[0] = 1;
      sphereArr[i].material.color[1] = 0;
      sphereArr[i].material.color[2] = 0;
      sphereArr[i].material.reflective = 0;
    }
    
  }
  RayHit rayHitArr[3];
  
  
  for(int y = 0; y < length; y++) {
    for(int x = 0; x < width; x++) {
      //printf("x: %d , y: %d\n", x, y);
      int i  = x + (y * 512);
      pixelX = (float) x;
      pixelY = (float) y;
      TwoDto3D(&rayArr[i].dir[0], &rayArr[i].dir[1], &rayArr[i].dir[2], &pixelX, &pixelY);
      normalize(&rayArr[i].dir[0], &rayArr[i].dir[1], &rayArr[i].dir[2]);
      if(x == 0 && y == 511) {
	//printf("Bottom left pixel\n");
	//printRay(rayArr[i]);
      }
      if(x == 511 && y == 0) {
	//printf("Top right pixel\n");
	//printRay(rayArr[i]);
      }
      if(x == 255 && y == 257) {
	//printf("Middle pixel\n");
	//printRay(rayArr[i]);
      }

      int max = 0;
      RayHit realRay;
      
      for(int j = 0; j < sphereArrSize; j++) {
	rayHitArr[j] = Sphere_intersectT(rayArr[i], sphereArr[j]);
      }

      //realRay = rayHitArr[0];
      
      float t = -1;
      for(int j = 0; j < sphereArrSize; j++) {
	if(rayHitArr[j].t > t && t < 0) {
	  t = rayHitArr[j].t;
	  max = j;
	  //printf("%d\n", max);
	}

	else if(t > 0 && rayHitArr[j].t > 0 && rayHitArr[j].t < t) {
	  t = rayHitArr[j].t;
	  max = j;
	}
	
      }
      realRay = rayHitArr[max];

      

      
      if(t > 0) {
	//If T hit the object, we want to calculate the intersection point with Ray direction * t
	//We can do this by getting the intersection point, the sphere center, and subtracting them
	//and then normalizing them
	//We will then get the vector from the intersection point to the light
	//Normalize it
	//Then we take the dot product of both of those
	//We will then store the result in a variable called diffuse, if diffuse is less than 0.2,
	//Make it 0.2
	if(realRay.material.reflective == 0) {
	  float diffuse = 0.2;
	  Ray ray = rayArr[i];
	  //Ray rayToIntersect;
	  Ray rayToLight;
	  for(int j = 0; j < 3; j++) {
	    //ray.pos[j] = ray.dir[j] * t;
	    //rayToIntersect.pos[j] = sphere.pos[j];
	    rayToLight.pos[j] = realRay.pos[j];
	  }

	  /*
	  subVector(ray.pos[0], ray.pos[1], ray.pos[2],
		    sphere.pos[0], sphere.pos[1], sphere.pos[2],
		    &rayToIntersect.dir[0], &rayToIntersect.dir[1], &rayToIntersect.dir[2]);
	  normalize(&rayToIntersect.dir[0], &rayToIntersect.dir[1], &rayToIntersect.dir[2]);
	  */
	  
	  //subvector is x1 - x2
	  subVector(lightPos[0], lightPos[1], lightPos[2],
		    rayToLight.pos[0], rayToLight.pos[1], rayToLight.pos[2],
		    &rayToLight.dir[0], &rayToLight.dir[1], &rayToLight.dir[2]);
	  normalize(&rayToLight.dir[0], &rayToLight.dir[1], &rayToLight.dir[2]);
	  
	  diffuse = dotProduct(rayToLight.dir[0], rayToLight.dir[1], rayToLight.dir[2],
			       realRay.surfaceNorm[0], realRay.surfaceNorm[1], realRay.surfaceNorm[2]);
	  if(diffuse < 0.2) {
	    diffuse = 0.2;
	  }
	    
	  pixelArr[i].r = realRay.material.color[0] * diffuse * 255;
	  pixelArr[i].g = realRay.material.color[1] * diffuse * 255;
	  pixelArr[i].b = realRay.material.color[2] * diffuse * 255;
	}
      }
      else {
	pixelArr[i].r = 0;
	pixelArr[i].g = 0;
	pixelArr[i].b = 0;
      }
    }
  }

  FILE *fp;
  fp = fopen("sphere.png", "w");
  stbi_write_png("sphere.png", 512, 512, 3, pixelArr, 512*3);
  fclose(fp);
  free(pixelArr);
  free(rayArr);
  return 0;
}

