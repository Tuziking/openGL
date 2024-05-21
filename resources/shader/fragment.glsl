#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform vec3 viewPos;

void main()
{
    //ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * texture(texture_diffuse1, TexCoords).rgb;

    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(texture_diffuse1, TexCoords).rgb;

    //specular
    // float specularStrength = 0.01;
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength*spec*texture(texture_specular1, TexCoords).rgb;

    FragColor = vec4(ambient+diffuse+specular,1.0);

    float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}