//#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

//#shader fragment

#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform sampler2D bloom;

uniform float intensity;

void main()
{
    float gamma = 2.2;

    vec3 imageColor = texture(image, TexCoords).rgb;
    vec3 bloomColor = texture(bloom, TexCoords).rgb;
    imageColor += bloomColor;

    vec3 result = vec3(1.0) - exp(-imageColor * intensity);
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}