#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Camera::Camera(glm::vec3 m_position, glm::vec3 up, float yaw, float pitch) :
    m_position(m_position),
    m_up(up),
    m_yaw(yaw),
    m_pitch(pitch),
    m_front(DefaultCamera::front),
    m_movementSpeed(DefaultCamera::speed),
    m_mouseSensitivity(DefaultCamera::sensitivity),
    m_zoom(DefaultCamera::zoom)
{
    m_worldUp = up;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch):
    m_position(glm::vec3(posX, posY, posZ)),
    m_up(glm::vec3(upX, upY, upZ)),
    m_yaw(yaw),
    m_pitch(pitch),
    m_front(DefaultCamera::front),
    m_movementSpeed(DefaultCamera::speed),
    m_mouseSensitivity(DefaultCamera::sensitivity),
    m_zoom(DefaultCamera::zoom)
{
    m_worldUp = m_up;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;
    if (direction == FORWARD)
        m_position += m_front * velocity;
    if (direction == BACKWARD)
        m_position -= m_front * velocity;
    if (direction == LEFT)
        m_position -= m_right * velocity;
    if (direction == RIGHT)
        m_position += m_right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    m_zoom -= yoffset;
    if (m_zoom < 1.0f)
        m_zoom = 1.0f;
    if (m_zoom > 45.0f)
        m_zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front{};

    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
