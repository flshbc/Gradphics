
#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>

enum RollDirection {
    ROLL_LEFT,
    ROLL_RIGHT
};


enum MovementDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD
};


class Camera {
public:
    glm::vec3 position_;
    glm::quat rotation_;  
    
    
    float roll_speed_;
    float mouse_sensitivity_;
    float movement_speed_;
    
    float zoom_;//

    Camera();
    Camera(glm::vec3 position, glm::vec3 front);

    glm::mat4 GetViewProjection(float aspect_ratio);
    virtual void Move(MovementDirection direction, float delta_time);
    virtual void ProcessKeyboardRoll(RollDirection direction, float delta_time);
    virtual void MouseMove(float xoffset, float yoffset);
    

protected:
    void UpdateCameraVectors();
    
private:
    glm::vec3 up_;
    glm::vec3 front_;
    glm::vec3 right_;
};

#endif  