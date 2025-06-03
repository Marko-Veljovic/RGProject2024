//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 clipSpaceCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 plane;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    gl_ClipDistance[0] = dot(vec4(FragPos, 1.0), plane);

    Normal = aNormal;
    clipSpaceCoords = projection * view * vec4(FragPos, 1.0);
    gl_Position = clipSpaceCoords;
}

//#shader fragment
#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;

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

in vec3 FragPos;
in vec3 Normal;
in vec4 clipSpaceCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform sampler2D reflectionTexture;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 reflectionTextureCoords);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 reflectionTextureCoords);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec2 ndc = (clipSpaceCoords.xy / clipSpaceCoords.w) / 2.0 + 0.5;
    vec2 reflectionTextureCoords = vec2(ndc.x, -ndc.y);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, reflectionTextureCoords);
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, reflectionTextureCoords);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 reflectionTextureCoords)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(reflectionTexture, reflectionTextureCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(reflectionTexture, reflectionTextureCoords));
    vec3 specular = light.specular * spec * vec3(texture(reflectionTexture, reflectionTextureCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 reflectionTextureCoords)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(reflectionTexture, reflectionTextureCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(reflectionTexture, reflectionTextureCoords));
    vec3 specular = light.specular * spec * vec3(texture(reflectionTexture, reflectionTextureCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}