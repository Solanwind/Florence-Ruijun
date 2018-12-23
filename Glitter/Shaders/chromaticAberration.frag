#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;
 
uniform samplerCube skybox;
uniform vec3 camPos;		// in world space
uniform float luminosity;

 
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


	// REFRACTION

	vec3 refractionRed = nLovernTRed * incident;
	float kred = 1 - nLovernTSquaredRed * (1 - pow(NdotL,2));
	if (kred < 0){
		refractionRed = vec3(0.0,0.0,0.0);
	}
	else{
		refractionRed = (nLovernTRed * NdotL - sqrt(kred)) * norm - refractionRed;
	}

	vec3 refractionGreen = nLovernTGreen * incident;
	float kGreen = 1 - nLovernTSquaredGreen * (1 - pow(NdotL,2));
	if (kGreen < 0){
		refractionGreen = vec3(0.0,0.0,0.0);
	}
	else{
		refractionGreen = (nLovernTGreen * NdotL - sqrt(kGreen)) * norm - refractionGreen;
	}

	vec3 refractionBlue = nLovernTBlue * incident;
	float kBlue = 1 - nLovernTSquaredBlue * (1 - pow(NdotL,2));
	if (kBlue < 0){
		refractionBlue = vec3(0.0,0.0,0.0);
	}
	else{
		refractionBlue = (nLovernTBlue * NdotL - sqrt(kBlue)) * norm - refractionBlue;
	}


    color.ra = texture(skybox, normalize(refractionRed)).ra * luminosity;
    color.g  = texture(skybox, normalize(refractionGreen)).g * luminosity;
    color.b  = texture(skybox, normalize(refractionBlue)).b * luminosity;
	
}



