#version 150 // GLSL 150 = OpenGL 3.2

in vec3 in_Position; // vertex position, object coordinates
in vec3 in_Color;    // vertex color
in vec2 in_TexCoord;
in vec1 textured;

uniform mat4 ModelView;
uniform mat4 Projection;

out vec3 color;
out vec1 istextured;

void main() 
{
	if(textured < 0.5) {
		// Output the color into the fragment program
		color = in_Color;
		vec4 pos = vec4(in_Position, 1.0);
		gl_Position = Projection * ModelView * pos;
		istextured = 0;
	}
	// Calculate the position of the vertex in NDC.
	else {
		vec4 pos = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
		out_TexCoord = in_TexCoord;
		gl_Position = Projection * ModelView * pos;
		istextured = 1;
	}
}
