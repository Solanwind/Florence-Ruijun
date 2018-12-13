#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 normal;
in vec3 Color;

out vec4 color;


//uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main(){
	
	vec3 objectColor = Color;
	
	// AMBIENT LIGHT
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	float emit = 0.1;

	// DIFFUSE LIGHT
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);     // angle > 90 degrees -> result of the dot product is negative -> negative 'color' -> not possible
													// solution: take the max between 0 and dot product

	// SPEULAR LIGHT
	vec3 viewDir = normalize(- FragPos);			// eye is in 0,0,0
	vec3 reflectDir = reflect(-lightDir, norm);  
	int shininess = 32;											// = factor n in '(cos(theta))^n)
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	
	// ATTENUATION 
	float kc = 0.1;   // constant attenuation
	float kl = 0.1;   // linear attenuation
	float kq = 0.1;   // quadratic attenuation
	float d = length(lightPos - FragPos);
	float att = 1 / (kc + kl*d * kq*d*d);

	// TOTAL LIGHT
	vec3 result = (ambient + emit) * objectColor + objectColor * (diff + spec) * lightColor;// * att;
	color = vec4(result, 1.0);

	
}


