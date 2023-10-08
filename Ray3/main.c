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

Ray crossProduct(Ray ray1, Ray ray2) {
  Ray ray;
  ray.dir[0] = (ray1.dir[1] * ray2.dir[2]) - (ray1.dir[2] * ray2.dir[1]);
  ray.dir[1] = (ray1.dir[2] * ray2.dir[0]) - (ray1.dir[0] * ray2.dir[2]);
  ray.dir[2] = (ray1.dir[0] * ray2.dir[1]) - (ray1.dir[1] * ray2.dir[0]);
  return ray;
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
  subVector(ray.pos[0], ray.pos[1], ray.pos[2],
	    sphere.pos[0], sphere.pos[1], sphere.pos[2],
	    &centerX, &centerY, &centerZ);
  
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
  if(otherT > 0 && t > 0) {
    if(otherT < t) {
      t = otherT;
    }
  }
 
  rayHit.t = t;
  normalize(&ray.dir[0], &ray.dir[1], &ray.dir[2]);
  for(int j = 0; j < 3; j++) {
    ray.pos[j] = ray.pos[j] + ray.dir[j] * t;
    rayHit.pos[j] = ray.pos[j];
  }
  
  subVector(ray.pos[0], ray.pos[1], ray.pos[2],
	    sphere.pos[0], sphere.pos[1], sphere.pos[2],
	    &rayHit.surfaceNorm[0], &rayHit.surfaceNorm[1], &rayHit.surfaceNorm[2]);
  //These two are equivalent
  //normalize(&rayHit.surfaceNorm[0], &rayHit.surfaceNorm[1], &rayHit.surfaceNorm[2]);
  rayHit.surfaceNorm[0] /= sphere.radius;
  rayHit.surfaceNorm[1] /= sphere.radius;
  rayHit.surfaceNorm[2] /= sphere.radius;
  
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
  float beta = (J*(E*I - H*F) + K*(G*F - D*I) + L*(D*H - E*G)) / M;
  float gamma = (I*(A*K - J*B) + H*(J*C - A*L) + G*(B*L - K*C)) / M;
  float t = (-(F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C))) / M;

  //Might want to fill out the rest of ray hit just in case
  if(t < 0) {
    rayHit.t = -1;
    rayHit.material = triangle.material;
    return rayHit;
  }

  if(gamma < 0 || gamma > 1) {
    rayHit.t = -1;
    rayHit.material = triangle.material;
    return rayHit;
  }

  if(beta < 0 || beta > (1-gamma)) {
    rayHit.t = -1;
    rayHit.material = triangle.material;
    return rayHit;
  }
  
  //Now that we know the rayHit, we need to find light info
  rayHit.t = t;
  rayHit.material = triangle.material;

  
  //Hopefull this look is ok
  normalize(&ray.dir[0], &ray.dir[1], &ray.dir[2]);
  for(int j = 0; j < 3; j++) {
    rayHit.pos[j] = ray.pos[j] + ray.dir[j] * t;
  }
  
  Ray vectorAtoC;
  Ray vectorAtoB;
  for(int i = 0; i < 3; i++) {
    vectorAtoC.dir[i] = triangle.v[2][i] - triangle.v[0][i];
    vectorAtoB.dir[i] = triangle.v[1][i] - triangle.v[0][i];
  }
  //Do we want to normalize these vectors? Doesn't matter if we do
  normalize(&vectorAtoC.dir[0], &vectorAtoC.dir[1], &vectorAtoC.dir[2]);
  normalize(&vectorAtoB.dir[0], &vectorAtoB.dir[1], &vectorAtoB.dir[2]);
  //We want to do B cross product C for the norm
  Ray norm = crossProduct(vectorAtoB, vectorAtoC);
  normalize(&norm.dir[0], &norm.dir[1], &norm.dir[2]);
  for(int i = 0; i < 3; i++) {
    rayHit.surfaceNorm[i] = norm.dir[i];
  }
  return rayHit;
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

RayHit collision(RayHit* rayHitArr, Ray ray, Sphere* sphereArr, Triangle* triangleArr,
		 int sphereArrSize, int triangleArrSize) {
  //This code goes through all the objects that can be instersected with
  //and returns the closest object hit
  int max = 0;
  RayHit realRay;
  //Loops through spheres and then triangles
  for(int j = 0; j < sphereArrSize + triangleArrSize; j++) {
    if(j < sphereArrSize) {
      rayHitArr[j] = Sphere_intersectT(ray, sphereArr[j]);
    }
    
    else {
      rayHitArr[j] = TriangleIntersection(ray, triangleArr[j-sphereArrSize]);
    }
  }

  //Checks t values and finds the smallest one to add to rayHit
  float t = -1;
  for(int j = 0; j < sphereArrSize + triangleArrSize; j++) {  
    if(rayHitArr[j].t > t && t < 0 && rayHitArr[j].t > 0) {
      t = rayHitArr[j].t;
      max = j;
    }

    
    else if(t > 0 && rayHitArr[j].t > 0 && rayHitArr[j].t < t) {
      t = rayHitArr[j].t;
      max = j;
    }
    
  }
  realRay = rayHitArr[max];
  return realRay;
}

//Code for the diffuse shading and shadows
float diffuseShading(float diffuse, RayHit realRay, Sphere* sphereArr,
		     Triangle* triangleArr, int sphereArrSize, int triangleArrSize,
		     RayHit* rayHitArr) {
  //Set diffuse to 0.2 immediately just in case
  diffuse = 0.2;
  Ray rayToLight;

  //This block of code determines the diffusion
  for(int j = 0; j < 3; j++) {
    rayToLight.pos[j] = realRay.pos[j];
  }
  subVector(lightPos[0], lightPos[1], lightPos[2],
	    rayToLight.pos[0], rayToLight.pos[1], rayToLight.pos[2],
	    &rayToLight.dir[0], &rayToLight.dir[1], &rayToLight.dir[2]);
  normalize(&rayToLight.dir[0], &rayToLight.dir[1], &rayToLight.dir[2]);
  diffuse = dotProduct(rayToLight.dir[0], rayToLight.dir[1], rayToLight.dir[2],
		       realRay.surfaceNorm[0], realRay.surfaceNorm[1], realRay.surfaceNorm[2]);

  if(diffuse < 0.2) {
    return diffuse = 0.2;
  }

  for(int j = 0; j < 3; j++) {
    rayToLight.pos[j] = rayToLight.pos[j] + rayToLight.dir[j] * 0.0001;
  }

  //Shadow code
  //Find out if the light is being blocked
  realRay = collision(rayHitArr, rayToLight, sphereArr, triangleArr,
		      sphereArrSize, triangleArrSize);
  
  
  float tlight = 0;
  tlight = sqrt( (lightPos[0] - rayToLight.pos[0]) * (lightPos[0] - rayToLight.pos[0]) +
		 (lightPos[1] - rayToLight.pos[1]) * (lightPos[1] - rayToLight.pos[1]) +
		 (lightPos[2] - rayToLight.pos[2]) * (lightPos[2] - rayToLight.pos[2]));
  
  float t = realRay.t;
  //if light it blocked, set minimum diffusion
  if(t < tlight && t > 0) {
    //printf("realRay.t = %f , tlight = %f\n", realRay.t, tlight);
    return 0.2;
  }
  
  return diffuse;
  
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


  //This is all just setting up the shapes
  Sphere sphereArr[3];
  Triangle triangleArr[5];
  int triangleArrSize = 5;
  int sphereArrSize = 3;
  for(int i = 0; i < sphereArrSize; i++) {
    if(i == 0) {
      sphereArr[i].pos[0] = 0;
      sphereArr[i].pos[1] = 0;
      sphereArr[i].pos[2] = -16;
      sphereArr[i].radius = 2;
      sphereArr[i].material.color[0] = 0;
      sphereArr[i].material.color[1] = 0;
      sphereArr[i].material.color[2] = 0;
      sphereArr[i].material.reflective = 1;
    }
    
    if(i == 1) {
      sphereArr[i].pos[0] = 3;
      sphereArr[i].pos[1] = -1;
      sphereArr[i].pos[2] = -14;
      sphereArr[i].radius = 1;
      sphereArr[i].material.color[0] = 0;
      sphereArr[i].material.color[1] = 0;
      sphereArr[i].material.color[2] = 0;
      sphereArr[i].material.reflective = 1;
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
  triangleArr[0].v[0][0] = -8;
  triangleArr[0].v[0][1] = -2;
  triangleArr[0].v[0][2] = -20;
  triangleArr[0].v[1][0] = 8;
  triangleArr[0].v[1][1] = -2;
  triangleArr[0].v[1][2] = -20;
  triangleArr[0].v[2][0] = 8;
  triangleArr[0].v[2][1] = 10;
  triangleArr[0].v[2][2] = -20;
  triangleArr[0].material.color[0] = 0;
  triangleArr[0].material.color[1] = 0;
  triangleArr[0].material.color[2] = 1;
  triangleArr[0].material.reflective = 0;

  triangleArr[1].v[0][0] = -8;
  triangleArr[1].v[0][1] = -2;
  triangleArr[1].v[0][2] = -20;
  triangleArr[1].v[1][0] = 8;
  triangleArr[1].v[1][1] = 10;
  triangleArr[1].v[1][2] = -20;
  triangleArr[1].v[2][0] = -8;
  triangleArr[1].v[2][1] = 10;
  triangleArr[1].v[2][2] = -20;
  triangleArr[1].material.color[0] = 0;
  triangleArr[1].material.color[1] = 0;
  triangleArr[1].material.color[2] = 1;
  triangleArr[1].material.reflective = 0;

  triangleArr[2].v[0][0] = -8;
  triangleArr[2].v[0][1] = -2;
  triangleArr[2].v[0][2] = -20;
  triangleArr[2].v[1][0] = 8;
  triangleArr[2].v[1][1] = -2;
  triangleArr[2].v[1][2] = -10;
  triangleArr[2].v[2][0] = 8;
  triangleArr[2].v[2][1] = -2;
  triangleArr[2].v[2][2] = -20;
  triangleArr[2].material.color[0] = 1;
  triangleArr[2].material.color[1] = 1;
  triangleArr[2].material.color[2] = 1;
  triangleArr[2].material.reflective = 0;

  triangleArr[3].v[0][0] = -8;
  triangleArr[3].v[0][1] = -2;
  triangleArr[3].v[0][2] = -20;
  triangleArr[3].v[1][0] = -8;
  triangleArr[3].v[1][1] = -2;
  triangleArr[3].v[1][2] = -10;
  triangleArr[3].v[2][0] = 8;
  triangleArr[3].v[2][1] = -2;
  triangleArr[3].v[2][2] = -10;
  triangleArr[3].material.color[0] = 1;
  triangleArr[3].material.color[1] = 1;
  triangleArr[3].material.color[2] = 1;
  triangleArr[3].material.reflective = 0;  
  
  triangleArr[4].v[0][0] = 8;
  triangleArr[4].v[0][1] = -2;
  triangleArr[4].v[0][2] = -20;
  triangleArr[4].v[1][0] = 8;
  triangleArr[4].v[1][1] = -2;
  triangleArr[4].v[1][2] = -10;
  triangleArr[4].v[2][0] = 8;
  triangleArr[4].v[2][1] = 10;
  triangleArr[4].v[2][2] = -20;
  triangleArr[4].material.color[0] = 1;
  triangleArr[4].material.color[1] = 0;
  triangleArr[4].material.color[2] = 0;
  triangleArr[4].material.reflective = 0;
  
  RayHit rayHitArr[8];
  
  
  for(int y = 0; y < length; y++) {
    for(int x = 0; x < width; x++) {
      int i  = x + (y * 512);
      pixelX = (float) x;
      pixelY = (float) y;
      TwoDto3D(&rayArr[i].dir[0], &rayArr[i].dir[1], &rayArr[i].dir[2], &pixelX, &pixelY);
      normalize(&rayArr[i].dir[0], &rayArr[i].dir[1], &rayArr[i].dir[2]);
      RayHit realRay;
      
      float t = -1;
      Ray ray = rayArr[i];

      //Find closest collision if there is one
      realRay = collision(rayHitArr, ray, sphereArr, triangleArr,
			  sphereArrSize, triangleArrSize);
      if(realRay.t > 0) {
	t = realRay.t;
      }

      //There is a collision
      if(t > 0) {
	//If it is diffuse, do the diffusion code
	if(realRay.material.reflective == 0) {
	  float diffuse = 0.2;
	  diffuse = diffuseShading(diffuse, realRay, sphereArr,
				   triangleArr, sphereArrSize, triangleArrSize, rayHitArr);
	  
	  pixelArr[i].r = realRay.material.color[0] * diffuse * 255;
	  pixelArr[i].g = realRay.material.color[1] * diffuse * 255;
	  pixelArr[i].b = realRay.material.color[2] * diffuse * 255;
	}

	//If it is not diffuse, do the reflection code
	else if(realRay.material.reflective > 0) {
	  Ray d = rayArr[i];
	  //Set a for loop for how many reflections we want to count up to
	  for(int j = 0; j < 10; j++) {
	    //Set the position of d to the spot on the sphere it landed on
	    d.pos[0] = realRay.pos[0];
	    d.pos[1] = realRay.pos[1];
	    d.pos[2] = realRay.pos[2];
	    normalize(&d.dir[0], &d.dir[1], &d.dir[2]);
	    normalize(&realRay.surfaceNorm[0], &realRay.surfaceNorm[1], &realRay.surfaceNorm[2]);

	    //This adapts the slides reflection equations
	    float dotProd = dotProduct(d.dir[0], -d.dir[1], d.dir[2],
				       realRay.surfaceNorm[0], -realRay.surfaceNorm[1],
				       realRay.surfaceNorm[2]);
	    
	    for(int k = 0; k < 3; k++) {
	      d.dir[k] = d.dir[k] + (-2 * (dotProd) * realRay.surfaceNorm[k]);
	    }
	    //d.pos[0] = d.pos[0] + 0.001*d.dir[0];
	    //d.pos[1] = d.pos[1] + 0.001*d.dir[1];
	    //d.pos[2] = d.pos[2] + 0.001*d.dir[2];
	    //reflection = d;
	    //Calculate if hit
	    normalize(&d.dir[0], &d.dir[1], &d.dir[2]);
	    //d.pos[0] = d.pos[0] + 0.001*d.dir[0];
	    //d.pos[1] = d.pos[1] + 0.001*d.dir[1];
	    //d.pos[2] = d.pos[2] + 0.001*d.dir[2];
	    realRay = collision(rayHitArr, d, sphereArr, triangleArr,
				sphereArrSize, triangleArrSize);
	    //printf("realRay t: %f\n", realRay.t);
	    for(int p = 0; p < 8; p++) {
	      if(rayHitArr[p].t < -1.001 || rayHitArr[p].t > 0) {
		//printf("Shape %d t: %f\n", p, rayHitArr[p].t);
	      }
	    }

	    
	    t = -1;
	    //Ignore small t values
	    if(realRay.t > 0.0001) {
	      if(realRay.material.reflective == 0) {
		float diffuse = 0.2;
		diffuse = diffuseShading(diffuse, realRay, sphereArr,
					 triangleArr, sphereArrSize, triangleArrSize, rayHitArr);
		pixelArr[i].r = realRay.material.color[0] * diffuse * 255;
		pixelArr[i].g = realRay.material.color[1] * diffuse * 255;
		pixelArr[i].b = realRay.material.color[2] * diffuse * 255;
		j = 10;
	      }
	    }
	    
	    else {
	      pixelArr[i].r = 0;
	      pixelArr[i].g = 0;
	      pixelArr[i].b = 0;
	      j = 10;
	    }
	  }  
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
  fp = fopen("reference.png", "w");
  stbi_write_png("reference.png", 512, 512, 3, pixelArr, 512*3);
  fclose(fp);
  free(pixelArr);
  free(rayArr);
  return 0;
}

