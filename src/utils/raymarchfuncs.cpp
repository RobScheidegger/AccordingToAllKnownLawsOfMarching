#include "raymarchfuncs.h"

float minUnion(std::vector<float>& shapeSDFs) {
    float minDist = std::numeric_limits<float>::infinity();
    for (float i : shapeSDFs) {
        minDist = std::min(minDist, i);
    }

    return minDist;
}

float sceneSDF(glm::vec4 worldSpacePoint, const RayTraceScene& scene) {
    const std::vector<Shape*>& shapes = scene.getShapes();
    std::vector<float> shapeSDFs;

    for(const Shape* shape : shapes){
        glm::vec4 objectSpacePos = shape->m_ctm_inverse * worldSpacePoint;
        shapeSDFs.push_back(shape->shapeSDF(objectSpacePos));
    }

    // For now, we do naive min union
    float sceneSDFVal = minUnion(shapeSDFs);
    return sceneSDFVal;
}

glm::vec3 worldSpaceNormal(glm::vec4 worldSpacePoint, const RayTraceScene& scene) {
    const float smallStep = 0.001;

    float gradient_x = sceneSDF(worldSpacePoint + glm::vec4(smallStep, 0.0f, 0.0f, 0.0f), scene)
            - sceneSDF(worldSpacePoint - glm::vec4(smallStep, 0.0f, 0.0f, 0.0f), scene);
    float gradient_y = sceneSDF(worldSpacePoint + glm::vec4(0.0f, smallStep, 0.0f, 0.0f), scene)
            - sceneSDF(worldSpacePoint - glm::vec4(0.0f, smallStep, 0.0f, 0.0f), scene);
    float gradient_z = sceneSDF(worldSpacePoint + glm::vec4(0.0f, 0.0f, smallStep, 0.0f), scene)
            - sceneSDF(worldSpacePoint - glm::vec4(0.0f, 0.0f, smallStep, 0.0f), scene);

    glm::vec3 normal = glm::vec3(gradient_x, gradient_y, gradient_z);

    return glm::normalize(normal);
}
