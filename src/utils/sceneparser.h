#pragma once

#include "scenedata.h"
#include <memory>
#include <vector>
#include <string>
#include "raytracer/intersect.h"

// Struct which contains data for a single primitive, to be used for rendering
struct RenderShapeData {
    ScenePrimitive primitive;
    glm::mat4 ctm; // the cumulative transformation matrix
    glm::mat4 ctmInverse;
};

// Struct which contains all the data needed to render a scene
struct RenderData {
    SceneGlobalData globalData;
    SceneCameraData cameraData;

    std::vector<SceneLightData> lights;
    std::vector<Shape*> shapes;
    //std::vector<glm::vec3> positionPoints;
    //std::vector<glm::vec3>
};

class SceneParser {
public:
    // Parse the scene and store the results in renderData.
    // @param filepath    The path of the scene file to load.
    // @param renderData  On return, this will contain the metadata of the loaded scene.
    // @return            A boolean value indicating whether the parse was successful.
    static bool parse(std::string filepath, RenderData &renderData);
};

Shape* makeShape(ScenePrimitive& primative, glm::mat4 ctm);

