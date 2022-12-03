#include "raytracer/raytracescene.h"

float sceneSDF(glm::vec3 worldSpacePoint, const RayTraceScene& shapes);
glm::vec3 worldSpaceNormal(glm::vec3 worldSpacePoint, const RayTraceScene& shapes);
