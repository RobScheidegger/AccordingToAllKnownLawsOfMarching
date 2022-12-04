#include "raymarchfuncs.h"

SDFResult minUnion(std::vector<float>& shapeSDFs, const std::vector<Shape*>& shapes) {
    float minDist = std::numeric_limits<float>::infinity();
    const Shape* minDistShape = nullptr;

    for (int i = 0; i < shapeSDFs.size(); i++) {
        if (shapeSDFs[i] < minDist) {
            minDist = shapeSDFs[i];
            minDistShape = shapes[i];
        }
    }

    return {minDistShape, minDist};
}

glm::vec2 smoothPolyMin2(float dist1, float dist2, float smoothFactor, float n) {
    float h = std::max(smoothFactor - abs(dist1 - dist2), 0.0f) / smoothFactor;
    float m = pow(h, n) * 0.5;
    float s = m * smoothFactor / n;

    return (dist1 < dist2) ? glm::vec2(dist1 - s, m) : glm::vec2(dist2 - s, 1.0 - m);
}

SDFResult smoothPolyMin(std::vector<float>& shapeSDFs, const std::vector<Shape*>& shapes) {
    float minDist = std::numeric_limits<float>::infinity();
    float secondMinDist = std::numeric_limits<float>::infinity();
    const Shape* minDistShape = nullptr;

    for (int i = 0; i < shapeSDFs.size(); i++) {
        if (shapeSDFs[i] < minDist) {
            secondMinDist = minDist;
            minDist = shapeSDFs[i];
            minDistShape = shapes[i];
        } else if (shapeSDFs[i] < secondMinDist) {
            secondMinDist = shapeSDFs[i];
        }
    }

    glm::vec2 blend = smoothPolyMin2(minDist, secondMinDist, 0.1, 2);

    return {minDistShape, blend[0], blend[1]};
}

SDFResult sceneSDF(glm::vec4 worldSpacePoint, const RayTraceScene& scene) {
    const std::vector<Shape*>& shapes = scene.getShapes();
    std::vector<float> shapeSDFs;

    for(const Shape* shape : shapes){
        glm::vec4 objectSpacePos = shape->m_ctm_inverse * worldSpacePoint;
        shapeSDFs.push_back(shape->shapeSDF(objectSpacePos));
    }

    // SDFResult sceneSDFVal = minUnion(shapeSDFs, shapes);
    SDFResult sceneSDFVal = smoothPolyMin(shapeSDFs, shapes);
    return sceneSDFVal;
}

glm::vec3 worldSpaceNormal(glm::vec4 worldSpacePoint, const RayTraceScene& scene) {
    const float smallStep = 0.0001;

    float gradient_x = sceneSDF(worldSpacePoint + glm::vec4(smallStep, 0.0f, 0.0f, 0.0f), scene).sceneSDFVal
            - sceneSDF(worldSpacePoint - glm::vec4(smallStep, 0.0f, 0.0f, 0.0f), scene).sceneSDFVal;
    float gradient_y = sceneSDF(worldSpacePoint + glm::vec4(0.0f, smallStep, 0.0f, 0.0f), scene).sceneSDFVal
            - sceneSDF(worldSpacePoint - glm::vec4(0.0f, smallStep, 0.0f, 0.0f), scene).sceneSDFVal;
    float gradient_z = sceneSDF(worldSpacePoint + glm::vec4(0.0f, 0.0f, smallStep, 0.0f), scene).sceneSDFVal
            - sceneSDF(worldSpacePoint - glm::vec4(0.0f, 0.0f, smallStep, 0.0f), scene).sceneSDFVal;

    glm::vec3 normal = glm::vec3(gradient_x, gradient_y, gradient_z);

    return glm::normalize(normal);
}
