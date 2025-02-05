//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// Texture coordinates are not needed since no texture is used.
// layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Transform vertex position into world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Correctly transform normals in case of non-uniform scaling
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;    // From Ka (ambient reflectivity)
    vec3 diffuse;    // From Kd (diffuse reflectivity)
    vec3 specular;   // From Ks (specular reflectivity)
    float shininess; // From Ns (specular exponent)
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // Normalize the normal vector
    vec3 norm = normalize(Normal);
    // Calculate the direction from the fragment to the light source
    vec3 lightDir = normalize(light.position - FragPos);

    // Ambient component
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    // Specular component
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
