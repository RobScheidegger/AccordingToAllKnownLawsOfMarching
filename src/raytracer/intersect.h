#pragma once

#include <glm/glm.hpp>
#include "raytracer.h"
#include <memory>
#include <optional>
#include <vector>
#include "utils/scenedata.h"
#include "shape.h"

const static float PI = 3.14159265358979323846;

/**
 * @brief intersect Finds the closest intersection between a ray and an object in the scene.
 */
std::optional<Intersect> intersect(const RayTraceScene& shapes, const Ray& ray);

// Utility functions
bool isClose(float a, float b);
void replaceIntercept(std::optional<Intersect>& current, Intersect replacement);
glm::vec3 objectToWorldNormal(glm::vec3 objectNormal, const Shape* shape);
std::pair<std::optional<float>, std::optional<float>> solveQuadratic(float a, float b, float c);
