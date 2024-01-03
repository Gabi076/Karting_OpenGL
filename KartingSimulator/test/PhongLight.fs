#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float exponent;

void main()
{

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize (lightPos - FragPos);

    vec3 viewDir= normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    vec3 ambient = Ka * lightColor;
    float diff1 = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Kd * lightColor * diff1;

    float diff2 = max(dot(viewDir, reflectDir), 0.0);
    vec3 specular = pow(diff2,exponent) * Ks * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor ;
    FragColor = vec4(result, 1.0);

} 
