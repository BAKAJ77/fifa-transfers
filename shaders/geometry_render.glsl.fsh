#version 330 core

in vec2 f_uvCoord;

uniform vec4 diffuseColor;
uniform sampler2D diffuseTexture;
uniform bool useDiffuseTexture;
uniform float gamma;

vec3 ApplyGammCorrection(vec3 color)
{
    return pow(color, vec3(1.0f / gamma));
}

void main()
{
    vec4 fragColor;
    if (useDiffuseTexture)
        fragColor = texture(diffuseTexture, f_uvCoord);
    else
        fragColor = diffuseColor;

    gl_FragColor = vec4(ApplyGammCorrection(fragColor.rgb), fragColor.a);
}
