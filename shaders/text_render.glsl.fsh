#version 330 core

in vec2 f_uvCoord;
uniform sampler2D fontBitmap;
uniform vec4 diffuseColor;

void main()
{
    gl_FragColor = vec4(diffuseColor.rgb, diffuseColor.a * texture(fontBitmap, f_uvCoord).r);
}
