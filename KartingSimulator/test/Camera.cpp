#include "Camera.h"

Camera::Camera(const int width, const int height, const glm::vec3& position, const float angle)
{
    startPosition = position;
    Set(width, height, position,angle);
}

void Camera::Set(const int width, const int height, const glm::vec3& position, const float angle)
{
    this->isPerspective = true;
    this->yaw = angle;
    this->pitch = PITCH;

    this->FoVy = FOV;
    this->width = width;
    this->height = height;
    this->zNear = zNEAR;
    this->zFar = zFAR;

    this->worldUp = glm::vec3(0, 1, 0);
    this->position = position;
    this->position.y = -0.35f;

    lastX = width / 2.0f;
    lastY = height / 2.0f;
    bFirstMouseMove = true;

    UpdateCameraVectors();
}

void Camera::Reset(const int width, const int height)
{
    Set(width, height, startPosition,0.0f);
}

void Camera::Reshape(int windowWidth, int windowHeight)
{
    width = windowWidth;
    height = windowHeight;
    glViewport(0, 0, windowWidth, windowHeight);
}

const glm::vec3 Camera::GetPosition() const
{
    return position;
}

const glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + forward, up);
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
    glm::mat4 Proj = glm::mat4(1);
    if (isPerspective) {
        float aspectRatio = ((float)(width)) / height;
        Proj = glm::perspective(glm::radians(FoVy), aspectRatio, zNear, zFar);
    }
    else {
        float scaleFactor = 2000.f;
        Proj = glm::ortho<float>(
            -width / scaleFactor, width / scaleFactor,
            -height / scaleFactor, height / scaleFactor, -zFar, zFar);
    }
    return Proj;
}

void Camera::SetMatrix(const glm::mat4& viewMatrix)
{ 
    // Extract the camera position, forward, and up vectors from the view matrix
    position = glm::vec3(viewMatrix[3]);
    forward = glm::normalize(glm::vec3(viewMatrix[2]));  // Assuming the forward vector is in the third column
    right = glm::normalize(glm::vec3(viewMatrix[0]));    // Assuming the right vector is in the first column
    up = glm::normalize(glm::vec3(viewMatrix[1]));       // Assuming the up vector is in the second column

    // Recalculate yaw and pitch from the new forward and up vectors
    yaw = glm::degrees(atan2(forward.x, forward.z));
    pitch = glm::degrees(asin(-forward.y));  // Assuming OpenGL conventions for the camera coordinate system

    // Update other camera parameters if needed
    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(ECameraMovementType direction, float deltaTime)
{
    float velocity = (float)(cameraSpeedFactor *0.9*deltaTime);
    glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
    float rotationAngle = 0.0f;
    switch (direction) {
    case ECameraMovementType::FORWARD:
        position += forward * velocity;
        break;
    case ECameraMovementType::BACKWARD:
        position -= forward * velocity;
        break;
    case ECameraMovementType::LEFT:
        position -= right * velocity;
        rotationAngle = glm::radians(-90.0f);
        break;
    case ECameraMovementType::RIGHT:
        position += right * velocity;
        rotationAngle = glm::radians(90.0f);
        break;
    case ECameraMovementType::UP:
        position += up * velocity;
        break;
    case ECameraMovementType::DOWN:
        position -= up * velocity;
        break;
    }
    if (rotationAngle != 0.0f) {
        yaw += 0.4*rotationAngle;
        UpdateCameraVectors();
    }
}

void Camera::MouseControl(float xPos, float yPos)
{
    if (bFirstMouseMove) {
        lastX = xPos;
        lastY = yPos;
        bFirstMouseMove = false;
    }

    float xChange = xPos - lastX;
    float yChange = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    if (fabs(xChange) <= 1e-6 && fabs(yChange) <= 1e-6) {
        return;
    }
    xChange *= mouseSensitivity;
    yChange *= mouseSensitivity;

    ProcessMouseMovement(xChange, yChange);
}

void Camera::ProcessMouseScroll(float yOffset)
{
    if (FoVy >= 1.0f && FoVy <= 90.0f) {
        FoVy -= yOffset;
    }
    if (FoVy <= 1.0f)
        FoVy = 1.0f;
    if (FoVy >= 90.0f)
        FoVy = 90.0f;
}

const glm::vec3 Camera::GetForward() const
{
    return forward;
}

void Camera::RotateCameraHorizontal(float kartZChange)
{
    float xChange = 0.0f;

    // Adjust these values based on your preference
    const float kartRotationSensitivity = 1.f;

    // Calculate the horizontal rotation based on kart's z-coordinate changes
    xChange = kartZChange * kartRotationSensitivity;

    if (fabs(xChange) <= 1e-6) {
        return;
    }

    xChange *= 1.f;

    ProcessMouseMovement(xChange, 0.0f);
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    yaw += xOffset;
    pitch += yOffset;
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    this->forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->forward.y = sin(glm::radians(pitch));
    this->forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->forward = glm::normalize(this->forward);
    right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, forward));
}
