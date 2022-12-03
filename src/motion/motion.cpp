#include "motion.h"
#include "qdir.h"
#include "qimage.h"
#include <iostream>
#include <stdio.h>

static const int FRAME_SIZE = 13;

void saveImage(int frameId, QImage& image){
    QString path = QString("frames/frame%05d.png").arg(frameId);

    bool success = image.save(path, "PNG");
    if (!success) {
        std::cerr << "Error: failed to save image to \"" << path.toStdString() << "\"" << std::endl;
    }
}

void computeMotionScene(MotionSettings& settings, RayTracer& raytracer,
                        RayTraceScene& scene, QImage& image){

    int totalFrames = settings.fps * settings.seconds;
    RGBA* data = reinterpret_cast<RGBA *>(image.bits());

    for(int i = 0; i < totalFrames; i++){
        float time = ((float)i) / (float)settings.fps;
        raytracer.render(data, scene, time);

        saveImage(i, image);
        printf("Generated frame %d\n", i);
    }
}

void createVideoFile(std::string outputPath){
    // Call FFMpeg to do a thing
}

void cleanupTemp(){
    QDir dir(TEMP_FRAME_PATH);
    dir.setNameFilters(QStringList() << "*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}
