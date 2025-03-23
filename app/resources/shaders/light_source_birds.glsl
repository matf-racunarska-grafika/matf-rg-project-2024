//#shader vertex
#version 330

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
//#shader fragment
#version 330

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main() {
    gl_FragColor = vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0f);
}