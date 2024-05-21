#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
// uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
//     // properties
    vec3 norm = normalize(Normal);
    vec3 light = normalize(dirLight.direction);
    vec3 viewDir = normalize(viewPos - FragPos);
//
//     // == =====================================================
//     // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
//     // For each phase, a calculate function is defined that calculates the corresponding color
//     // per lamp. In the main() function we take all the calculated colors and sum them up for
//     // this fragment's final color.
//     // == =====================================================
//     // phase 1: directional lighting
//     vec3 result_ambient = CalcDirLight(dirLight, norm, viewDir);
//     // phase 2: point lights
//     vec3 result_diffuse;
//     for(int i = 0; i < NR_POINT_LIGHTS; i++)
//         result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
//
//     // phase 3: spot light
//     vec3 result_diffuse;
//     result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

//     if(dot(viewDir, norm)/(|viewDir|*|norm|)>-0.1&&dot(viewDir, norm)/(|viewDir|*|norm|)<0.1){
//         FragColor = vec4(0.0,0.0,0.0,1.0);
//     }
//     else{
//         FragColor = vec4(result, 1.0);
//     }
//     FragColor = vec4(result, 1.0);
//     float intensity;
//     vec3 color;
//     intensity = -dot(light,norm);

    vec3 lightDir = normalize(-dirLight.direction);
    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = dirLight.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = dirLight.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = dirLight.specular * spec * vec3(texture(texture_specular1, TexCoords));

//     if (intensity > 0.85)
//         color = specular+diffuse;
//     else if (intensity > 0.5)
//         color = diffuse;
//     else
//         color = color = vec3(0.3,0.3,0.3);
//     if (intensity > 0.85)
//         color = vec3(1.0,0.5,0.5);
//     else if (intensity > 0.5)
//         color = vec3(0.6,0.3,0.3);
//     else
//         color = vec3(0.3,0.3,0.3);
//     FragColor = vec4(color,1.0);

//     FragColor = vec4(ambient+diffuse+specular,1.0);

    float intensity;
    vec3 color;
    intensity = -dot(light,normalize(norm));

    if (intensity >= 0.85)
        color = diffuse*1.5;
    else if (intensity >= 0.5)
        color = diffuse;
    else
        color = ambient;
    FragColor = vec4(color,1.0);

}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
//     if(dot(viewDir, normal)>-0.6&&dot(viewDir, normal)<0.6){
//         return vec3(0.0,0.0,0.0);
//     }
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//     float spec = 32.0f;
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
//     if(dot(viewDir, normal)>-0.6&&dot(viewDir, normal)<0.6){
//         return vec3(0.0,0.0,0.0);
//     }
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//     float spec = 0.1;
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}