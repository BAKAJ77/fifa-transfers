#version 330 core

in vec2 f_uvCoord;
uniform sampler2D fontBitmap;
uniform vec4 diffuseColor;
uniform float gamma;

vec3 ApplyGammCorrection(vec3 color)
{
    return pow(color, vec3(1.0f / gamma));
}

void main()
{
    vec3 gammaCorrectedColor = ApplyGammCorrection(diffuseColor.rgb);
    gl_FragColor = vec4(gammaCorrectedColor, diffuseColor.a * texture(fontBitmap, f_uvCoord).r);
}
