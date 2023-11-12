#version 150 // GLSL 150 = OpenGL 3.2
 
out vec4 fragColor;

in vec4 out_Position_CC;  // Fragment position (in cam coords)
in vec3 out_Normal_CC; // normal vector     (in cam coords)
in vec2 out_TexCoord;


uniform int red;
uniform sampler2D tex2;

/** Calculate diffuse shading. Normal and light direction do not need
 * to be normalized. */
float diffuseScalar(vec3 normal, vec3 lightDir, bool frontBackSame) {
      float diffuse = dot(normalize(lightDir), normalize(normal));

      if(frontBackSame) {
      	  diffuse = abs(diffuse);
      }

      else {
      	   diffuse = clamp(diffuse, 0, 1);
      }

      diffuse = diffuse/2 + 0.5;
      return diffuse;
}




void main() 
{
	vec3 lightPos = vec3(0,0,0);
	vec3 lightDir = lightPos - out_Position_CC.xyz;

	float diffuse = diffuseScalar(out_Normal_CC, lightDir, false);
	
	if(true) {
		 fragColor = texture(tex2, out_TexCoord) * diffuse;
	}

	else {
	     fragColor = vec4((normalize(out_Normal_CC)+1)/2, 1);
	}
}
