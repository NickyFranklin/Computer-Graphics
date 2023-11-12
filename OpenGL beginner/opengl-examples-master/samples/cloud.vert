#version 150 // GLSL 150 = OpenGL 3.2

in vec3 in_Position;
in vec2 in_TexCoord;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform sampler2D tex;

out vec2 out_TexCoord;
out float transparency;

void main() 
{
	vec4 pos = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
	out_TexCoord = in_TexCoord;
	vec4 color = texture(tex, out_TexCoord);
	transparency = color.r * 1;
	gl_Position = Projection * ModelView * pos;
}
