#include <stdexcept>
#include "raytracescene.h"
#include "camera/camera.h"
#include "utils/sceneparser.h"

RayTraceScene::RayTraceScene(int width, int height, const RenderData& metaData): m_camera{metaData.cameraData, height, width} {
    m_renderData = metaData;
    m_width = width;
    m_height = height;
}

const int& RayTraceScene::width() const {
    return m_width;
}

const int& RayTraceScene::height() const {
    return m_height;
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    return m_renderData.globalData;
}

const Camera& RayTraceScene::getCamera() const {
    return m_camera;
}

const std::vector<Shape*>& RayTraceScene::getShapes() const {
    return m_renderData.shapes;
}

const std::vector<SceneLightData>& RayTraceScene::getLights() const {
    return m_renderData.lights;
}

void RayTraceScene::updateTemporalData(const float time){
    // Update camera position and rotation
    m_camera.update(time);

    // LATER: Update shape CTMs if they are temporal
}
