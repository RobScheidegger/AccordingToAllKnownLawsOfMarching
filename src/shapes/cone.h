#pragma once

#include "raytracer/intersect.h"

class Cone final: public Shape {
public:
    Cone(ScenePrimitive primative, glm::mat4 ctm): Shape(primative, ctm) {}
    ~Cone() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    TextureMap getTextureMap(glm::vec4 position) const override;
};
