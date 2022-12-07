struct RayMarchSettings {
    bool enabled = false;
    int maxSteps = 1000;
    float maxDistance = 500.0f;
    bool blendEnabled = false;
    float blendFactor = 0.5f;
    int polyExponent = 2;
    bool multipleBlend = false;
};

extern RayMarchSettings rayMarchSettings; // Defined in raymarchsettings.cpp
