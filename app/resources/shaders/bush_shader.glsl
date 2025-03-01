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
uniform float time; // For wind animation
uniform vec3 windDirection = vec3(1.0, 0.0, 0.0); // Default wind direction
uniform float windStrength = 0.1; // Wind strength multiplier

void main()
{
    // Apply wind effect to bushes - only affects the top parts
    vec3 windOffset = vec3(0.0);
    // Only apply wind to vertices above a certain height threshold (assuming y is up)
    if(aPos.y > 0.3) {
        // Simple sine wave animation based on time and position
        float windFactor = sin(time + aPos.x * 0.5) * windStrength;
        windFactor *= (aPos.y - 0.3) / 0.7; // Gradual increase of wind effect with height
        windOffset = normalize(windDirection) * windFactor;
    }

    // Apply modified position with wind
    vec3 posWithWind = aPos + windOffset;

    FragPos = vec3(model * vec4(posWithWind, 1.0));
    // Normals might need adjustment for very strong wind, but for subtle movement this is fine
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
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
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float alphaThreshold = 0.5; // Adjustable alpha threshold
uniform bool useSubsurfaceScattering = true; // Toggle for subsurface scattering
uniform vec3 subsurfaceColor = vec3(0.2, 0.5, 0.1); // Default green tint for subsurface

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    // Alpha testing with adjustable threshold
    if (texColor.a < alphaThreshold)
    discard;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    // Base ambient lighting
    vec3 ambient = light.ambient * texColor.rgb;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor.rgb;

    if (useSubsurfaceScattering) {
        float backLightFactor = max(0.0, -dot(norm, lightDir));
        backLightFactor = pow(backLightFactor, 2.0) * 0.5; // Control the intensity

        diffuse += backLightFactor * light.diffuse * subsurfaceColor * texColor.rgb;
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float modifiedShininess = material.shininess * 0.5;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), modifiedShininess);

    vec3 specular = light.specular * spec * material.specular * 0.3;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, texColor.a);
}