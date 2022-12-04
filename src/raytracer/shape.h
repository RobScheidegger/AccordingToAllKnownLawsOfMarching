#pragma once

#include "glm/fwd.hpp"
#include "raytracer/raytracer.h"
#include "utils/scenedata.h"
#include <optional>

class Shape; // Forward ref

struct Intersect{
    const Shape* shape;
    float t;
    glm::vec3 normal;
};

struct TextureMap {
    float u;
    float v;
};

class Shape {
public:
    glm::mat4 m_ctm;
    glm::mat4 m_ctm_inverse;
    glm::mat3 m_worldNormal;
    ScenePrimitive m_primative;
    float m_minScale;
    // destructors must always be virtual if you decide to use virtual functions
    Shape(ScenePrimitive primative, glm::mat4 ctm, float minScale): m_primative{primative} {
        m_ctm = ctm;
        m_ctm_inverse = glm::inverse(ctm);
        glm::mat3 m3 = ctm;
        m_worldNormal = glm::inverse(glm::transpose(m3));
        m_minScale = minScale;
    }
    virtual ~Shape() = default;
    virtual glm::vec3 getNormal(glm::vec4 position) const = 0;
    virtual std::optional<Intersect> intersect(Ray ray) const = 0;
    virtual float shapeSDF(glm::vec4 position) const = 0;
    virtual TextureMap getTextureMap(glm::vec4 position) const = 0;
};
