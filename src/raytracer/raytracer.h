#pragma once

#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>
#include "utils/rgba.h"
#include "utils/scenedata.h"

struct Ray{
    glm::vec4 p;
    glm::vec4 d;
    glm::vec4 evaluate(float t){
        return p + t * d;
    }
};

struct Texture {
    int width;
    int height;
    std::vector<RGBA> data;
};

// A forward declaration for the RaytraceScene class

class RayTraceScene;

// A class representing a ray-tracer

class RayTracer
{
public:
    struct Config {
        bool enableShadow        = false;
        bool enableReflection    = false;
        bool enableRefraction    = false;
        bool enableTextureMap    = false;
        bool enableTextureFilter = false;
        bool enableParallelism   = false;
        bool enableSuperSample   = false;
        bool enableAcceleration  = false;
        bool enableDepthOfField  = false;
    };

public:
    RayTracer(Config config);

    // Renders the scene synchronously.
    // The ray-tracer will render the scene and fill imageData in-place.
    // @param imageData The pointer to the imageData to be filled.
    // @param scene The scene to be rendered.
    void render(RGBA *imageData, const RayTraceScene &scene);

    RGBA raytrace(Ray ray, const RayTraceScene& scene);

    void loadTexture(const SceneFileMap& fileMap);
    Texture& getTexture(const SceneFileMap& fileMap);
    const Config m_config;
private:

    std::map<const std::string, Texture> m_textures;
};

