//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0f);
    fragPos = worldPos.xyz;
    normal = aNormal;
    texCoord = aTexCoord;
    gl_Position = projection * view * worldPos;
}

//#shader fragment
#version 330 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texture1;

out vec4 fragColor;

void main() {
    fragColor = texture(texture1, texCoord);
}