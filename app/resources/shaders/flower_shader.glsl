//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float WindEffect;
out vec3 OriginalPos;

uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform vec3 windDirection = vec3(1.0, 0.0, 0.3);
uniform float windStrength = 0.05;
uniform float windSpeed = 1.0;

void main()
{
    // Store original position for effects calculation
    OriginalPos = aPos;

    // Apply wind effect based on height (affects stems more than flowers)
    float height = aPos.y;
    vec3 adjustedPos = aPos;

    // Wind effect calculation - more effect on taller parts (stems)
    float windFactor = height * windStrength;
    float windNoise = sin(time * windSpeed + instanceMatrix[3][0] * 0.5 + instanceMatrix[3][2] * 0.5);
    vec3 windOffset = normalize(windDirection) * windFactor * windNoise;

    // Apply wind only to parts of the plant above ground level
    if (height > 0.1) {
        adjustedPos += windOffset;
    }

    // Pass wind effect to fragment shader for visual effects
    WindEffect = windNoise * 0.5 + 0.5; // Normalize to 0-1 range

    // Transform position with instance matrix
    FragPos = vec3(instanceMatrix * vec4(adjustedPos, 1.0));
    Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
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
in float WindEffect;
in vec3 OriginalPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;  // Optional: for normal mapping
uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float time;
uniform float brightnessVariation = 0.1;  // Controls color variation
uniform float petalTranslucency = 0.15;   // Controls translucency of petals
uniform float saturationBoost = 0.2;      // Makes colors more vibrant

// Helper function to convert RGB to HSV
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// Helper function to convert HSV to RGB
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    // Sample textures
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    // Early discard for transparent pixels
    if (texColor.a < 0.1)
    discard;

    // Detect if this pixel is part of a petal (assume brighter colors for petals, darker for stems)
    float isPetal = smoothstep(0.3, 0.7, length(texColor.rgb));

    // Apply slight color variation for more natural look
    vec3 hsv = rgb2hsv(texColor.rgb);

    // Enhance saturation for petals
    hsv.y = min(1.0, hsv.y + saturationBoost * isPetal);

    // Slight variation in brightness based on instance position and wind
    float brightnessVar = sin(FragPos.x * 0.1 + FragPos.z * 0.1 + time * 0.2) * brightnessVariation;
    hsv.z += brightnessVar * isPetal; // Only apply to petals

    // Convert back to RGB
    vec3 modifiedColor = hsv2rgb(hsv);

    // Normal lighting calculations
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    // Add subtle normal perturbation for petals to simulate surface microdetail
    if (isPetal > 0.5) {
        float normalNoise = sin(TexCoords.x * 20.0) * cos(TexCoords.y * 20.0) * 0.05;
        norm = normalize(norm + vec3(normalNoise));
    }

    // Basic ambient lighting
    vec3 ambient = light.ambient * modifiedColor;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * modifiedColor;

    // Specular highlighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    // Subsurface scattering effect for petals
    vec3 subsurface = vec3(0.0);
    if (isPetal > 0.5) {
        float sss = pow(max(0.0, dot(viewDir, -lightDir)), 1.0) * petalTranslucency;
        subsurface = modifiedColor * light.diffuse * sss * 2.0;
    }

    // Edge highlight for petals to create more definition
    float rim = 1.0 - max(dot(viewDir, norm), 0.0);
    rim = smoothstep(0.5, 1.0, rim) * isPetal * 0.3;

    // Add slight color variation to stems based on wind
    vec3 stemColor = mix(modifiedColor, modifiedColor * vec3(0.9, 1.1, 0.9), WindEffect * (1.0 - isPetal));

    // Combine all lighting components
    vec3 result = ambient + diffuse + specular + subsurface + (rim * light.diffuse);

    // Apply stem color variation only to non-petal parts
    result = mix(result, stemColor, (1.0 - isPetal) * 0.7);

    // Final color with original alpha
    FragColor = vec4(result, texColor.a);
}