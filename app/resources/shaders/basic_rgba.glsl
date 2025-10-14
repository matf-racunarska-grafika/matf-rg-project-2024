//#shader vertex
#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

    FragPos = vec3(model * vec4(aPos,1.0));
    Normal = aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos,1.0);
}

//#shader fragment

#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main(){
    vec4 texColor = vec4(texture(texture_diffuse1,TexCoords));
    if(texColor.a<0.9){
        discard;
    }
    FragColor = texColor;
}

