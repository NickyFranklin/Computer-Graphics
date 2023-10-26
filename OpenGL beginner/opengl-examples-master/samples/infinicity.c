/* Copyright (c) 2014-2015 Scott Kuhl. All rights reserved.
 * License: This code is licensed under a 3-clause BSD license. See
 * the file named "LICENSE" for a full copy of the license.
 */

/** @file Demonstrates drawing a colored 3D triangle.
 *
 * @author Scott Kuhl
 */

#include "libkuhl.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static GLuint program = 0; /**< id value for the GLSL program */
static GLuint program2 = 1;

static kuhl_geometry buildingBottom[10][10];
static kuhl_geometry windowBottom[10][10];
static kuhl_geometry buildingTop[10][10];
static kuhl_geometry windowTop[10][10];
static int isComplex[10][10];
static kuhl_geometry road1;
static originalIndex[10][10];

int start = 1;
float bottomWidth = 0.83;
float bottomDepth = 0.83;
float bottomHeight = 2;
float topWidth = 0.25;
float topDepth = 0.25;
float topHeight = 1;
int complex = 1;
int seed = 0;
float maxWidth = 1.65;
float maxDepth = 0.83;
int backward = 10;

int isForward = 0;
int isBackward = 0;
int isNew = 1;
float transMat[16];
float translation = 0;
int zOrigin = 0;
int groundShift = 0;

/* Called by GLFW whenever a key is pressed. */
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/* If the library handles this keypress, return */
	if (kuhl_keyboard_handler(window, key, scancode, action, mods))
		return;

	/* Action can be press (key down), release (key up), or repeat
	   (key is held down, the delay until the first repeat and the
	   rate at which repeats fire may depend on the OS.)
	   
	   Here, we ignore any press events so the event will only
	   happen when the key is released or it will happen repeatedly
	   when the key is held down.
	*/
	if(action == GLFW_PRESS || action == GLFW_REPEAT) {
	  if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
	    isForward = 1;
	  }

	  if(key == GLFW_KEY_B && action == GLFW_PRESS) {
	    isBackward = 1;
	  }

	  if(key == GLFW_KEY_SPACE && action == GLFW_REPEAT) {
	    isForward = 1;
	  }

	  if(key == GLFW_KEY_B && action == GLFW_REPEAT) {
	    isBackward = 1;
	  }
	  
	  return;
	}

	else {
	  isForward = 0;
	  isBackward = 0;
	}
	/* Custom key handling code below: 
	   For a list of keys, see: https://www.glfw.org/docs/latest/group__keys.html  */
	
}
/*
/** Draws the 3D scene. */
/*
void display()
{
	/* Render the scene once for each viewport. Frequently one
	 * viewport will fill the entire screen. However, this loop will
	 * run twice for HMDs (once for the left eye and once for the
	 * right). */
/*
viewmat_begin_frame();
	for(int viewportID=0; viewportID<viewmat_num_viewports(); viewportID++)
	{
		viewmat_begin_eye(viewportID);

		/* Where is the viewport that we are drawing onto and what is its size? */
//		int viewport[4]; // x,y of lower left corner, width, height
//		viewmat_get_viewport(viewport, viewportID);
		/* Tell OpenGL the area of the window that we will be drawing in. */
//		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		/* Clear the current viewport. Without glScissor(), glClear()
		 * clears the entire screen. We could call glClear() before
		 * this viewport loop---but in order for all variations of
		 * this code to work (Oculus support, etc), we can only draw
		 * after viewmat_begin_eye(). *//*
		glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
		glEnable(GL_SCISSOR_TEST);
		glClearColor(.2,.2,.2,0); // set clear color to grey
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST); // turn on depth testing
		kuhl_errorcheck();

		/* Get the view matrix and the projection matrix */
//	float viewMat[16], perspective[16];
//		viewmat_get(viewMat, perspective, viewportID);

		/* Calculate an angle to rotate the object. glfwGetTime() gets
		 * the time in seconds since GLFW was initialized. Rotates 45 degrees every second. */
//		float angle = fmod(0, 360);

		/* Make sure all computers/processes use the same angle */
//		dgr_setget("angle", &angle, sizeof(GLfloat));

		/* Create a 4x4 rotation matrix based on the angle we computed. */
//		float rotateMat[16];
//		mat4f_rotateAxis_new(rotateMat, angle, 1,0,0);

		/* Create a scale matrix. */
//		float scaleMat[16];
//		mat4f_scale_new(scaleMat, 3, 3, 3);

		//float transMat[16];
		//float transMat2[16];
/*
if(isNew) {
		  mat4f_identity(transMat);
		  isNew = 0;
		}

		if(isForward) {
		  translation = translation + 0.05f;
		}

		else if(isBackward) {
		  translation = translation - 0.05f;
		}
		
		groundShift = translation;
		if(groundShift > zOrigin) {
		  zOrigin++; 
		}
		
		if(groundShift < zOrigin) {
		  zOrigin--;
		}
		float startpos = -1;
		mat4f_translate_new(transMat, 0, startpos, translation-(float) zOrigin);
		/* Last parameter must be NULL, otherwise your program
		   can/will crash. The modelview matrix is (the view matrix) *
		   (the model matrix). Here, we have two matrices in the model
		   matrix, and multiply everything together at once into the
		   modelview matrix.
		   
		   modelview = viewmat * sclaeMat * rotateMat 
		                         ^---model matrix---^
		*/
//		float modelview[16];
//		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);

		/* Tell OpenGL which GLSL program the subsequent
		 * glUniformMatrix4fv() calls are for. */
/*		kuhl_errorcheck();

		glUseProgram(program2);
		
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value
		/* Send the modelview matrix to the vertex program. */
		
/*		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&road1);
		glUseProgram(0);
		kuhl_errorcheck();
		glUseProgram(program);
		/* Send the perspective projection matrix to the vertex program. */
/*		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value
		/* Send the modelview matrix to the vertex program. */
/*		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_errorcheck();
		/* Draw the geometry using the matrices that we sent to the
		 * vertex programs immediately above */
		//startpos = -10;
/*	        if(groundShift > zOrigin) {
		  zOrigin++; 
		}
		
		if(groundShift < zOrigin) {
		  zOrigin--;
		}
		
		for(int i = 0; i < 10; i++) {
		  for(int j = 0; j < 10; j++) {
		    //0.33 for width for road offset
		    seed = i*i + j*j + zOrigin*7; 
		    if(groundShift < (zOrigin + 1) || groundShift > (zOrigin - 1)) {
		      srand48(seed);
		      bottomWidth = drand48();
		      bottomWidth *= maxWidth;
		      bottomDepth = drand48();
		      bottomDepth *= maxDepth;
		      bottomHeight = drand48();
		      bottomHeight *= 10;
		      init_geometryBuilding(&buildingBottom[i][j], program,
					    bottomWidth, bottomDepth, bottomHeight, seed);
		      init_windowGrid(&windowBottom[i][j], program,
				      bottomWidth, bottomDepth, bottomHeight, seed);
		      complex = drand48() * 2;
		      if(complex >= 1) {
			topWidth = drand48() * bottomWidth;
			topDepth = drand48() * bottomDepth;
			topHeight = drand48() * 10;
			init_geometryComplexBuilding(&buildingTop[i][j], program,
						     topWidth, topDepth, topHeight,
						     bottomWidth, bottomDepth, bottomHeight, seed);
			init_complexWindowGrid(&windowTop[i][j], program, topWidth, topDepth, topHeight,
					       bottomWidth, bottomDepth, bottomHeight, seed);
		      }
		      isComplex[i][j] = complex;
		    }
		    mat4f_translate_new(transMat, (0.33+j*2)-10, startpos,
					translation+i-(float) zOrigin);
		    mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);

		    
		    glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
				       1, // number of 4x4 float matrices
				       0, // transpose
				       modelview); // value
		    
		    kuhl_geometry_draw(&buildingBottom[i][j]);
		    kuhl_geometry_draw(&windowBottom[i][j]);
		    if(isComplex[i][j] >= 1) {
		      kuhl_geometry_draw(&buildingTop[i][j]);
		      kuhl_geometry_draw(&windowTop[i][j]);
		    }
		    
		  }
		}
		/*
		mat4f_translate_new(transMat, 0.33, startpos, translation);
		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);

		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		
		kuhl_geometry_draw(&building);
		kuhl_geometry_draw(&windows);
		//startpos = -10;
		mat4f_translate_new(transMat, 0.33, startpos, translation+1);
		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);
		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value

		kuhl_geometry_draw(&complexBuilding);
		kuhl_geometry_draw(&windows2);
		*/
//		glUseProgram(0);

		/*
		glUseProgram(program2);
		
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value
		/* Send the modelview matrix to the vertex program. 
		
		glUniformMatrix4f(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&road1);
		*/
		/* If we wanted to draw multiple triangles and quads at
		 * different locations, we could call glUniformMatrix4fv again
		 * to change the ModelView matrix and then call
		 * kuhl_geometry_draw() again to draw that object again using
		 * the new model matrix. */

//		glUseProgram(0); // stop using a GLSL program.
//		viewmat_end_eye(viewportID);
//	} // finish viewport loop
//	viewmat_end_frame();

	/* Check for errors. If there are errors, consider adding more
	 * calls to kuhl_errorcheck() in your code. */
//	kuhl_errorcheck();
/*
}
*/

void init_windowGrid(kuhl_geometry *geom, GLuint prog, float width, float depth,
		     float height, float seed) {
  //Find the amount of windows that can fit in each face
  //Windows will be 0.5 by 0.5
  float triangleWidth = 0.1;
  float triangleHeight = 0.1;
  //Small line between triangles
  float lineSpace = 0.01;
  int rows = width / (triangleHeight + lineSpace);
  int collumns = height / (triangleWidth + lineSpace);
  int wideRows = depth / (triangleWidth + lineSpace);
  //lineSpace = (width - (((float) rows) * triangleWidth)) / ((float) rows);
  //n windows = n*2 triangles = n*2*3 verticies = n*2*3*3 datapoints
  int windows = rows * collumns;
  int wideWindows = wideRows * collumns; 
  int totalVerts = 2*3*windows;
  int wideTotalVerts = 2*3*wideWindows;
  int windowCount = 0;
  kuhl_geometry_new(geom, prog, totalVerts*2 + wideTotalVerts*2, // num vertices
		    GL_TRIANGLES); // primitive type
  
  float rowInc = triangleHeight + lineSpace;
  float collumnInc = triangleWidth + lineSpace;
  
  GLfloat vertexPositions[totalVerts*2*3 + wideTotalVerts*2*3];

  float currentYInc = 0;
  float currentXInc = 0;
  
  for(int i = 0; i < totalVerts*2*3; i+=3) {
    int trianglePlace = i % 18;
    if(i % (totalVerts*3) == 0) {
      currentYInc = 0;
      currentXInc = 0;
    }
    
    if(i / ((totalVerts*3)) == 0) {
      if(trianglePlace == 0) {
	vertexPositions[i] =  currentXInc+ lineSpace;
	vertexPositions[i+1] =  currentYInc+ lineSpace;
	vertexPositions[i+2] = depth+ 0.005;
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = triangleWidth + currentXInc + lineSpace;
	vertexPositions[i+1] = currentYInc +lineSpace;
	vertexPositions[i+2] = depth+0.005;
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  currentXInc + lineSpace;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace;
	vertexPositions[i+2] = depth+0.005;
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = currentXInc + triangleWidth + lineSpace;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace;
	vertexPositions[i+2] = depth+0.005;
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
	
	if(windowCount >= rows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
    
    if(i / ((totalVerts*3)) == 1) {
      
      if(trianglePlace == 0) {
	vertexPositions[i] =  currentXInc+ lineSpace;
	vertexPositions[i+1] =  currentYInc+ lineSpace;
	vertexPositions[i+2] = -1 * (0.005);
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = triangleWidth + currentXInc + lineSpace;
	vertexPositions[i+1] = currentYInc +lineSpace;
	vertexPositions[i+2] = -1 * (0.005);
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  currentXInc + lineSpace;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace;
	vertexPositions[i+2] = -1 * (0.005);
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = currentXInc + triangleWidth + lineSpace;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace;
	vertexPositions[i+2] = -1 * (0.005);
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
	
	if(windowCount >= rows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
  }

  currentYInc = 0;
  currentXInc = 0;
  
  for(int i = totalVerts*2*3; i < totalVerts*2*3+2*wideTotalVerts*3; i+=3) {
    int trianglePlace = i % 18;
    if(i / ((totalVerts*3*2+wideTotalVerts*3)) == 0) { 
      if(trianglePlace == 0) {
	
	vertexPositions[i] =  -0.005;
	vertexPositions[i+1] =  currentYInc+ lineSpace;
	vertexPositions[i+2] = currentXInc+ lineSpace;
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = -0.005;
	vertexPositions[i+1] = currentYInc +lineSpace;
	vertexPositions[i+2] = triangleWidth + currentXInc + lineSpace;
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  -0.005;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace;
	vertexPositions[i+2] = currentXInc + lineSpace; 
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = -0.005;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace;
	vertexPositions[i+2] = currentXInc + triangleWidth + lineSpace;
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
	
	if(windowCount >= wideRows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
    
    if(i / ((totalVerts*3*2+wideTotalVerts*3)) == 1) { 
      if(i == totalVerts*3*2+wideTotalVerts*3) {
	currentYInc = 0;
      }
      if(trianglePlace == 0) {
	
	vertexPositions[i] =  width+0.005;
	vertexPositions[i+1] =  currentYInc+ lineSpace;
	vertexPositions[i+2] = currentXInc+ lineSpace;
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = width+0.005;
	vertexPositions[i+1] = currentYInc +lineSpace;
	vertexPositions[i+2] = triangleWidth + currentXInc + lineSpace;
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  width+0.005;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace;
	vertexPositions[i+2] = currentXInc + lineSpace; 
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = width+0.005;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace;
	vertexPositions[i+2] = currentXInc + triangleWidth + lineSpace;
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
     
	if(windowCount >= wideRows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
  }
  
  kuhl_geometry_attrib(geom, vertexPositions, // data
		       3, // number of components (x,y,z)
		       "in_Position", // GLSL variable
		       KG_WARN); // warn if attribute is missing in GLSL program?
  
  GLfloat colorData[totalVerts*3*2 + wideTotalVerts*3*2];
  for(int i = 0; i < totalVerts*3*2+wideTotalVerts*3*2; i+=18) {
    if(drand48() < 0.3) {
      colorData[i] = 1;
      colorData[i+1] = 1;
      colorData[i+2] = 0;
      colorData[i+3] = 1;
      colorData[i+4] = 1;
      colorData[i+5] = 0;
      colorData[i+6] = 1;
      colorData[i+7] = 1;
      colorData[i+8] = 0;
      colorData[i+9] = 1;
      colorData[i+10] = 1;
      colorData[i+11] = 0;
      colorData[i+12] = 1;
      colorData[i+13] = 1;
      colorData[i+14] = 0;
      colorData[i+15] = 1;
      colorData[i+16] = 1;
      colorData[i+17] = 0;
    }
    else {
      colorData[i] = 0;
      colorData[i+1] = 0;
      colorData[i+2] = 0;
      colorData[i+3] = 0;
      colorData[i+4] = 0;
      colorData[i+5] = 0;
      colorData[i+6] = 0;
      colorData[i+7] = 0;
      colorData[i+8] = 0;
      colorData[i+9] = 0;
      colorData[i+10] = 0;
      colorData[i+11] = 0;
      colorData[i+12] = 0;
      colorData[i+13] = 0;
      colorData[i+14] = 0;
      colorData[i+15] = 0;
      colorData[i+16] = 0;
      colorData[i+17] = 0;
    }
  }
  float isTextured = 0;
  kuhl_geometry_attrib(geom, colorData, 3, "in_Color", KG_WARN);
  //kuhl_geometry_attrib(geom, &isTextured, 3, "textured", KG_WARN);
}

void init_complexWindowGrid(kuhl_geometry *geom, GLuint prog, float width, float depth,
			    float height, float bottomWidth,
			    float bottomDepth, float bottomHeight,float seed) {
  //Find the amount of windows that can fit in each face
  //Windows will be 0.5 by 0.5
  float triangleWidth = 0.05;
  float triangleHeight = 0.05;
  //Small line between triangles
  float lineSpace = 0.007;
  int rows = width / (triangleHeight + lineSpace);
  int collumns = height / (triangleWidth + lineSpace);
  int wideRows = depth / (triangleWidth + lineSpace);
  //lineSpace = (width - (((float) rows) * triangleWidth)) / ((float) rows);
  //n windows = n*2 triangles = n*2*3 verticies = n*2*3*3 datapoints
  int windows = rows * collumns;
  int wideWindows = wideRows * collumns; 
  int totalVerts = 2*3*windows;
  int wideTotalVerts = 2*3*wideWindows;
  int windowCount = 0;
  kuhl_geometry_new(geom, prog, totalVerts*2 + wideTotalVerts*2, // num vertices
		    GL_TRIANGLES); // primitive type
  
  float rowInc = triangleHeight + lineSpace;
  float collumnInc = triangleWidth + lineSpace;
  
  GLfloat vertexPositions[totalVerts*2*3 + wideTotalVerts*2*3];

  float currentYInc = 0;
  float currentXInc = 0;
  float centerWidth = (bottomWidth/2) - (width/2);
  float centerDepth = (bottomDepth/2) - (depth/2);
  GLfloat colorData[totalVerts*3*2 + wideTotalVerts*3*2];
  for(int i = 0; i < totalVerts*2*3; i+=3) {

    int trianglePlace = i % 18;
    if(i % (totalVerts*3) == 0) {
      currentYInc = 0;
      currentXInc = 0;
    }
    
    if(i / ((totalVerts*3)) == 0) {
      if(trianglePlace == 0) {
	vertexPositions[i] =  currentXInc+ lineSpace+centerWidth;
	vertexPositions[i+1] =  currentYInc+ lineSpace+bottomHeight;
	vertexPositions[i+2] = depth+ 0.005+centerDepth;
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = triangleWidth + currentXInc + lineSpace+centerWidth;
	vertexPositions[i+1] = currentYInc +lineSpace+bottomHeight;
	vertexPositions[i+2] = depth+0.005+centerDepth;
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  currentXInc + lineSpace+centerWidth;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace+bottomHeight;
	vertexPositions[i+2] = depth+0.005+centerDepth;
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = currentXInc + triangleWidth + lineSpace+centerWidth;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace+bottomHeight;
	vertexPositions[i+2] = depth+0.005+centerDepth;
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
	
	if(windowCount >= rows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
    
    if(i / ((totalVerts*3)) == 1) {
      
      if(trianglePlace == 0) {
	vertexPositions[i] =  currentXInc+ lineSpace+centerWidth;
	vertexPositions[i+1] =  currentYInc+ lineSpace+bottomHeight;
	vertexPositions[i+2] = -1 * (0.005)+centerDepth;
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = triangleWidth + currentXInc + lineSpace+centerWidth;
	vertexPositions[i+1] = currentYInc +lineSpace+bottomHeight;
	vertexPositions[i+2] = -1 * (0.005)+centerDepth;
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  currentXInc + lineSpace+centerWidth;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace+bottomHeight;
	vertexPositions[i+2] = -1 * (0.005)+centerDepth;
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = currentXInc + triangleWidth + lineSpace+centerWidth;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace+bottomHeight;
	vertexPositions[i+2] = -1 * (0.005)+centerDepth;
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
	
	if(windowCount >= rows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
  }

  currentYInc = 0;
  currentXInc = 0;
  
  for(int i = totalVerts*2*3; i < totalVerts*2*3+2*wideTotalVerts*3; i+=3) {
    int trianglePlace = i % 18;
    if(i / ((totalVerts*3*2+wideTotalVerts*3)) == 0) { 
      if(trianglePlace == 0) {
	
	vertexPositions[i] =  -0.005+centerWidth;
	vertexPositions[i+1] =  currentYInc+ lineSpace+bottomHeight;
	vertexPositions[i+2] = currentXInc+ lineSpace+centerDepth;
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = -0.005+centerWidth;
	vertexPositions[i+1] = currentYInc +lineSpace+bottomHeight;
	vertexPositions[i+2] = triangleWidth + currentXInc + lineSpace+centerDepth;
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  -0.005+centerWidth;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace+bottomHeight;
	vertexPositions[i+2] = currentXInc + lineSpace+centerDepth; 
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = -0.005+centerWidth;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace+bottomHeight;
	vertexPositions[i+2] = currentXInc + triangleWidth + lineSpace+centerDepth;
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
       
	if(windowCount >= wideRows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
    
    if(i / ((totalVerts*3*2+wideTotalVerts*3)) == 1) { 
      if(i == totalVerts*3*2+wideTotalVerts*3) {
	currentYInc = 0;
      }
      if(trianglePlace == 0) {
	
	vertexPositions[i] =  width+0.005+centerWidth;
	vertexPositions[i+1] =  currentYInc+ lineSpace+bottomHeight;
	vertexPositions[i+2] = currentXInc+ lineSpace+centerDepth;
      }
      
      if(trianglePlace == 3 || trianglePlace == 9) {
	vertexPositions[i] = width+0.005+centerWidth;
	vertexPositions[i+1] = currentYInc +lineSpace+bottomHeight;
	vertexPositions[i+2] = triangleWidth + currentXInc + lineSpace+centerDepth;
      }
      
      if(trianglePlace == 6 || trianglePlace == 12) {
	vertexPositions[i] =  width+0.005+centerWidth;
	vertexPositions[i+1] = currentYInc + triangleHeight + lineSpace+bottomHeight;
	vertexPositions[i+2] = currentXInc + lineSpace+centerDepth; 
      }
      
      if(trianglePlace == 15) {
	vertexPositions[i] = width+0.005+centerWidth;
	vertexPositions[i+1] = triangleHeight + currentYInc + lineSpace+bottomHeight;
	vertexPositions[i+2] = currentXInc + triangleWidth + lineSpace+centerDepth;
	currentXInc += triangleWidth + lineSpace;
	windowCount++;
	
	
	if(windowCount >= wideRows) {
	  currentXInc = 0;
	  currentYInc += triangleHeight + lineSpace;
	  windowCount = 0;
	}
      }
    }
  }
  
  kuhl_geometry_attrib(geom, vertexPositions, // data
		       3, // number of components (x,y,z)
		       "in_Position", // GLSL variable
		       KG_WARN); // warn if attribute is missing in GLSL program? 
  //GLfloat colorData[totalVerts*3*2 + wideTotalVerts*3*2];
  for(int i = 0; i < totalVerts*3*2+wideTotalVerts*3*2; i+=18) {
    if(drand48() < 0.3) {
      colorData[i] = 1;
      colorData[i+1] = 1;
      colorData[i+2] = 0;
      colorData[i+3] = 1;
      colorData[i+4] = 1;
      colorData[i+5] = 0;
      colorData[i+6] = 1;
      colorData[i+7] = 1;
      colorData[i+8] = 0;
      colorData[i+9] = 1;
      colorData[i+10] = 1;
      colorData[i+11] = 0;
      colorData[i+12] = 1;
      colorData[i+13] = 1;
      colorData[i+14] = 0;
      colorData[i+15] = 1;
      colorData[i+16] = 1;
      colorData[i+17] = 0;
    }
    else {
      colorData[i] = 0;
      colorData[i+1] = 0;
      colorData[i+2] = 0;
      colorData[i+3] = 0;
      colorData[i+4] = 0;
      colorData[i+5] = 0;
      colorData[i+6] = 0;
      colorData[i+7] = 0;
      colorData[i+8] = 0;
      colorData[i+9] = 0;
      colorData[i+10] = 0;
      colorData[i+11] = 0;
      colorData[i+12] = 0;
      colorData[i+13] = 0;
      colorData[i+14] = 0;
      colorData[i+15] = 0;
      colorData[i+16] = 0;
      colorData[i+17] = 0;
    }
  }
  kuhl_geometry_attrib(geom, colorData, 3, "in_Color", KG_WARN);
  float isTextured = 0;
  //kuhl_geometry_attrib(geom, &isTextured, 3, "textured", KG_WARN);
}



void init_geometryComplexBuilding(kuhl_geometry *geom, GLuint prog, float width, float depth,
				  float height, float bottomWidth,
				  float bottomDepth, float bottomHeight ,float seed) {
  kuhl_geometry_new(geom, prog, 36, // num vertices
	                  GL_TRIANGLES); // primitive type

	/* Vertices that we want to form triangles out of. Every 3 numbers
	 * is a vertex position. Since no indices are provided, every
	 * three vertex positions form a single triangle.*/
  float centerWidth = (bottomWidth/2) - (width/2);
  float centerDepth = (bottomDepth/2) - (depth/2);
	//                         Face 1 Front
	//                         Triangle 1
  GLfloat vertexPositions[] ={0 + centerWidth, 0+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      //triangle 2
			      0+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      0+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      //Face2 Back
			      //Triangle 1
			      0+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      width+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      width+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      //Triangle 2
			      0+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      0+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      width+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      //Face 3 Bottom
			      //Triangle 1
			      0+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      0+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      width+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      //Triangle 2
			      0+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      //Face 4 Top
			      //Triangle 1
			      0+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      0+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      width+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      //Triangle 2
			      0+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      //Face 5 East
			      //Triangle 1
			      0+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      0+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      0+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      //Triangle 2
			      0+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      0+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      0+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      //Face 6 Wesy
			      //Triangle 1
			      width+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, 0+bottomHeight, depth+ centerDepth,
			      width+ centerWidth, height+bottomHeight, depth+ centerDepth,
			      //Triangle 2
			      width+ centerWidth, 0+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, height+bottomHeight, 0+ centerDepth,
			      width+ centerWidth, height+bottomHeight, depth+ centerDepth};
	kuhl_geometry_attrib(geom, vertexPositions, // data
	                     3, // number of components (x,y,z)
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?
	
	/* The colors of each of the vertices */
	GLfloat colorData[] = {0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face2
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 3
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 4
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 5
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 6
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5};
	kuhl_geometry_attrib(geom, colorData, 3, "in_Color", KG_WARN);
	float isTextured = 0;
	//	kuhl_geometry_attrib(geom, &isTextured, 3, "textured", KG_WARN);
}

void init_geometryBuilding(kuhl_geometry *geom, GLuint prog, float width, float depth,
			   float height, float seed)
{
	kuhl_geometry_new(geom, prog, 36, // num vertices
	                  GL_TRIANGLES); // primitive type

	/* Vertices that we want to form triangles out of. Every 3 numbers
	 * is a vertex position. Since no indices are provided, every
	 * three vertex positions form a single triangle.*/

	//                         Face 1 Front
	//                         Triangle 1
	GLfloat vertexPositions[] ={0, 0, 0,
				    width, 0, 0,
				    width, height, 0,
				    //triangle 2
				    0, 0, 0,
				    0, height, 0,
				    width, height, 0,
				    //Face2 Back
				    //Triangle 1
				    0, 0, depth,
				    width, 0, depth,
				    width, height, depth,
				    //Triangle 2
				    0, 0, depth,
				    0, height, depth,
				    width, height, depth,
				    //Face 3 Bottom
				    //Triangle 1
				    0, 0, 0,
				    0, 0, depth,
				    width, 0, depth,
				    //Triangle 2
				    0, 0, 0,
				    width, 0, 0,
				    width, 0, depth,
				    //Face 4 Top
				    //Triangle 1
				    0, height, 0,
				    0, height, depth,
				    width, height, depth,
				    //Triangle 2
				    0, height, 0,
				    width, height, 0,
				    width, height, depth,
				    //Face 5 East
				    //Triangle 1
				    0, 0, 0,
				    0, 0, depth,
				    0, height, depth,
				    //Triangle 2
				    0, 0, 0,
				    0, height, 0,
				    0, height, depth,
				    //Face 6 Wesy
				    //Triangle 1
				    width, 0, 0,
				    width, 0, depth,
				    width, height, depth,
				    //Triangle 2
				    width, 0, 0,
				    width, height, 0,
				    width, height, depth};
	kuhl_geometry_attrib(geom, vertexPositions, // data
	                     3, // number of components (x,y,z)
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?
	
	/* The colors of each of the vertices */
	GLfloat colorData[] = {0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face2
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 3
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 4
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 5
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Face 6
			       //Triangle 1
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       //Triangle 2
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5,
			       0.5, 0.5, 0.5};
	kuhl_geometry_attrib(geom, colorData, 3, "in_Color", KG_WARN);
	float isTextured = 0;
	//	kuhl_geometry_attrib(geom, &isTextured, 3, "textured", KG_WARN);
}

void init_geometryRoad(kuhl_geometry *geom, GLuint prog)
{ 
  kuhl_geometry_new(geom, prog, 4, GL_TRIANGLES);
  GLfloat vertexPositions[] = {-10, 0, 0,
			       10, 0, 0,
			       -10, 0, 10,
			       10, 0, 10};
  kuhl_geometry_attrib(geom, vertexPositions, 3, "in_Position", KG_WARN);
  GLuint indexData[] = {0,1,3,
			0,2,3};
  kuhl_geometry_indices(geom, indexData, 6);
  
  
  GLfloat texcoordData[] = {0,0,
			    10,0,
			    0,10,
			    10,10};
  
  kuhl_geometry_attrib(geom, texcoordData, 2, "in_TexCoord", KG_WARN);
  
  GLuint texId = 0;
  kuhl_read_texture_file_wrap("../images/road.png", &texId, GL_REPEAT, GL_REPEAT);
  kuhl_geometry_texture(geom,texId, "tex", KG_WARN);

  kuhl_errorcheck();
}

void display()
{
	/* Render the scene once for each viewport. Frequently one
	 * viewport will fill the entire screen. However, this loop will
	 * run twice for HMDs (once for the left eye and once for the
	 * right). */
	viewmat_begin_frame();
	for(int viewportID=0; viewportID<viewmat_num_viewports(); viewportID++)
	{
		viewmat_begin_eye(viewportID);

		/* Where is the viewport that we are drawing onto and what is its size? */
		int viewport[4]; // x,y of lower left corner, width, height
		viewmat_get_viewport(viewport, viewportID);
		/* Tell OpenGL the area of the window that we will be drawing in. */
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		/* Clear the current viewport. Without glScissor(), glClear()
		 * clears the entire screen. We could call glClear() before
		 * this viewport loop---but in order for all variations of
		 * this code to work (Oculus support, etc), we can only draw
		 * after viewmat_begin_eye(). */
		glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
		glEnable(GL_SCISSOR_TEST);
		glClearColor(.2,.2,.2,0); // set clear color to grey
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST); // turn on depth testing
		kuhl_errorcheck();

		/* Get the view matrix and the projection matrix */
		float viewMat[16], perspective[16];
		viewmat_get(viewMat, perspective, viewportID);

		/* Calculate an angle to rotate the object. glfwGetTime() gets
		 * the time in seconds since GLFW was initialized. Rotates 45 degrees every second. */
		float angle = fmod(0, 360);

		/* Make sure all computers/processes use the same angle */
		dgr_setget("angle", &angle, sizeof(GLfloat));

		/* Create a 4x4 rotation matrix based on the angle we computed. */
		float rotateMat[16];
		mat4f_rotateAxis_new(rotateMat, angle, 1,0,0);

		/* Create a scale matrix. */
		float scaleMat[16];
		mat4f_scale_new(scaleMat, 3, 3, 3);

		//float transMat[16];
		//float transMat2[16];
		if(isNew) {
		  mat4f_identity(transMat);
		  isNew = 0;
		}

		if(isForward) {
		  translation = translation + 0.05f;
		}

		else if(isBackward) {
		  translation = translation - 0.05f;
		}
		
		groundShift = translation;
		/*
		if(groundShift > zOrigin) {
		  zOrigin++; 
		}
		*/
		/*
		if(groundShift < zOrigin) {
		  zOrigin--;
		}
		*/
		float startpos = -1;
		mat4f_translate_new(transMat, 0, startpos, translation-(float) zOrigin);
		/* Last parameter must be NULL, otherwise your program
		   can/will crash. The modelview matrix is (the view matrix) *
		   (the model matrix). Here, we have two matrices in the model
		   matrix, and multiply everything together at once into the
		   modelview matrix.
		   
		   modelview = viewmat * sclaeMat * rotateMat 
		                         ^---model matrix---^
		*/
		float modelview[16];
		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);

		/* Tell OpenGL which GLSL program the subsequent
		 * glUniformMatrix4fv() calls are for. */
		kuhl_errorcheck();

		glUseProgram(program2);
		
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value
		/* Send the modelview matrix to the vertex program. */
		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&road1);
		glUseProgram(0);
		kuhl_errorcheck();
		glUseProgram(program);
		/* Send the perspective projection matrix to the vertex program. */
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value
		/* Send the modelview matrix to the vertex program. */
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_errorcheck();
		/* Draw the geometry using the matrices that we sent to the
		 * vertex programs immediately above */
		//startpos = -10;
		if(start) {
		  for(int i = 0; i < 10; i++) {
		    for(int j = 0; j < 10; j++) {
		      seed = i*92+j*21+zOrigin;
		      printf("%d seed\n", seed);
		      srand48(seed);
		      
		      bottomWidth = drand48()+0.001;
		      bottomWidth *= maxWidth;
		      if(bottomWidth < 0.15) {
			bottomWidth = 0.15;
		      }
		      bottomDepth = drand48()+0.001;
		      bottomDepth *= maxDepth;
		      if(bottomDepth < 0.15) {
			bottomDepth = 0.15;
		      }
		      bottomHeight = drand48()+0.001;
		      bottomHeight *= 10;
		      if(bottomHeight < 1) {
			bottomHeight = 1;
		      }
		      printf("%f %f %f \n", bottomWidth, bottomDepth, bottomHeight);
		      init_geometryBuilding(&buildingBottom[i][j], program,
					    bottomWidth, bottomDepth, bottomHeight, seed);
		      init_windowGrid(&windowBottom[i][j], program,
				      bottomWidth, bottomDepth, bottomHeight, seed);
		      complex = drand48() * 2;
		      
		      topWidth = drand48() * bottomWidth+0.001;
		      topDepth = drand48() * bottomDepth+0.001;
		      topHeight = drand48() * 10+0.001;
		      if(topWidth > bottomWidth || topWidth < 0.1) {
			topWidth = bottomWidth;
		      }
		      if(topDepth > bottomDepth || topDepth < 0.1) {
			topDepth = bottomDepth;
		      }
		      if(topHeight < 1) {
			topHeight = 1;
		      }
		      printf("%f %f %f \n", topWidth, topDepth, topHeight);
		      init_geometryComplexBuilding(&buildingTop[i][j], program,
						   topWidth, topDepth, topHeight,
						   bottomWidth, bottomDepth, bottomHeight, seed);
		      init_complexWindowGrid(&windowTop[i][j], program, topWidth, topDepth, topHeight,
					     bottomWidth, bottomDepth, bottomHeight, seed);
		      
		      printf("%d\n", complex);
		      isComplex[i][j] = complex;
		      start = 0;
		    }
		  }
		}
		if(groundShift < zOrigin) {
		  zOrigin--;
		  for(int i = 0; i < 10; i++) {
		    for(int j = 0; j < 10; j++) {
		      seed = i*92+j*21+zOrigin;
		      //backward--;
		      //printf("%d seed\n", seed);
		      //srand(seed);
		      if(i == 0) {
		        kuhl_geometry_delete(&(buildingBottom[i][j]));
			kuhl_geometry_delete(&(buildingTop[i][j]));
			kuhl_geometry_delete(&(windowBottom[i][j]));
			kuhl_geometry_delete(&(windowTop[i][j]));
		      }
		      if(i!=9) {
			buildingBottom[i][j] = buildingBottom[i+1][j];
			windowBottom[i][j] = windowBottom[i+1][j];
			windowTop[i][j] = windowTop[i+1][j];
			buildingTop[i][j] = buildingTop[i+1][j];
			isComplex[i][j] = isComplex[i+1][j];
			//originalIndex[i][j] = originalIndex[i+1][j];
			//originalIndex[i][j]--;
		      }

		      else {
			//originalIndex[i][j]--;
			//seed = 0;
			//backward--;
			printf("%d seed\n", seed);
			srand48(seed);
			bottomWidth = drand48()+0.001;
			bottomWidth *= maxWidth;
			if(bottomWidth < 0.15) {
			  bottomWidth = 0.15;
			}
			bottomDepth = drand48()+0.001;
			bottomDepth *= maxDepth;
			if(bottomDepth < 0.15) {
			  bottomDepth = 0.15;
			}
			bottomHeight = drand48()+0.001;
			bottomHeight *= 10;
			if(bottomHeight < 1) {
			  bottomHeight = 1;
			}
			printf("%f %f %f \n", bottomWidth, bottomDepth, bottomHeight);
			init_geometryBuilding(&buildingBottom[i][j], program,
					      bottomWidth, bottomDepth, bottomHeight, seed);
			init_windowGrid(&windowBottom[i][j], program,
					bottomWidth, bottomDepth, bottomHeight, seed);
			
			complex = drand48() * 2;
			
			topWidth = drand48() * bottomWidth+0.001;
			topDepth = drand48() * bottomDepth+0.001;
			topHeight = drand48() * 10+0.001;
			if(topWidth > bottomWidth || topWidth < 0.1) {
			  topWidth = bottomWidth;
			}
			if(topDepth > bottomDepth || topDepth < 0.1) {
			  topDepth = bottomDepth;
			}
			if(topHeight < 1) {
			  topHeight = 1;
			}
			printf("%f %f %f \n", topWidth, topDepth, topHeight);
			init_geometryComplexBuilding(&buildingTop[i][j], program,
						     topWidth, topDepth, topHeight,
						     bottomWidth, bottomDepth, bottomHeight, seed);
			init_complexWindowGrid(&windowTop[i][j], program, topWidth, topDepth, topHeight,
					       bottomWidth, bottomDepth, bottomHeight, seed);
			
			printf("%d\n", complex);
			isComplex[i][j] = complex;
		      }
		      
		    }
		  }

		  
		}


		
	        if(groundShift > zOrigin) {
		  zOrigin++;
		  for(int i = 9; i > -1; i--) {
		    for(int j = 9; j > -1; j--) {
		      seed = (zOrigin+i*92+j*21);
		      //printf("%d seed\n", seed);
		      if(i == 9) {
			kuhl_geometry_delete(&(buildingBottom[i][j]));
			kuhl_geometry_delete(&(buildingTop[i][j]));
			kuhl_geometry_delete(&(windowBottom[i][j]));
			kuhl_geometry_delete(&(windowTop[i][j]));
		      }
		      if(i!=0) {
			buildingBottom[i][j] = buildingBottom[i-1][j];
			windowBottom[i][j] = windowBottom[i-1][j];
			windowTop[i][j] = windowTop[i-1][j];
			buildingTop[i][j] = buildingTop[i-1][j];
			isComplex[i][j] = isComplex[i-1][j];
			//originalIndex[i][j] = originalIndex[i-1][j];
			//originalIndex[i][j]++;
			//printf("original index: %d\n", originalIndex[i][j]);
		      }
		      
		      else {
			//originalIndex[i][j]++;
			//seed = 0;
			//originalIndex[i][j]++;
			printf("%d seed\n", seed);
			srand48(seed);
			bottomWidth = drand48()+0.001;
			bottomWidth *= maxWidth;
			if(bottomWidth < 0.15) {
			  bottomWidth = 0.15;
			}
			bottomDepth = drand48()+0.001;
			bottomDepth *= maxDepth;
			if(bottomDepth < 0.15) {
			  bottomDepth = 0.15;
			}
			bottomHeight = drand48()+0.001;
			bottomHeight *= 10;
			if(bottomHeight < 1) {
			  bottomHeight = 1;
			}
			printf("%f %f %f \n", bottomWidth, bottomDepth, bottomHeight);
			init_geometryBuilding(&buildingBottom[i][j], program,
					      bottomWidth, bottomDepth, bottomHeight, seed);
			init_windowGrid(&windowBottom[i][j], program,
					bottomWidth, bottomDepth, bottomHeight, seed);
			
			complex = drand48() * 2;
			
			topWidth = drand48() * bottomWidth+0.001;
			topDepth = drand48() * bottomDepth+0.001;
			topHeight = drand48() * 10+0.001;
			if(topWidth > bottomWidth || topWidth < 0.1) {
			  topWidth = bottomWidth;
			}
			if(topDepth > bottomDepth || topDepth < 0.1) {
			  topDepth = bottomDepth;
			}
			if(topHeight < 1) {
			  topHeight = 1;
			}
			printf("%f %f %f \n", topWidth, topDepth, topHeight);
			init_geometryComplexBuilding(&buildingTop[i][j], program,
						     topWidth, topDepth, topHeight,
						     bottomWidth, bottomDepth, bottomHeight, seed);
			init_complexWindowGrid(&windowTop[i][j], program, topWidth, topDepth, topHeight,
					       bottomWidth, bottomDepth, bottomHeight, seed);
			
			printf("%d\n", complex);
			isComplex[i][j] = complex;
		      
		      }
		      
		    }
		  }
		  
		}
		
		
		for(int i = 0; i < 10; i++) {
		  for(int j = 0; j < 10; j++) {
		    
		    //0.33 for width for road offset 
		    mat4f_translate_new(transMat, (0.33+j*2)-10, startpos,
					translation+i-(float) zOrigin);
		    mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);

		    
		    glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
				       1, // number of 4x4 float matrices
				       0, // transpose
				       modelview); // value
		    
		    kuhl_geometry_draw(&buildingBottom[i][j]);
		    kuhl_geometry_draw(&windowBottom[i][j]);
		    if(isComplex[i][j] >= 1) {
		      kuhl_geometry_draw(&buildingTop[i][j]);
		      kuhl_geometry_draw(&windowTop[i][j]);
		    }
		    
		  }
		}
		/*
		mat4f_translate_new(transMat, 0.33, startpos, translation);
		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);

		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		
		kuhl_geometry_draw(&building);
		kuhl_geometry_draw(&windows);
		//startpos = -10;
		mat4f_translate_new(transMat, 0.33, startpos, translation+1);
		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMat, NULL);
		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value

		kuhl_geometry_draw(&complexBuilding);
		kuhl_geometry_draw(&windows2);
		*/
		glUseProgram(0);

		/*
		glUseProgram(program2);
		
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value
		/* Send the modelview matrix to the vertex program. 
		
		glUniformMatrix4f(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&road1);
		*/
		/* If we wanted to draw multiple triangles and quads at
		 * different locations, we could call glUniformMatrix4fv again
		 * to change the ModelView matrix and then call
		 * kuhl_geometry_draw() again to draw that object again using
		 * the new model matrix. */

		glUseProgram(0); // stop using a GLSL program.
		viewmat_end_eye(viewportID);
	} // finish viewport loop
	viewmat_end_frame();

	/* Check for errors. If there are errors, consider adding more
	 * calls to kuhl_errorcheck() in your code. */
	kuhl_errorcheck();

}


int main(int argc, char** argv)
{
	/* Initialize GLFW and GLEW */
	kuhl_ogl_init(&argc, argv, -1, -1, 32, 4);
	//Offset from road is 0.17
	/* Specify function to call when keys are pressed. */
	glfwSetKeyCallback(kuhl_get_window(), keyboard);
	// glfwSetFramebufferSizeCallback(window, reshape);

	/* Compile and link a GLSL program comxposed of a vertex shader and
	 * a fragment shader. */
 	program = kuhl_create_program("triangle-color.vert", "triangle-color.frag");
	program2 = kuhl_create_program("texture.vert", "texture.frag");
	/* Use the GLSL program so subsequent calls to glUniform*() send the variable to
	   the correct program. */
	glUseProgram(program);
	kuhl_errorcheck();

	/* Good practice: Unbind objects until we really need them. */
	glUseProgram(0);

	/* Create kuhl_geometry structs for the objects that we want to
	 * draw. */
	//Max depth is 0.83
	//Max width is 1.65
	/*
	float bottomWidth = 0.83;
	float bottomDepth = 0.83;
	float bottomHeight = 2;
	float topWidth = 0.25;
	float topDepth = 0.25;
	float topHeight = 1;
	int complex = 1;
	*/
	int bonus = 0;
	/*
	for(int i = 9; i > -1; i--) {
	  for(int j = 9; j > -1; j--) {
	     printf("%d seed\n", seed);
	    srand48(seed);
	   
	    bottomWidth = drand48()+0.001;
	    bottomWidth *= maxWidth;
	    if(bottomWidth < 0.15) {
	      bottomWidth = 0.15;
	    }
	    bottomDepth = drand48()+0.001;
	    bottomDepth *= maxDepth;
	    if(bottomDepth < 0.15) {
	      bottomDepth = 0.15;
	    }
	    bottomHeight = drand48()+0.001;
	    bottomHeight *= 10;
	    if(bottomHeight < 1) {
	      bottomHeight = 1;
	    }
	    printf("%f %f %f \n", bottomWidth, bottomDepth, bottomHeight);
	    init_geometryBuilding(&buildingBottom[i][j], program,
				  bottomWidth, bottomDepth, bottomHeight, seed);
	    init_windowGrid(&windowBottom[i][j], program,
			    bottomWidth, bottomDepth, bottomHeight, seed);
	    complex = drand48() * 2;
	  
	    topWidth = drand48() * bottomWidth+0.001;
	    topDepth = drand48() * bottomDepth+0.001;
	    topHeight = drand48() * 10+0.001;
	    if(topWidth > bottomWidth || topWidth < 0.1) {
	      topWidth = bottomWidth;
	    }
	    if(topDepth > bottomDepth || topDepth < 0.1) {
	      topDepth = bottomDepth;
	    }
	    if(topHeight < 1) {
	      topHeight = 1;
	    }
	    printf("%f %f %f \n", topWidth, topDepth, topHeight);
	    init_geometryComplexBuilding(&buildingTop[i][j], program,
					 topWidth, topDepth, topHeight,
					 bottomWidth, bottomDepth, bottomHeight, seed);
	    init_complexWindowGrid(&windowTop[i][j], program, topWidth, topDepth, topHeight,
				   bottomWidth, bottomDepth, bottomHeight, seed);
	    
	    printf("%d\n", complex);
	    isComplex[i][j] = complex;
	  }
	}*/
	
	
	init_geometryRoad(&road1, program2);
	dgr_init();     /* Initialize DGR based on config file. */

	float initCamPos[3]  = {0,7,30}; // location of camera
	float initCamLook[3] = {0,-6,2}; // a point the camera is facing at
	float initCamUp[3]   = {0,1,0}; // a vector indicating which direction is up
	viewmat_init(initCamPos, initCamLook, initCamUp);
	
	while(!glfwWindowShouldClose(kuhl_get_window()))
	{
		display();
		kuhl_errorcheck();

		/* process events (keyboard, mouse, etc) */
		glfwPollEvents();
	}
	exit(EXIT_SUCCESS);
}
