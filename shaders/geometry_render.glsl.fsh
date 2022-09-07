#version 330 core

in vec2 f_uvCoord;

uniform vec4 diffuseColor;
uniform sampler2D diffuseTexture;
uniform bool useDiffuseTexture;
uniform float gamma;

vec3 ApplyGammaCorrection(vec3 color)
{
    return pow(color, vec3(1.0f / gamma));
}

void main()
{
    vec4 fragColor;
    if (useDiffuseTexture)
    {
        vec4 texelColor = texture(diffuseTexture, f_uvCoord);
        fragColor = vec4(ApplyGammaCorrection(texelColor.rgb), texelColor.a) * diffuseColor;
    }
    else
        fragColor = diffuseColor;

    gl_FragColor = fragColor;
}
