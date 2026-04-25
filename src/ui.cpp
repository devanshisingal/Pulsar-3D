#include "pulsar/ui.hpp"

#include "pulsar/app_state.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "imgui.h"

namespace pulsar {

namespace {

struct SciencePreset {
    const char* name;
    float spinPeriod;
    float magneticAxisTiltDeg;
    float jetDensity;
    float fieldStrength;
    float pulseBrightness;
};

constexpr SciencePreset kSciencePresets[] = {
    {"Custom", 2.2f, 0.0f, 1.0f, 1.0f, 1.0f},
    {"Crab Pulsar", 0.033f, 45.0f, 1.3f, 1.1f, 1.6f},
    {"Vela Pulsar", 0.089f, 63.0f, 1.1f, 1.0f, 1.35f},
    {"Millisecond", 0.005f, 18.0f, 0.9f, 0.85f, 1.2f},
    {"Magnetar-like", 7.5f, 32.0f, 0.7f, 1.8f, 1.9f}
};

void applySciencePreset(AppState& app, int presetIndex) {
    const SciencePreset& preset = kSciencePresets[presetIndex];
    app.presetIndex = presetIndex;
    app.spinPeriod = preset.spinPeriod;
    app.magneticAxisTiltDeg = preset.magneticAxisTiltDeg;
    app.jetDensity = preset.jetDensity;
    app.fieldStrength = preset.fieldStrength;
    app.pulseBrightness = preset.pulseBrightness;

    app.uiSpinPeriod = preset.spinPeriod;
    app.uiMagneticAxisTiltDeg = preset.magneticAxisTiltDeg;
    app.uiJetDensity = preset.jetDensity;
    app.uiFieldStrength = preset.fieldStrength;
    app.uiPulseBrightness = preset.pulseBrightness;
    app.syncScienceState();
}

AppState* getApp(GLFWwindow* window) {
    return static_cast<AppState*>(glfwGetWindowUserPointer(window));
}

void framebufferSizeCB(GLFWwindow* window, int w, int h) {
    AppState* app = getApp(window);
    if (app == nullptr) {
        return;
    }
    app->width = w;
    app->height = h;
    glViewport(0, 0, w, h);
}

void mouseButtonCB(GLFWwindow* window, int button, int action, int) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    AppState* app = getApp(window);
    if (app == nullptr) {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        app->cam.dragging = (action == GLFW_PRESS);
        glfwGetCursorPos(window, &app->cam.lastX, &app->cam.lastY);
    }
}

void cursorPosCB(GLFWwindow* window, double x, double y) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    AppState* app = getApp(window);
    if (app == nullptr || !app->cam.dragging) {
        return;
    }

    double dx = x - app->cam.lastX;
    double dy = y - app->cam.lastY;
    app->cam.lastX = x;
    app->cam.lastY = y;
    app->cam.yaw -= static_cast<float>(dx * 0.005f);
    app->cam.pitch += static_cast<float>(dy * 0.005f);
    app->cam.pitch = glm::clamp(app->cam.pitch, -1.5f, 1.5f);
}

void scrollCB(GLFWwindow* window, double, double dy) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    AppState* app = getApp(window);
    if (app == nullptr) {
        return;
    }

    app->cam.dist -= static_cast<float>(dy) * 0.3f;
    app->cam.dist = glm::clamp(app->cam.dist, 0.8f, 1400.0f);
}

void keyCB(GLFWwindow* window, int key, int, int action, int) {
    if (ImGui::GetIO().WantCaptureKeyboard || action != GLFW_PRESS) {
        return;
    }

    AppState* app = getApp(window);
    if (app == nullptr) {
        return;
    }

    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_SPACE:
            app->rotating = !app->rotating;
            break;
        case GLFW_KEY_P:
            app->showJets = !app->showJets;
            break;
        case GLFW_KEY_M:
            app->showField = !app->showField;
            break;
        case GLFW_KEY_G:
            app->showGrid = !app->showGrid;
            break;
        case GLFW_KEY_R:
            app->showAxis = !app->showAxis;
            break;
        case GLFW_KEY_EQUAL:
        case GLFW_KEY_KP_ADD:
            app->uiTimeScale = glm::clamp(app->uiTimeScale + 0.1f, 0.25f, 3.0f);
            app->timeScale = app->uiTimeScale;
            app->presetIndex = 0;
            break;
        case GLFW_KEY_MINUS:
        case GLFW_KEY_KP_SUBTRACT:
            app->uiTimeScale = glm::clamp(app->uiTimeScale - 0.1f, 0.25f, 3.0f);
            app->timeScale = app->uiTimeScale;
            app->presetIndex = 0;
            break;
        case GLFW_KEY_UP:
            app->uiMagneticAxisTiltDeg = glm::clamp(app->uiMagneticAxisTiltDeg + 5.0f, 0.0f, 100.0f);
            app->magneticAxisTiltDeg = app->uiMagneticAxisTiltDeg;
            app->presetIndex = 0;
            app->syncScienceState();
            break;
        case GLFW_KEY_DOWN:
            app->uiMagneticAxisTiltDeg = glm::clamp(app->uiMagneticAxisTiltDeg - 5.0f, 0.0f, 100.0f);
            app->magneticAxisTiltDeg = app->uiMagneticAxisTiltDeg;
            app->presetIndex = 0;
            app->syncScienceState();
            break;
        default:
            break;
    }
}

}  // namespace

void registerWindowCallbacks(GLFWwindow* window, AppState& app) {
    glfwSetWindowUserPointer(window, &app);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCB);
    glfwSetMouseButtonCallback(window, mouseButtonCB);
    glfwSetCursorPosCallback(window, cursorPosCB);
    glfwSetScrollCallback(window, scrollCB);
    glfwSetKeyCallback(window, keyCB);
}

void applySpaceStyle() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 6.0f;
    s.FrameRounding = 4.0f;
    s.GrabRounding = 4.0f;
    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize = 0.0f;
    s.WindowPadding = ImVec2(12.0f, 10.0f);
    s.ItemSpacing = ImVec2(8.0f, 6.0f);
    s.GrabMinSize = 10.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.10f, 0.88f);
    c[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.14f, 1.00f);
    c[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.20f, 1.00f);
    c[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.14f, 0.22f, 1.00f);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.32f, 1.00f);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.38f, 1.00f);
    c[ImGuiCol_SliderGrab] = ImVec4(0.27f, 0.75f, 0.40f, 1.00f);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.90f, 0.50f, 1.00f);
    c[ImGuiCol_Button] = ImVec4(0.15f, 0.45f, 0.80f, 1.00f);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.58f, 0.95f, 1.00f);
    c[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.35f, 0.70f, 1.00f);
    c[ImGuiCol_CheckMark] = ImVec4(0.27f, 0.85f, 0.45f, 1.00f);
    c[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.28f, 1.00f);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.22f, 0.38f, 1.00f);
    c[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.45f, 0.60f);
    c[ImGuiCol_Text] = ImVec4(0.90f, 0.92f, 1.00f, 1.00f);
    c[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.48f, 0.58f, 1.00f);
    c[ImGuiCol_Border] = ImVec4(0.28f, 0.30f, 0.48f, 0.70f);
}

void drawImGuiPanel(AppState& app) {
    ImGuiIO& io = ImGui::GetIO();
    const float panelW = 210.0f;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - panelW - 10.0f, 10.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelW, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.85f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::Begin("##controls", nullptr, flags)) {
        ImGui::TextDisabled("Playback Rate:");
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat("##timescale", &app.uiTimeScale, 0.25f, 3.0f, "%.2fx")) {
            app.timeScale = app.uiTimeScale;
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::SeparatorText("Science Panel");

        int presetIndex = app.presetIndex;
        if (ImGui::Combo("Preset", &presetIndex, "Custom\0Crab Pulsar\0Vela Pulsar\0Millisecond\0Magnetar-like\0")) {
            applySciencePreset(app, presetIndex);
        }

        if (ImGui::SliderFloat("Spin Period (s)", &app.uiSpinPeriod, 0.005f, 8.0f, "%.3f")) {
            app.spinPeriod = app.uiSpinPeriod;
            app.presetIndex = 0;
        }

        if (ImGui::SliderFloat("Magnetic Axis Tilt", &app.uiMagneticAxisTiltDeg, 0.0f, 100.0f, "%.0f deg")) {
            app.magneticAxisTiltDeg = app.uiMagneticAxisTiltDeg;
            app.presetIndex = 0;
            app.syncScienceState();
        }

        if (ImGui::SliderFloat("Jet Density", &app.uiJetDensity, 0.25f, 1.5f, "%.2f")) {
            app.jetDensity = app.uiJetDensity;
            app.presetIndex = 0;
        }

        if (ImGui::SliderFloat("Field Strength", &app.uiFieldStrength, 0.4f, 2.0f, "%.2f")) {
            app.fieldStrength = app.uiFieldStrength;
            app.presetIndex = 0;
            app.fieldDirty = true;
        }

        if (ImGui::SliderFloat("Pulse Brightness", &app.uiPulseBrightness, 0.2f, 2.2f, "%.2f")) {
            app.pulseBrightness = app.uiPulseBrightness;
            app.presetIndex = 0;
        }

        ImGui::TextDisabled("Approx pulse freq: %.2f Hz", 1.0f / app.spinPeriod);

        ImGui::Spacing();
        ImGui::SeparatorText("Rendering");

        ImGui::TextDisabled("Ambient Light:");
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat("##ambient", &app.uiAmbient, 0.1f, 2.5f, "")) {
            app.ambientStr = app.uiAmbient;
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::TextDisabled("Specular:");
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat("##specular", &app.uiSpecular, 0.0f, 1.5f, "")) {
            app.specularStr = app.uiSpecular;
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::TextDisabled("Shininess:");
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat("##shininess", &app.uiShininess, 4.0f, 96.0f, "%.0f")) {
            app.shininess = app.uiShininess;
        }
        ImGui::PopItemWidth();

        ImGui::Spacing();

        ImGui::TextDisabled("Emissive Hotspots:");
        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat("##emissive", &app.uiEmissive, 0.0f, 3.0f, "")) {
            app.emissiveStr = app.uiEmissive;
        }
        ImGui::PopItemWidth();

        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Radiation Jets  [P]", &app.showJets);
        ImGui::Checkbox("Rotation Axis   [R]", &app.showAxis);
        ImGui::Checkbox("Magnetic Field  [M]", &app.showField);
        ImGui::Checkbox("Reference Grid  [G]", &app.showGrid);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        const float btnW = (panelW - 36.0f) * 0.5f;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.15f, 0.15f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.25f, 0.25f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.55f, 0.10f, 0.10f, 1.00f));
        if (ImGui::Button("Reset", ImVec2(btnW, 0.0f))) {
            app.reset();
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.60f, 0.25f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.78f, 0.35f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.45f, 0.18f, 1.00f));
        const char* playLabel = app.rotating ? "Pause [Spc]" : "Play  [Spc]";
        if (ImGui::Button(playLabel, ImVec2(btnW, 0.0f))) {
            app.rotating = !app.rotating;
        }
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.42f, 0.55f, 1.0f));
        ImGui::SetWindowFontScale(0.80f);
        ImGui::TextWrapped("+/- Playback  Arrows Tilt\nDrag Orbit  Scroll Zoom");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
    }
    ImGui::End();
}

}  // namespace pulsar
