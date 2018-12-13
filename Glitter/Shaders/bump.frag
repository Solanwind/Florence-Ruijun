
#version 330 core

in vec2 texCoord;  
in vec3 tangentFragPos;
in vec3 tangentLightPos;
in vec3 tangentCamPos;


out vec4 color;

uniform sampler2D myTextureDiffuse; //same name as in c++ with glUniform1i()
uniform sampler2D myTextureNormal; //same name as in c++ with glUniform1i()
uniform vec3 lightColor;
uniform mat4 Rotation;


void main(){

	vec3 objectColor = texture(myTextureDiffuse, texCoord).rgb;
	vec3 Normal = texture(myTextureNormal, texCoord).rgb;
	Normal = mat3(transpose(inverse(Rotation))) * Normal;
	Normal = normalize(Normal * 2.0 - 1.0);  
	

	// AMBIENT LIGHT
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	float emit = 0.1;

	// DIFFUSE LIGHT
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(tangentLightPos - tangentFragPos);  
	float diff = max(dot(norm, lightDir), 0.0);     // angle > 90 degrees -> result of the dot product is negative -> negative 'color' -> not possible
													// solution: take the max between 0 and dot product

	// SPEULAR LIGHT
	vec3 viewDir = normalize(tangentCamPos- tangentFragPos);			
	vec3 reflectDir = reflect(-lightDir, norm);  
	int shininess = 32;											// = factor n in '(cos(theta))^n)
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	
	// ATTENUATION 
	float kc = 0.5;   // constant attenuation
	float kl = 0.5;   // linear attenuation
	float kq = 0.5;   // quadratic attenuation
	float d = length(tangentLightPos - tangentFragPos);
	float att = 1 / (kc + kl*d * kq*d*d);
	
	// TOTAL LIGHT
	vec3 result = (ambient + emit) * objectColor + objectColor * (diff + spec) * lightColor * att;
	color = vec4(result, 1.0);

	
}




