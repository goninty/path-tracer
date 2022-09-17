#version 460 core

out vec4 fragColor;

in vec2 textureCoordinate;

uniform sampler2D sTexture;

void main()
{
	fragColor = texture(sTexture, textureCoordinate);
	//fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}