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

void main()
{
    float gray = dot(texture(screenTexture, TexCoords).rgb, vec3(0.299, 0.587, 0.114));

    float bw = step(0.11f, gray);

    FragColor = vec4(vec3(bw), 1.0);
}