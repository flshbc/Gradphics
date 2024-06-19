#include "camera.h"

static const float ZOOM = 45.0f;

Camera::Camera(): zoom_(ZOOM) {}

Camera::Camera(glm::vec3 position, glm::vec3 front)
        : MovableObject(position, front), zoom_(ZOOM) {}

glm::mat4 Camera::GetViewProjection(float aspect_ratio){
    glm::mat4 view = glm::lookAt(position_, position_ + GetFront(), GetUp());
    glm::mat4 projection = glm::perspective(glm::radians(zoom_), aspect_ratio, 0.1f, 100.0f);
    return projection * view;
}
