#version 150 // GLSL 150 = OpenGL 3.2

out vec4 fragColor;
in vec2 out_TexCoord;
in float transparency;

uniform sampler2D tex;

void main() 
{
	if(true)
	{
		vec4 color = texture(tex, out_TexCoord);
		if(color.r > 0.01) {
		  fragColor = vec4(1,1,1, color.r);
		}
		else {
		     fragColor = vec4(1,1,1, 0);
		}
	}
	else
	{
		// Debugging tip: Try displaying texture coordinate as a
		// color.
		fragColor = vec4(out_TexCoord,0,1);
	}
}
