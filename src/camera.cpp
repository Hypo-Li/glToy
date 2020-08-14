#include <ys/camera.h>

ys::Camera::Camera(CameraParam& cp)
{
    this->position = cp.position;
    glm::vec3 direction = glm::normalize(cp.position - cp.target); //摄像机方向(目标指向摄像机)
    glm::vec3 rightAxis = glm::normalize(glm::cross(cp.WorldUpAxis, direction)); //计算摄像机的右轴
    this->upAxis = glm::cross(direction, rightAxis);
    this->frontAxis = glm::cross(upAxis, rightAxis);
    this->sensit = cp.sensit;
    this->fov = cp.fov;
    this->yMax = cp.yMax;
    this->yMin = cp.yMin;
    this->projectionMax = cp.projectionMax;
    this->projectionMin = cp.projectionMin;
    this->pitch = 0.0f;
    this->yaw = glm::degrees(glm::acos(glm::dot(glm::vec2(1.0f, 0.0f), glm::vec2(frontAxis.x, frontAxis.z))));
    this->yaw *= frontAxis.z < 0 ? -1 : 1;

    this->firstMouse = true;
}

void ys::Camera::MoveView(double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensit;
    yoffset *= sensit;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > yMax) {
        pitch = yMax;
    }
    if (pitch < yMin) {
        pitch = yMin;
    }

    glm::vec3 UNFAxis; //unNormalizeFrontAxis
    UNFAxis.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    UNFAxis.y = sin(glm::radians(pitch));
    UNFAxis.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->frontAxis = glm::normalize(UNFAxis);

}