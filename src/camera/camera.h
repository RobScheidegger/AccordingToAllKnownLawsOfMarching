#pragma once

#include "utils/scenedata.h"
#include <glm/glm.hpp>


// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
private:
    SceneCameraData m_cameraData;
public:
    float m_k = 1.0f;
    int m_height;
    int m_width;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_viewMatrixInverse;

    Camera(const SceneCameraData& cameraData, int height, int width);

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;
    glm::mat4 getViewMatrixInverse() const;

    glm::mat4 computeViewMatrix() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    glm::vec4 getPosition() const;

    void update(glm::vec4 position, glm::vec4 look);
};
