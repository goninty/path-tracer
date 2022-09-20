#include "ComputeShader.h"

#include <GL/glew.h>

#include <fstream>
#include <string>
#include <sstream>

ComputeShader::ComputeShader(const char* csPath)
{
    std::string csSrc = loadShader(csPath);
    createShader(csSrc.c_str());
}

ComputeShader::~ComputeShader()
{
    glDeleteProgram(shaderProgramId);
}

std::string ComputeShader::loadShader(const char* filePath)
{
    std::ifstream stream(filePath);
    std::stringstream ss;

    ss << stream.rdbuf();
    std::string shaderStr = ss.str();

    return shaderStr;
}

void ComputeShader::createShader(const char* csSrc)
{
    shaderProgramId = glCreateProgram();
    // Compile shaders.
    unsigned int csId = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(csId, 1, &csSrc, nullptr);
    glCompileShader(csId);

    // Bind current shaders to shader program.
    glAttachShader(shaderProgramId, csId);
    glLinkProgram(shaderProgramId);
    glValidateProgram(shaderProgramId);

    glDeleteShader(csId);
}

int ComputeShader::getUniformLocation(const char* uniformName)
{
    return glGetUniformLocation(shaderProgramId, uniformName);
}

void ComputeShader::bind()
{
    glUseProgram(shaderProgramId);
}
