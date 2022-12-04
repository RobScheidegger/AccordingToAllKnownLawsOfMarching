#pragma once

#include "glm/fwd.hpp"
#include "raytracer/raytracer.h"
#include "utils/scenedata.h"
#include "raytracer/intersect.h"


class Cube final: public Shape {
public:
    Cube(ScenePrimitive primative, glm::mat4 ctm, float minScale): Shape(primative, ctm, minScale) {}
    ~Cube() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    float shapeSDF(glm::vec4 position) const override;
    TextureMap getTextureMap(glm::vec4 position) const override;

private:
    glm::vec3 sideLengths{0.5f, 0.5f, 0.5f};
};
