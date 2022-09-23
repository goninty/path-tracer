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
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/quaternion.hpp>

#include "Shader.h"
#include "ComputeShader.h"
#include "Camera.h";

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

// Store these so we can calculate angles of mouse movement on each new frame.
float lastMouseX = WINDOW_WIDTH / 2;
float lastMouseY = WINDOW_HEIGHT / 2;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) return;

    Camera* cam = (Camera*)glfwGetWindowUserPointer(window);
    switch (key)
    {
        case GLFW_KEY_W:
            cam->moveForward();
            break;
        case GLFW_KEY_A:
            cam->moveLeft();
            break;
        case GLFW_KEY_S:
            cam->moveBackward();
            break;
        case GLFW_KEY_D:
            cam->moveRight();
            break;
    }
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    Camera* cam = (Camera*)glfwGetWindowUserPointer(window);

    float xOffset = xpos - lastMouseX;
    float yOffset = ypos - lastMouseY;
    lastMouseX = xpos;
    lastMouseY = ypos;

    const float sens = 0.2f;

    // negative y offset otherwise inverted y axis movement
    cam->rotate(xOffset*sens, -yOffset*sens);  
}

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

    /* Initialise GLEW only once an OpenGL context has been created */
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

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Vertex and fragment shaders */
    Shader shader = Shader("./shaders/vertex.glsl", "./shaders/fragment.glsl");

    /* Compute shader */
    ComputeShader computeShader = ComputeShader("./shaders/compute.glsl");

    /* Positionable camera init */
    Camera camera = Camera();

    // Need to feed camera position and look(?) to compute shader.
    unsigned int camPosLocation = computeShader.getUniformLocation("cameraPosition");
    unsigned int viewMatLocation = computeShader.getUniformLocation("viewMatrix");
    
    /* Set a GLFW pointer to camera object so it can be accessed easily in callback fucntions */
    glfwSetWindowUserPointer(window, &camera);
    /* Set callbacks for GLFW keyboard and mouse input */
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorCallback);

    // Some notes:
    // Rotation requires recalculating the view matrix, while maintaining the same look point.
    // (At least, it seems for orbiting cameras and not FPS-style ones).
    // But what changes when we rotate for the view matrix to have to be re-calculated?
    // Well, for orbiting camera our position will change
    

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        computeShader.bind();
        // Upload (write) to uniform variable.
        if (camPosLocation >= 0) glUniform3fv(camPosLocation, 1, glm::value_ptr(camera.getPos()));
        if (viewMatLocation >= 0) glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix));
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