#pragma once

#include "raytracer/intersect.h"

class Cylinder final: public Shape {
public:
    Cylinder(ScenePrimitive primative, glm::mat4 ctm, float minScale): Shape(primative, ctm, minScale) {}
    ~Cylinder() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    float shapeSDF(glm::vec4 position) const override;
    TextureMap getTextureMap(glm::vec4 position) const override;

private:
    float m_height = 1;
    float m_radius = 0.5;

};
