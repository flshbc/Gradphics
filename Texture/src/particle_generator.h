// https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
#ifndef HAND_PARTICLE_GENERATOR_H
#define HAND_PARTICLE_GENERATOR_H

#include <memory>
#include <glm/glm.hpp>
#include "gl_env.h"
#include "move_system.h"
#include "particle.h"

class ParticleGenerator: public MovableObject{
public:
    ParticleGenerator(glm::vec3 position, glm::vec3 velocity, glm::vec4 color,
                      float particle_size, float life, float radius,
                      float respawn_rate, int max_particle_num);
    void Update(float delta_time);
    void Draw(const glm::mat4& view_projection, const glm::vec3& camera_pos);
    virtual void PrintDebugInfo();
private:
    static const int INSTANCE_ATTRIB_SIZE = 7;

    const float vertices_[12] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    const unsigned int indices_[6] = {
        0, 1, 3,
        1, 2, 3
    };
    glm::vec4 color_;
    glm::vec3 velocity_;
    float particle_size_;
    float life_;
    float radius_;
    float respawn_rate_;
    int max_particle_num_;
    unsigned int VAO_;
    unsigned int instance_VBO_;
    unsigned int shader_;

    // {offset_x, offset_y, offset_z, R, G, B, A}
    std::unique_ptr<float[]> instance_buffer_;
    std::unique_ptr<Particle[]> particles_;
};

#endif  // HAND_PARTICLE_GENERATOR_H