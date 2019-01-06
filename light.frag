#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 camPos;
uniform vec3 objectColor;
uniform samplerCube skybox;
uniform float percReflection;
uniform float luminosity;



struct PointLight {    
    vec3 position;
    vec3 color;
    float attenuation;
}; 

uniform PointLight pointLights[6];

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);



void main(){

// LIGHTING
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(camPos - FragPos);	
	vec3 result = vec3(0,0,0);
	for(int i = 0; i < 6; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	color = vec4(result, 1.0);

// REFLECTION OF SKYBOX

	float NdotL = dot(norm, viewDir);
	vec3 reflection = 2 * NdotL * norm - viewDir;

	
// THE TWO EFFETS COMBINED
	
	vec4 colorRefl = texture(skybox, normalize(reflection)) * luminosity;
	color = mix(color, colorRefl, percReflection);


	
}




vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
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
	float kl = 0.08;   // linear attenuation
	float kq = 0.05;   // quadratic attenuation
	float d = length(light.position - fragPos);
	float att = 1 / (kc + kl*d * kq*d*d);

	// TOTAL LIGHT
	vec3 result = (ambient + emit) * objectColor + objectColor * (diff + spec) * light.color * att * light.attenuation;
	return color = vec4(result, 1.0);
} 



