//#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 uv;

void main()
{
    uv = texCoord;
    gl_Position = vec4(position, 1.0);
}


//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D bloomBlur;
uniform bool horizontal;

const float kernel[5] = float[](
    0.227027, 0.194595, 0.121622, 0.054054, 0.016216
);

void main()
{
    vec2 texel = 1.0 / vec2(textureSize(bloomBlur, 0));
    vec3 color = texture(bloomBlur, uv).rgb * kernel[0];

    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            float offset = texel.x * float(i);
            color += texture(bloomBlur, uv + vec2(offset, 0.0)).rgb * kernel[i];
            color += texture(bloomBlur, uv - vec2(offset, 0.0)).rgb * kernel[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            float offset = texel.y * float(i);
            color += texture(bloomBlur, uv + vec2(0.0, offset)).rgb * kernel[i];
            color += texture(bloomBlur, uv - vec2(0.0, offset)).rgb * kernel[i];
        }
    }

    FragColor = vec4(color, 1.0);
}

