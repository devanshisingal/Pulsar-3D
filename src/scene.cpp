#include "pulsar/scene.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <vector>

namespace pulsar {

namespace {

constexpr int kNumParticles = 10000;
constexpr float kConeHeight = 2000.0f;
constexpr float kBaseRadiusStart = 0.015f;
constexpr float kBaseRadiusEnd = 40.0f;
constexpr float kSegmentLength = 10.0f;
constexpr float kJetVelocity = 70.0f;

glm::vec3 magneticField(glm::vec3 r, const glm::vec3& m) {
    float rMag = glm::length(r);
    if (rMag < 1e-5f) {
        return glm::vec3(0.0f);
    }

    float rDotM = glm::dot(r, m);
    float r5 = powf(rMag, 5.0f);
    float r3 = powf(rMag, 3.0f);
    return r * (3.0f * rDotM / r5) - m * (1.0f / r3);
}

glm::vec3 buildMagneticAxis(float tiltDegrees) {
    float tiltRadians = glm::radians(tiltDegrees);
    return glm::normalize(glm::vec3(sinf(tiltRadians), cosf(tiltRadians), 0.0f));
}

}  // namespace

void JetSystem::init(int dir, std::mt19937& rng) {
    direction = dir;
    std::uniform_real_distribution<float> randF(0.0f, 1.0f);
    data.resize(kNumParticles);
    positions.resize(kNumParticles * 6, 0.0f);
    activeCount = kNumParticles;

    for (int i = 0; i < kNumParticles; ++i) {
        auto& p = data[i];
        p.altura = randF(rng) * kConeHeight;
        p.theta = randF(rng) * glm::two_pi<float>();
        p.raioNorm = sqrtf(randF(rng));
        p.velocidade = kJetVelocity * (0.65f + randF(rng) * 0.7f);
        fillSegment(i);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void JetSystem::setDensity(float value) {
    density = value;
    activeCount = glm::clamp(static_cast<int>(kNumParticles * density), 250, kNumParticles);
}

void JetSystem::fillSegment(int i) {
    const auto& p = data[i];
    float t = p.altura / kConeHeight;
    float raioMax = kBaseRadiusStart + t * (kBaseRadiusEnd - kBaseRadiusStart);
    float raio = raioMax * p.raioNorm;
    float x = raio * cosf(p.theta);
    float z = raio * sinf(p.theta);
    float yStart = (direction > 0) ? p.altura : -p.altura;
    float yEnd = yStart + kSegmentLength * static_cast<float>(direction);
    int base = i * 6;
    positions[base + 0] = x;
    positions[base + 1] = yStart;
    positions[base + 2] = z;
    positions[base + 3] = x;
    positions[base + 4] = yEnd;
    positions[base + 5] = z;
}

void JetSystem::update(float deltaTime) {
    for (int i = 0; i < activeCount; ++i) {
        auto& p = data[i];
        p.altura += p.velocidade * deltaTime;
        if (p.altura > kConeHeight) {
            p.altura = 0.0f;
        }
        fillSegment(i);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(activeCount * 6 * sizeof(float)), positions.data());
}

void JetSystem::draw(GLuint program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const {
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform4f(glGetUniformLocation(program, "lineColor"), 0.98f, 0.93f, 0.23f, 0.35f + 0.65f * density);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, activeCount * 2);
    glBindVertexArray(0);
}

void FieldLines::clear() {
    if (!vaos.empty()) {
        glDeleteVertexArrays(static_cast<GLsizei>(vaos.size()), vaos.data());
    }
    if (!vbos.empty()) {
        glDeleteBuffers(static_cast<GLsizei>(vbos.size()), vbos.data());
    }
    vaos.clear();
    vbos.clear();
    counts.clear();
}

void FieldLines::build(float fieldStrength, float magneticAxisTiltDegrees) {
    clear();
    strength = fieldStrength;
    magneticAxisTiltDeg = magneticAxisTiltDegrees;
    const int numLines = 8 + static_cast<int>(10.0f * strength);
    const float step = 0.05f + 0.08f * strength;
    const float maxDist = 400.0f + 850.0f * strength;
    const float seedAngle = glm::radians(30.0f);
    const glm::vec3 dipoleAxis = buildMagneticAxis(magneticAxisTiltDeg);
    glm::vec3 basis1 = glm::cross(dipoleAxis, glm::vec3(0.0f, 0.0f, 1.0f));
    if (glm::dot(basis1, basis1) < 1e-6f) {
        basis1 = glm::cross(dipoleAxis, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    basis1 = glm::normalize(basis1);
    glm::vec3 basis2 = glm::normalize(glm::cross(dipoleAxis, basis1));

    for (int i = 0; i < numLines; ++i) {
        float theta = static_cast<float>(i) / static_cast<float>(numLines) * glm::two_pi<float>();
        glm::vec3 ringOffset =
            basis1 * (sinf(seedAngle) * cosf(theta)) +
            basis2 * (sinf(seedAngle) * sinf(theta));
        glm::vec3 start = (dipoleAxis * cosf(seedAngle) + ringOffset) * 0.5f;

        std::vector<glm::vec3> pts;
        glm::vec3 pos = start;
        for (int j = 0; j < 1061; ++j) {
            pts.push_back(pos);
            glm::vec3 b = glm::normalize(magneticField(pos, dipoleAxis));
            pos += b * step;
            if (glm::length(pos) > maxDist) {
                break;
            }
        }

        GLuint vaoLocal = 0;
        GLuint vboLocal = 0;
        glGenVertexArrays(1, &vaoLocal);
        glGenBuffers(1, &vboLocal);
        glBindVertexArray(vaoLocal);
        glBindBuffer(GL_ARRAY_BUFFER, vboLocal);
        glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(glm::vec3), pts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        vaos.push_back(vaoLocal);
        vbos.push_back(vboLocal);
        counts.push_back(static_cast<int>(pts.size()));
    }
}

void FieldLines::draw(GLuint program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const {
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform4f(
        glGetUniformLocation(program, "lineColor"),
        0.35f + 0.16f * strength,
        0.72f + 0.12f * strength,
        0.95f + 0.05f * strength,
        0.45f + 0.35f * strength
    );

    for (std::size_t i = 0; i < vaos.size(); ++i) {
        glBindVertexArray(vaos[i]);
        glDrawArrays(GL_LINE_STRIP, 0, counts[i]);
    }
    glBindVertexArray(0);
}

void Grid::build(float size, int divs) {
    std::vector<float> pts;
    float half = size * 0.5f;
    float step = size / static_cast<float>(divs);
    for (int i = 0; i <= divs; ++i) {
        float t = -half + static_cast<float>(i) * step;
        pts.insert(pts.end(), {t, 0.0f, -half, t, 0.0f, half});
        pts.insert(pts.end(), {-half, 0.0f, t, half, 0.0f, t});
    }

    count = static_cast<int>(pts.size() / 3);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(float), pts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Grid::draw(GLuint prog, const glm::mat4& view, const glm::mat4& proj) const {
    glUseProgram(prog);
    glm::mat4 model(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(prog, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform4f(glGetUniformLocation(prog, "lineColor"), 0.94f, 0.94f, 0.94f, 0.4f);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, count);
    glBindVertexArray(0);
}

void AxisArrows::build() {
    float pts[] = {
        0.0f, -4.0f, 0.0f,
        0.0f, 4.0f, 0.0f
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void AxisArrows::draw(GLuint prog, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const {
    glUseProgram(prog);
    glUniformMatrix4fv(glGetUniformLocation(prog, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform4f(glGetUniformLocation(prog, "lineColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

}  // namespace pulsar
