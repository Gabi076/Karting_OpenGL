#include <stdlib.h>
#include <stdio.h>
#include <math.h> 

#include "IncludeDir/include/GL/glew.h"

#define GLM_FORCE_CTOR_INIT 
#include "IncludeDir/glm/glm.hpp"
#include "IncludeDir/glm/ext/matrix_transform.hpp"
#include "IncludeDir/glm/gtc/type_ptr.hpp"

#include "IncludeDir/GLFW/glfw3.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "Camera.h"
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION 
#include"IncludeDir/stb-master/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment (lib, "LibrariesDir/lib-vc2019/glfw3dll.lib")
#pragma comment (lib, "LibrariesDir/Win32/glew32.lib")
#pragma comment (lib, "LibrariesDir/lib/x64/assimp-vc143-mt.lib")
#pragma comment (lib, "OpenGL32.lib")
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

Camera* pCamera = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;

unsigned int CreateTexture(const std::string& strTexturePath)
{
    unsigned int textureId = -1;

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(strTexturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture: " << strTexturePath << std::endl;
    }
    stbi_image_free(data);

    return textureId;
}
unsigned int planeVAO = 0;
void renderFloor()
{
    unsigned int planeVBO;

    if (planeVAO == 0) {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        float planeVertices[] = {
            // positions            // normals         // texcoords
            25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

            25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
            25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
        };
        // plane VAO
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }

    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

/*void processNode(aiNode* node, const aiScene* scene) {
    // Process node meshes, materials, etc.
    // Recursively process child nodes
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

void loadModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        // Handle error
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Process the imported model data
    processNode(scene->mRootNode, scene);
}*/


void renderScene(const Shader& shader)
{
    // floor
    glm::mat4 model;
    shader.SetMat4("model", model);
    renderFloor();
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pCamera->ProcessKeyboard(FORWARD, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pCamera->ProcessKeyboard(BACKWARD, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        pCamera->ProcessKeyboard(LEFT, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        pCamera->ProcessKeyboard(RIGHT, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        pCamera->ProcessKeyboard(UP, (float)deltaTime);
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        pCamera->ProcessKeyboard(DOWN, (float)deltaTime);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        pCamera->Reset(width, height);

    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
    pCamera->ProcessMouseScroll((float)yOffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Cleanup()
{
    delete pCamera;
}


int main(int argc, char** argv)
{

    std::string strFullExeFileName = argv[0];
    std::string strExePath;
    const size_t last_slash_idx = strFullExeFileName.rfind('\\');
    if (std::string::npos != last_slash_idx) {
        strExePath = strFullExeFileName.substr(0, last_slash_idx);
    }

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Karting Simulator", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewInit();

    glEnable(GL_DEPTH_TEST);

    // Floor vertices
    float floorVertices[] = {
        // positions          // texture Coords 
        5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f,

        5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
        5.0f, -0.5f, -5.0f,  1.0f, 1.0f
    };

    // Floor VAO si VBO
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int floorTexture = CreateTexture(strExePath + "\\..\\test\\PrejmerTrack.png");

    //loadModel("KartObject/kart.obj");

    // Create camera
    pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 1.0, 3.0));

    Shader shaderFloor("Floor.vs", "Floor.fs");
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0);

        shaderFloor.Use();
        glm::mat4 projection = pCamera->GetProjectionMatrix();
        glm::mat4 view = pCamera->GetViewMatrix();
        shaderFloor.SetMat4("projection", projection);
        shaderFloor.SetMat4("view", view);

        // Draw floor
        glBindVertexArray(floorVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        model = glm::mat4();
        shaderFloor.SetMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    Cleanup();

    glfwTerminate();
    return 0;
}