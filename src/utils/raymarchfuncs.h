#include "raytracer/raytracescene.h"

struct SDFResult {
    const Shape* intersectedShape;
    float sceneSDFVal;
};

SDFResult sceneSDF(glm::vec4 worldSpacePoint, const RayTraceScene& scene);
glm::vec3 worldSpaceNormal(glm::vec4 worldSpacePoint, const RayTraceScene& scene);
