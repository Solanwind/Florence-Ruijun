
#version 330 core

in vec2 texCoord;  
in vec3 tangentFragPos;
in vec3 tangentLightPos1;
in vec3 tangentLightPos2;
in vec3 tangentLightPos3;
in vec3 tangentLightPos4;
in vec3 tangentLightPos5;
in vec3 tangentLightPos6;
in vec3 tangentCamPos;


out vec4 color;

uniform sampler2D myTextureDiffuse; //same name as in c++ with glUniform1i()
uniform sampler2D myTextureNormal; //same name as in c++ with glUniform1i()
uniform vec3 lightColor;
uniform mat4 Model;



struct PointLight {    
    vec3 position;
    vec3 color;
	float attenuation;
}; 

uniform PointLight pointLights[6];

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 objectColor);


void main(){

	vec3 objectColor = texture(myTextureDiffuse, texCoord).rgb;
	vec3 Normal = texture(myTextureNormal, texCoord).rgb;
	Normal = mat3(transpose(inverse(Model))) * Normal;
	Normal = normalize(Normal * 2.0 - 1.0);  
	
	

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(tangentCamPos- tangentFragPos);
	PointLight pointLight[6] = pointLights;
	pointLight[0].position = tangentLightPos1;
	pointLight[1].position = tangentLightPos2;
	pointLight[2].position = tangentLightPos3;
	pointLight[3].position = tangentLightPos4;
	pointLight[4].position = tangentLightPos5;
	pointLight[5].position = tangentLightPos6;
	vec3 result = vec3(0,0,0);
	for(int i = 0; i < 6; i++)
        result += CalcPointLight(pointLight[i], norm, tangentFragPos, viewDir, objectColor);
	color = vec4(result, 1.0);
}




vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 objectColor)
{
   
	// AMBIENT LIGHT
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * light.color;
	float emit = 0.0;

	// DIFFUSE LIGHT
	vec3 lightDir = normalize(light.position - fragPos);  
	float diff = max(dot(norm, lightDir), 0.0);     // angle > 90 degrees -> result of the dot product is negative -> negative 'color' -> not possible
													// solution: take the max between 0 and dot product

	// SPECULAR LIGHT
	vec3 reflectDir = 2 * diff * norm - lightDir;  //reflect(-lightDir, norm);  
	int shininess = 32;											// = factor n in '(cos(theta))^n)
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	
	// ATTENUATION 
	float kc = 0.5;   // constant attenuation
	float kl = 0.07;   // linear attenuation
	float kq = 0.03;   // quadratic attenuation
	float d = length(light.position - fragPos);
	float att = 1 / (kc + kl*d * kq*d*d);

	// TOTAL LIGHT
	vec3 result = (ambient + emit) * objectColor + objectColor * (diff + spec) * light.color * att * light.attenuation;
	return color = vec4(result, 1.0);
} 



