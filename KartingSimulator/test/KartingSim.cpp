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
#define STB_IMAGE_IMPLEMENTATION 
#include"IncludeDir/stb-master/stb_image.h"

#pragma comment (lib, "LibrariesDir/lib-vc2019/glfw3dll.lib")
#pragma comment (lib, "LibrariesDir/Win32/glew32.lib")
#pragma comment (lib, "OpenGL32.lib")
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

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

int main(int argc, char** argv)
{
    std::cout << "Hello World!\n";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "KartingSimulation", NULL, NULL);
    glfwMakeContextCurrent(window);

}