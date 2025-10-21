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

layout (location=0) out vec4 FragColor;
layout (location=1) out vec4 BrightColor;
uniform vec3 emissiveColor;

void main(){
    FragColor = vec4(emissiveColor,1.0);
    //bloom deo
    float brightness = dot(emissiveColor,vec3(0.2126,0.7152,0.0722));
    if(brightness>1.0){
        BrightColor = vec4(emissiveColor,1.0);
    }else{
        BrightColor = vec4(0.0,0.0,0.0,1.0);
    }
}

