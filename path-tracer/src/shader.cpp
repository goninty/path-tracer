#include "Shader.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <glm.hpp>

Shader::Shader(const char* vsPath, const char* fsPath)
{
    std::string vertexSrc = loadShader(vsPath);
    std::string fragmentSrc = loadShader(fsPath);
    // Need to use std::string as an intermediary because...?
    createShader(vertexSrc.c_str(), fragmentSrc.c_str());
}


Shader::~Shader()
{
    std::cout << "Is the destructor ever called?" << std::endl;
    glDeleteProgram(shaderProgramId);
}

std::string Shader::loadShader(const char* filePath)
{
    std::ifstream stream(filePath);
    std::stringstream ss;

    ss << stream.rdbuf();
    std::string shaderStr = ss.str();

    return shaderStr;
}

void Shader::createShader(const char* vsSrc, const char* fsSrc)
{
    shaderProgramId = glCreateProgram();
    // Compile shaders.
    unsigned int vsId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsId, 1, &vsSrc, nullptr);
    glCompileShader(vsId);

    unsigned int fsId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fsId, 1, &fsSrc, nullptr);
    glCompileShader(fsId);

    // Bind current shaders to shader program.
    glAttachShader(shaderProgramId, vsId);
    glAttachShader(shaderProgramId, fsId);
    glLinkProgram(shaderProgramId);
    glValidateProgram(shaderProgramId);

    glDeleteShader(vsId);
    glDeleteShader(fsId);
}

void Shader::bind()
{
    glUseProgram(shaderProgramId);
}

// Update the uniform colour.
void Shader::update(float r, float g, float b, float a)
{
    //glUniform4f(colorLoc, r, g, b, a);
}