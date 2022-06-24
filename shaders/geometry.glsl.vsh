#version 330 core

layout (location = 0) in vec2 vertexCoord;
uniform mat4 modelMatrix, cameraMatrix;

void main()
{
    gl_Position = cameraMatrix * modelMatrix * vec4(vertexCoord, 0.0f, 1.0f);
}
