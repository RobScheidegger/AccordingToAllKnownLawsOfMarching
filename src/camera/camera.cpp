#include <stdexcept>
#include "camera.h"

Camera::Camera(const SceneCameraData& cameraData, int height, int width){
    m_cameraData = cameraData;
    m_height = height;
    m_width = width;
    m_viewMatrix = computeViewMatrix();
    m_viewMatrixInverse = glm::inverse(m_viewMatrix);
}

glm::mat4 Camera::computeViewMatrix() const {
    glm::vec3 up = m_cameraData.up;
    glm::vec3 pos = m_cameraData.pos;
    glm::vec3 look = m_cameraData.look;

    glm::mat4 translate = glm::mat4{
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      -pos.x, -pos.y, -pos.z, 1
    };

    glm::vec3 w = -glm::normalize(look);
    glm::vec3 v = glm::normalize(up - glm::dot(up, w) * w);
    glm::vec3 u = glm::cross(v, w);
    glm::mat4 rotate = glm::mat4{u.x, v.x, w.x, 0,
                                 u.y, v.y, w.y, 0,
                                 u.z, v.z, w.z, 0,
                                 0, 0, 0, 1};
    return rotate * translate;
}

glm::mat4 Camera::getViewMatrix() const {
    return m_viewMatrix;
}

glm::mat4 Camera::getViewMatrixInverse() const {
    return m_viewMatrixInverse;
}

float Camera::getAspectRatio() const {
    return (float)m_width / (float) m_height;
}

float Camera::getHeightAngle() const {
    return m_cameraData.heightAngle;
}

float Camera::getFocalLength() const {
    return m_cameraData.focalLength;
}

float Camera::getAperture() const {
    return m_cameraData.aperture;
}

glm::vec4 Camera::getPosition() const {
    return m_cameraData.pos;
}
