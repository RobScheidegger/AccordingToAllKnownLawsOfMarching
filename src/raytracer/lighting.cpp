#include "lighting.h"
#include "glm/geometric.hpp"

inline uint8_t clamp(float value){
    return 255 * std::min(1.0f, std::max(0.0f, value));
}

RGBA toRGBA(const glm::vec4 &illumination) {
    return RGBA{clamp(illumination.x), clamp(illumination.y), clamp(illumination.z), 255};
}

glm::vec4 getLightDirection(glm::vec4 point, const SceneLightData& light){
    switch(light.type){
    case LightType::LIGHT_DIRECTIONAL:
        return light.dir;
    case LightType::LIGHT_POINT:
    case LightType::LIGHT_SPOT:
    case LightType::LIGHT_AREA:
        return point - light.pos;
    }
    return glm::vec4{};
}

float getAttenuation(glm::vec4 point, const SceneLightData& light){
    switch(light.type){
    case LightType::LIGHT_DIRECTIONAL:
        return 1.0f;
    case LightType::LIGHT_POINT:
    case LightType::LIGHT_SPOT:
    case LightType::LIGHT_AREA:
        // Compute attenuation based on distance
        float distance = glm::distance(point, light.pos);
        return std::min(1.0f, 1.0f / (light.function.x + distance * light.function.y + distance * distance * light.function.z));
    }
    return 0;
}

inline float fallout(float x, float inner, float outer){
    float xp = (x - inner) / (outer - inner);
    return -2 * std::pow(xp, 3) + 3 * std::pow(xp, 2);
}

const static float AREA_LIGHT_INTERVAL = 0.1;

glm::vec4 getAreaLightIllumination(glm::vec4 point, const SceneLightData& light, RayTracer& raytracer, const RayTraceScene& scene){
    float lightWidthRadius = light.width / 2;
    float lightHeightRadius = light.height / 2;
    glm::vec3 up {0, 1, 0};

    glm::vec3 pos = light.pos;
    glm::vec3 look = light.dir;

    glm::mat4 translate = glm::mat4{
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      -pos.x, -pos.y, -pos.z, 1
    };

    glm::vec3 w = -glm::normalize(look);
    glm::vec3 v = glm::normalize(up - glm::dot(up, w) * w);
    glm::vec3 u = glm::cross(v, w);
    glm::mat4 rotate = glm::mat4{u.x, v.x, w.x, 0,
                                 u.y, v.y, w.y, 0,
                                 u.z, v.z, w.z, 0,
                                 0, 0, 0, 1};
    glm::mat4 lightSpaceCTM = glm::inverse(rotate * translate);

    float illuminationFactor = 0;
    float numPoints = 0;
    for(float widthOffset = -lightWidthRadius; widthOffset < lightWidthRadius; widthOffset += AREA_LIGHT_INTERVAL){
        for(float heightOffset = -lightHeightRadius; heightOffset < lightHeightRadius; heightOffset += AREA_LIGHT_INTERVAL){
            numPoints++;
            // Compute the point on the light we want to trace to
            glm::vec4 offset {0, widthOffset, heightOffset, 1};

            glm::vec4 lightPosition = lightSpaceCTM * offset;

            // Find intersection if possible
            Ray ray {point, lightPosition - point};

            std::optional<Intersect> intersection = intersect(scene, ray);
            // If intersection, continue
            if(intersection.has_value())
                continue;

            // If no intersection, compute attenuation add add to illumination
            float distance = glm::distance(point, lightPosition);
            float attenuation = std::min(1.0f, 1.0f / (light.function.x + distance * light.function.y + distance * distance * light.function.z));

            illuminationFactor += attenuation;
        }
    }
    return light.color * illuminationFactor / numPoints;
}

glm::vec4 getIllumination(glm::vec4 point, const SceneLightData& light, RayTracer& raytracer, const RayTraceScene& scene){
    float attenuation = getAttenuation(point, light);
    switch(light.type){
    case LightType::LIGHT_AREA:
        // Need to trace several beams to different points on the light
        return getAreaLightIllumination(point, light, raytracer, scene);
    case LightType::LIGHT_DIRECTIONAL:
    case LightType::LIGHT_POINT:
        return attenuation * light.color;
    case LightType::LIGHT_SPOT:
        float attenuation = getAttenuation(point, light);
        // Get angle for the point in the spotlight view
        float inner = light.angle - light.penumbra;
        float x = glm::acos(glm::dot(glm::normalize(light.dir), glm::normalize(point - light.pos)));
        if(x <= inner)
            return light.color;

        float outer = light.angle;
        if(x > outer)
            return glm::vec4{0,0,0,0};

        float f = fallout(x, inner, outer);
        return attenuation * (1 - f) * light.color;
    }
    return glm::vec4{};
}

bool inShadow(glm::vec4 point, const SceneLightData& light, const RayTraceScene& scene, RayTracer& raytracer){
    // If have area light (soft shadow), never assume that we are in a shadow (will be handled later).
    if(light.type == LightType::LIGHT_AREA)
        return false;
    glm::vec4 lightDirection = getLightDirection(point, light);
    Ray shadowRay {point, -lightDirection};

    glm::vec3 direction3 = lightDirection;
    float lightLength = glm::length(direction3);

    std::optional<Intersect> shadowResult = intersect(scene, shadowRay);
    glm::vec3 shadowIntersection = shadowRay.evaluate(shadowResult->t);
    glm::vec3 point3 = point;
    float shadowTLength = glm::length(shadowIntersection - point3) / lightLength;
    return shadowResult.has_value() && shadowResult.value().t >= 0.01f  && (light.type == LightType::LIGHT_DIRECTIONAL || shadowTLength <= 1.0f);
}

glm::vec4 getTextureColor(const Shape* shape, glm::vec4 position, const SceneMaterial& material, RayTracer& raytracer, float kd){
    if(!raytracer.m_config.enableTextureMap || material.blend == 0)
        return material.cDiffuse * kd;

    TextureMap map = shape->getTextureMap(position);
    Texture& texture = raytracer.getTexture(material.textureMap);
    int w = texture.width;
    int h = texture.height;

    int c = (int)std::floor(map.u * material.textureMap.repeatU * w) % w;
    int r = (int)std::floor(std::max(1 - map.v, 0.0f) * material.textureMap.repeatV * h) % h;

    const int idx = w * r + c;
    RGBA& textureValue = texture.data[idx];
    glm::vec4 textureColor{
        (float)textureValue.r / 255,
        (float)textureValue.g / 255,
        (float)textureValue.b / 255,
        1,
    };

    return material.blend * textureColor + (1 - material.blend) * material.cDiffuse * kd;
}

const static int RECURSIVE_DEPTH_LIMIT = 5;

// Calculates the RGBA of a pixel from intersection infomation and globally-defined coefficients
SceneColor computePixelLighting(glm::vec4  position,
           glm::vec4  normal,
           glm::vec4  directionToCamera,
           PPShape shape,
           int recursiveDepth,
           const RayTraceScene& scene,
           RayTracer& raytracer) {

    if (shape.isPlural) {
        std::vector<SceneColor> colors;
        for (const Shape* s : shape.shapes) {
            std::vector<float> blends{1.0f};
            std::vector<const Shape*> shapeVec;
            shapeVec.emplace_back(s);

            SceneColor currColor = computePixelLighting(position,
                                                        normal,
                                                        directionToCamera,
                                                        {false, blends, shapeVec},
                                                        recursiveDepth,
                                                        scene,
                                                        raytracer);

            colors.push_back(currColor);
        }

        SceneColor finalSceneColor = glm::vec4(0.0f);
        for (int i = 0; i < colors.size(); i++) {
            finalSceneColor += shape.blends[i] * colors[i];
        }

        return finalSceneColor;
    }

    // Normalizing directions
    normal            = glm::normalize(normal);
    directionToCamera = glm::normalize(directionToCamera);

    // Output illumination (we can ignore opacity)
    glm::vec4 illumination(0, 0, 0, 1);
    const std::vector<SceneLightData>& lights = scene.getLights();
    const SceneGlobalData& globalData = scene.getGlobalData();
    const SceneMaterial& material = shape.shapes[0]->m_primative.material;
    illumination += globalData.ka * material.cAmbient;
    glm::vec4 diffuseColor = getTextureColor(shape.shapes[0], position, material, raytracer, globalData.kd);

    for (const SceneLightData& light : lights) {
        // Check if there is a shadow with the light source (aka if we trace a ray, it can reach the light source)
        if(raytracer.m_config.enableShadow && inShadow(position, light, scene, raytracer))
            continue;

        glm::vec4 direction = getLightDirection(position, light);
        glm::vec4 di = -glm::normalize(direction);
        glm::vec4 ri = 2 * std::max(0.0f, glm::dot(normal, di)) * normal - di;

        // Gets the luminance of light at that point (including attenuation)
        glm::vec4 luminance = getIllumination(position, light, raytracer, scene);
        // If our luminance is very low, just move on to the next light
        // Can give shadowing effect for area lights
        if(isClose(glm::length(luminance), 0))
            continue;

        illumination += luminance * diffuseColor * std::max(0.0f, glm::dot(normal, di));

        illumination += luminance * globalData.ks * material.cSpecular
                * std::max(0.0f, (float)std::pow(glm::dot(glm::normalize(ri), directionToCamera), material.shininess));
    }

    // Reflections
    if(raytracer.m_config.enableReflection && recursiveDepth < RECURSIVE_DEPTH_LIMIT && glm::length(material.cReflective) != 0){
        glm::vec4 reflectedDirection = 2 * std::max(0.0f, glm::dot(normal, directionToCamera)) * normal - directionToCamera;

        Ray reflectedRay{position, reflectedDirection};
        std::optional<Intersect> reflectionIntersect = intersect(scene, reflectedRay);
        if(reflectionIntersect.has_value() && reflectionIntersect.value().t >= 0.01){
            Intersect& inter = reflectionIntersect.value();
            glm::vec4 position = reflectedRay.evaluate(inter.t);
            SceneColor reflectedColor = computePixelLighting(position, glm::vec4{inter.normal, 0}, -reflectedDirection, inter.shape, recursiveDepth + 1, scene, raytracer);
            illumination += material.cReflective * reflectedColor * globalData.ks;
        }
    }

    return illumination;
}
