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
out vec4 FragColor;
in  vec2 TexCoords;

uniform sampler2D image;
uniform bool  horizontal;

const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2  tex_off = 1.0 / vec2(textureSize(image, 0));
    vec3  col     = texture(image, TexCoords).rgb * weight[0];
    for(int i = 1; i < 5; ++i) {
        if(horizontal) {
            col += texture(image, TexCoords + vec2(tex_off.x * i, 0.0)).rgb * weight[i];
            col += texture(image, TexCoords - vec2(tex_off.x * i, 0.0)).rgb * weight[i];
        } else {
            col += texture(image, TexCoords + vec2(0.0, tex_off.y * i)).rgb * weight[i];
            col += texture(image, TexCoords - vec2(0.0, tex_off.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(col, 1.0);
}

