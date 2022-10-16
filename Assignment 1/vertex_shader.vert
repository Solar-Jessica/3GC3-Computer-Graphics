#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 Color;

uniform mat4 transform;

out vec4 vColor;

void main()
{
    vColor = Color;
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}