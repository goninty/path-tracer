#pragma once
#include <string>

class ComputeShader {
public:
	ComputeShader(const char* csPath);
	~ComputeShader();

	//void assignToUniform(const char* uniformName, glm)
	int getUniformLocation(const char* uniformName);
	void bind();
	//void update(float r, float g, float b, float a);

private:
	unsigned int shaderProgramId = 0;

	std::string loadShader(const char* filePath);
	void createShader(const char* csSrc);
};