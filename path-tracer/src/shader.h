#pragma once

#include <string>

class Shader 
{
public:
	Shader(const char* vsPath, const char* fsPath);
	~Shader();

	void bind();
	void update(float r, float g, float b, float a);

private:
	unsigned int shaderProgramId = 0;

	std::string loadShader(const char* filePath);
	void createShader(const char* vertexSrc, const char* fragmentSrc);
};