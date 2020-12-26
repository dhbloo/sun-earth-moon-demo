#version 330 core
in vec2 vTexCoord;
in vec3 vPos;       // view space position
in vec3 vNormal;    // view space normal

uniform sampler2D emissionTex;
uniform vec3 fresnelEmission;
uniform float time;
uniform float brightness;

out vec4 FragColor;

float hash(in vec2 p, in float scale)
{
	// This is tiling part, adjusts with the scale...
	p = mod(p, scale);
	return fract(sin(dot(p, vec2(27.16898, 38.90563))) * 5151.5473453);
}

float noise(in vec2 p, in float scale) 
{
	p *= scale;
	
	vec2 f = fract(p);		// Separate integer from fractional
    p = floor(p);
	
    f = f*f*(3.0-2.0*f);	// Cosine interpolation approximation
	
    float res = mix(mix(hash(p, 				 scale),
						hash(p + vec2(1.0, 0.0), scale), f.x),
					mix(hash(p + vec2(0.0, 1.0), scale),
						hash(p + vec2(1.0, 1.0), scale), f.x), f.y);
    return res;
}

float fbm(in vec2 p)
{
	float f = 0.0;
	float scale = 10.;
    p = mod(p, scale);
	float amp   = 0.6;
	
	for (int i = 0; i < 5; i++)
	{
		f += noise(p, scale) * amp;
		amp *= .5;
		// Scale must be multiplied by an integer value...
		scale *= 2.;
	}
	// Clamp it just in case....
	return min(f, 1.0);
}

vec3 fresnel_schlick(vec3 H, vec3 V, vec3 F0, vec3 E)
{
	return F0 + (E - F0) * pow(1.0 - max(dot(H, V), 0.0), 5.0);
}

void main()
{
	vec2 t = vec2(time, -time) * 0.01;
	vec2 coordOffset = vec2(fbm(vTexCoord + t), fbm(vTexCoord - t)) * 0.01;
	vec3 emission = texture(emissionTex, vTexCoord + coordOffset).rgb;

	vec3 v = normalize(-vPos);
	vec3 h = normalize(vNormal);
	vec3 fresnel = fresnel_schlick(h, v, emission, fresnelEmission);

    FragColor = vec4(fresnel * brightness, 1.0);
}