// https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
// https://blog.csdn.net/cgsmalcloud/article/details/120470810
// https://zhuanlan.zhihu.com/p/194239682

#ifndef HAND_PARTICLE_H
#define HAND_PARTICLE_H

#include <glm/glm.hpp>

class Particle {
public:
    Particle();
    bool IsDead();
    void Set(const glm::vec3& position, const glm::vec3& velocity, glm::vec4& color, float life);
    void Update(float delta_time);
    void WritePosition(float* buffer);
    void WriteColor(float* buffer);
    void PrintDebugInfo();

private:
    glm::vec3 position_;
    glm::vec3 velocity_;
    glm::vec4 color_;
    float life_;
    float initial_life_;
    float initial_alpha_;
};

#endif  // HAND_PARTICLE_H