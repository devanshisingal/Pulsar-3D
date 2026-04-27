#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pulsar { 

struct Camera {
    float yaw = 0.0f;
    float pitch = 0.15f;
    float dist = 5.0f;
    float fov = 50.0f;
    glm::vec3 target{0.0f};

    double lastX = 0.0;
    double lastY = 0.0;
    bool dragging = false;

    glm::mat4 view() const {
        return glm::lookAt(position(), target, {0.0f, 1.0f, 0.0f});
    }

    glm::vec3 position() const {
        return target + glm::vec3(
            dist * cosf(pitch) * sinf(yaw),
            dist * sinf(pitch),
            dist * cosf(pitch) * cosf(yaw)
        );
    }

    glm::mat4 projection(float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 9100.0f);
    }

    void setFromDirection(const glm::vec3& dir, float newDist) {
        glm::vec3 n = glm::normalize(dir);
        yaw = atan2f(n.x, n.z);
        pitch = asinf(glm::clamp(n.y, -1.0f, 1.0f));
        dist = newDist;
    }
};

}  // namespace pulsar
