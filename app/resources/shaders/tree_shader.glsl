//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(mat3(model) * aTangent);
    vec3 B = normalize(mat3(model) * aBitangent);
    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);
    TBN = mat3(T, B, N);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

struct Material {
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform bool enableWind;
uniform float time;

vec2 calculateWindEffect(vec2 texCoords) {
    if (enableWind) {
        // Simple wind animation effect
        float windStrength = 0.005;
        float windSpeed = 1.0;
        vec2 windOffset = vec2(
        sin(time * windSpeed + texCoords.y * 10.0) * windStrength,
        cos(time * windSpeed * 0.5 + texCoords.x * 10.0) * windStrength * 0.5
        );
        return texCoords + windOffset;
    }
    return texCoords;
}

void main()
{
    // Apply wind effect to texture coordinates for leaves
    vec2 windTexCoords = calculateWindEffect(TexCoords);

    // Sample textures
    vec4 texColor = texture(texture_diffuse1, windTexCoords);

    // Alpha testing for leaves/foliage
    if (texColor.a < 0.1)
    discard;

    // Sample and transform normal from normal map
    vec3 normalMap = texture(texture_normal1, windTexCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Transform from [0,1] to [-1,1]
    vec3 norm = normalize(TBN * normalMap);

    // Ambient lighting
    vec3 ambient = light.ambient * texColor.rgb;

    // Diffuse lighting
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor.rgb;

    // Specular lighting (reduced for leaves)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Subsurface scattering approximation for leaves
    float leafFactor = 0.25;
    float backLight = max(dot(-lightDir, norm), 0.0) * leafFactor;
    vec3 subsurface = light.diffuse * backLight * texColor.rgb * vec3(0.5, 1.0, 0.3); // Green tint

    // Regular specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    // Final color calculation with subsurface scattering
    vec3 result = ambient + diffuse + specular + subsurface;

    FragColor = vec4(result, texColor.a);
}