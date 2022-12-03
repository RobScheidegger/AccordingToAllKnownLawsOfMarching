#include "raymarchfuncs.h"

float sceneSDF(glm::vec3 worldSpacePoint, const RayTraceScene& scene) {
    const std::vector<Shape*>& shapes = scene.getShapes();

    for(const Shape* shape : shapes){
    }
}
