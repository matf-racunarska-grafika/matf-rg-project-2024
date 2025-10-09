//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos  = vec3(model * vec4(aPos, 1.0));
    Normal   = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

uniform float ambient;
uniform vec3  lightDir;
uniform vec3  lightColor;

uniform vec3 spotPos1, spotDir1; // far1
uniform vec3 spotPos2, spotDir2; // far2
uniform vec3  spotColor;
uniform float spotInner;
uniform float spotOuter;
uniform float spotConst;
uniform float spotLin;
uniform float spotQuad;


uniform bool  isTruck;
uniform float headGlowRadius;
uniform float headGlowStrength;

uniform vec3  viewPos;
uniform float materialShininess;
uniform float specularStrength;

vec3 spotTerm(vec3 pos, vec3 dir, vec3 N){
    vec3 L_toFragment = normalize(FragPos - pos);
    vec3 L = -L_toFragment;

    float theta = dot(normalize(dir), L_toFragment);
    float eps   = max(spotInner - spotOuter, 1e-5);
    float t     = clamp((theta - spotOuter) / eps, 0.0, 1.0);

    float spotF = pow(t, 4.0);
    float ndotl = max(dot(normalize(N), L), 0.0);
    float dist  = length(FragPos - pos);
    float atten = 1.0 / (spotConst + spotLin * dist + spotQuad * dist * dist);

    return spotColor * ndotl * spotF * atten;

}

void main() {
    vec3 base = texture(texture_diffuse1, TexCoords).rgb;
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 Lm = normalize(-lightDir);
    float diff = max(dot(N, Lm), 0.0);
    vec3 ambientLight = lightColor * diff + vec3(ambient);

    float specDir = 0.0;
    if (diff > 0.0) {
        vec3 Rm = reflect(-Lm, N);
        specDir = pow(max(dot(V, Rm), 0.0), materialShininess);
    }

    vec3 dirSpec = lightColor * (specularStrength * specDir);

    vec3 s1 = spotTerm(spotPos1, spotDir1, N);
    vec3 s2 = spotTerm(spotPos2, spotDir2, N);

    // mora odvojeno za oba fara spekularna komponenta
    vec3  L1 = normalize(spotPos1 - FragPos);
    float th1 = dot(normalize(spotDir1), normalize(FragPos - spotPos1));
    float eps1 = max(spotInner - spotOuter, 1e-5);
    float t1 = clamp((th1 - spotOuter) / eps1, 0.0, 1.0);
    float spotF1 = pow(t1, 4.0);
    float d1 = length(FragPos - spotPos1);
    float att1  = 1.0 / (spotConst + spotLin * d1 + spotQuad * d1 * d1);

    //isto kao gore
    float spec1 = 0.0;
    if (dot(N, L1) > 0.0) {
        vec3 R1 = reflect(-L1, N);
        spec1 = pow(max(dot(V, R1), 0.0), materialShininess);
    }
    vec3 spotSpec1 = spotColor * (specularStrength * spec1) * spotF1 * att1;

    vec3  L2 = normalize(spotPos2 - FragPos);
    float th2 = dot(normalize(spotDir2), normalize(FragPos - spotPos2));
    float eps2 = max(spotInner - spotOuter, 1e-5);
    float t2 = clamp((th2 - spotOuter) / eps2, 0.0, 1.0);
    float spotF2 = pow(t2, 4.0);
    float d2 = length(FragPos - spotPos2);
    float att2 = 1.0 / (spotConst + spotLin * d2 + spotQuad * d2 * d2);

    float spec2 = 0.0;
    if (dot(N, L2) > 0.0) {
        vec3 R2 = reflect(-L2, N);
        spec2 = pow(max(dot(V, R2), 0.0), materialShininess);
    }
    vec3 spotSpec2 = spotColor * (specularStrength * spec2) * spotF2 * att2;

    float emissiveGlow = 0.0;
    if(isTruck){
        float d1 = length(FragPos - spotPos1);
        float d2 = length(FragPos - spotPos2);

        float g1 = exp(- (d1*d1) / (2.0 * headGlowRadius * headGlowRadius));
        float g2 = exp(- (d2*d2) / (2.0 * headGlowRadius * headGlowRadius));
        emissiveGlow = headGlowStrength * (g1 + g2);
    }

    vec3 color = base * (ambientLight + s1 + s2) + dirSpec + spotSpec1 + spotSpec2 + spotColor * emissiveGlow;
    FragColor = vec4(color, 1.0);
}