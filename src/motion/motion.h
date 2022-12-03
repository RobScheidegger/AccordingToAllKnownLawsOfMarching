#pragma once

#include "qimage.h"
#include "raytracer/raytracer.h"
#include "utils/motionsettings.h"
#include "qstring.h"

const static QString TEMP_FRAME_PATH = "frames";

void computeMotionScene(MotionSettings& settings, RayTracer& raytracer,
                        RayTraceScene& scene, QImage& image);

void createVideoFile(std::string outputPath);

void cleanupTemp();
