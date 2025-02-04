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

struct Material {
// Specular color of the model.
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

// You can either set these as uniforms from your code,
// or assign constant values here if they’re fixed.
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

void main()
{
    // Normalize input normals
    vec3 norm = normalize(Normal);
    // Calculate the direction from the fragment to the light source
    vec3 lightDir = normalize(light.position - FragPos);

    // Ambient component: base color modulated by the ambient light
    vec3 ambient = light.ambient * vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0).xyz;

    // Diffuse component: Lambert’s cosine law
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0).xyz;

    // Specular component: using Blinn-Phong or Phong reflection model.
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
