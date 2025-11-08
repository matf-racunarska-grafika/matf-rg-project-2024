//#shader vertex
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform int mode; // 0 = bright pass, 1 = blur, 2 = composite
uniform sampler2D sceneTexture;
uniform sampler2D brightTexture;
uniform sampler2D bloomTexture;

uniform float threshold;
uniform bool horizontal;
uniform float bloomIntensity;
uniform float exposure;

// gaussian blur weights
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    if (mode == 0) {
        vec3 color = texture(sceneTexture, TexCoords).rgb;
        float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

        float softThreshold = threshold * 0.5;
        float knee = threshold * 0.5;

        float contribution = smoothstep(softThreshold - knee, softThreshold + knee, brightness);

        vec3 brightColor = color * contribution * max(0.0, (brightness - threshold) / brightness);

        FragColor = vec4(brightColor, 1.0);
    }
    else if (mode == 1) {
        vec2 tex_offset = 1.0 / textureSize(brightTexture, 0);
        vec3 result = texture(brightTexture, TexCoords).rgb * weight[0];

        if(horizontal) {
            for(int i = 1; i < 5; ++i) {
                result += texture(brightTexture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
                result += texture(brightTexture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            }
        } else {
            for(int i = 1; i < 5; ++i) {
                result += texture(brightTexture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
                result += texture(brightTexture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            }
        }

        FragColor = vec4(result, 1.0);
    }
    else if (mode == 2) {
        vec3 sceneColor = texture(sceneTexture, TexCoords).rgb;
        vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

        vec3 result = sceneColor + bloomColor * bloomIntensity;

        FragColor = vec4(result, 1.0);
    }
    else if (mode == 3) {
        vec3 color = texture(sceneTexture, TexCoords).rgb;

        FragColor = vec4(color, 1.0);
    }
}