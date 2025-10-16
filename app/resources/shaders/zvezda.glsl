//#shader vertex
#version 330 core

layout (location=0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

    FragPos = vec3(model * vec4(aPos,1.0));
    gl_Position = projection * view * vec4(FragPos,1.0);
}

//#shader fragment

#version 330 core

out vec4 FragColor;
uniform vec3 emissiveColor;

void main(){
    FragColor = vec4(emissiveColor,1.0);
}

