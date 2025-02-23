//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;  // <-- Add this

out vec2 TexCoords;

void main() {
    // Apply the model matrix to the vertex position
    gl_Position = model * vec4(aPos, 1.0f);
    TexCoords = aTexCoords;
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D heartTexture;

void main() {
    vec4 color = texture(heartTexture, TexCoords);
    if (color.a < 0.1) discard;  // Discard transparent pixels
    FragColor = color;
}