#version 330 core
in vec3 vPos;       // view space position
in vec2 vTexCoord;
in vec3 vSunPos;    // view space sun position
in vec3 vMoonPos;   // view space moon position
in vec3 vEarthPos;  // view space earth position
in mat3 vTBN;       // view space TBN matrix

uniform mat4 view;
uniform sampler2D nightTex;
uniform sampler2D cloudTex;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalMapTex;

uniform vec3 sunColor;
uniform float sunRadius;
uniform float moonRadius;
uniform float earthRadius;
uniform float bumpiness;
uniform float atmosphereThickness;
uniform int lightSamples;
uniform int scatterViewSamples;
uniform int scatterLightSamples;

out vec4 FragColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 randomSpherePoint(vec2 rand) 
{
    float ang1 = rand.x * 2 * PI; // [0..1) -> [0..2*PI)
    float u = rand.y * 2 - 1; // [0..1), cos and acos(2v-1) cancel each other out, so we arrive at [-1..1)
    float u2 = u * u;
    float sqrt1MinusU2 = sqrt(1.0 - u2);
    float x = sqrt1MinusU2 * cos(ang1);
    float y = sqrt1MinusU2 * sin(ang1);
    float z = u;
    return vec3(x, y, z);
}

float rand(vec2 c, float i) {
    return fract(sin(dot(c, i + vec2(12.9898, 78.2334))) * 43758.5453);
}

float intersectSphere(vec3 o, vec3 d, vec3 c, float r)
{
    vec3 op = c - o;
    float b = dot(op, d);
    float delta = b * b - dot(op, op) + r * r;

    if (delta < 0.0)
        return 0.0; 		        
    else
        delta = sqrt(delta);
    
    float t;
    if ((t = b - delta) > 1e-4)
        return t;
    else if ((t = b + delta) > 1e-4)
        return t;
    else
        return 0.0;
}

void main()
{
    float radiusScale = length(mat3(view) * vec3(1)) / length(vec3(1));
    float sunR = sunRadius * radiusScale;
    float moonR = moonRadius * radiusScale;
    float earthR = earthRadius * radiusScale;
    float atmosphereR = atmosphereThickness * radiusScale;

    vec3 night = texture(nightTex, vTexCoord).rgb;
    vec3 cloud = texture(cloudTex, vTexCoord).rgb;
    vec3 albedo = texture(diffuseTex, vTexCoord).rgb + cloud;
    float kSpecular = texture(specularTex, vTexCoord).r;
    float metallic = mix(0.2, 0.95, kSpecular);
    float roughness = mix(0.8, 0.4, kSpecular - cloud.r);

    vec3 normal = texture(normalMapTex, vTexCoord).rgb;
    normal = normalize(2.0 * normal - 1.0); // Map [0,1] to [-1,1]
    normal *= vec3(bumpiness, bumpiness, 1);
    vec3 N = normalize(vTBN * normal);
    vec3 V = normalize(-vPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lightSamples; i++) {
        vec2 rand = vec2(rand(vTexCoord, float(i)), rand(-vTexCoord, float(-i)));
        vec3 offset = randomSpherePoint(rand) * sunR;
        vec3 lightPos = vSunPos + mat3(view) * offset;

        vec3 lightDir = lightPos - vPos;
        vec3 L = normalize(lightDir);
        float NdotL = max(dot(N, L), 0.0);

        if (intersectSphere(vPos, L, vMoonPos, moonR) == 0.0) {
            float lightDist = length(lightDir);
            vec3 H = normalize(V + L);

            // calculate per-light radiance
            vec3 Li = sunColor / (lightDist * lightDist);
    
            // cook-torrance brdf
            float NDF = DistributionGGX(N, H, roughness);
            float G   = GeometrySmith(N, V, L, roughness);
            vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;
    
            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
            vec3 specular     = numerator / max(denominator, 0.001);

            // add to outgoing radiance Lo
            Lo += (kD * albedo / PI + specular) * NdotL * Li;
        }

        float emission = 1.0 - smoothstep(0, 0.1, NdotL);
        Lo += night * mix(1, 0.5, cloud.r) * emission;
    }
    Lo *= 1.0 / lightSamples;

    // rayleigh scattering equation
    vec3 pDir = normalize(vPos - vEarthPos);
    vec3 P = vEarthPos + pDir * earthR;
    vec3 L = normalize(vSunPos - P);

    float viewScatterDist = intersectSphere(P, V, vEarthPos, earthR + atmosphereR);
    float viewStep = viewScatterDist / scatterViewSamples;
    float viewOpticalDepth = 0.0;

    const vec3 scatterCoef = vec3(0.0519673, 0.121497, 0.296453);
    vec3 Lp = vec3(0.0);

    for (int i = scatterViewSamples; i >= 1; i--) {
        vec3 p = P + viewStep * float(i) * V;
        float hv = distance(p, vEarthPos) - earthR;
        float opticalDepthSegment = exp(-hv / atmosphereR) * viewStep;
        viewOpticalDepth += opticalDepthSegment;

        if (intersectSphere(p, L, vEarthPos, earthR) == 0.0) {
            float lightScatterDist = intersectSphere(p, L, vEarthPos, earthR + atmosphereR);
            float lightStep = lightScatterDist / scatterLightSamples;
            float lightOpticalDepth = 0.0;

            for (int i = 0; i < scatterLightSamples; i++) {
                vec3 q = p + lightStep * float(i) * L;
                float hl = distance(q, vEarthPos) - earthR;
                lightOpticalDepth += exp(-hl / atmosphereR) * lightStep;
            }

            float opticalDepth = viewOpticalDepth + lightOpticalDepth;
            vec3 transmittance = exp(-scatterCoef * opticalDepth);

            Lp += transmittance * opticalDepthSegment;
        }
    }

    float LoOpticalDepth = viewOpticalDepth + viewStep;
    float lightScatterDist = intersectSphere(P, L, vEarthPos, earthR + atmosphereR);
    float lightStep = lightScatterDist / scatterLightSamples;
    for (int i = 0; i < scatterLightSamples; i++) {
        vec3 q = P + lightStep * float(i) * L;
        float hl = distance(q, vEarthPos) - earthR;
        if (hl < 0.0)
            break;
        LoOpticalDepth += exp(-hl / atmosphereR) * lightStep;
    }
    
    float lightDist = length(vSunPos - P) - atmosphereR;
    vec3 Is = sunColor / (lightDist * lightDist);
    float cosTheta = dot(V, L);
    float phase = 3.0 / (16.0 * PI) * (1.0 + cosTheta * cosTheta);
    Lp = pow(Lp, vec3(2.2)) * 100;  // strength tweak
    vec3 Ls = Is * scatterCoef * phase * Lp;
    vec3 LoTransmittance = exp(-scatterCoef * LoOpticalDepth);

    vec3 color = Lo * LoTransmittance + Ls;

    FragColor = vec4(color, 1.0);
} 