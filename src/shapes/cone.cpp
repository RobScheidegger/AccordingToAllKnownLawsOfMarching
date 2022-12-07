#include "cone.h"

std::optional<Intersect> Cone::intersect(Ray ray) const{

    // Assumes that ray is now in _object_ space, so we can just intersect with the traditional sphere.
    // Centered at origin with radius 1/2
    std::optional<Intersect> intersect = std::nullopt;

    glm::vec4 d = ray.d;
    glm::vec4 p = ray.p;
    // Conical top
    float A = d.x * d.x + d.z * d.z - 0.25f * d.y * d.y;
    float B = 2.0f * p.x * d.x + 2.0f * p.z * d.z - 0.5f * p.y * d.y + 0.25f * d.y;
    float C = p.x * p.x + p.z * p.z - 0.25 * p.y * p.y + 0.25f * p.y - 1.0f/16.0f;

    std::pair<std::optional<float>, std::optional<float>> t_top = solveQuadratic(A, B, C);

    if(t_top.first.has_value()){
        glm::vec4 top_pos = t_top.first.has_value() ? ray.evaluate(t_top.first.value()) : glm::vec4{};
        if(top_pos.y < 0.5 && top_pos.y > -0.5){
            std::vector<float> blends{1.0f};
            std::vector<const Shape*> shapeVec;
            shapeVec.emplace_back(this);

            replaceIntercept(intersect, Intersect{{false, blends, shapeVec}, t_top.first.value(), getNormal(top_pos)});
        }
    }
    if(t_top.second.has_value()){
        glm::vec4 top_pos = t_top.second.has_value() ? ray.evaluate(t_top.second.value()) : glm::vec4{};
        if(top_pos.y < 0.5 && top_pos.y > -0.5){
            std::vector<float> blends{1.0f};
            std::vector<const Shape*> shapeVec;
            shapeVec.emplace_back(this);

            replaceIntercept(intersect, Intersect{{false, blends, shapeVec}, t_top.second.value(), getNormal(top_pos)});
        }
    }


    float t_bot = (-0.5f - p.y) / d.y;
    if(t_bot > 0){
        glm::vec4 bot_pos = ray.evaluate(t_bot);
        if(bot_pos.x * bot_pos.x + bot_pos.z * bot_pos.z <= 0.25f){
            std::vector<float> blends{1.0f};
            std::vector<const Shape*> shapeVec;
            shapeVec.emplace_back(this);

            replaceIntercept(intersect, Intersect{{false, blends, shapeVec}, t_bot, objectToWorldNormal(glm::vec3{0,-1, 0}, this)});
        }
    }

    return intersect;
}

glm::vec3 Cone::getNormal(glm::vec4 position) const{
    glm::vec3 normal {2.0f * position.x, 0.25f - 0.5f * position.y, 2.0f * position.z};
    return objectToWorldNormal(normal, this);
}

float Cone::shapeSDF(glm::vec4 position) const {
    float coneSideLen = std::sqrt(std::pow(m_radius, 2) + std::pow(m_height, 2));
    float sinVal = m_height / coneSideLen;
    float cosVal = m_radius / coneSideLen;

    float q = length(glm::vec2(position[0], position[2]));
    return std::max(glm::dot(glm::vec2(sinVal, cosVal), glm::vec2(q, (position[1] - m_height / 2.0f))), (-1.0f * m_height) - (position[1] - m_height / 2.0f));
}

TextureMap Cone::getTextureMap(glm::vec4 position) const{
    position = m_ctm_inverse * position;
    if(isClose(position.y, -0.5)){
        // On the bottom face
        return TextureMap{position.x + 0.5f, position.z + 0.5f};
    } else {
        float theta = std::atan2(position.z, position.x);
        float u = theta < 0 ? (-theta / (2 * PI)) : (1 - theta / (2 * PI));
        float v = position.y + 0.5f;
        return TextureMap{u, v};
    }
}
