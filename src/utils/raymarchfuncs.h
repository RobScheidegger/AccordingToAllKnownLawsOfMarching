#include "raytracer/raytracescene.h"

float sceneSDF(glm::vec4 worldSpacePoint, const RayTraceScene& scene);
glm::vec3 worldSpaceNormal(glm::vec4 worldSpacePoint, const RayTraceScene& scene);
