#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoords;

void main()
{
    gl_Position =vec4(vec3(position.x,position.y,0), 1.0f);
    TexCoords = texCoords;
}  