#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
 
#include "pulsar/camera.hpp"

namespace pulsar {

struct AppState {
    Camera cam;

    bool rotating = false;
    float inclination = 0.0f;
    float rotY = glm::pi<float>();
    bool showJets = false;
    bool showField = false;
    bool showGrid = false;
    bool showAxis = false;
    bool fieldDirty = true;

    float ambientStr = 1.25f;
    float specularStr = 0.65f;
    float shininess = 48.0f;
    float emissiveStr = 1.35f;

    float timeScale = 1.0f;
    float magneticAxisTiltDeg = 0.0f;
    float spinPeriod = 2.2f;
    float jetDensity = 1.0f;
    float fieldStrength = 1.0f;
    float pulseBrightness = 1.0f;
    int presetIndex = 0;

    float uiTimeScale = 1.0f;
    float uiSpinPeriod = 2.2f;
    float uiMagneticAxisTiltDeg = 0.0f;
    float uiJetDensity = 1.0f;
    float uiFieldStrength = 1.0f;
    float uiPulseBrightness = 1.0f;
    float uiAmbient = 1.25f;
    float uiSpecular = 0.65f;
    float uiShininess = 48.0f;
    float uiEmissive = 1.35f;

    int cameraPresetIndex = 0;
    bool autoOrbit = false;
    float autoOrbitSpeed = 0.2f;
    float uiAutoOrbitSpeed = 0.2f;

    int width = 1280;
    int height = 600;

    void syncScienceState() {
        inclination = glm::radians(magneticAxisTiltDeg);
        fieldDirty = true;
    }

    void reset() {
        rotating = false;
        inclination = 0.0f;
        rotY = glm::pi<float>();
        showJets = false;
        showField = false;
        showGrid = false;
        showAxis = false;
        fieldDirty = true;

        ambientStr = 1.25f;
        specularStr = 0.65f;
        shininess = 48.0f;
        emissiveStr = 1.35f;

        timeScale = 1.0f;
        magneticAxisTiltDeg = 0.0f;
        spinPeriod = 2.2f;
        jetDensity = 1.0f;
        fieldStrength = 1.0f;
        pulseBrightness = 1.0f;
        presetIndex = 0;

        uiTimeScale = 1.0f;
        uiSpinPeriod = 2.2f;
        uiMagneticAxisTiltDeg = 0.0f;
        uiJetDensity = 1.0f;
        uiFieldStrength = 1.0f;
        uiPulseBrightness = 1.0f;
        uiAmbient = 1.25f;
        uiSpecular = 0.65f;
        uiShininess = 48.0f;
        uiEmissive = 1.35f;

        cameraPresetIndex = 0;
        autoOrbit = false;
        autoOrbitSpeed = 0.2f;
        uiAutoOrbitSpeed = 0.2f;

        cam = Camera{};
        syncScienceState();
    }
};

}  // namespace pulsar
