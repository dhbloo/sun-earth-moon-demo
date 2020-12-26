#version 330 core
in vec2 texCoord;

uniform sampler2D inputTexture;
uniform vec2 resolution;
uniform vec2 blurDir;
uniform float blurSigma;

out vec4 FragColor;

const int mSize = 31;
const int kSize = (mSize - 1) / 2;
float kernel[kSize + 1];
float normCoef;

float normpdf(in float x, in float sigma)
{
	return 0.39894228 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
}

void initKernel(in float sigma)
{
    float Z = 0.0;
    for (int i = 0; i <= kSize; ++i) {
        kernel[i] = normpdf(float(i), sigma);
        Z += kernel[i];
    }
    Z = 2 * Z - kernel[0];
    normCoef = 1.0 / Z;
}

void main()
{
    initKernel(blurSigma);

    vec3 color = vec3(0.0);
    for (int i = -kSize; i <= kSize; ++i) {
        vec2 offset = i * blurDir / resolution;
        vec3 sampledColor = texture(inputTexture, texCoord + offset).rgb;
        color += kernel[abs(i)] * sampledColor;
    }

    FragColor = vec4(color * normCoef, 1.0);
}