#pragma once

struct GLFWwindow;

namespace pulsar {
 
struct AppState;

void registerWindowCallbacks(GLFWwindow* window, AppState& app);
void applySpaceStyle();
void drawImGuiPanel(AppState& app);

}  // namespace pulsar
