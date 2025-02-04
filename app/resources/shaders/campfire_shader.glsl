//#shader vertex
#version 330 core

// Input vertex attributes (adjust locations as needed)
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent; // required for normal mapping

// Uniform transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Outputs to the fragment shader
out vec2 TexCoord;
out vec3 FragPos;
out mat3 TBN; // Tangent, Bitangent, Normal matrix

void main()
{
    // Calculate the world-space position of the fragment.
    FragPos = vec3(model * vec4(aPosition, 1.0));
    TexCoord = aTexCoord;

    // Transform the normal and tangent vectors to world space.
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    // Re-compute bitangent (could also be provided as a vertex attribute)
    vec3 B = normalize(cross(N, T));

    // Build TBN matrix to transform normal from tangent space to world space.
    TBN = mat3(T, B, N);

    // Final vertex position in clip space.
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D baseTexture;
uniform sampler2D normalMap;
uniform sampler2D occlusionRoughMetalMap;
uniform sampler2D emissiveMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec4 baseColor = texture(baseTexture, TexCoord);

    // Sample and unpack the normal map from [0, 1] to [-1, 1].
    vec3 normalTex = texture(normalMap, TexCoord).rgb;
    normalTex = normalize(normalTex * 2.0 - 1.0);
    // Transform the normal from tangent space to world space.
    vec3 normal = normalize(TBN * normalTex);
    vec3 emissive = texture(emissiveMap, TexCoord).rgb;

    // --- Simple Phong Lighting ---
    // Calculate light direction.
    vec3 lightDir = normalize(lightPos - FragPos);
    // Diffuse shading.
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading.
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 ambient = 0.1 * baseColor.rgb;         // Ambient term
    vec3 diffuse = diff * baseColor.rgb;          // Diffuse term
    vec3 specular = spec * vec3(0.5);             // Specular term (adjust specular color/intensity as needed)

    vec3 finalColor = ambient + diffuse + specular + emissive;

    FragColor = vec4(finalColor, baseColor.a);
}