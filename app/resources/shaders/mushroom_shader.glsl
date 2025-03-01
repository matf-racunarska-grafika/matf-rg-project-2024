//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    // Apply slight undulation to simulate natural mushroom cap curves
    float undulation = sin(aTexCoords.x * 10.0) * cos(aTexCoords.y * 10.0) * 0.02;
    vec3 adjustedPos = aPos;
    if (adjustedPos.y > 0.5) { // Only apply to upper part (cap)
        adjustedPos.y += undulation;
    }

    FragPos = vec3(model * vec4(adjustedPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
    float subsurface;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

// New parameters for enhanced mushroom rendering
uniform float wetness = 0.6;       // Controls how wet/slimy the mushroom appears
uniform float capTranslucency = 0.3; // Simulates thin cap edges
uniform float rimLight = 0.2;      // Enhances edges

void main()
{
    // Sample textures
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    vec3 specMap = texture(texture_specular1, TexCoords).rgb;

    // Calculate normals
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Ambient light
    vec3 ambient = light.ambient * texColor.rgb;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor.rgb;

    // Specular lighting with varying intensity based on wetness
    float glossiness = material.shininess * (specMap.r + wetness);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), glossiness);
    vec3 specular = light.specular * spec * (specMap + wetness * 0.5);

    // Subsurface scattering effect for mushroom cap
    float sss = 0.0;
    float capMask = smoothstep(0.5, 1.0, TexCoords.y); // Assume cap is in upper region
    if (capMask > 0.0) {
        float NdotL = dot(norm, -lightDir);
        sss = pow(max(0.0, NdotL), 8.0) * capTranslucency * capMask;
        sss += pow(max(0.0, dot(viewDir, -lightDir)), 2.0) * capTranslucency * 0.5;
    }

    // Rim lighting effect for edges
    float rim = 1.0 - max(dot(viewDir, norm), 0.0);
    rim = smoothstep(0.5, 1.0, rim);
    vec3 rimEffect = light.diffuse * rim * rimLight;

    // Create realistic mushroom surface variation
    float noise = fract(sin(dot(TexCoords, vec2(12.9898, 78.233))) * 43758.5453);
    float surfaceVariation = noise * 0.05 + 1.0;

    // Final color calculation
    vec3 result = (ambient + diffuse) * surfaceVariation + specular + rimEffect;

    // Add subsurface scattering
    vec3 ssColor = texColor.rgb * light.diffuse * sss * 2.0;
    result += ssColor;

    // Output final color
    FragColor = vec4(result, texColor.a);
}