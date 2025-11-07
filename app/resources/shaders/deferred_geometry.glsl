//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = (model * vec4(aPos, 1.0)).xyz;
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;
    gl_Position = projection * view * model*vec4(aPos,1.0f);
}

//#shader fragment
#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gEmissiveShine;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_emissive1;

uniform bool is_emissive;
uniform bool is_specular;
uniform int shininess;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gEmissiveShine.a = float(shininess)/128.0f;
    gEmissiveShine.rgb=vec3(0.0);
    gAlbedoSpec.a = 0.0;

    if(is_emissive)
        gEmissiveShine.rgb=texture(texture_emissive1, TexCoords).rgb;
    if(is_specular)
        gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}