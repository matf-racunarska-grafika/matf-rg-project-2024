//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

//#shader fragment
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform float treshold;

void main()
{
    FragColor = texture(skybox, TexCoords)*0.08f;
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness*15 > treshold)
    BrightColor = 15*vec4(FragColor.rgb, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}