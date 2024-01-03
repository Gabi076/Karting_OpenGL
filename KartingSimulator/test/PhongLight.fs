#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float Ka=0.1;
uniform float Kd=0.5;
uniform float Ks=0.1;
uniform float n=1;
void main()
{
     vec3 ambient=Ka*lightColor;
     vec3 norm=normalize(Normal);
     vec3 lightDir=normalize(lightPos-FragPos);
     float diff=Kd*max(dot(norm,lightDir),0.0);
     vec3 diffuse=diff*lightColor;
     vec3 viewDir=normalize(viewPos-FragPos);
     vec3 reflectDir=reflect(-lightDir,norm);
     float spec=Ks*pow(max(dot(viewDir,reflectDir),0.0),n);
     vec3 result=(ambient+diffuse+spec)*objectColor;
    FragColor = vec4(result, 1.0);
} 