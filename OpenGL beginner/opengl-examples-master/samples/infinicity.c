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

static kuhl_geometry building;
static kuhl_geometry windows;
int isForward = 0;
int isBackward = 0;
int isNew = 1;
float transMat[16];
float translation = 0;

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
	    printf("pressed space\n");
	    isForward = 1;
	  }

	  if(key == GLFW_KEY_B && action == GLFW_PRESS) {
	    printf("pressed b\n");
	    isBackward = 1;
	  }

	  if(key == GLFW_KEY_SPACE && action == GLFW_REPEAT) {
	    printf("held space\n");
	    isForward = 1;
	  }

	  if(key == GLFW_KEY_B && action == GLFW_REPEAT) {
	    printf("held b\n");
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

/** Draws the 3D scene. */
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
		float angle = fmod(10, 360);

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
		  translation = translation + 0.1f;
		}

		else if(isBackward) {
		  translation = translation - 0.1f;
		}
		mat4f_translate_new(transMat, 0, -3, translation);
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
		glUseProgram(program);
		kuhl_errorcheck();
		
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
		kuhl_geometry_draw(&building);
		kuhl_geometry_draw(&windows);

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

void init_windowGrid(kuhl_geometry *geom, GLuint prog, float width, float depth,
		     float height, float seed) {
  //Find the amount of windows that can fit in each face
  //Windows will be 0.5 by 0.5
  float triangleWidth = 0.5;
  float triangleHeight = 0.5;
  //Small line between triangles
  float lineSpace = 0.07;
  int rows = width / (triangleHeight + lineSpace);
  int collumns = height / (triangleWidth + lineSpace);
  //lineSpace = (width - (((float) rows) * triangleWidth)) / ((float) rows);
  //n windows = n*2 triangles = n*2*3 verticies = n*2*3*3 datapoints
  int windows = rows * collumns;
  int totalVerts = 2*3*windows;
  int windowCount = 0;
  kuhl_geometry_new(geom, prog, totalVerts, // num vertices
		    GL_TRIANGLES); // primitive type
  
  float rowInc = triangleHeight + lineSpace;
  float collumnInc = triangleWidth + lineSpace;
  
  GLfloat vertexPositions[totalVerts*3];

  float currentYInc = 0;
  float currentXInc = 0;
  for(int i = 0; i < totalVerts*3; i+=3) {
    int trianglePlace = i % 18;

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
      
      printf("window count: %d\nrows: %d\n", windowCount, rows);
      if(windowCount >= rows) {
	currentXInc = 0;
	currentYInc += triangleHeight + lineSpace;
	windowCount = 0;
      }
    }
  }
  

  kuhl_geometry_attrib(geom, vertexPositions, // data
		       3, // number of components (x,y,z)
		       "in_Position", // GLSL variable
		       KG_WARN); // warn if attribute is missing in GLSL program?
  
  GLfloat colorData[totalVerts*3];
  for(int i = 0; i < totalVerts*3; i++) {
    colorData[i] = 0;
  }
  kuhl_geometry_attrib(geom, colorData, 3, "in_Color", KG_WARN);
}


void init_geometryBuilding(kuhl_geometry *geom, GLuint prog, float width, float depth,
			   float height, float seed)
{
	kuhl_geometry_new(geom, prog, 36, // num vertices
	                  GL_TRIANGLES); // primitive type

	/* Vertices that we want to form triangles out of. Every 3 numbers
	 * is a vertex position. Since no indices are provided, every
	 * three vertex positions form a single triangle.*/

	//Find the amount of windows that can fit in each face
	//Windows will be 0.5 by 0.5
	/*
	float triangleWidth = 0.5;
	float trianlgeHeight = 0.5;
	//Small line between triangles
	float lineSpace = 0.05;
	int rows = height / (triangleHeight + lineSpace);
	int collumns = width / (triangleWidth + lineSpace);
	int windows = rows * collumns;
	//3 verts per triangle, 2 triangles per window, rows*collumns windows
	
	// 3 verts per triangle, 2 triagles per face, 6 faces
	//36 verts
	int totalVerts = 36 * (windows * 2 * 3);
	*/
	//init_windowGrid(&windows, program, 2, 2, 6, 4);
	
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
}

int main(int argc, char** argv)
{
	/* Initialize GLFW and GLEW */
	kuhl_ogl_init(&argc, argv, -1, -1, 32, 4);

	/* Specify function to call when keys are pressed. */
	glfwSetKeyCallback(kuhl_get_window(), keyboard);
	// glfwSetFramebufferSizeCallback(window, reshape);

	/* Compile and link a GLSL program comxposed of a vertex shader and
	 * a fragment shader. */
	program = kuhl_create_program("triangle-color.vert", "triangle-color.frag");

	/* Use the GLSL program so subsequent calls to glUniform*() send the variable to
	   the correct program. */
	glUseProgram(program);
	kuhl_errorcheck();

	/* Good practice: Unbind objects until we really need them. */
	glUseProgram(0);

	/* Create kuhl_geometry structs for the objects that we want to
	 * draw. */
	
	init_geometryBuilding(&building, program, 2, 2, 6, 4);
	init_windowGrid(&windows, program, 2, 2, 6, 4);
	
	dgr_init();     /* Initialize DGR based on config file. */

	float initCamPos[3]  = {0,0,10}; // location of camera
	float initCamLook[3] = {0,0,0}; // a point the camera is facing at
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
