#version 330 core
layout (location = 0) in vec2 vertexCoord;
layout (location = 1) in vec2 uvCoord;

out vec2 f_uvCoord;
uniform mat4 modelMatrix, cameraMatrix;

void main()
{
    gl_Position = cameraMatrix * modelMatrix * vec4(vertexCoord, 0.0f, 1.0f);
    f_uvCoord = uvCoord;
}
