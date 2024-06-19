#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "camera.h"

class CameraTransition: public Camera {
public:
    enum TransitionState {
        INIT,
        DESTINATION_SET,
        TRANSITIONING
    };
    
    float transition_speed_;

    CameraTransition(glm::vec3 position, glm::vec3 front);
    void SetDestination(glm::vec3 position, glm::quat orientation);
    void ToggleTransition(float cur_time);
    void UpdatePose(float cur_time);
    TransitionState GetState();
    
    
    virtual void Move(MovementDirection direction, float delta_time);
    virtual void ProcessKeyboardRoll(RollDirection direction, float delta_time);
    virtual void MouseMove(float xoffset, float yoffset);
    

private:
    TransitionState state_;
    glm::vec3 start_position_;
    glm::quat start_orientation_;
    glm::vec3 dest_position_;
    glm::quat dest_orientation_;
    float transition_start_time_;
    float transition_period_;
};