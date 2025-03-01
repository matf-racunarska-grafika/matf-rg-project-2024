//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 FragPos;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    TBN = mat3(T, B, N);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform float shininess = 32.0;
uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;

// Light attenuation coefficients
uniform float constant = 1.0;    // Prevents division by zero when distance is very small
uniform float linear = 0.09;     // Linear attenuation factor
uniform float quadratic = 0.032; // Quadratic attenuation factor

void main()
{
    // Sample textures
    vec3 diffuseColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 specularMap = texture(texture_specular1, TexCoords).rgb;

    // Process normal map
    vec3 normalMap = texture(texture_normal1, TexCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0;  // Transform from [0,1] to [-1,1]
    vec3 normal = normalize(TBN * normalMap);

    // Calculate distance to light
    float distance = length(lightPos - FragPos);

    distance = max(distance, 0.1); //
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    attenuation = max(attenuation, 0.01);

    // Ambient (not affected by attenuation)
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * specularMap * lightColor;

    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse) * diffuseColor + specular;

    FragColor = vec4(result, 1.0);
}