#include "cylinder.h"

std::optional<Intersect> Cylinder::intersect(Ray ray) const{

    // Assumes that ray is now in _object_ space, so we can just intersect with the traditional sphere.
    // Centered at origin with radius 1/2
    std::optional<Intersect> intersect = std::nullopt;

    // Check the infinite cylinder
    float a = std::pow(ray.d.x, 2) + std::pow(ray.d.z, 2);
    float b = 2.0f * (ray.d.x * ray.p.x + ray.d.z * ray.p.z);
    float c = std::pow(ray.p.x, 2) + std::pow(ray.p.z, 2) - 0.25f;

    std::pair<std::optional<float>, std::optional<float>> t = solveQuadratic(a, b, c);
    if(t.first.has_value()){
        glm::vec4 position = ray.evaluate(t.first.value());
        if(position.y < 0.5f && position.y > -0.5f){
            replaceIntercept(intersect, Intersect{this, t.first.value(), getNormal(position)});
        }
    }
    if(t.second.has_value()){
        glm::vec4 position = ray.evaluate(t.second.value());
        if(position.y < 0.5f && position.y > -0.5f){
            replaceIntercept(intersect, Intersect{this, t.second.value(), getNormal(position)});
        }
    }

    // Bottom face, intersect with plane `y = -0.5`
    float tb = (-0.5f - ray.p.y) / ray.d.y;
    glm::vec4 bpos = ray.evaluate(tb);
    if(tb > 0 && (bpos.x * bpos.x + bpos.z * bpos.z) <= 0.25f){
        replaceIntercept(intersect, Intersect{this, tb, objectToWorldNormal(glm::vec3{0, -1, 0}, this)});
    }

    float tt = (0.5f - ray.p.y) / ray.d.y;
    glm::vec4 tpos = ray.evaluate(tt);
    if(tt > 0 && (tpos.x * tpos.x + tpos.z * tpos.z) <= 0.25f){
        replaceIntercept(intersect, Intersect{this, tt, objectToWorldNormal(glm::vec3{0, 1, 0}, this)});
    }

    return intersect;
}

glm::vec3 Cylinder::getNormal(glm::vec4 position) const{
    // Gets the world normal _along the sides of the cylinder_
    glm::vec3 objectNormal{position.x, 0, position.z};
    // Change object normal to world normal.
    return objectToWorldNormal(objectNormal, this);

}

TextureMap Cylinder::getTextureMap(glm::vec4 position) const{
    position = m_ctm_inverse * position;
    if(isClose(position.y, -0.5)){
        // On the bottom face
        return TextureMap{position.x + 0.5f, position.z + 0.5f};
    } else if(isClose(position.y, 0.5)){
        // On the bottom face
        return TextureMap{position.x + 0.5f, position.z + 0.5f};
    } else {
        float theta = std::atan2(position.z, position.x);
        float u = theta < 0 ? (-theta / (2 * PI)) : (1 - theta / (2 * PI));
        float v = position.y + 0.5f;
        return TextureMap{u, v};
    }
}
