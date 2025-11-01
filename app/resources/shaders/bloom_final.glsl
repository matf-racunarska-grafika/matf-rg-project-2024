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

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;
uniform float bloomStrength;

void main()
{
    const float gammaVal = 1.25;

    vec3 base = texture(scene, uv).rgb;
    vec3 glow = texture(bloomBlur, uv).rgb;

    if (bloom)
        base += glow * bloomStrength;

    vec3 toneMapped = vec3(1.0) - exp(-base * exposure);
    toneMapped = pow(toneMapped, vec3(1.0 / gammaVal));

    FragColor = vec4(toneMapped, 1.0);
}

