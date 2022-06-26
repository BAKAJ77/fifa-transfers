#version 330 core
layout (location = 0) in vec2 vertexCoord;
layout (location = 1) in vec2 uvCoord;

out vec2 f_uvCoord;

void main()
{
    gl_Position = vec4(vertexCoord * 2.0f, 0.0f, 1.0f);
    f_uvCoord = uvCoord;
}
