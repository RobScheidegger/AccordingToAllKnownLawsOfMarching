#pragma once

#include "raytracer/intersect.h"

class Cone final: public Shape {
public:
    Cone(ScenePrimitive primative, glm::mat4 ctm, float minScale): Shape(primative, ctm, minScale) {}
    ~Cone() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    float shapeSDF(glm::vec4 position) const override;
    TextureMap getTextureMap(glm::vec4 position) const override;

private:
    float m_height = 1.0f;
    float m_radius = 0.5f;
};
