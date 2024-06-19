// https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
#include "particle_generator.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include "particle_shader.h"


ParticleGenerator::ParticleGenerator(glm::vec3 position, glm::vec3 velocity,
                                     glm::vec4 color, float particle_size, float life,
                                     float radius, float respawn_rate, int max_particle_num)
        : MovableObject(position), velocity_(velocity), color_(color),
          particle_size_(particle_size), life_(life), radius_(radius),
          respawn_rate_(respawn_rate), max_particle_num_(max_particle_num){
    particles_ = std::make_unique<Particle[]>(max_particle_num);
    instance_buffer_ = std::make_unique<float[]>(max_particle_num * INSTANCE_ATTRIB_SIZE);

    glEnable(GL_BLEND);
    
    // VAO
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    unsigned int vertex_VBO;
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenBuffers(1, &instance_VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO_);
    glBufferData(GL_ARRAY_BUFFER, max_particle_num * INSTANCE_ATTRIB_SIZE, instance_buffer_.get(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, INSTANCE_ATTRIB_SIZE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, INSTANCE_ATTRIB_SIZE * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // shader
    unsigned int vertex_shader, fragment_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertex_shader, 1, &SkeletalAnimation::vertex_shader_330, NULL);
    glShaderSource(vertex_shader, 1, &particle_shader::vertex_shader_330, NULL);
    glCompileShader(vertex_shader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragment_shader, 1, &SkeletalAnimation::fragment_shader_330, NULL);
    glShaderSource(fragment_shader, 1, &particle_shader::fragment_shader_330, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shader_ = glCreateProgram();
    glAttachShader(shader_, vertex_shader);
    glAttachShader(shader_, fragment_shader);
    glLinkProgram(shader_);
    glUseProgram(shader_);
}

void ParticleGenerator::Update(float delta_time) {
    for (int i = 0; i < max_particle_num_; i++)
        particles_[i].Update(delta_time);

    // spawn new articles
    int num_spawn = ceil(respawn_rate_ * delta_time);
    for (int i = 0; i < max_particle_num_; i++) {
        if (particles_[i].IsDead()) {
            // 0.5 ~ 1.5 x life
            float life = (0.5f + (float)rand() / RAND_MAX) * life_;
            glm::vec3 velocity = (0.5f + (float)rand() / RAND_MAX) * velocity_;

            // Generate random point within a ball
            float x, y, z;
            do {
                x = ((float) rand() / RAND_MAX) * 2.0f - 1.0f;
                y = ((float) rand() / RAND_MAX) * 2.0f - 1.0f;
                z = ((float) rand() / RAND_MAX) * 2.0f - 1.0f;
            } while(x * x + y * y + z * z > 1.0f);
            glm::vec3 position(x * radius_, y * radius_, z * radius_);
            particles_[i].Set(position, velocity, color_, life);
            num_spawn--;
        }
        if (num_spawn == 0) break;
    }
}

static const glm::vec3 ORIGIN(0.0f, 0.0f, 0.0f);
static const glm::vec3 STD_FRONT(0.0f, 0.0f, -1.0f);
static const glm::vec3 STD_UP(0.0f, 1.0f, 0.0f);
static const glm::vec3 STD_RIGHT(1.0f, 0.0f, 0.0f);

void ParticleGenerator::Draw(const glm::mat4& view_projection, const glm::vec3& camera_pos) {
    glm::mat4 view = glm::lookAt(ORIGIN, camera_pos, STD_UP);
    glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), position_);
    glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), glm::vec3(particle_size_, particle_size_, particle_size_));
    glm::mat4 mvp = view_projection * translate * scale;
    for (int i = 0; i < max_particle_num_; i++) {
        particles_[i].WritePosition(instance_buffer_.get() + INSTANCE_ATTRIB_SIZE * i);
        particles_[i].WriteColor(instance_buffer_.get() + INSTANCE_ATTRIB_SIZE * i + 3);
    }

    glUseProgram(shader_);
    glUniformMatrix4fv(glGetUniformLocation(shader_, "mvp"), 1, GL_FALSE, (const GLfloat*) &mvp);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindBuffer(GL_ARRAY_BUFFER, instance_VBO_);
    glBufferData(GL_ARRAY_BUFFER, max_particle_num_ * INSTANCE_ATTRIB_SIZE, instance_buffer_.get(), GL_DYNAMIC_DRAW);
    glBindVertexArray(VAO_);
    glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices_), GL_UNSIGNED_INT, 0, max_particle_num_);
    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::PrintDebugInfo() {
    printf("[Particle Generator]");
    MovableObject::PrintDebugInfo();

    int alive = 0;
    bool first_alive = true;
    for (int i = 0; i < max_particle_num_; i++) {
        if (!particles_[i].IsDead()) {
            alive ++;
            if (first_alive) {
                particles_[i].PrintDebugInfo();
                first_alive = false;
            }
        }
    }
    printf("num_alive = %d\n", alive);
}
