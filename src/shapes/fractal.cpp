#include "fractal.h"

std::optional<Intersect> Fractal::intersect(Ray ray) const{

    // Assumes that ray is now in _object_ space, so we can just intersect with the traditional sphere.
    // Centered at origin with radius 1/2
    std::optional<Intersect> intersect = std::nullopt;

    float a = glm::dot(ray.d, ray.d);
    float b = 2 * glm::dot(ray.p, ray.d);
    float c = glm::dot(ray.p, ray.p) - 1.25; // extra - 1 from dot product being weird with 4-vectors

    std::pair<std::optional<float>, std::optional<float>> t = solveQuadratic(a, b, c);
    if(t.first.has_value()){
        replaceIntercept(intersect,
                    Intersect{this, t.first.value(), getNormal(ray.evaluate(t.first.value()))}
               );
    }
    if(t.second.has_value()){
        replaceIntercept(intersect,
                    Intersect{this, t.second.value(), getNormal(ray.evaluate(t.second.value()))}
               );
    }

    return intersect;
}

float Fractal::shapeSDF(glm::vec4 position) const {
    glm::vec3 newPosition(position);
    // apply modulo to the query position every unit distance to acheive same thing as 'duplicating' the SDF
    newPosition[0] = glm::mod(position[0], m_sphereSeparation) - m_sphereSeparation/2;
    newPosition[1] = glm::mod(position[1], m_sphereSeparation) - m_sphereSeparation/2;
    return length(newPosition) - m_sphereRadius;
}

TextureMap Fractal::getTextureMap(glm::vec4 position) const{
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

glm::vec3 Fractal::getNormal(glm::vec4 position) const{
    glm::vec3 objectNormal = position;
    // Change object normal to world normal.
    return objectToWorldNormal(objectNormal, this);
}
