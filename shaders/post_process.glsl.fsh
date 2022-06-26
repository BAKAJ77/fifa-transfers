#version 330 core

in vec2 f_uvCoord;

uniform sampler2DMS multisampleSceneTexture;
uniform int numSamplesPerPixel;
uniform float gamma;

// Returns the sampled color from a multisample texel.
vec4 GetMultisamplePixelColor()
{
    vec4 sampledPixelColor;
    for (int i = 0; i < numSamplesPerPixel; i++)
        sampledPixelColor += texelFetch(multisampleSceneTexture, ivec2(gl_FragCoord.xy), i);

    return sampledPixelColor / numSamplesPerPixel;
}

// Returns gamma corrected color.
vec4 ApplyGammaCorrection(vec4 pixelColor)
{
    return vec4(pow(pixelColor.rgb, vec3(1.0f / gamma)), pixelColor.a);
}

void main()
{
    vec4 sampledFragColor = GetMultisamplePixelColor();
    gl_FragColor = ApplyGammaCorrection(sampledFragColor);
}
