#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "pulsar/app_state.hpp"
#include "pulsar/render_utils.hpp"
#include "pulsar/scene.hpp"
#include "pulsar/ui.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main() {
    pulsar::AppState app;

    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(app.width, app.height, "Pulsar 3D - OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    pulsar::registerWindowCallbacks(window, app);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    pulsar::applySpaceStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    GLuint pulsarProg = pulsar::buildProgram(pulsar::kVertexShaderSrc, pulsar::kPulsarFragSrc);
    GLuint glowProg = pulsar::buildProgram(pulsar::kVertexShaderSrc, pulsar::kGlowFragSrc);
    GLuint lineProg = pulsar::buildProgram(pulsar::kLineVertSrc, pulsar::kLineFragSrc);
    GLuint skyboxProg = pulsar::buildProgram(pulsar::kSkyboxVertSrc, pulsar::kSkyboxFragSrc);

    std::vector<pulsar::Vertex> sphereVerts;
    std::vector<unsigned int> sphereIdx;
    pulsar::generateSphere(0.5f, 64, 64, sphereVerts, sphereIdx);
    GLuint sphereVbo = 0;
    GLuint sphereEbo = 0;
    GLuint sphereVao = pulsar::uploadSphere(sphereVerts, sphereIdx, sphereVbo, sphereEbo);
    GLuint starTex = pulsar::loadTexture("images/map.jpg");

    std::vector<pulsar::Vertex> glowVerts;
    std::vector<unsigned int> glowIdx;
    pulsar::generateSphere(0.52f, 32, 32, glowVerts, glowIdx);
    GLuint glowVbo = 0;
    GLuint glowEbo = 0;
    GLuint glowVao = pulsar::uploadSphere(glowVerts, glowIdx, glowVbo, glowEbo);

    std::vector<std::string> skyFaces = {
        "skybox/right.jpg",
        "skybox/left.jpg",
        "skybox/top.jpg",
        "skybox/bottom.jpg",
        "skybox/front.jpg",
        "skybox/back.jpg"
    };
    GLuint cubemapTex = pulsar::loadCubemap(skyFaces);

    float skyVerts[] = {
        -1, 1, -1,  -1, -1, -1,  1, -1, -1,  1, -1, -1,  1, 1, -1,  -1, 1, -1,
        -1, -1, 1,  -1, -1, -1,  -1, 1, -1,  -1, 1, -1,  -1, 1, 1,  -1, -1, 1,
        1, -1, -1,  1, -1, 1,  1, 1, 1,  1, 1, 1,  1, 1, -1,  1, -1, -1,
        -1, -1, 1,  -1, 1, 1,  1, 1, 1,  1, 1, 1,  1, -1, 1,  -1, -1, 1,
        -1, 1, -1,  1, 1, -1,  1, 1, 1,  1, 1, 1,  -1, 1, 1,  -1, 1, -1,
        -1, -1, -1,  -1, -1, 1,  1, -1, -1,  1, -1, -1,  -1, -1, 1,  1, -1, 1
    };
    GLuint skyVao = 0;
    GLuint skyVbo = 0;
    glGenVertexArrays(1, &skyVao);
    glGenBuffers(1, &skyVbo);
    glBindVertexArray(skyVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerts), skyVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::mt19937 rng(42);
    pulsar::JetSystem jet1;
    pulsar::JetSystem jet2;
    jet1.init(1, rng);
    jet2.init(-1, rng);

    pulsar::FieldLines field;
    field.build(app.fieldStrength, app.magneticAxisTiltDeg);
    pulsar::Grid grid;
    grid.build(30.0f, 50);
    pulsar::AxisArrows axis;
    axis.build();

    double prevTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = static_cast<float>(now - prevTime);
        prevTime = now;

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        jet1.setDensity(app.jetDensity);
        jet2.setDensity(app.jetDensity);
        if (app.fieldDirty) {
            field.build(app.fieldStrength, app.magneticAxisTiltDeg);
            app.fieldDirty = false;
        }

        if (app.rotating) {
            app.rotY -= glm::two_pi<float>() * dt * app.timeScale / app.spinPeriod;
        }
        if (app.showJets) {
            jet1.update(dt);
            jet2.update(dt);
        }

        float aspect = static_cast<float>(app.width) / static_cast<float>(std::max(app.height, 1));
        glm::mat4 view = app.cam.view();
        glm::mat4 proj = app.cam.projection(aspect);
        glm::vec3 camPos = app.cam.position();
        glm::vec3 lightPos(3.2f, 2.3f, 2.4f);
        glm::vec3 lightColor(0.95f, 0.98f, 1.0f);

        glm::mat4 pulsarModel(1.0f);
        pulsarModel = glm::rotate(pulsarModel, app.rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        pulsarModel = glm::rotate(pulsarModel, app.inclination, glm::vec3(0.0f, 0.0f, 1.0f));

        glViewport(0, 0, app.width, app.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxProg);

        glm::mat4 skyView = glm::mat4(glm::mat3(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxProg, "view"), 1, GL_FALSE, glm::value_ptr(skyView));
        glUniformMatrix4fv(glGetUniformLocation(skyboxProg, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform1i(glGetUniformLocation(skyboxProg, "skybox"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
        glBindVertexArray(skyVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glUseProgram(pulsarProg);
        glUniformMatrix4fv(glGetUniformLocation(pulsarProg, "model"), 1, GL_FALSE, glm::value_ptr(pulsarModel));
        glUniformMatrix4fv(glGetUniformLocation(pulsarProg, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(pulsarProg, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform1f(glGetUniformLocation(pulsarProg, "ambientStrength"), app.ambientStr);
        glUniform1f(glGetUniformLocation(pulsarProg, "specularStrength"), app.specularStr);
        glUniform1f(glGetUniformLocation(pulsarProg, "shininess"), app.shininess);
        glUniform1f(glGetUniformLocation(pulsarProg, "emissiveStrength"), app.emissiveStr * app.pulseBrightness);
        glUniform1f(glGetUniformLocation(pulsarProg, "time"), static_cast<float>(now));
        glUniform3fv(glGetUniformLocation(pulsarProg, "viewPos"), 1, glm::value_ptr(camPos));
        glUniform3fv(glGetUniformLocation(pulsarProg, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(pulsarProg, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform1i(glGetUniformLocation(pulsarProg, "starTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, starTex);
        glBindVertexArray(sphereVao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIdx.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glUseProgram(glowProg);
        glUniformMatrix4fv(glGetUniformLocation(glowProg, "model"), 1, GL_FALSE, glm::value_ptr(pulsarModel));
        glUniformMatrix4fv(glGetUniformLocation(glowProg, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(glowProg, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform4f(
            glGetUniformLocation(glowProg, "glowColor"),
            0.66f + 0.08f * app.pulseBrightness,
            0.89f,
            1.0f,
            0.18f + 0.10f * app.pulseBrightness
        );
        glBindVertexArray(glowVao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(glowIdx.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        if (app.showJets) {
            jet1.draw(lineProg, pulsarModel, view, proj);
            jet2.draw(lineProg, pulsarModel, view, proj);
        }
        if (app.showField) {
            field.draw(lineProg, pulsarModel, view, proj);
        }
        if (app.showGrid) {
            grid.draw(lineProg, view, proj);
        }
        if (app.showAxis) {
            axis.draw(lineProg, pulsarModel, view, proj);
        }

        pulsar::drawImGuiPanel(app);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &sphereVao);
    glDeleteBuffers(1, &sphereVbo);
    glDeleteBuffers(1, &sphereEbo);
    glDeleteVertexArrays(1, &glowVao);
    glDeleteBuffers(1, &glowVbo);
    glDeleteBuffers(1, &glowEbo);
    glDeleteVertexArrays(1, &skyVao);
    glDeleteBuffers(1, &skyVbo);
    glDeleteTextures(1, &starTex);
    glDeleteTextures(1, &cubemapTex);
    glDeleteProgram(pulsarProg);
    glDeleteProgram(glowProg);
    glDeleteProgram(lineProg);
    glDeleteProgram(skyboxProg);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
