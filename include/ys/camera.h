#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
namespace ys
{
    typedef struct CameraParam {
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 WorldUpAxis;
        float sensit;
        float fov;
        float yMax;
        float yMin;
        float projectionMax;
        float projectionMin;
    }CameraParam;

    class Camera
    {
    public:
        Camera(CameraParam& cp);

        glm::mat4 CreateViewMat();

        glm::mat4 CreatePerspectiveProjectionMat(float width, float height);

        glm::mat4 CreateOrthographicProjectionMat(float width, float height);

        void MoveView(double xpos, double ypos);

        void MoveFront(float speed);

        void MoveBehind(float speed);

        void MoveLeft(float speed);

        void MoveRight(float speed);

        glm::vec3 GetPosition();

        void SetPositionY(float y);

        float GetYaw();

        float GetPitch();

    private:
        glm::vec3 position;
        glm::vec3 upAxis;
        glm::vec3 frontAxis;
        float fov;
        float sensit;
        float yMax, yMin;
        float projectionMax, projectionMin;
        float pitch, yaw;

        bool firstMouse;
        double lastX, lastY;
    };
}

inline glm::mat4 ys::Camera::CreateViewMat()
{
    return glm::lookAt(position, position + frontAxis, upAxis);
}

inline glm::mat4 ys::Camera::CreatePerspectiveProjectionMat(float width, float height)
{
    return glm::perspective(glm::radians(fov), width / height, projectionMin, projectionMax);
}

inline glm::mat4 ys::Camera::CreateOrthographicProjectionMat(float width, float height)
{
    return glm::ortho(0.0f, width, 0.0f, height, projectionMin, projectionMax);
}

inline void ys::Camera::MoveFront(float speed)
{
    position += speed * frontAxis;
}

inline void ys::Camera::MoveBehind(float speed)
{
    position -= speed * frontAxis;
}

inline void ys::Camera::MoveLeft(float speed)
{
    position -= speed * glm::normalize(glm::cross(frontAxis, upAxis));
}

inline void ys::Camera::MoveRight(float speed)
{
    position += speed * glm::normalize(glm::cross(frontAxis, upAxis));
}

inline glm::vec3 ys::Camera::GetPosition()
{
    return this->position;
}

inline void ys::Camera::SetPositionY(float y)
{
    this->position[1] = y;
}

inline float ys::Camera::GetYaw()
{
    return this->yaw;
}

inline float ys::Camera::GetPitch()
{
    return this->pitch;
}