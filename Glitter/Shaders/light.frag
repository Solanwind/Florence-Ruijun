#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 camPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform samplerCube skybox;
uniform mat4 View;
uniform mat4 Model;


void main(){

// LIGHTING
	
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
	vec3 viewDir = normalize(camPos - FragPos);	
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
	vec3 result = (ambient + emit) * objectColor + objectColor * (diff + spec) * lightColor; // * att;
	color = vec4(result, 1.0);



// REFLECTION OF SKYBOX

	float NdotL = dot(norm, viewDir);
	vec3 reflection = 2 * NdotL * norm - viewDir;
	reflection = vec3(inverse(Model) * vec4(reflection, 0.0));	// return in local (object) space (required for texture function)


	
// THE TWO EFFETS COMBINED
		
	float percReflection =  1;
	float percLight = 1.0 - percReflection;
	vec4 colorRefl = texture(skybox, normalize(reflection));
	color = mix(color, colorRefl, percReflection);


	
}



