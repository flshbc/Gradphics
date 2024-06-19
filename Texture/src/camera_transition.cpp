#include "camera_transition.h"
#include <cstdio>

static const float TRANSITION_SPEED = 5.0f;

CameraTransition::CameraTransition(glm::vec3 position, glm::vec3 front)
        : Camera(position, front), transition_speed_(TRANSITION_SPEED), state_(INIT) {}

void CameraTransition::SetDestination(glm::vec3 position, glm::quat orientation) {
    switch (state_) {
    case INIT:
    case DESTINATION_SET:
        printf("Set Camera Transition Destination.\n");
        dest_position_ = position;
        dest_orientation_ = orientation;
        state_ = DESTINATION_SET;
        break;
    case TRANSITIONING:
        break;  // Do nothing.
    }
}

void CameraTransition::ToggleTransition(float cur_time) {
    switch (state_) {
        case INIT:
            break;
        case DESTINATION_SET: {
            printf("Start Camera Transition.\n");
            state_ = TRANSITIONING;
            start_position_ = position_;
            start_orientation_ = rotation_;
            transition_start_time_ = cur_time;
            float transition_dist = (dest_position_ - position_).length() * 2;
            transition_period_ = transition_dist / transition_speed_;
            break;
        }
        case TRANSITIONING:
            printf("Stop Camera Transition.\n");
            state_ = DESTINATION_SET;
            break;
    }
}

void CameraTransition::UpdatePose(float cur_time) {
    if (state_ == TRANSITIONING) {
        float time_in_period = fmod(cur_time - transition_start_time_, transition_period_);
        float place = 1.0f - abs(time_in_period / (transition_period_ * 0.5f) - 1.0f);
        position_ = glm::mix(start_position_, dest_position_, place);
        rotation_ = glm::mix(start_orientation_, dest_orientation_, place);
        UpdateDirectionVectors();
    }
}

CameraTransition::TransitionState CameraTransition::GetState() {
    return state_;
}



void CameraTransition::Move(MovementDirection direction, float delta_time) {
    if (state_ == TRANSITIONING) return;
    Camera::Move(direction, delta_time);
}

void CameraTransition::ProcessKeyboardRoll(RollDirection direction, float delta_time) {
    if (state_ == TRANSITIONING) return;
    Camera::ProcessKeyboardRoll(direction, delta_time);
}

void CameraTransition::MouseMove(float xoffset, float yoffset) {
    if (state_ == TRANSITIONING) return;
    Camera::ProcessMouseMovement(xoffset, yoffset);
}


