#pragma once

#include "raytracer/intersect.h"

class Fractal final: public Shape {
public:
    Fractal(ScenePrimitive primative, glm::mat4 ctm, float minScale): Shape(primative, ctm, minScale) {}
    ~Fractal() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    float shapeSDF(glm::vec4 position) const override;
    TextureMap getTextureMap(glm::vec4 position) const override;
private:
    float m_sphereRadius = 0.3;
    float m_sphereSeparation = 1;
};
