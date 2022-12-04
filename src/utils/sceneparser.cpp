#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/fractal.h"
#include "shapes/sphere.h"
#include "shapes/cone.h"
#include "shapes/spherescene.h"

#include <chrono>
#include <memory>
#include <iostream>

glm::mat4 transformationToMatrix(SceneTransformation transformation){
    switch(transformation.type){
        case TransformationType::TRANSFORMATION_MATRIX:
            return transformation.matrix;
        case TransformationType::TRANSFORMATION_ROTATE:
            return glm::rotate(glm::mat4(1), transformation.angle, transformation.rotate);
        case TransformationType::TRANSFORMATION_SCALE:
            return glm::scale(glm::mat4(1), transformation.scale);
        case TransformationType::TRANSFORMATION_TRANSLATE:
            return glm::translate(glm::mat4(1), transformation.translate);
    }
    return glm::mat4(1);
}

Shape* makeShape(ScenePrimitive& primative, glm::mat4 ctm, float minScale){
    switch(primative.type){
        case PrimitiveType::PRIMITIVE_SPHERE:
            return new Sphere(primative, ctm, minScale);
        case PrimitiveType::PRIMITIVE_CYLINDER:
            return new Cylinder(primative, ctm, minScale);
        case PrimitiveType::PRIMITIVE_CUBE:
            return new Cube(primative, ctm, minScale);
        case PrimitiveType::PRIMITIVE_CONE:
            return new Cone(primative, ctm, minScale);
        case PrimitiveType::PRIMITIVE_MESH:
        case PrimitiveType::PRIMITIVE_TORUS:
            throw std::invalid_argument("received unsupported primitive type");
        case PrimitiveType::SPHERE_SCENE:
            return new SphereScene(primative, ctm, minScale);
        case PrimitiveType::PRIMITIVE_FRACTAL:
            return new Fractal(primative, ctm, minScale);
    }
    return NULL;
}

void traverseSceneGraph(SceneNode* node, glm::mat4 mParent, std::vector<Shape*>& shapes, float minScale){
    float currMin = minScale;

    for(int i = 0; i < node->transformations.size(); i++){
        SceneTransformation transformation = *node->transformations[i];

        if (transformation.type == TransformationType::TRANSFORMATION_SCALE) {
            currMin = std::min(currMin,
                               std::min(transformation.scale[0], std::min(transformation.scale[1], transformation.scale[2])));
        }

        mParent *= transformationToMatrix(transformation);
    }

    for(int i = 0; i < node->primitives.size(); i++){
        Shape* shape;

        if (currMin == std::numeric_limits<float>::infinity()) {
            shape = makeShape(*node->primitives[i], mParent, 1.0f);
        } else {
            shape = makeShape(*node->primitives[i], mParent, currMin);
        }

        shapes.emplace_back(shape);
    }

    for(int i = 0; i < node->children.size(); i++){
        traverseSceneGraph(node->children[i], mParent, shapes, currMin);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    renderData.cameraData = fileReader.getCameraData();
    renderData.globalData = fileReader.getGlobalData();
    renderData.lights = fileReader.getLights();

    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();

    traverseSceneGraph(root, glm::mat4(1), renderData.shapes, std::numeric_limits<float>::infinity());

    return true;
}
