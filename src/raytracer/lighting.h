#pragma once

#include "glm/fwd.hpp"
#include "raytracer/raytracescene.h"
#include "utils/scenedata.h"
#include <vector>

SceneColor computePixelLighting(glm::vec4  position,
           glm::vec4  normal,
           glm::vec4  directionToCamera,
           PPShape shape,
           int recursiveDepth,
           const RayTraceScene& scene,
           RayTracer& rayTracer);

RGBA toRGBA(const glm::vec4 &illumination);
