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

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform Light light;

void main()
{
    // Normalize the input normal vector.
    vec3 norm = normalize(Normal);

    // Calculate the light direction.
    vec3 lightDir = normalize(light.position - FragPos);

    // Ambient lighting.
    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;

    // Diffuse lighting.
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    // Combine the lighting contributions.
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
