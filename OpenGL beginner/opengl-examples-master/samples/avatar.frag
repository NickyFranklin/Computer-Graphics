#version 150 // GLSL 150 = OpenGL 3.2

out vec4 fragColor;
in vec2 out_TexCoord; // Vertex texture coordinate
in vec3 out_Color;    // Vertex color
in vec3 out_Normal_CC;   // Normal vector in camera coordinates
in vec3 out_Position_CC; // Position of fragment in camera coordinates
in vec3 out_Normal;
in mat3 normal_mat;
in vec3 Tangent;
in vec3 Bitangent;

uniform int HasTex;    // Is there a texture in tex?
uniform sampler2D tex; // Diffuse texture
uniform sampler2D tex_NORMALS;
uniform sampler2D tex_SPECULAR;
uniform int renderStyle;

/** Calculate diffuse shading. Normal and light direction do not need
 * to be normalized. */
float diffuseScalar(vec3 normal, vec3 lightDir, bool frontBackSame)
{
	/* Basic equation for diffuse shading */
	float diffuse = dot(normalize(lightDir), normalize(normal));

	/* The diffuse value will be negative if the normal is pointing in
	 * the opposite direction of the light. Set diffuse to 0 in this
	 * case. Alternatively, we could take the absolute value to light
	 * the front and back the same way. Either way, diffuse should now
	 * be a value from 0 to 1. */
	if(frontBackSame)
		diffuse = abs(diffuse);
	else
		diffuse = clamp(diffuse, 0, 1);

	/* Keep diffuse value in range from .5 to 1 to prevent any object
	 * from appearing too dark. Not technically part of diffuse
	 * shading---however, you may like the appearance of this. */
	diffuse = diffuse/2 + .5;
	
	return diffuse;
}

float specular(vec3 normal, vec3 lightDir, vec3 eyeDir) {
      vec3 halfVector = normalize(lightDir) + normalize(-eyeDir);
      halfVector = normalize(halfVector);

      float spec = dot(normalize(halfVector), normalize(normal));
      if(spec < 0) {
      	spec = 0;
      }
      
      return pow(spec, 10);
}

void main() 
{
	/* Get position of light in camera coordinates. When we do
	 * headlight style rendering, the light will be at the position of
	 * the camera! */
	vec3 lightPos = vec3(0,1000,0);
	vec4 color = texture(tex, out_TexCoord);

	/* Calculate a vector pointing from our current position (in
	 * camera coordinates) to the light position. */
	vec3 lightDir = lightPos - out_Position_CC;

	/* Calculate diffuse shading */
	float original_diffuse = diffuseScalar(out_Normal_CC, lightDir, false);	

	
	mat3 tbn;
	tbn[0] = Tangent;
	tbn[1] = Bitangent;
	tbn[2] = out_Normal; 
	
	vec4 texColor = texture(tex_NORMALS, out_TexCoord);
	texColor = vec4(pow(texColor.r, 1.0f / 2.2f), pow(texColor.g, 1.0f / 2.2f),
		   	pow(texColor.b, 1.0f / 2.2f), texColor.a);
	texColor = vec4(((texColor.r * 2) - 1), ((texColor.g * 2) - 1),
		         ((texColor.b * 2) - 1), texColor.a);
	vec3 adjustedNormal = normalize(normal_mat * (tbn * texColor.rgb));

	float adjusted_diffuse = diffuseScalar(adjustedNormal, lightDir, false);;

	vec4 specularColor = texture(tex_SPECULAR, out_TexCoord);
      	specularColor = vec4(pow(specularColor.r, 1.0f / 2.2f), pow(specularColor.g, 1.0f / 2.2f),
		      	     pow(specularColor.b, 1.0f / 2.2f), specularColor.a);
	
	//Specular
	float specularNum = specular(adjustedNormal, lightDir, out_Position_CC);
	specularNum = specularNum * specularColor.r;

	if(renderStyle == 0)
	{
		//shading no color
		fragColor = vec4(original_diffuse, original_diffuse, original_diffuse, 1);
	}
	else if(renderStyle == 1)
	{
		/* diffuse shading with normal map (no color) */
		fragColor = vec4(adjusted_diffuse, adjusted_diffuse, adjusted_diffuse, 1); 
	}
	else if(renderStyle == 2)
	{
		/* diffuse/color texture with diffuse shading using normalmap norm */
		fragColor = vec4((out_Normal_CC+1)/2, 1);	
	}
	else if(renderStyle == 3)
	{
		/* Display normals as a color (with normals updated by normal map) */
		fragColor = vec4((adjustedNormal+1)/2, 1);
	}
	else if(renderStyle == 4)
	{
		/* diffuse/color texture with diffuse shading using original normal */
		fragColor = vec4(color.rgb * original_diffuse, color.a);
	}
	else if(renderStyle == 5)
	{
		/* Display normals as a color. */
		fragColor = vec4(color.rgb * adjusted_diffuse, color.a);
	}
	else if(renderStyle == 6)
	{
		/* diffuse/color texture, diffuse shading with normalmap, specular shading */
		fragColor = vec4(color.rgb * adjusted_diffuse + specularNum * vec3(1,1,1), color.a);
	}
	
}