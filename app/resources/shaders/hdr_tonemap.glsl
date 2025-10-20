//#shader vertex
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main(){
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos,0.0,1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
//uniform za bloom
uniform float exposure;

void main(){
    const float gamma = 2.2f;
    vec3 hdrColor = texture(hdrBuffer,TexCoords).rgb;
    //bloom dodati
    vec3 result = vec3(1.0) - exp(-hdrColor*exposure);
    result = pow(result,vec3(1.0/gamma));
    FragColor = vec4(result,1.0);
}