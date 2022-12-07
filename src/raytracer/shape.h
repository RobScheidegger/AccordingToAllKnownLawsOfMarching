#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "raytracer/raytracer.h"
#include "utils/scenedata.h"
#include <iostream>
#include <optional>
#include "utils/bezierfuncs.h"

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
    glm::mat4 m_origCtm;
    glm::mat4 m_ctm;
    glm::mat4 m_ctm_inverse;
    glm::mat3 m_worldNormal;
    ScenePrimitive m_primative;
    float m_minScale;
    // destructors must always be virtual if you decide to use virtual functions
    Shape(ScenePrimitive primative, glm::mat4 ctm, float minScale): m_primative{primative} {
        m_origCtm = ctm;
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

    void updatePosition(float time)  { // for translating the shape relative to its original world space position over time
        if (!m_primative.useBezierCurves) {
            return;
        }
        glm::mat4 translationMatrix(1.);
        // get the relative position along this shape's bezier curve
        double intpart;
        float bezierTime = modf((double)time * m_primative.movementSpeed, &intpart);
        std::cout << bezierTime << std::endl;
        glm::vec3 currRelPos = bezier(m_primative.controlPoints, bezierTime);
        translationMatrix[3] = glm::vec4(bezier(m_primative.controlPoints, bezierTime), 1.f);
        m_ctm = translationMatrix * m_origCtm;
        // update view and normal transformation matrices
        m_ctm_inverse = glm::inverse(m_ctm);
        glm::mat3 m3 = m_ctm;
        m_worldNormal = glm::inverse(glm::transpose(m3));
    }

};

// Possibly Paired Shape
struct PPShape {
    bool isPair = false;
    float blendFactor = 1.0f;
    const Shape* shape1;
    const Shape* shape2 = nullptr;
};
