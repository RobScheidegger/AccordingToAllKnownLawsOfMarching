#include "raytracer.h"
#include "qimage.h"
#include "utils/raymarchsettings.h"
#include "raytracer/lighting.h"
#include "raytracescene.h"
#include "lighting.h"
#include <iostream>

RayTracer::RayTracer(Config config) :
    m_config(config)
{}

/**
 * Clamps the provided float value to the range [0,255]
 */
inline uint8_t clamp(float value){
    return 255 * std::min(1.0f, std::max(0.0f, value));
}

// A factor with which to super sample in *each direction*.
// Ex. 4 indicates 4 options for width/height, so 16 samples total.
const int SUPER_SAMPLE_FACTOR = 4;

/**
 * Makes a ray (in world space) from the eye of the camera to the given pixel index in the scene.
 */
Ray makeRay(const Camera& camera, const RayTraceScene& scene, const int i, const int j){
    // Make a ray that goes to the right point (in camera space).
    float viewplaneHeight = 2 * std::tan(camera.getHeightAngle()/2);
    float aspectRatio = camera.getAspectRatio();
    float viewplaneWidth = viewplaneHeight * aspectRatio;

    float H = scene.height();
    float W = scene.width();

    float x = viewplaneWidth * (((float)i + 0.5f)/W - 0.5f);
    float y = viewplaneHeight * ((H - 0.5f - (float)j)/H - 0.5f);

    // Now find the world direction
    glm::vec4 ppixel {x, y, -1, 1};
    glm::vec4 peye = camera.getPosition();
    glm::mat4 viewMatrix = camera.getViewMatrixInverse();
    glm::vec4 dworld = glm::normalize(viewMatrix * ppixel - peye);

    return Ray{peye, dworld};
}

RGBA RayTracer::raytrace(Ray ray, RayTraceScene& scene){

    const Camera& camera = scene.getCamera();
    const SceneGlobalData& globalData = scene.getGlobalData();
    const std::vector<SceneLightData> lights = scene.getLights();

    std::optional<Intersect> intersection;
    if (rayMarchSettings.enabled) {
        intersection = intersectMarch(scene, ray);
    } else {
        intersection = intersect(scene, ray);
    }

    if(intersection.has_value()){
        Intersect& inter = intersection.value();
        glm::vec4 point = ray.evaluate(inter.t);
        glm::vec4 directionToCamera = camera.getPosition() - point;


        return toRGBA(computePixelLighting(point, glm::vec4{inter.normal, 0}, directionToCamera, inter.shape, 0, scene, *this));
    } else {
        return RGBA{0,0,0};
    }
}

/**
 * Renders the scene to an array representing individual pixel data
 */
void RayTracer::render(RGBA *imageData, RayTraceScene& scene, const float time) {
   // Update temporal data
    scene.updateTemporalData(time);

    const Camera& camera = scene.getCamera();

    #pragma omp parallel for
    for(int i = 0; i < scene.width(); i++){
        for(int j = 0; j < scene.height(); j++){
            /*
            if (i < 253 || j < 394) {
                continue;
            }
            if (i == 253 && j == 394) {
                std::cout << "desired pixel" << std::endl;
            }
            */
            const int idx = j * scene.width() + i;
            Ray ray = makeRay(camera, scene, i, j);
            if(m_config.enableSuperSample){
                // Supersamples the image by perturbing the original ray in each direction,
                // and averaging the results to construct the final pixel.
                float viewplaneHeight = 2 * std::tan(camera.getHeightAngle()/2);
                float aspectRatio = camera.getAspectRatio();
                float viewplaneWidth = viewplaneHeight * aspectRatio;
                float pixelSizeX = viewplaneWidth / (float)scene.width();
                float pixelSizeY = viewplaneHeight / (float)scene.height();

                float racc = 0; float gacc = 0; float bacc = 0;
                for(int x = 0; x < SUPER_SAMPLE_FACTOR; x++){
                    for(int y = 0; y < SUPER_SAMPLE_FACTOR; y++){
                        // Perturb the ray in the correct way
                        float deltax = x - SUPER_SAMPLE_FACTOR / 2;
                        float deltay = y - SUPER_SAMPLE_FACTOR / 2;
                        float xoffset = deltax * pixelSizeX;
                        float yoffset = deltay * pixelSizeY;

                        Ray superSampleRay = Ray{ray.p, glm::vec4{ray.d.x - xoffset, ray.d.y - yoffset, ray.d.z, ray.d.w}};
                        RGBA superSampleValue = raytrace(superSampleRay, scene);

                        racc += (float)superSampleValue.r / 255;
                        bacc += (float)superSampleValue.b / 255;
                        gacc += (float)superSampleValue.g / 255;
                    }
                }
                float points = SUPER_SAMPLE_FACTOR * SUPER_SAMPLE_FACTOR;
                imageData[idx] = RGBA{clamp(racc/points), clamp(gacc/points), clamp(bacc/points), 0};
            } else {
                imageData[idx] = raytrace(ray, scene);
            }
        }
    }
}

void RayTracer::loadTexture(const SceneFileMap& fileMap) {

    std::string path = fileMap.filename;
    if(m_textures.contains(path)){
        // Don't load the same texture twice
        return;
    }

    QImage myImage;
    if (!myImage.load(QString::fromStdString(path))) {
        std::cout<<"Failed to load in image"<<std::endl;
        return;
    }
    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    int width = myImage.width();
    int height = myImage.height();

    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

    std::vector<RGBA> data;

    data.reserve(width * height);
    for (int i = 0; i < arr.size() / 4.f; i++){
        data.push_back(RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]});
    }

    m_textures[path] = Texture{width, height, data};
}

Texture& RayTracer::getTexture(const SceneFileMap &fileMap){
    if(!m_textures.contains(fileMap.filename))
        loadTexture(fileMap);

    return m_textures[fileMap.filename];
}
