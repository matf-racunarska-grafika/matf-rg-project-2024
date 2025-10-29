//#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, TexCoords).rgb * 0.227027;

    if (horizontal) {
        result += texture(image, TexCoords + vec2(tex_offset.x * 1.0, 0.0)).rgb * 0.1945946;
        result += texture(image, TexCoords - vec2(tex_offset.x * 1.0, 0.0)).rgb * 0.1945946;
        result += texture(image, TexCoords + vec2(tex_offset.x * 2.0, 0.0)).rgb * 0.1216216;
        result += texture(image, TexCoords - vec2(tex_offset.x * 2.0, 0.0)).rgb * 0.1216216;
        result += texture(image, TexCoords + vec2(tex_offset.x * 3.0, 0.0)).rgb * 0.054054;
        result += texture(image, TexCoords - vec2(tex_offset.x * 3.0, 0.0)).rgb * 0.054054;
        result += texture(image, TexCoords + vec2(tex_offset.x * 4.0, 0.0)).rgb * 0.016216;
        result += texture(image, TexCoords - vec2(tex_offset.x * 4.0, 0.0)).rgb * 0.016216;
    } else {
        result += texture(image, TexCoords + vec2(0.0, tex_offset.y * 1.0)).rgb * 0.1945946;
        result += texture(image, TexCoords - vec2(0.0, tex_offset.y * 1.0)).rgb * 0.1945946;
        result += texture(image, TexCoords + vec2(0.0, tex_offset.y * 2.0)).rgb * 0.1216216;
        result += texture(image, TexCoords - vec2(0.0, tex_offset.y * 2.0)).rgb * 0.1216216;
        result += texture(image, TexCoords + vec2(0.0, tex_offset.y * 3.0)).rgb * 0.054054;
        result += texture(image, TexCoords - vec2(0.0, tex_offset.y * 3.0)).rgb * 0.054054;
        result += texture(image, TexCoords + vec2(0.0, tex_offset.y * 4.0)).rgb * 0.016216;
        result += texture(image, TexCoords - vec2(0.0, tex_offset.y * 4.0)).rgb * 0.016216;
    }

    FragColor = vec4(result, 1.0);
}