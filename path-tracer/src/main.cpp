#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>

#include <GL/glew.h>
/* Note that GLEW_STATIC is defined in the compiler preprocessor directives */
/* As we are statically linking GLEW */
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include "shader.h"
#include "computeShader.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialise GLEW only once an OpenGL has been created */
    if (glewInit() != GLEW_OK)
    {
        return -1;
    }

    /* Specify triangle vertex positions */
    float vertices[] = {
        // positions        // texture coords
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    /* Create and bind vertex buffer object */
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* Enable vertex attribute array */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    /* Specify vertex attribute stuff */
    // Start index, num of components per vertex attribute, 
    // type of component, whether to normalise, 
    // stride (size of each set of vert attribs), pointer to first attrib in vert
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

    /* Create texture */
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    //glGenerateMipmap(GL_TEXTURE_2D);

    //delete[] texData;

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Vertex and fragment shaders */
    Shader shader = Shader("./shaders/vertex.glsl", "./shaders/fragment.glsl");

    /* Compute shader */
    ComputeShader computeShader = ComputeShader("./shaders/compute.glsl");


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        computeShader.bind();
        glDispatchCompute((unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT, 1);
        // Barrier (stop execution) to ensure data writing is finished before access.
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Bind vertex and fragment shaders after compute shader.
        shader.bind();

        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(float), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}