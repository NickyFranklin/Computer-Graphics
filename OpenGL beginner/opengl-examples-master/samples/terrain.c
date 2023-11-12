/* Copyright (c) 2014-2015 Scott Kuhl. All rights reserved.
 * License: This code is licensed under a 3-clause BSD license. See
 * the file named "LICENSE" for a full copy of the license.
 */

/** @file Demonstrates drawing a shaded 3D triangle.
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
static GLuint program2 = 0;

static kuhl_geometry groundQuad;
static kuhl_geometry quad;


static int red = 0;

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
	if(action == GLFW_PRESS)
		return;

	/* Custom key handling code below: 
	   For a list of keys, see: https://www.glfw.org/docs/latest/group__keys.html  */
	if(key == GLFW_KEY_N)
	{
		printf("toggling color\n");
		red = !red;
	}
	
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
		float angle = fmod(glfwGetTime()*45, 360);

		/* Make sure all computers/processes use the same angle */
		dgr_setget("angle", &angle, sizeof(GLfloat));

		/* Create a 4x4 rotation matrix based on the angle we computed. */
		float rotateMat[16];
		mat4f_rotateAxis_new(rotateMat, 0, 0,1,0);

		/* Create a scale matrix. */
		float scaleMat[16];
		mat4f_scale_new(scaleMat, 3, 3, -3);


		/* Last parameter must be NULL, otherwise your program
		   can/will crash. The modelview matrix is (the view matrix) *
		   (the model matrix). Here, we have two matrices in the model
		   matrix, and multiply everything together at once into the
		   modelview matrix.
		   
		   modelview = viewmat * sclaeMat * rotateMat 
		                         ^---model matrix---^
		*/
		float modelview[16];
		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, NULL);

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
		//kuhl_geometry_draw(&triangle);
		kuhl_geometry_draw(&groundQuad);

		glUseProgram(program2);
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
		kuhl_geometry_draw(&quad);
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


/* This illustrates how to draw a quad by drawing two triangles and reusing vertices. */
void init_geometryGroundQuad(kuhl_geometry *geom, GLuint prog)
{
	kuhl_geometry_new(geom, prog,
	                  2001*1001, // number of vertices
	                  GL_TRIANGLES); // type of thing to draw

	/* Vertices that we want to form triangles out of. Every 3 numbers
	 * is a vertex position. Below, we provide indices to form
	 * triangles out of these vertices. */


	long numVertex = 2001 * 1001;
	//GLfloat vertexPositions[numVertex*3];
	GLfloat *vertexPositions = (GLfloat*) malloc(numVertex*3 * sizeof(GLfloat));
	//a n x m square has n+1 X m+1 verticies
	//2000 X 1000 = 2001 * 1001 = 2003001 verticies
	//long numVertex = 2001 * 1001;

	//numVertex*3 is because each vertex has three components
	//GLfloat vertexPositions[numVertex*3];
	
	//Tiles vertically after a row is done
	//k is here because every three positions is a vertex 
	long k = 0;
	for(int i = 0; i < 1001; i++) {

	  for(int j = 0; j < 2001; j++) {
	    //x coord is just j
	    vertexPositions[k] = ((float) j)/100;
	    //y coord is just 0
	    vertexPositions[k+1] = 0;
	    //z coord will always be i
	    vertexPositions[k+2] = ((float) i) /100;
	    k += 3;
	  }
	  
	}


	
	kuhl_geometry_attrib(geom, vertexPositions,
	                     3, // number of components x,y,z
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?




	GLfloat *texcoordData = (GLfloat*) malloc(numVertex*3*sizeof(GLfloat));

	k = 0;
	for(int i = 0; i < 1001; i++) {
	  for(int j = 0; j < 2001; j++) {
	    texcoordData[k] = ((float) j) / 2001;
	    texcoordData[k+1] = ((float) i) / 1001;
	    k+=2;
	  }
	}
	
	
	kuhl_geometry_attrib(geom, texcoordData, 2, "in_TexCoord", KG_WARN);


	GLuint texId = 0;
	float aspectRatio = kuhl_read_texture_file("../images/topography-smaller.png", &texId);
	msg(MSG_DEBUG, "Aspect ratio of image is %f\n", aspectRatio);
	kuhl_geometry_texture(geom, texId, "tex", KG_WARN);

	GLuint texId2 = 1;
	float aspectRatio2 = kuhl_read_texture_file("../images/world-smaller.png", &texId2);
	msg(MSG_DEBUG, "Aspect ratio of image is %f\n", aspectRatio2);
	kuhl_geometry_texture(geom, texId2, "tex2", KG_WARN);
	
	/* The normals for each vertex */
	//GLfloat normalData[numVertex*3];

	/*
	GLfloat *normalData = (GLfloat *) malloc(numVertex*3 * sizeof(GLfloat));
	for(int i = 0; i < numVertex*3; i++) {
	  normalData[i] = 1;
	}
	kuhl_geometry_attrib(geom, normalData, 3, "in_Normal", KG_WARN);
	*/
	
	/* A list of triangles that we want to draw. "0" refers to the
	 * first vertex in our list of vertices. Every three numbers forms
	 * a single triangle. */
	//The number of entries is the number of squares *
	//two triangles per square * 3 indicies per triangle
	//GLuint indexData[2000*1000*2*3];
	GLuint *indexData = (GLuint*) malloc(2000*1000*2*3*sizeof(GLuint)); 
	k = 0;
	for(int i = 0; i < 1000; i++) {

	  for(int j = 0; j < 2000; j++) {
	    //to get the index of the i'th row index value, it is i*2000+j
	    //top left of a square
	    indexData[k] = ((i+1)*2001)+j;
	    //bottom left of a square
	    indexData[k+1] = (i*2001)+j;
	    //bottom right of a square
	    indexData[k+2] = (i*2001)+j+1;
	    //Bottom right of a square
	    indexData[k+3] = (i*2001)+j+1;
	    //top right of a square
	    indexData[k+4] = ((i+1)*2001)+j+1;
	    //top left of a square
	    indexData[k+5] = ((i+1)*2001)+j;
	    k+=6;
	  }
	  
	}
	kuhl_geometry_indices(geom, indexData, 2000*1000*2*3);

	kuhl_errorcheck();
}

void init_geometryQuad(kuhl_geometry *geom, GLuint prog) {
  kuhl_geometry_new(geom, prog, 4, GL_TRIANGLES);

  GLfloat vertexPositions[] = {0, 1, 0,
			       20, 1, 0,
			       0, 1, 10,
			       20, 1, 10};
  kuhl_geometry_attrib(geom, vertexPositions, 3, "in_Position", KG_WARN);

  GLfloat texcoordData[] = {0,0,
			    1,0,
			    0,1,
			    1,1};
  
  
  kuhl_geometry_attrib(geom, texcoordData, 2, "in_TexCoord", KG_WARN);
  
  
  GLuint texId3 = 2;
  float aspectRatio3 = kuhl_read_texture_file("../images/clouds.jpg", &texId3);
  msg(MSG_DEBUG, "Aspect ratio of image is %f\n", aspectRatio3);
  kuhl_geometry_texture(geom, texId3, "tex", KG_WARN);
  

  
  
  GLuint indexData[] = {0,1,2,
			3,2,1};
  kuhl_geometry_indices(geom, indexData, 6);
  kuhl_errorcheck();

    
}

int main(int argc, char** argv)
{
	/* Initialize GLFW and GLEW */
	kuhl_ogl_init(&argc, argv, -1, -1, 32, 4);
	
	
	/* Specify function to call when keys are pressed. */
	glfwSetKeyCallback(kuhl_get_window(), keyboard);
	// glfwSetFramebufferSizeCallback(window, reshape);

	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	/* Compile and link a GLSL program composed of a vertex shader and
	 * a fragment shader. */
	program = kuhl_create_program("terrain-ground.vert", "terrain-ground.frag");
	program2 = kuhl_create_program("cloud.vert", "cloud.frag");
	/* Use the GLSL program so subsequent calls to glUniform*() send the variable to
	   the correct program. */
	
	glUseProgram(program);
	kuhl_errorcheck();
	/* Set the uniform variable in the shader that is named "red" to the value 1. */
	//glUniform1i(kuhl_get_uniform("red"), red);
	kuhl_errorcheck();
	/* Good practice: Unbind objects until we really need them. */
	glUseProgram(0);

	/* Create kuhl_geometry structs for the objects that we want to
	 * draw. */
	init_geometryGroundQuad(&groundQuad, program);
	init_geometryQuad(&quad, program2);
	dgr_init();     /* Initialize DGR based on config file. */

	float initCamPos[3]  = {0,1,10}; // location of camera
	float initCamLook[3] = {0,-0.25,0}; // a point the camera is facing at
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
