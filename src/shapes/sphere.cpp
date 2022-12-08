#include "sphere.h"

std::optional<Intersect> Sphere::intersect(Ray ray) const{

    // Assumes that ray is now in _object_ space, so we can just intersect with the traditional sphere.
    // Centered at origin with radius 1/2
    std::optional<Intersect> intersect = std::nullopt;

    float a = glm::dot(ray.d, ray.d);
    float b = 2 * glm::dot(ray.p, ray.d);
    float c = glm::dot(ray.p, ray.p) - 1.25; // extra - 1 from dot product being weird with 4-vectors

    std::pair<std::optional<float>, std::optional<float>> t = solveQuadratic(a, b, c);
    if(t.first.has_value()){
        std::vector<float> blends{1.0f};
        std::vector<const Shape*> shapeVec;
        shapeVec.emplace_back(this);

        replaceIntercept(intersect,
                    Intersect{{false, blends, shapeVec}, t.first.value(), getNormal(ray.evaluate(t.first.value()))}
               );
    }
    if(t.second.has_value()){
        std::vector<float> blends{1.0f};
        std::vector<const Shape*> shapeVec;
        shapeVec.emplace_back(this);

        replaceIntercept(intersect,
                    Intersect{{false, blends, shapeVec}, t.second.value(), getNormal(ray.evaluate(t.second.value()))}
               );
    }

    return intersect;
}

float Sphere::shapeSDF(glm::vec4 position) const {
    return glm::length(glm::vec3(position)) - 0.5f;
}

TextureMap Sphere::getTextureMap(glm::vec4 position) const{
    position = m_ctm_inverse * position;
    // Get phi and theta angles, extrapolate from there

    float r = std::sqrt(position.x * position.x + position.z * position.z + position.y * position.y);
    float phi = std::asin(position.y / r);
    float v = phi / PI + 0.5f;
    float u;
    if(isClose(v,0) || isClose(v, 1)){
        u = 0.5f;
    } else {
        float theta = std::atan2(position.z, position.x);
        u = theta < 0 ? -theta / (2 * PI) : 1 - theta / (2 * PI);
    }

    return TextureMap{u, v};
}

glm::vec3 Sphere::getNormal(glm::vec4 position) const{
    glm::vec3 objectNormal = position;
    // Change object normal to world normal.
    return objectToWorldNormal(objectNormal, this);
}
