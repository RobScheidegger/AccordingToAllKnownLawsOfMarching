#include "raytracer/raytracescene.h"

struct SDFResult {
    const Shape* intersectedShape;
    float sceneSDFVal;
    float blendFactor = 1.0f;
};

SDFResult sceneSDF(glm::vec4 worldSpacePoint, const RayTraceScene& scene);
glm::vec3 worldSpaceNormal(glm::vec4 worldSpacePoint, const RayTraceScene& scene);
