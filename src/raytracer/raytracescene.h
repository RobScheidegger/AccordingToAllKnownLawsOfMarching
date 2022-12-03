#pragma once

#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "camera/camera.h"

// A class representing a scene to be ray-traced
class RayTraceScene
{
private:

    int m_width;
    int m_height;
    RenderData m_renderData;
    Camera m_camera;

public:
    RayTraceScene(int width, int height, const RenderData &metaData);

    // The getter of the width of the scene
    const int& width() const;

    // The getter of the height of the scene
    const int& height() const;

    // The getter of the global data of the scene
    const SceneGlobalData& getGlobalData() const;

    const std::vector<Shape*>& getShapes() const;
    const std::vector<SceneLightData>& getLights() const;

    // The getter of the shared pointer to the camera instance of the scene
    const Camera& getCamera() const;

    void updateTemporalData(const float time);
};


