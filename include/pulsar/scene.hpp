#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <random> 
#include <vector>

namespace pulsar {

struct ParticleData {
    float altura;
    float theta;
    float raioNorm;
    float velocidade;
};

struct JetSystem {
    std::vector<ParticleData> data;
    std::vector<float> positions;
    GLuint vao = 0;
    GLuint vbo = 0;
    int direction = 1;
    int activeCount = 0;
    float density = 1.0f;

    void init(int dir, std::mt19937& rng);
    void setDensity(float value);
    void update(float deltaTime);
    void draw(GLuint program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const;

private:
    void fillSegment(int i);
};

struct FieldLines {
    std::vector<GLuint> vaos;
    std::vector<GLuint> vbos;
    std::vector<int> counts;
    float strength = 1.0f;
    float magneticAxisTiltDeg = 0.0f;

    void clear();
    void build(float fieldStrength, float magneticAxisTiltDegrees);
    void draw(GLuint program, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const;
};

struct Grid {
    GLuint vao = 0;
    GLuint vbo = 0;
    int count = 0;

    void build(float size, int divs);
    void draw(GLuint prog, const glm::mat4& view, const glm::mat4& proj) const;
};

struct AxisArrows {
    GLuint vao = 0;
    GLuint vbo = 0;

    void build();
    void draw(GLuint prog, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const;
};

}  // namespace pulsar
