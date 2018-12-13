#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;
 
uniform samplerCube skybox;
uniform vec3 camPos;		// in world space
uniform mat4 View;
uniform mat4 Rotation;	// = model matrix
uniform float nLovernTSquared;
uniform float nLovernT;

 
void main()
{             
													 //				  			       . . .
    vec3 incident = normalize(-(FragPos - camPos));  // Minus sign (because we defined  \|/   , where . is the arrow)
												     //               . .
													 // instead of  \.|/          
	vec3 norm = normalize(Normal);
	float NdotL = dot(norm, incident);

	// REFLECTION

	vec3 reflection = 2 * NdotL * norm - incident;
	reflection = vec3(inverse(Rotation) * vec4(reflection, 0.0));	// return in local (object) space (required for texture function)


	// REFRACTION

	vec3 refraction = nLovernT * incident;
	float k = 1 - nLovernTSquared * (1 - pow(NdotL,2));
	if (k < 0){
		refraction = vec3(0.0,0.0,0.0);
	}
	else{
		refraction = (nLovernT * NdotL - sqrt(k)) * norm - refraction;
	}
	refraction = vec3(inverse(Rotation) * vec4(refraction, 0.0));
	// image is upside down, this is normal (https://stackoverflow.com/questions/13386003/glsl-refraction-getting-mapped-upside-down/13386403)
	// To change it, switch values of nt and nL


	// THE TWO EFFETS COMBINED
	// Decide which percentage is reflected, and which percentage is transmitted
	
	float percReflection =  0.0;
	float percRefraction = 1.0 - percReflection;

    //color = texture(skybox, normalize(percReflection * reflection + percRefraction * refraction));
	vec4 colorRefl = texture(skybox, normalize(reflection));
	vec4 colorRefr = texture(skybox, normalize(refraction));
	color = mix(colorRefr, colorRefl, percReflection);
}



