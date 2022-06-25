#version 330 core

in vec2 f_uvCoord;

uniform vec4 diffuseColor;
uniform sampler2D diffuseTexture;
uniform bool useDiffuseTexture;

void main()
{
    if (useDiffuseTexture)
        gl_FragColor = texture(diffuseTexture, f_uvCoord);
    else
        gl_FragColor = diffuseColor;
}
