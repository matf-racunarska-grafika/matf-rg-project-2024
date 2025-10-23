//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sphereTexture;
uniform vec3 sphereColor;

void main() {
    //     FragColor = vec4(0.3f, 0.2f, 0.5f, 1.0f);
    FragColor = texture(sphereTexture, TexCoords) * vec4(sphereColor, 1.0f); //* vec4(1.0f, 0.0f, 0.0f, 1.0f);
}