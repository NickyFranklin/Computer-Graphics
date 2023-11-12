#version 150 // GLSL 150 = OpenGL 3.2

/* Vertex position and normal vector from the C program. These are in
 * object coordinates. */
in vec3 in_Position;
in vec2 in_TexCoord;
/* We output a position and normal in "camera coordinates = CC". After
 * the vertex program is run, these are interpolated across the
 * object, so they are no longer the vertex position and normal in the
 * fragment program...they are the fragment's position and normal. */
out vec4 out_Position_CC;
out vec3 out_Normal_CC;
out vec2 out_TexCoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform sampler2D tex;

uniform int red;

void main() 
{
	// Construct a normal matrix from the ModelView matrix. The
	// modelview is the same for all vertices, the normal matrix is
	// too. It would be more efficient to calculate it in our C
	// program once for this object. However, it is easier to
	// calculate here.
	mat3 NormalMat = transpose(inverse(mat3(ModelView)));
	     
	out_TexCoord = in_TexCoord;

	vec2 normCoord1 = in_TexCoord;
	normCoord1.x = normCoord1.x + 0.0005;

	vec2 normCoord2 = in_TexCoord;
	normCoord2.y = normCoord2.y + 0.0005;

	vec4 color = texture(tex, out_TexCoord);
	vec4 color2 = texture(tex, normCoord1);
	vec4 color3 = texture(tex, normCoord2);
	
	vec3 pos = in_Position;
	pos.y = (1 * color.r + 1 * color.b + 1 * color.g)/3;
	
	vec3 pos2 = in_Position;
	pos2.x = pos2.x + 0.0005;
	pos2.y = (1 * color2.r + 1 * color2.b + 1 * color2.g)/3;

	vec3 pos3 = in_Position;
	pos3.z = pos3.z + 0.0005;
	pos3.y = (1 * color3.r + 1 * color3.b + 1 * color3.g)/3;

	vec3 subPair1 = pos - pos2;

	vec3 subPair2 = pos - pos3;

	vec3 normal = cross(subPair2, subPair1);
	//vec3 normal = pos;

	//out_TexCoord = vec2((float) in_Position.x / 2001, (float) in_Position.z / 1001);
	// Transform the normal by the NormalMat and send it to the
	// fragment program.
	out_Normal_CC = normalize(NormalMat * normal);

	// Transform the vertex position from object coordinates into
	// camera coordinates.
	out_Position_CC = ModelView * vec4(pos, 1);

	// Transform the vertex position from object coordinates into
	// Normalized Device Coordinates (NDC).
	gl_Position = Projection * ModelView * vec4(pos, 1);
}
