// https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
// https://blog.csdn.net/cgsmalcloud/article/details/120470810
// https://zhuanlan.zhihu.com/p/194239682


#include "particle.h"
#include <cstdio>
#include <algorithm>

Particle::Particle():position_(0.0f), velocity_(0.0f), color_(0.0f), life_(0.0f),
                     initial_alpha_(0.0f), initial_life_(0.0f) {}

bool Particle::IsDead() {
    return life_ <= 0.0f;
}

void Particle::Set(const glm::vec3 &position, const glm::vec3 &velocity, glm::vec4 &color, float life) {
    position_ = position;
    velocity_ = velocity;
    color_ = color;
    initial_alpha_ = color.a;
    life_ = initial_life_ = life;
}

// HACK: Dead & uninitialized paticles are garanteed to have alpha = 0,
// so they won't show on screen.
void Particle::Update(float delta_time) {
    if (IsDead()) return;
    life_ -= delta_time;
    position_ += velocity_ * delta_time;
    color_.a = std::max(initial_alpha_ * life_ / initial_life_, 0.0f);
}

void Particle::WritePosition(float *buffer) {
    buffer[0] = position_.x;
    buffer[1] = position_.y;
    buffer[2] = position_.z;
}

void Particle::WriteColor(float *buffer) {
    buffer[0] = color_.r;
    buffer[1] = color_.g;
    buffer[2] = color_.b;
    buffer[3] = color_.a;
}

void Particle::PrintDebugInfo() {
    printf("[Particle] alpha = %f\n", color_.a);
}
