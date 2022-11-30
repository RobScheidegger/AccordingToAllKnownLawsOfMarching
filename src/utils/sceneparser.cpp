#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "shapes/cone.h"

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

Shape* makeShape(ScenePrimitive& primative, glm::mat4 ctm){
    switch(primative.type){
        case PrimitiveType::PRIMITIVE_SPHERE:
            return new Sphere(primative, ctm);
        case PrimitiveType::PRIMITIVE_CYLINDER:
            return new Cylinder(primative, ctm);
        case PrimitiveType::PRIMITIVE_CUBE:
            return new Cube(primative, ctm);
        case PrimitiveType::PRIMITIVE_CONE:
            return new Cone(primative, ctm);
        case PrimitiveType::PRIMITIVE_MESH:
        case PrimitiveType::PRIMITIVE_TORUS:
            throw std::invalid_argument("received unsupported primitive type");
    }
    return NULL;
}

void traverseSceneGraph(SceneNode* node, glm::mat4 mParent, std::vector<Shape*>& shapes){
    for(int i = 0; i < node->transformations.size(); i++){
        SceneTransformation transformation = *node->transformations[i];

        mParent *= transformationToMatrix(transformation);
    }

    for(int i = 0; i < node->primitives.size(); i++){
        Shape* shape = makeShape(*node->primitives[i], mParent);
        shapes.emplace_back(shape);
    }

    for(int i = 0; i < node->children.size(); i++){
        traverseSceneGraph(node->children[i], mParent, shapes);
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

    traverseSceneGraph(root, glm::mat4(1), renderData.shapes);

    return true;
}
