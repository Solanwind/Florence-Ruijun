#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;
 
uniform samplerCube skybox;
uniform vec3 camPos;		// in world space
uniform mat4 View;
uniform mat4 Rotation;	// = model matrix


 
void main()
{             
													 //				  			       . . .
    vec3 incident = normalize(-(FragPos - camPos));  // Minus sign (because we defined  \|/   , where . is the arrow)
												     //               . .
													 // instead of  \.|/          
	vec3 norm = normalize(Normal);
	float NdotL = dot(norm, incident);

	float nLovernTRed = 1/1.46;
	float nLovernTGreen = 1/1.43;
	float nLovernTBlue = 1/1.40;
	float nLovernTSquaredRed = pow(nLovernTRed,2);
	float nLovernTSquaredGreen = pow(nLovernTGreen,2);
	float nLovernTSquaredBlue = pow(nLovernTBlue,2);


	// REFLECTION

	vec3 reflection = 2 * NdotL * norm - incident;
	reflection = vec3(inverse(Rotation) * vec4(reflection, 0.0));	// return in local (object) space (required for texture function)


	// REFRACTION

	vec3 refractionRed = nLovernTRed * incident;
	float kred = 1 - nLovernTSquaredRed * (1 - pow(NdotL,2));
	if (kred < 0){
		refractionRed = vec3(0.0,0.0,0.0);
	}
	else{
		refractionRed = (nLovernTRed * NdotL - sqrt(kred)) * norm - refractionRed;
	}
	refractionRed = vec3(inverse(Rotation) * vec4(refractionRed, 0.0));

	vec3 refractionGreen = nLovernTGreen * incident;
	float kGreen = 1 - nLovernTSquaredGreen * (1 - pow(NdotL,2));
	if (kGreen < 0){
		refractionGreen = vec3(0.0,0.0,0.0);
	}
	else{
		refractionGreen = (nLovernTGreen * NdotL - sqrt(kGreen)) * norm - refractionGreen;
	}
	refractionGreen = vec3(inverse(Rotation) * vec4(refractionGreen, 0.0));

	vec3 refractionBlue = nLovernTBlue * incident;
	float kBlue = 1 - nLovernTSquaredBlue * (1 - pow(NdotL,2));
	if (kBlue < 0){
		refractionBlue = vec3(0.0,0.0,0.0);
	}
	else{
		refractionBlue = (nLovernTBlue * NdotL - sqrt(kBlue)) * norm - refractionBlue;
	}
	refractionBlue = vec3(inverse(Rotation) * vec4(refractionBlue, 0.0));



	// THE TWO EFFETS COMBINED
	// Decide which percentage is reflected, and which percentage is transmitted
	
	float percReflection =  1.0;
	float percRefraction = 1.0 - percReflection;

	vec4 colorRefr;
    colorRefr.ra = texture(skybox, normalize(refractionRed)).ra;
    colorRefr.g  = texture(skybox, normalize(refractionGreen)).g;
    colorRefr.b  = texture(skybox, normalize(refractionBlue)).b;
	
	vec4 colorRefl = texture(skybox, normalize(reflection));
	color = mix(colorRefr, colorRefl, percReflection);

	float perc = 0.0;
	vec4 colorObject = vec4(0.0,0.0,0.0,1.0);
	color = mix(color, colorObject, perc);
}



