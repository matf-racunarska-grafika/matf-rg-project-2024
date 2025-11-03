//#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}

//#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D fboTexture;

// 0 = normalno, 1 = grayscale, 2 = invert
uniform int effectType;

void main() {
    vec3 color = texture(fboTexture, TexCoord).rgb;

    if (effectType == 1) {
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        color = vec3(gray);
    }
    else if (effectType == 2) {
        color = vec3(1.0) - color;
    }

    FragColor = vec4(color, 1.0);
}
