#include "motion.h"
#include "qdir.h"
#include "qimage.h"
#include "qprocess.h"
#include <iostream>
#include <stdio.h>

static const int FRAME_SIZE = 13;

void saveImage(int frameId, QImage& image){

    QString path = QString("frames/frame%0.png").arg(frameId, 5, 10, QLatin1Char('0'));

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

void createVideoFile(int frameRate, std::string outputPath){
    // Call FFMpeg to do a thing
    QString cmd = "ffmpeg";
    QProcess process;
    std::cout << "ffmpeg command: " << cmd.toStdString() << std::endl;
    QStringList args;
    args << "-framerate" << QString::fromStdString(std::to_string(frameRate))
         << "-pattern_type" << "sequence"
         << "-start_number" << "00001"
         << "-i" << "frames\\frame%05d.png"
         << "-c:v" << "libx264"
         << "-pix_fmt" << "yuv420p"
         << QString::fromStdString(outputPath);
    std::cout << "Starting ffmpeg with args:" << args.join(' ').toStdString() << std::endl;
    process.start(cmd, args);
    std::cout << "Waiting for ffmpeg" << std::endl;
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    std::cout << "ffmpeg output: " << output.toStdString() << std::endl;
    process.close();
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
