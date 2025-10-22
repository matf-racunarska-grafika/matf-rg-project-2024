//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    TexCoords = aTexCoords;
    FragPos   = vec3(model * vec4(aPos, 1.0));
    Normal    = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


#version 330 core
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;


uniform vec3  uColorTint   = vec3(1.0);
uniform float uUseTexture  = 1.0;


uniform vec3  uViewPos;


uniform float uAmbient           = 0.15;
uniform float uSpecularStrength  = 0.25;
uniform float uShininess         = 32.0;


uniform vec3  uDirLightDir;
uniform vec3  uDirLightColor;


uniform vec3  uLightPos;
uniform vec3  uLightColor;


uniform vec3  uLanternPos;
uniform vec3  uLanternColor;
uniform float uLanternOn;

vec4 sampleDiffuse()
{
    if (uUseTexture < 0.5) return vec4(1.0);
    vec4 c = texture(texture_diffuse1, TexCoords);
    if (c.a == 0.0 && c.rgb == vec3(0.0)) c = texture(texture_diffuse2, TexCoords);
    if (c.a == 0.0 && c.rgb == vec3(0.0)) c = texture(texture_diffuse3, TexCoords);
    return c;
}

void main()
{
    vec3 base = sampleDiffuse().rgb * uColorTint;

    vec3 N = normalize(Normal);
    vec3 V = normalize(uViewPos - FragPos);


    vec3 Ld = normalize(-uDirLightDir);
    float NdotLd = max(dot(N, Ld), 0.0);
    vec3 diffuseD = uDirLightColor * NdotLd;
    vec3 R_d = reflect(-Ld, N);
    vec3 specD = uDirLightColor * pow(max(dot(V, R_d), 0.0), uShininess) * uSpecularStrength;


    vec3 Lp = normalize(uLightPos - FragPos);
    float NdotLp = max(dot(N, Lp), 0.0);
    float distP = length(uLightPos - FragPos);
    float attP  = 1.0 / (1.0 + 0.22*distP + 0.20*distP*distP);
    vec3 diffuseP = uLightColor * NdotLp * attP;
    vec3 R_p = reflect(-Lp, N);
    vec3 specP = uLightColor * pow(max(dot(V, R_p), 0.0), uShininess) * uSpecularStrength * attP;


    vec3 Ll = normalize(uLanternPos - FragPos);
    float NdotLl = max(dot(N, Ll), 0.0);
    float distL = length(uLanternPos - FragPos);
    float attL  = 1.0 / (1.0 + 0.22*distL + 0.20*distL*distL);
    vec3 diffuseL = uLanternColor * NdotLl * attL * uLanternOn;
    vec3 R_l = reflect(-Ll, N);
    vec3 specL = uLanternColor * pow(max(dot(V, R_l), 0.0), uShininess) * uSpecularStrength * attL * uLanternOn;

    vec3 ambient = base * uAmbient;
    vec3 color = base * (diffuseD + diffuseP + diffuseL) + (specD + specP + specL) + ambient;

    FragColor = vec4(color, 1.0);
}
