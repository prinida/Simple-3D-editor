#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

namespace DefaultCamera
{
    inline constexpr float yaw = -90.0f;
    inline constexpr float pitch = 0.0f;
    inline constexpr float speed = 1.0f;
    inline constexpr float sensitivity = 0.1f;
    inline constexpr float zoom = 45.0f;

    inline constexpr glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    inline constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    inline constexpr glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
}

class Camera
{
public:
    Camera(glm::vec3 position = DefaultCamera::position, 
        glm::vec3 up = DefaultCamera::up, 
        float yaw = DefaultCamera::yaw, 
        float pitch = DefaultCamera::pitch);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    glm::mat4 getViewMatrix() const;
    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);

private:
    void updateCameraVectors();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
};

#endif