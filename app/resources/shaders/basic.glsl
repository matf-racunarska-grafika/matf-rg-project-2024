//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


layout (location = 8)  in vec4 aInstRow0;
layout (location = 9)  in vec4 aInstRow1;
layout (location = 10) in vec4 aInstRow2;
layout (location = 11) in vec4 aInstRow3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool uInstanced = false;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    TexCoords = aTexCoords;

    mat4 instanceModel = mat4(aInstRow0, aInstRow1, aInstRow2, aInstRow3);
    mat4 modelM = uInstanced ? instanceModel : model;

    FragPos = vec3(modelM * vec4(aPos, 1.0));
    Normal  = mat3(transpose(inverse(modelM))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


//#shader fragment
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


uniform float uExposure          = 1.0;// tonemap
uniform float uBloomFakeStrength = 1.4;// jačina glow halo-a
uniform float uGlowFalloffFire   = 1.6;// koliko brzo opada oko vatre
uniform float uGlowFalloffLamp   = 2.2;// koliko brzo opada oko fenjera

vec4 sampleDiffuse()
{
    if (uUseTexture < 0.5) return vec4(uColorTint, 1.0);

    vec4 texColor = texture(texture_diffuse1, TexCoords);


    if (texColor.a < 0.1 && length(texColor.rgb) < 0.1) {
        return vec4(uColorTint, 1.0);
    }

    return texColor * vec4(uColorTint, 1.0);
}


float glowFalloff(vec3 P, vec3 L, float falloff)
{
    float d = length(P - L);
    float g = exp(- (falloff * d) * (falloff * d));
    return g;
}

void main()
{
    vec3 base = sampleDiffuse().rgb;

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


    vec3 lighting = base * (diffuseD + diffuseP + diffuseL) + (specD + specP + specL) + base * uAmbient;


    float glowFire   = glowFalloff(FragPos, uLightPos, uGlowFalloffFire);
    float glowLamp   = glowFalloff(FragPos, uLanternPos, uGlowFalloffLamp) * uLanternOn;

    vec3 fakeHalo = (uLightColor   * glowFire +
    uLanternColor * glowLamp) * uBloomFakeStrength;

    vec3 color = lighting + fakeHalo;


    color = vec3(1.0) - exp(-color * uExposure);

    FragColor = vec4(color, 1.0);
}