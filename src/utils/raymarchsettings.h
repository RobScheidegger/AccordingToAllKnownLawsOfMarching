struct RayMarchSettings {
    bool enabled = false;
    int maxSteps = 1000;
    float maxDistance = 500.0f;
    bool colorBlendEnabled = false;
    bool smoothMergeEnabled = false;
    float mergeFactor = 0.5f;
    int polyExponent = 2;
    bool multipleMerge = false;
};

extern RayMarchSettings rayMarchSettings; // Defined in raymarchsettings.cpp
