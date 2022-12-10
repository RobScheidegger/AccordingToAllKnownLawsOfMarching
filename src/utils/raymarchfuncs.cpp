#include "raymarchfuncs.h"
#include "utils/raymarchsettings.h"

SDFResult minUnion(std::vector<float>& shapeSDFs, const std::vector<Shape*>& shapes) {
    float minDist = std::numeric_limits<float>::infinity();
    const Shape* minDistShape = nullptr;

    for (int i = 0; i < shapeSDFs.size(); i++) {
        if (shapeSDFs[i] < minDist) {
            minDist = shapeSDFs[i];
            minDistShape = shapes[i];
        }
    }

    std::vector<float> blends{1.0f};
    std::vector<const Shape*> shapeVec;
    shapeVec.emplace_back(minDistShape);

    return {{false, blends, shapeVec}, minDist};
}

// Return value: first value is smooth min, second value is blend factor
glm::vec2 smoothPolyMin2(float dist1, float dist2, float smoothFactor, float n) {
    float h = std::max(smoothFactor - abs(dist1 - dist2), 0.0f) / smoothFactor;
    float m = pow(h, n) * 0.5;
    float s = m * smoothFactor / n;

    return (dist1 < dist2) ? glm::vec2(dist1 - s, m) : glm::vec2(dist2 - s, 1.0 - m);
}

SDFResult smoothPolyMinPair(std::vector<float>& shapeSDFs, const std::vector<Shape*>& shapes) {
    float minDist = std::numeric_limits<float>::infinity();
    float secondMinDist = std::numeric_limits<float>::infinity();
    const Shape* minDistShape = nullptr;
    const Shape* secondMinDistShape = nullptr;

    for (int i = 0; i < shapeSDFs.size(); i++) {
        if (shapeSDFs[i] < minDist) {
            secondMinDist = minDist;
            secondMinDistShape = minDistShape;

            minDist = shapeSDFs[i];
            minDistShape = shapes[i];
        } else if (shapeSDFs[i] < secondMinDist) {
            secondMinDist = shapeSDFs[i];
            secondMinDistShape = shapes[i];
        }
    }

    if (secondMinDistShape != nullptr) {
        glm::vec2 blend = smoothPolyMin2(minDist,
                                         secondMinDist,
                                         rayMarchSettings.mergeFactor,
                                         rayMarchSettings.polyExponent);

        std::vector<float> blends{1.0f - blend[1], blend[1]};
        std::vector<const Shape*> shapeVec;
        shapeVec.emplace_back(minDistShape);
        shapeVec.emplace_back(secondMinDistShape);

        return {{true, blends, shapeVec}, blend[0]};
    } else {
        std::vector<float> blends{1.0f};
        std::vector<const Shape*> shapeVec;
        shapeVec.emplace_back(minDistShape);

        return {{false, blends, shapeVec}, minDist};
    }
}

SDFResult smoothPolyMinMultiple(std::vector<float>& shapeSDFs, const std::vector<Shape*>& shapes) {
    if (shapeSDFs.size() == 1) {
        // If only 1 element, no blending needed regardless
        std::vector<float> blends{1.0f};
        std::vector<const Shape*> shapeVec;
        shapeVec.push_back(shapes[0]);
        return {{false, blends, shapeVec}, shapeSDFs[0]};
    }

    std::vector<std::pair<float, const Shape*>> shapeDists;

    // Sort the shapes by distance (ascending order)
    for (int i = 0; i < shapeSDFs.size(); i++) {
        shapeDists.push_back({shapeSDFs[i], shapes[i]});
    }

    // Default sort is ascending order by first element;
    // reverse to get descending order
    sort(shapeDists.begin(), shapeDists.end());
    // reverse(shapeDists.begin(), shapeDists.end());

    std::vector<float> blends;
    std::vector<const Shape*> shapeVec;
    shapeVec.push_back(shapeDists[0].second);

    // Merge in sorted order
    float currDist = shapeDists[0].first;
    for (int i = 1; i < shapeDists.size(); i++) {
        glm::vec2 blend = smoothPolyMin2(currDist,
                               shapeDists[i].first,
                               rayMarchSettings.mergeFactor,
                               rayMarchSettings.polyExponent);
        currDist = blend[0];
        shapeVec.push_back(shapeDists[i].second);

        // If no color blending, use the color of the closest object only
        if (rayMarchSettings.colorBlendEnabled) {
            blends.push_back(1.0f - blend[1]);
        }
    }

    // If blend is not enabled, we can remove all of the other shapes.
    if (!rayMarchSettings.colorBlendEnabled) {
        std::vector<float> singleBlend{1.0f};
        std::vector<const Shape*> singleShapeVec;
        singleShapeVec.push_back(shapeVec[0]);

        return {{true, singleBlend, singleShapeVec}, currDist};
    }

    // Merge the blends
    std::vector<float> finalBlends;
    float sum = 0.0f;
    float currBlend = 1.0f;
    for (int i = 0; i < blends.size(); i++) {
        finalBlends.push_back(blends[i] * currBlend);
        sum += blends[i] * currBlend;
        currBlend *= 1.0f - blends[i];
    }
    finalBlends.push_back(1.0f - sum);

    return {{true, finalBlends, shapeVec}, currDist};

}

SDFResult sceneSDF(glm::vec4 worldSpacePoint, const RayTraceScene& scene) {
    const std::vector<Shape*>& shapes = scene.getShapes();
    std::vector<float> shapeSDFs;

    for(const Shape* shape : shapes){
        glm::vec4 objectSpacePos = shape->m_ctm_inverse * worldSpacePoint;
        // NOTE: must scale the distance by the minimum scale factor in the CTM to avoid stepping over the shape
        shapeSDFs.push_back( shape->shapeSDF( objectSpacePos) * shape->m_minScale );
    }

    if (rayMarchSettings.smoothMergeEnabled) {
        if (rayMarchSettings.multipleMerge) {
            return smoothPolyMinMultiple(shapeSDFs, shapes);
        } else {
            return smoothPolyMinPair(shapeSDFs, shapes);
        }
    } else {
        return minUnion(shapeSDFs, shapes);
    }
}

glm::vec3 worldSpaceNormal(glm::vec4 worldSpacePoint, const RayTraceScene& scene) {
    const float smallStep = 0.01;

    float gradient_x = sceneSDF(worldSpacePoint + glm::vec4(smallStep, 0.0f, 0.0f, 0.0f), scene).sceneSDFVal
            - sceneSDF(worldSpacePoint - glm::vec4(smallStep, 0.0f, 0.0f, 0.0f), scene).sceneSDFVal;
    float gradient_y = sceneSDF(worldSpacePoint + glm::vec4(0.0f, smallStep, 0.0f, 0.0f), scene).sceneSDFVal
            - sceneSDF(worldSpacePoint - glm::vec4(0.0f, smallStep, 0.0f, 0.0f), scene).sceneSDFVal;
    float gradient_z = sceneSDF(worldSpacePoint + glm::vec4(0.0f, 0.0f, smallStep, 0.0f), scene).sceneSDFVal
            - sceneSDF(worldSpacePoint - glm::vec4(0.0f, 0.0f, smallStep, 0.0f), scene).sceneSDFVal;

    glm::vec3 normal = glm::vec3(gradient_x, gradient_y, gradient_z);

    return glm::normalize(normal);
}
