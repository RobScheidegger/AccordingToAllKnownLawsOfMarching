#pragma once

#include "raytracer/intersect.h"

class SphereScene final: public Shape {
public:
    SphereScene(ScenePrimitive primative, glm::mat4 ctm, float minScale): Shape(primative, ctm, minScale) {}
    ~SphereScene() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    float shapeSDF(glm::vec4 position) const override;
    TextureMap getTextureMap(glm::vec4 position) const override;
private:
    float m_sphereRadius = 0.25;
    float m_sphereSeparation = 1;
};
