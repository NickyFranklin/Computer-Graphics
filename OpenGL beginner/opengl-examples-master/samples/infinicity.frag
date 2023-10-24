#version 150 // GLSL 150 = OpenGL 3.2

out vec4 fragColor;
in vec3 color;
in vec2 out_TexCoord;
in vec1 istextured;
uniform sampler2D tex;

void main() {
	if(istextured > 0.5) {
		fragColor = texture(tex, out_TexCoord);
	}
	else {
		fragColor = vec4(color.r, color.g, color.b, 1.0);
	}
}
