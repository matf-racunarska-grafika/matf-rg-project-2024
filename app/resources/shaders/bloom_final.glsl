//#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

//#shader fragment
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float bloomIntensity;

void main() {
    const float  gamma= 1.5;
    vec3 hdr   = texture(scene,    TexCoords).rgb;
    vec3 bloom = texture(bloomBlur, TexCoords).rgb;
    hdr += bloom;
    vec3 color = vec3(1.0) - exp(-hdr* bloomIntensity);
    color = pow(color, vec3(1.0/gamma));
    FragColor = vec4(color, 1.0);
}

