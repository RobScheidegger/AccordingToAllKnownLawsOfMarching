#include "raytracer/raytracer.h"
#include "raytracer/shape.h"
#include "cube.h"
#include "raytracer/intersect.h"

std::optional<Intersect> Cube::intersect(Ray ray) const{

    // Assumes that ray is now in _object_ space, so we can just intersect with the traditional sphere.
    // Centered at origin with radius 1/2
    std::optional<Intersect> intersect = std::nullopt;

    // Top face, y = 1/2
    float t_top = (0.5f - ray.p.y) / ray.d.y;
    glm::vec4 top_pos = ray.evaluate(t_top);
    if(abs(top_pos.x) < 0.5 && abs(top_pos.z) < 0.5){
        replaceIntercept(intersect, Intersect{this, t_top, objectToWorldNormal(glm::vec3{0, 1, 0}, this)});
    }

    // Bottom face, y = -1/2
    float t_bottom = (-0.5f - ray.p.y) / ray.d.y;
    glm::vec4 bottom_pos = ray.evaluate(t_bottom);
    if(abs(bottom_pos.x) < 0.5 && abs(bottom_pos.z) < 0.5){
        replaceIntercept(intersect, Intersect{this, t_bottom, objectToWorldNormal(glm::vec3{0, -1, 0}, this)});
    }

    // Left face, z = -1/2
    float t_left = (-0.5f - ray.p.z) / ray.d.z;
    glm::vec4 left_pos = ray.evaluate(t_left);
    if(abs(left_pos.x) < 0.5 && abs(left_pos.y) < 0.5){
        replaceIntercept(intersect, Intersect{this, t_left, objectToWorldNormal(glm::vec3{0, 0, -1}, this)});
    }


    // Right face, z = 1/2
    float t_right = (0.5f - ray.p.z) / ray.d.z;
    glm::vec4 right_pos = ray.evaluate(t_right);
    if(abs(right_pos.x) < 0.5 && abs(right_pos.y) < 0.5){
        replaceIntercept(intersect, Intersect{this, t_right, objectToWorldNormal(glm::vec3{0, 0, 1}, this)});
    }

    // Front face, x = 1/2
    float t_front = (0.5f - ray.p.x) / ray.d.x;
    glm::vec4 front_pos = ray.evaluate(t_front);
    if(abs(front_pos.z) < 0.5 && abs(front_pos.y) < 0.5){
        replaceIntercept(intersect, Intersect{this, t_front, objectToWorldNormal(glm::vec3{1, 0, 0}, this)});
    }

    // Back face, x = -1/2
    float t_back = (-0.5f - ray.p.x) / ray.d.x;
    glm::vec4 back_pos = ray.evaluate(t_back);
    if(abs(back_pos.z) < 0.5 && abs(back_pos.y) < 0.5){
        replaceIntercept(intersect, Intersect{this, t_back, objectToWorldNormal(glm::vec3{-1, 0, 0}, this)});
    }

    return intersect;
}

glm::vec3 Cube::getNormal(glm::vec4 position) const{
    position = m_ctm_inverse * position;
    if(isClose(position.z, -0.5)){
        return objectToWorldNormal(glm::vec3{0,0,-1}, this);
    }
    else if(isClose(position.z, 0.5)){
         return objectToWorldNormal(glm::vec3{0,0,1}, this);
    }
    else if(isClose(position.y, 0.5)){
         return objectToWorldNormal(glm::vec3{0,1,0}, this);
    }
    else if(isClose(position.y, -0.5)){
        return objectToWorldNormal(glm::vec3{0,-1,0}, this);
    }
    else if(isClose(position.x, 0.5)){
        return objectToWorldNormal(glm::vec3{1,0,0}, this);
    }
    else if(isClose(position.x, -0.5)){
        return objectToWorldNormal(glm::vec3{-1,0,0}, this);
    }

    // Should never have this case, assuming we are on the surface of the object
    return glm::vec3{0,0,0};
}

float Cube::shapeSDF(glm::vec4 position) const {
    glm::vec3 q = abs(glm::vec3(position)) - sideLengths;
    return length(glm::max(q, glm::vec3(0.0))) + std::min(std::max(q[0],std::max(q[1],q[2])), 0.f);
}

TextureMap Cube::getTextureMap(glm::vec4 position) const{
    position = m_ctm_inverse * position;
    if(isClose(position.z, -0.5)){
        return TextureMap{1 - (position.x + 0.5f), position.y + 0.5f};
    }
    if(isClose(position.z, 0.5)){
        return TextureMap{position.x + 0.5f, position.y + 0.5f};
    }
    if(isClose(position.y, 0.5)){
        return TextureMap{position.x + 0.5f, 1 - (position.z + 0.5f)};
    }
    if(isClose(position.y, -0.5)){
        return TextureMap{position.x + 0.5f, position.z + 0.5f};
    }
    if(isClose(position.x, 0.5)){
        return TextureMap{1 - (position.z + 0.5f), position.y + 0.5f};
    }
    if(isClose(position.x, -0.5)){
        return TextureMap{position.z + 0.5f, position.y + 0.5f};
    }

    // Should never have this case, assuming we are on the surface of the object
    return TextureMap{0, 0};
}
