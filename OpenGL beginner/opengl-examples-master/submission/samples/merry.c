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

//static kuhl_geometry triangle;
static kuhl_geometry quad1;
static kuhl_geometry roof;
static kuhl_geometry ground;
static kuhl_geometry *hippo;
static kuhl_geometry *cow;
static kuhl_geometry *lion;

static int isRotating=1;
static int isMoving = 1;

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
	if(key == GLFW_KEY_SPACE)
	{
		printf("toggling rotation\n");
		isRotating = !isRotating;
	}
	if(key == GLFW_KEY_U) {
	  printf("toggling movement\n");
	  isMoving = !isMoving;
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
		mat4f_rotateAxis_new(rotateMat, angle, 0,1,0);
	  
		/* Create a scale matrix. */
		float scaleMat[16];
		mat4f_scale_new(scaleMat, 3, 3, 3);
		

		/* Last parameter must be NULL, otherwise your program
		   can/will crash. The modelview matrix is (the view matrix) *
		   (the model matrix). Here, we have two matrices in the model
		   matrix, and multiply everything together at once into the
		   modelview matrix.
		   
		   modelview = viewmat * sclaeMat * rotateMat 
		                         ^---model matrix---^
		*/
		float modelview[16];
		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, NULL);
		}

		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, NULL);
		}
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
		kuhl_geometry_draw(&ground);
		kuhl_geometry_draw(&roof);
		//kuhl_geometry_draw(&quad1);
		/* If we wanted to draw multiple triangles and quads at
		 * different locations, we could call glUniformMatrix4fv again
		 * to change the ModelView matrix and then call
		 * kuhl_geometry_draw() again to draw that object again using
		 * the new model matrix. */
		float transMatrix[] = {1, 0, 0, 0,
				       0, 1, 0, 0,
				       0, 0, 1, 0,
				       0, 0, -4, 1};
		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrix,NULL);
		}
		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, transMatrix, NULL);
		}
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&quad1);


		float transMatrixHippo[] = {-1, 0, 0, 0,
					    0, 1, 0, 0,
					    0, 0, 1, 0,
					    0, 0, -4, 1};

		float transMatrixHippo2[] = {1, 0, 0, 0,
					     0, 1, 0, 0,
					     0, 0, 1, 0,
					     0, 0, 0, 1};
		if(isMoving) {
		  transMatrixHippo2[13] = ((sin(glfwGetTime()+160) + 1) / 2.0f);
		  //printf("%f\n", ((sin(glfwGetTime()) + 1) / 2.0f));
		}

		else {
		  transMatrixHippo2[13] = 0;
		}

		
		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrixHippo,
					transMatrixHippo2, NULL);
		}
		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, transMatrixHippo,
					transMatrixHippo2, NULL);
		}
		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value 
		kuhl_geometry_draw(hippo);
		
		float transMatrix2[] = {1, 0, 0, 0,
				       0, 1, 0, 0,
				       0, 0, 1, 0,
				       0, 0, 4, 1};
		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrix2,NULL);
		}
		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, transMatrix2, NULL);
		}
		
		//mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrix2,NULL);
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&quad1);

		float transMatrixCow[] =  {1, 0, 0, 0,
					    0, 1, 0, 0,
					    0, 0, 1, 0,
					    0, 0, 4, 1};
		
		float transMatrixCow2[] = {1, 0, 0, 0,
					    0, 1, 0, 0,
					    0, 0, 1, 0,
					    0, 0, 0, 1};
		if(isMoving) {
		  transMatrixCow2[13] = ((sin(glfwGetTime()+80) + 1) / 2.0f);
		  //printf("%f\n", ((sin(glfwGetTime()) + 1) / 2.0f));
		}

		else {
		  transMatrixCow[13] = 0;
		}

		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrixCow,
					transMatrixCow2, NULL);
		}
		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, transMatrixCow,
					transMatrixCow2, NULL);
		}
		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value 
		kuhl_geometry_draw(cow);

		
		float transMatrix3[] = {1, 0, 0, 0,
				       0, 1, 0, 0,
				       0, 0, 1, 0,
				       4, 0, 0, 1};
		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrix3, NULL);
		}
		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, transMatrix3, NULL);
		}
		
		//mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrix3,NULL);
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&quad1);

		float transMatrixLion[] = {cos(M_PI/2), 0, -sin(M_PI/2), 0,
					   0, 1, 0, 0,
					   sin(M_PI/2), 0, cos(M_PI/2), 0,
					   4, 0, 0, 1};

		float transMatrixLion2[] = {1, 0, 0, 0,
					    0, 1, 0, 0,
					    0, 0, 1, 0,
					    0, 0, 0, 1};
		if(isMoving) {
		  transMatrixLion2[13] = ((sin(glfwGetTime()) + 1) / 2.0f);
		  //printf("%f\n", ((sin(glfwGetTime()) + 1) / 2.0f));
		}

		else {
		  transMatrixLion2[13] = 0;
		}
		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat,
					transMatrixLion, transMatrixLion2, NULL);
		}
		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat,
					transMatrixLion, transMatrixLion2,NULL);
		}
		
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value 
		kuhl_geometry_draw(lion);
		
		float transMatrix4[] = {1, 0, 0, 0,
				       0, 1, 0, 0,
				       0, 0, 1, 0,
				       -4, 0, 0, 1};
		if(isRotating) {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrix4,NULL);
		}
		
		else {
		  mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, transMatrix4, NULL);
		}
		
		//mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, rotateMat, transMatrix4,NULL);
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   modelview); // value
		kuhl_geometry_draw(&quad1);
		//kuhl_geometry_draw(&quad2);
		//kuhl_geometry_draw(&quad3);
		//kuhl_geometry_draw(&quad4);
		

		glUseProgram(0); // stop using a GLSL program.
		viewmat_end_eye(viewportID);
	} // finish viewport loop
	viewmat_end_frame();

	/* Check for errors. If there are errors, consider adding more
	 * calls to kuhl_errorcheck() in your code. */
	kuhl_errorcheck();

}

void init_geometryTriangle(kuhl_geometry *geom, GLuint prog)
{
	kuhl_geometry_new(geom, prog, 3, // num vertices
	                  GL_TRIANGLES); // primitive type

	/* Vertices that we want to form triangles out of. Every 3 numbers
	 * is a vertex position. Since no indices are provided, every
	 * three vertex positions form a single triangle.*/
	GLfloat vertexPositions[] = {0, 0, 0,
	                             1, 0, 0,
	                             1, 1, 0};
	kuhl_geometry_attrib(geom, vertexPositions, // data
	                     3, // number of components (x,y,z)
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?

	/* The normals for each vertex */
	GLfloat normalData[] = {0, 0, 1,
	                        0, 0, 1,
	                        0, 0, 1};
	kuhl_geometry_attrib(geom, normalData, 3, "in_Normal", KG_WARN);
}

void init_ground(kuhl_geometry *geom, GLuint prog) {
  //kuhl_geometry_new(geom, prog, 7, GL_TRIANGLES);
  kuhl_geometry_new(geom, prog, 18, GL_TRIANGLES);
  GLfloat vertexPositions[] = {0, 0, 0,
			       5*sin(M_PI/3), 0, 5*cos(M_PI/3),
                               5*sin((2 * M_PI)/3), 0, 5*cos((2 * M_PI)/3),
			       //Triangle rotated 60 degrees
			       0, 0, 0,
			       5*sin((2 * M_PI)/3), 0, 5*cos((2 * M_PI)/3),
                               5*sin((3 * M_PI)/3), 0, 5*cos((3 * M_PI)/3),
			       //Triangle rotated 120 Degrees
			       0, 0, 0,
			       5*sin((3 * M_PI)/3), 0, 5*cos((3 * M_PI)/3),
                               5*sin((4 * M_PI)/3), 0, 5*cos((4 * M_PI)/3),
			       //Triangle flipped 180 degrees
			       0, 0, 0,
			       5*sin((4 * M_PI)/3), 0, 5*cos((4 * M_PI)/3),
                               5*sin((5 * M_PI)/3), 0, 5*cos((5 * M_PI)/3),
			       //Triangle flipped 240 degrees
			       0, 0, 0,
			       5*sin((5 * M_PI)/3), 0, 5*cos((5 * M_PI)/3),
                               5*sin((6 * M_PI)/3), 0, 5*cos((6 * M_PI)/3),
			       //Triamgle flipped 300 Degrees
			       0, 0, 0,
			       5*sin((6 * M_PI)/3), 0, 5*cos((6 * M_PI)/3),
                               5*sin((7 * M_PI)/3), 0, 5*cos((7 * M_PI)/3)};
  kuhl_geometry_attrib(geom, vertexPositions, // data
	                     3, // number of components (x,y,z)
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?

  /* The normals for each vertex */
  GLfloat normalData[] = {0, 0, 1,
			  0, 0, 1,
			  0, 0, 1,
			  //
			  0, 0, 1,
			  0, 0, 1,
			  0, 0, 1,
			  //
			  0, 0, 1,
			  0, 0, 1,
			  0, 0, 1,
			  //
			  0, 0, 1,
			  0, 0, 1,
			  0, 0, 1,
			  //
			  0, 0, 1,
			  0, 0, 1,
			  0, 0, 1,
			  //
			  0, 0, 1,
			  0, 0, 1,
			  0, 0, 1};
  kuhl_geometry_attrib(geom, normalData, 3, "in_Normal", KG_WARN);
}

void init_roof(kuhl_geometry *geom, GLuint prog) {
  //kuhl_geometry_new(geom, prog, 7, GL_TRIANGLES);
  kuhl_geometry_new(geom, prog, 18, GL_TRIANGLES);
  GLfloat vertexPositions[] = {0, 6, 0,
			       5*sin(M_PI/3), 5, 5*cos(M_PI/3),
                               5*sin((2 * M_PI)/3), 5, 5*cos((2 * M_PI)/3),
			       //Triangle rotated 60 degrees
			       0, 6, 0,
			       5*sin((2 * M_PI)/3), 5, 5*cos((2 * M_PI)/3),
                               5*sin((3 * M_PI)/3), 5, 5*cos((3 * M_PI)/3),
			       //Triangle rotated 120 Degrees
			       0, 6, 0,
			       5*sin((3 * M_PI)/3), 5, 5*cos((3 * M_PI)/3),
                               5*sin((4 * M_PI)/3), 5, 5*cos((4 * M_PI)/3),
			       //Triangle flipped 180 degrees
			       0, 6, 0,
			       5*sin((4 * M_PI)/3), 5, 5*cos((4 * M_PI)/3),
                               5*sin((5 * M_PI)/3), 5, 5*cos((5 * M_PI)/3),
			       //Triangle flipped 240 degrees
			       0, 6, 0,
			       5*sin((5 * M_PI)/3), 5, 5*cos((5 * M_PI)/3),
                               5*sin((6 * M_PI)/3), 5, 5*cos((6 * M_PI)/3),
			       //Triamgle flipped 300 Degrees
			       0, 6, 0,
			       5*sin((6 * M_PI)/3), 5, 5*cos((6 * M_PI)/3),
                               5*sin((7 * M_PI)/3), 5, 5*cos((7 * M_PI)/3)};
  kuhl_geometry_attrib(geom, vertexPositions, // data
	                     3, // number of components (x,y,z)
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?

  float norm1[3];
  float norm2[3];
  float norm3[3];
  float norm4[3];
  float norm5[3];
  float norm6[3];
  float triangle1P1[3];
  float triangle1P2[3];
  float triangle2P1[3];
  float triangle2P2[3];
  float triangle3P1[3];
  float triangle3P2[3];
  float triangle4P1[3];
  float triangle4P2[3];
  float triangle5P1[3];
  float triangle5P2[3];
  float triangle6P1[3];
  float triangle6P2[3];
  triangle1P1[0] = 5*sin(M_PI/3);
  triangle1P1[1] =  -1;
  triangle1P1[2] =  5*cos(M_PI/3);
  triangle1P2[0] = 5*sin(2 * M_PI/3);
  triangle1P2[1] =  -1;
  triangle1P2[2] =  5*cos(2 * M_PI/3);
  triangle2P1[0] = 5*sin(2 * M_PI/3);
  triangle2P1[1] =  -1;
  triangle2P1[2] =  5*cos(2 * M_PI/3);
  triangle2P2[0] = 5*sin(3 * M_PI/3);
  triangle2P2[1] =  -1;
  triangle2P2[2] =  5*cos(3 * M_PI/3);
  triangle3P1[0] = 5*sin(3 * M_PI/3);
  triangle3P1[1] =  -1;
  triangle3P1[2] =  5*cos(3 * M_PI/3);
  triangle3P2[0] = 5*sin(4 * M_PI/3);
  triangle3P2[1] =  -1;
  triangle3P2[2] =  5*cos(4 * M_PI/3);
  triangle4P1[0] = 5*sin(4 * M_PI/3);
  triangle4P1[1] =  -1;
  triangle4P1[2] =  5*cos(4 * M_PI/3);
  triangle4P2[0] = 5*sin(5 * M_PI/3);
  triangle4P2[1] =  -1;
  triangle4P2[2] =  5*cos(5 * M_PI/3);
  triangle5P1[0] = 5*sin(5 * M_PI/3);
  triangle5P1[1] =  -1;
  triangle5P1[2] =  5*cos(5 * M_PI/3);
  triangle5P2[0] = 5*sin(6 * M_PI/3);
  triangle5P2[1] =  -1;
  triangle5P2[2] =  5*cos(6 * M_PI/3);
  triangle6P1[0] = 5*sin(6 * M_PI/3);
  triangle6P1[1] =  -1;
  triangle6P1[2] =  5*cos(6 * M_PI/3);
  triangle6P2[0] = 5*sin(7 * M_PI/3);
  triangle6P2[1] =  -1;
  triangle6P2[2] =  5*cos(7 * M_PI/3);
  
  vec3f_cross_new(norm1, triangle1P1, triangle1P2);
  vec3f_cross_new(norm2, triangle2P1, triangle2P2);
  vec3f_cross_new(norm3, triangle3P1, triangle3P2);
  vec3f_cross_new(norm4, triangle4P1, triangle4P2);
  vec3f_cross_new(norm5, triangle5P1, triangle5P2);
  vec3f_cross_new(norm6, triangle6P1, triangle6P2);
  
  
  /* The normals for each vertex */
  GLfloat normalData[] = {norm1[0], norm1[1], norm1[2],
			  norm1[0], norm1[1], norm1[2],
			  norm1[0], norm1[1], norm1[2],
			  //
			  norm2[0], norm2[1], norm2[2],
			  norm2[0], norm2[1], norm2[2],
			  norm2[0], norm2[1], norm2[2],
			  //
			  norm3[0], norm3[1], norm3[2],
			  norm3[0], norm3[1], norm3[2],
			  norm3[0], norm3[1], norm3[2],
			  //
			  norm4[0], norm4[1], norm4[2],
			  norm4[0], norm4[1], norm4[2],
			  norm4[0], norm4[1], norm4[2],
			  //
			  norm5[0], norm5[1], norm5[2],
			  norm5[0], norm5[1], norm5[2],
			  norm5[0], norm5[1], norm5[2],
			  //
			  norm6[0], norm6[1], norm6[2],
			  norm6[0], norm6[1], norm6[2],
			  norm6[0], norm6[1], norm6[2],};
  kuhl_geometry_attrib(geom, normalData, 3, "in_Normal", KG_WARN);
}

/* This illustrates how to draw a quad by drawing two triangles and reusing vertices. */
void init_geometryQuad(kuhl_geometry *geom, GLuint prog)
{
	kuhl_geometry_new(geom, prog,
	                  4, // number of vertices
	                  GL_TRIANGLES); // type of thing to draw

	/* Vertices that we want to form triangles out of. Every 3 numbers
	 * is a vertex position. Below, we provide indices to form
	 * triangles out of these vertices. */
	GLfloat vertexPositions[] = {-0.05, 0, 0,
	                             0.05, 0, 0,
	                             0.05, 5, 0,
	                             -0.05, 5, 0};
	kuhl_geometry_attrib(geom, vertexPositions,
	                     3, // number of components x,y,z
	                     "in_Position", // GLSL variable
	                     KG_WARN); // warn if attribute is missing in GLSL program?

	/* The normals for each vertex */
	GLfloat normalData[] = {1, 1, 1,
	                        1, 1, 1,
	                        1, 1, 1,
	                        1, 1, 1};
	kuhl_geometry_attrib(geom, normalData, 3, "in_Normal", KG_WARN);
	
	/* A list of triangles that we want to draw. "0" refers to the
	 * first vertex in our list of vertices. Every three numbers forms
	 * a single triangle. */
	GLuint indexData[] = { 0, 1, 2,  
	                       0, 2, 3 };
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

	/* Compile and link a GLSL program composed of a vertex shader and
	 * a fragment shader. */
	program = kuhl_create_program("triangle-shade.vert", "triangle-shade.frag");

	/* Use the GLSL program so subsequent calls to glUniform*() send the variable to
	   the correct program. */
	glUseProgram(program);
	kuhl_errorcheck();
	/* Set the uniform variable in the shader that is named "red" to the value 1. */
	glUniform1i(kuhl_get_uniform("red"), 0);
	kuhl_errorcheck();
	/* Good practice: Unbind objects until we really need them. */
	glUseProgram(0);

	/* Create kuhl_geometry structs for the objects that we want to
	 * draw. */
	//init_geometryTriangle(&triangle, program);
	init_geometryQuad(&quad1, program);
	init_ground(&ground, program);
	init_roof(&roof, program);
	hippo = kuhl_load_model("../models/merry/hippo.ply", NULL, program, NULL);
	cow = kuhl_load_model("../models/merry/cow.ply", NULL, program, NULL);
	lion = kuhl_load_model("../models/merry/lion.ply", NULL, program, NULL);
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
