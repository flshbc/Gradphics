#ifndef HAND_MOVABLE_OBJECT_H
#define HAND_MOVABLE_OBJECT_H


// formal carema -> move_system for the particle system


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

enum MovementDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD
};

enum RollDirection {
    ROLL_LEFT,
    ROLL_RIGHT
};

// An object with position, orientation, but has no shape or volume.
// Implemented movement and rotation from keyboard and mouse input.
// It maintains 3 direction vectors: up, font, and right.
// When you modify an object's orientation directly (not by calling a method),
// be sure to call UpdateDirectionVectors() afterwise.
class MovableObject {
public:
    glm::vec3 position_;
    glm::quat rotation_;  // rotation from standard camera orientation to current orientation.
    float movement_speed_;
    float roll_speed_;
    float mouse_sensitivity_;

    MovableObject();
    MovableObject(glm::vec3 position);
    MovableObject(glm::vec3 position, glm::vec3 front);

    virtual void Move(MovementDirection direction, float delta_time);
    virtual void ProcessKeyboardRoll(RollDirection direction, float delta_time);
    virtual void ProcessMouseMovement(float xoffset, float yoffset);
    virtual void PrintDebugInfo();
    virtual void UpdateDirectionVectors() final;

    virtual const glm::vec3& GetUp() final {return up_;}
    virtual const glm::vec3& GetFront() final {return front_;}
    virtual const glm::vec3& GetRight() final {return right_;}

private:
    glm::vec3 up_;
    glm::vec3 front_;
    glm::vec3 right_;
};

#endif  // HAND_MOVABLE_OBJECT_H