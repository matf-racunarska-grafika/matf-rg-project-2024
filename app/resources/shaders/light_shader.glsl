
//#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}

//#shader fragment

#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;

struct LightData {
    vec3 position;
    vec3 direction;

    float cutoff;
    float outercutoff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform LightData light;

void main()
{
    FragColor = vec4(light.diffuse, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
    else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}