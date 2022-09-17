#version 460 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 texCoord;

out vec2 textureCoordinate;

void main()
{
	gl_Position = pos;
	textureCoordinate = texCoord;
}