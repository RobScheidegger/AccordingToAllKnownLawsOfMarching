#pragma once

#include "raytracer/intersect.h"

class Sphere final: public Shape {
public:
    Sphere(ScenePrimitive primative, glm::mat4 ctm, float minScale): Shape(primative, ctm, minScale) {}
    ~Sphere() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    float shapeSDF(glm::vec4 position) const override;
    TextureMap getTextureMap(glm::vec4 position) const override;
};
