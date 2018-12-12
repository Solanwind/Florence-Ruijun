#version 330 core

in vec3 Normal;
in vec3 FragPos;  // world coordinates
in vec2 texCoord;

out vec4 color;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec4 maincolor;  
uniform sampler2D myTexture1;
uniform vec3 lightPos;
uniform vec3 viewPos;  // the position of the camera


void main() {
//color = texture(myTexture1,texCoord);

// ambient light
float ambientStrength = 0.1;
vec3 ambient = ambientStrength * lightColor;

// diffuse
vec3 norm = normalize(Normal); 
vec3 lightDir = normalize(lightPos - FragPos);  // the directed light ray
float diff = max(dot(norm, lightDir), 0.0);  // to make sure diffuse is not negative
vec3 diffuse = diff * lightColor;

// specular
float specularStrength = 0.5;
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);  // lightDir is from fragment pointing to the light 
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  // 32 is the shininess, dot((viewDir, reflectDir)) is the angle between rlection light and camera postion
vec3 specular = specularStrength * spec * lightColor;


vec3 result = (ambient + diffuse + specular) * objectColor;
FragColor = vec4(result, 1.0);

}