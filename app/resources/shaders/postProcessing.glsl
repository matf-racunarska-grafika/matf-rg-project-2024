//#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
    TexCoords = aTexCoords;
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int effectType = 0;

const float offset = 1.0 / 300.0;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;

    if (effectType == 1) {
        // Grayscale
        float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        FragColor = vec4(average, average, average, 1.0);
    } else if (effectType == 2) {
        // Negative
        FragColor = vec4(vec3(1.0 - color), 1.0);
    } else if (effectType == 3) {
        // Black & White
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        float bw = step(0.125, gray);
        FragColor = vec4(vec3(bw), 1.0);
    } else if (effectType == 4) {
        // Deepfried
        vec2 offset[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0f, offset),
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(offset, 0.0f),
        vec2(-offset, -offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
        );

        float kernel[9] = float[](
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1
        );

        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++) {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offset[i]));
        }
        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

        FragColor = vec4(col, 1.0);
    } else if (effectType == 5) {
        vec2 offset[9] = vec2[](
            vec2(-offset, offset),
            vec2(0.0f, offset),
            vec2(offset, offset),
            vec2(-offset, 0.0f),
            vec2(0.0f, 0.0f),
            vec2(offset, 0.0f),
            vec2(-offset, -offset),
            vec2(0.0f, -offset),
            vec2(offset, -offset)
        );

        float kernel[9] = float[](
            1,  1, 1,
            1, -8, 1,
            1,  1, 1
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++) {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offset[i]));
        }
        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

        FragColor = vec4(col, 1.0);
    } else {
        // Original
        FragColor = vec4(color, 1.0);
    }
}