#include "Camera.h"

Camera::Camera(const int width, const int height, const glm::vec3& position)
{
	startPosition = position;
	Set(width, height, position);
    this->isPerspective = true;
    this->yaw = YAW;
    this->pitch = PITCH;

    this->FoVy = FOV;
    this->width = width;
    this->height = height;
    this->zNear = zNEAR;
    this->zFar = zFAR;

    this->worldUp = glm::vec3(0, 1, 0);
    this->position = position;

    lastX = width / 2.0f;
    lastY = height / 2.0f;
    bFirstMouseMove = true;

    UpdateCameraVectors();
}

void Camera::Set(const int width, const int height, const glm::vec3& position)
{
    Set(width, height, startPosition);
}

void Camera::Reset(const int width, const int height)
{
    width = windowWidth;
    height = windowHeight;
    glViewport(0, 0, windowWidth, windowHeight);
}

void Camera::Reshape(int windowWidth, int windowHeight)
{
}
