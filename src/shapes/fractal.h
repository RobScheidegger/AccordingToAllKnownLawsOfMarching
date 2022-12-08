#pragma once

#include "raytracer/intersect.h"



class Fractal final: public Shape {
public:
    Fractal(ScenePrimitive primative, glm::mat4 ctm, float minScale, FractalType type): Shape(primative, ctm, minScale), m_type(type) {}
    ~Fractal() = default;

    std::optional<Intersect> intersect(Ray ray) const override;
    glm::vec3 getNormal(glm::vec4 position) const override;
    float shapeSDF(glm::vec4 position) const override;

    // Specific shape SDFs
    float mandelbulbSDF(glm::vec4 p) const;
    float mandelboxSDF(glm::vec3 p) const;
    float serpinskiSDF(glm::vec3 p) const;

    TextureMap getTextureMap(glm::vec4 position) const override;
private:
    int m_numIterations = 50;
    FractalType m_type;
};
