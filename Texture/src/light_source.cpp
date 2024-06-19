#include "light_source.h"

LightSource::LightSource(glm::vec3 position, glm::vec3 light_color)
        : MovableObject(position, glm::vec3(0.0f, 0.0f, 1.0f)), light_color_(light_color) {}
