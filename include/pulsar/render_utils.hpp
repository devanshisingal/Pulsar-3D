#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace pulsar {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

extern const char* kVertexShaderSrc;
extern const char* kPulsarFragSrc;
extern const char* kGlowFragSrc;
extern const char* kLineFragSrc;
extern const char* kLineVertSrc;
extern const char* kSkyboxVertSrc;
extern const char* kSkyboxFragSrc;

GLuint compileShader(GLenum type, const char* src);
GLuint buildProgram(const char* vert, const char* frag);

void generateSphere(
    float radius,
    int stacks,
    int slices,
    std::vector<Vertex>& verts,
    std::vector<unsigned int>& idx
);

GLuint uploadSphere(
    const std::vector<Vertex>& verts,
    const std::vector<unsigned int>& idx,
    GLuint& vbo,
    GLuint& ebo
);

GLuint loadTexture(const std::string& path);
GLuint loadCubemap(const std::vector<std::string>& faces);

}  // namespace pulsar
