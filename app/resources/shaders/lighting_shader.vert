//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


//#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform vec3 dirLightDir;
uniform vec3 dirLightColor;

uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_bump1;
uniform vec3 objectColor;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(texture_bump1, texCoords).r;
    float heightScale = 0.04;
    vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
    return texCoords - p;
}

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);

    vec2 parallaxTexCoords = ParallaxMapping(TexCoords, viewDir);

    if (parallaxTexCoords.x < 0.0 || parallaxTexCoords.x > 1.0 ||
        parallaxTexCoords.y < 0.0 || parallaxTexCoords.y > 1.0)
        discard;

    vec3 baseColor = texture(texture_diffuse1, parallaxTexCoords).rgb;
    if (baseColor == vec3(0.0))
        baseColor = objectColor;

    float bump = texture(texture_bump1, parallaxTexCoords).r;
    bump = bump * 2.0 - 1.0;
    vec3 perturbedNormal = normalize(Normal + bump * 0.3);
    vec3 norm = normalize(perturbedNormal);

    vec3 lightDir = normalize(-dirLightDir);
    float diffDir = max(dot(norm, lightDir), 0.0);
    vec3 diffuseDir = diffDir * dirLightColor;

    vec3 toLight = normalize(pointLightPos - FragPos);
    float diffPoint = max(dot(norm, toLight), 0.0);
    float dist = length(pointLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
    vec3 diffusePoint = diffPoint * pointLightColor * attenuation;

    vec3 ambient = 0.25 * baseColor;
    vec3 result = (ambient + diffuseDir + diffusePoint) * baseColor;

    FragColor = vec4(result, 1.0);
}
