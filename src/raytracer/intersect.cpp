#include <iostream>
#include <memory>
#include "utils/raymarchfuncs.h"

const static int MAX_NUM_RAYMARCH_STEPS = 100;
const static float MAX_RAYMARCH_DISTANCE = 1000.0;
const static float EPSILON = 0.0001;

bool isClose(float a, float b){
    return std::abs(a - b) <= EPSILON;
}

void replaceIntercept(std::optional<Intersect>& current, Intersect replacement){
    if(replacement.t <= EPSILON)
        return;
    if(!current.has_value() || current.value().t > replacement.t){
        current = std::make_optional(replacement);
    }
}

glm::vec3 objectToWorldNormal(glm::vec3 objectNormal, const Shape* shape){
    return glm::normalize(shape->m_worldNormal * objectNormal);
}

std::pair<std::optional<float>, std::optional<float>> solveQuadratic(float a, float b, float c){
    float d = b * b - 4 * a * c;
    if(d < 0)
        return std::pair{std::nullopt, std::nullopt};

    float x1 = (-b + std::sqrt(d)) / (2.0f * a);
    float x2 = (-b - std::sqrt(d)) / (2.0f * a);
    return std::pair{
        x1 > 0 ? std::make_optional(x1) : std::nullopt,
        x2 > 0 ? std::make_optional(x2) : std::nullopt
    };
}


std::optional<Intersect> intersect(const RayTraceScene& scene, const Ray& ray){
    std::optional<Intersect> intersection = std::nullopt;
    const std::vector<Shape*>& shapes = scene.getShapes();


    for(const Shape* shape : shapes){
        Ray objectRay = Ray{shape->m_ctm_inverse * ray.p, shape->m_ctm_inverse * ray.d};
        std::optional<Intersect> shapeIntersection = shape->intersect(objectRay);

        if(!shapeIntersection.has_value())
            continue;

        replaceIntercept(intersection, shapeIntersection.value());
    }

    return intersection;
}

std::optional<Intersect> intersectMarch(const RayTraceScene& shapes, const Ray& worldSpaceRay) {
    std::optional<Intersect> intersection; // in WORLD SPACE

    float distTraveledAlongRay = 0;
    for (int currStep = 0; currStep < MAX_NUM_RAYMARCH_STEPS; currStep++) {

        glm::vec4 currPointAlongRay = worldSpaceRay.p + distTraveledAlongRay*worldSpaceRay.d;
        // get dist to nearest surface point in scene
        float currSDF = sceneSDF(currPointAlongRay, shapes);

        // hit: exit if we are below a distance threshold to any surface in the scene
        if (currSDF < EPSILON) {
            // record the intersection point and its normal
            replaceIntercept(intersection, Intersect{nullptr, distTraveledAlongRay, worldSpaceNormal(currPointAlongRay, shapes)}); // TODO: pass in shape?
            break;
        }
        // miss: exit if we have not intersected after the max march distance
        if (currSDF > MAX_RAYMARCH_DISTANCE) {
            break;
        }

        // take step along ray according to the sdf
        distTraveledAlongRay += currSDF;

    }

    return intersection;
}
