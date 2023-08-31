#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

  FILE *checkerBoard;
  checkerBoard = fopen("checkboard.png", "w");
  int assignmentSize = 512 * 512 * 3;
  long pixelX;
  long pixelY;
  for(int i = 0; i < assignmentSize; i++) {
    pixelX = xCoord(i);
    pixelY = yCoord(i);
    //If it fits these parameters, then it should be red
    if(pixelY % 128 < 64) {
      if(pixelX % 128 < 64) {
	if(i % 3 == 0) {
	  pixelArray[i] = 255;
	}
	else if(i % 3 == 1) {
	  pixelArray[i] = 0;
	}
	else if(i % 3 == 2) {
	  pixelArray[i] = 0;
	}
      }
    
      //else it should be blue
      else {
	if(i % 3 == 0) {
	  pixelArray[i] = 0;
	}
	else if(i % 3 == 1) {
	  pixelArray[i] = 0;
	}
	else if (i % 3 == 2) {
	  pixelArray[i] = 255;
	}
      }
    }

    //If the first if statement fails, reverse the pattern
    else {
      if(pixelX % 128 < 64) {
	if(i % 3 == 0) {
	  pixelArray[i] = 0; 
	}
	else if(i % 3 == 1) {
	  pixelArray[i] = 0;
	}
	else if(i % 3 == 2) {
	  pixelArray[i] = 255;
	}
      }

      else {
	if(i % 3 == 0) {
	  pixelArray[i] = 255;
	}
	else if(i % 3 == 1) {
	  pixelArray[i] = 0;
	}
	else if(i % 3 == 2) {
	  pixelArray[i] = 0;
	}
      }
      
    }
    
  }

  stbi_write_png("checkerboard.png", 512, 512, 3, pixelArray, 512*3);
  fclose(checkerBoard);
  //This is the gradient Code
  float fpixelX;
  float fpixelY;
  for(int i = 0; i < assignmentSize; i++) {
    fpixelX = (float) xCoord(i);
    fpixelY = (float) yCoord(i);

    
    if(i % 3 == 0) {
      //pixel number / pixel total * color code
      pixelArray[i] = round((fpixelX / 511) * 255);
    }

    else if(i % 3 == 1) {
      pixelArray[i] = round((fpixelY / 511) * 255);
    }

    else if(i % 3 == 2) {
      pixelArray[i] = 128;
    }
    
  }

  FILE *gradient;
  gradient = fopen("gradient.png" , "w");
  stbi_write_png("gradient.png", 512, 512, 3, pixelArray, 512*3);
  fclose(gradient);
  free(pixelArray);
  return 0;
}

