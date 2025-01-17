// Hand Example
// 

// #define DIFFUSE_TEXTURE_MAPPING

#include "gl_env.h"

#include <cstdlib>
#include <cstdio>
#include <config.h>

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#include <iostream>

#include "skeletal_mesh.h"

#include <glm/gtc/matrix_transform.hpp>

namespace SkeletalAnimation
{
    const char *vertex_shader_330 =
        "#version 330 core\n"
        "const int MAX_BONES = 100;\n"
        "uniform mat4 u_bone_transf[MAX_BONES];\n"
        "uniform mat4 u_mvp;\n"
        "layout(location = 0) in vec3 in_position;\n"
        "layout(location = 1) in vec2 in_texcoord;\n"
        "layout(location = 2) in vec3 in_normal;\n"
        "layout(location = 3) in ivec4 in_bone_index;\n"
        "layout(location = 4) in vec4 in_bone_weight;\n"
        "out vec2 pass_texcoord;\n"
        "void main() {\n"
        "    float adjust_factor = 0.0;\n"
        "    for (int i = 0; i < 4; i++) adjust_factor += in_bone_weight[i] * 0.25;\n"
        "    mat4 bone_transform = mat4(1.0);\n"
        "    if (adjust_factor > 1e-3) {\n"
        "        bone_transform -= bone_transform;\n"
        "        for (int i = 0; i < 4; i++)\n"
        "            bone_transform += u_bone_transf[in_bone_index[i]] * in_bone_weight[i] / adjust_factor;\n"
        "	 }\n"
        "    gl_Position = u_mvp * bone_transform * vec4(in_position, 1.0);\n"
        "    pass_texcoord = in_texcoord;\n"
        "}\n";

    const char *fragment_shader_330 =
        "#version 330 core\n"
        "uniform sampler2D u_diffuse;\n"
        "in vec2 pass_texcoord;\n"
        "out vec4 out_color;\n"
        "void main() {\n"
#ifdef DIFFUSE_TEXTURE_MAPPING
        "    out_color = vec4(texture(u_diffuse, pass_texcoord).xyz, 1.0);\n"
#else
        "    out_color = vec4(pass_texcoord, 0.0, 1.0);\n"
#endif
        "}\n";
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

/*Note: Use diretion keys(l,r) or number keys(1,2,3) to move between motion patterns.*/
int MotionNum = 1;

/* Note: Use Mouse(MB1,MB2) or Keyboard(ENTER) to Control Time Progression */
bool TimeProg = true;
float PauseInterval = 0.0f;
float MotionInterval = 0.0f;

float TotalPausedTime = 0.0f;
float TotalMotionTime = 0.0f;

float passed_time = 0.0f;

float MotionMark = 0.0f;
float PauseMark = 0.0f;

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (TimeProg)
        {
            std::cout << "  ! Motion has started." << std::endl;
        }
        else
        {
            TimeProg = true;
            MotionMark = (float)glfwGetTime();
            PauseInterval = MotionMark - PauseMark;
            TotalPausedTime += PauseInterval;
            std::cout << "  - Press ENTER or MB2 to pause. Total motion time has reached " << TotalMotionTime << "s." << std::endl;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        if (!TimeProg)
        {
            std::cout << "  ! Motion has been paused." << std::endl;
        }
        else
        {
            TimeProg = false;
            PauseMark = (float)glfwGetTime();
            MotionInterval = PauseMark - MotionMark;
            TotalMotionTime += MotionInterval;
            std::cout << "  - Press ENTER or MB1 to start. Total paused time has reached " << TotalPausedTime << "s." << std::endl;
        }
    }
}
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        TimeProg = !TimeProg;
        if (TimeProg)
        {
            MotionMark = (float)glfwGetTime();
            PauseInterval = MotionMark - PauseMark;
            TotalPausedTime += PauseInterval;
            std::cout << "  - Press ENTER or MB2 to pause. Total motion time has reached " << TotalMotionTime << "s." << std::endl;
        }
        else
        {
            PauseMark = (float)glfwGetTime();
            MotionInterval = PauseMark - MotionMark;
            TotalMotionTime += MotionInterval;
            std::cout << "  - Press ENTER or MB1 to start. Total paused time has reached " << TotalPausedTime << "s." << std::endl;
        }
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        if (MotionNum > 1)
        {
            MotionNum -= 1;
            std::cout << "Now Proceeding Motion " << MotionNum << "." << std::endl;
        }
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        if (MotionNum < 3)
        {
            MotionNum += 1;
            std::cout << "Now Proceeding Motion " << MotionNum << "." << std::endl;
        }
    }
    else if ((key == GLFW_KEY_1 || key == GLFW_KEY_KP_1) && action == GLFW_PRESS)
    {
        MotionNum = 1;
        std::cout << "Now Proceeding Motion 1." << std::endl;
    }
    else if ((key == GLFW_KEY_2 || key == GLFW_KEY_KP_2) && action == GLFW_PRESS)
    {
        MotionNum = 2;
        std::cout << "Now Proceeding Motion 2." << std::endl;
    }
    else if ((key == GLFW_KEY_3 || key == GLFW_KEY_KP_3) && action == GLFW_PRESS)
    {
        MotionNum = 3;
        std::cout << "Now Proceeding Motion 3." << std::endl;
    }
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(int argc, char *argv[])
{
    GLFWwindow *window;
    GLuint vertex_shader, fragment_shader, program;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // for macos
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(800, 800, "OpenGL output", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    /*Note: Mouse and Keyboard Control*/
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
        exit(EXIT_FAILURE);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &SkeletalAnimation::vertex_shader_330, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &SkeletalAnimation::fragment_shader_330, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int linkStatus;
    if (glGetProgramiv(program, GL_LINK_STATUS, &linkStatus), linkStatus == GL_FALSE)
        std::cout << "Error occured in glLinkProgram()" << std::endl;

    SkeletalMesh::Scene &sr = SkeletalMesh::Scene::loadScene("Hand", DATA_DIR "/Hand.fbx");
    if (&sr == &SkeletalMesh::Scene::error)
        std::cout << "Error occured in loadMesh()" << std::endl;

    sr.setShaderInput(program, "in_position", "in_texcoord", "in_normal", "in_bone_index", "in_bone_weight");

    // float passed_time;
    SkeletalMesh::SkeletonModifier modifier;

    glEnable(GL_DEPTH_TEST);
    std::cout << "                          <Hand Graphics>" << std::endl;
    std::cout<<  "                 Based on example program of course: Computer Graphics"<<std::endl;
    std::cout << "****************************************************************************************" << std::endl;
    std::cout << "Use diretion keys(Left,Right) or number keys(1,2,3) to move between motion patterns." << std::endl;
    std::cout << "Press ENTER to switch the motion status." << std::endl;
    std::cout << "Use Mouse button also to start[MB1] and pause[MB2] the motion." << std::endl;
    std::cout << "Press ESC to quit." << std::endl;
    std::cout << "****************************************************************************************" << std::endl;
    while (!glfwWindowShouldClose(window))
    {

        if (TimeProg)
            passed_time = (float)glfwGetTime() - TotalPausedTime;

        // --- You may edit below ---

        // Example: Rotate the hand
        // * turn around every 4 seconds
        float metacarpals_angle = (float)glfwGetTime() * (M_PI / 6.0f);
        // * target = metacarpals
        // * rotation axis = (1, 0, 0)
        modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle, glm::fvec3(1.0, 0.0, 0.0)); // 法向，展向、流向

        /**********************************************************************************\
        *
        * To animate fingers, modify modifier["HAND_SECTION"] each frame,
        * where HAND_SECTION can only be one of the bone names in the Hand's Hierarchy.
        *
        * A virtual hand's structure is like this: (slightly DIFFERENT from the real world)
        *    5432 1
        *    ....        1 = thumb           . = fingertip
        *    |||| .      2 = index finger    | = distal phalange
        *    $$$$ |      3 = middle finger   $ = intermediate phalange
        *    #### $      4 = ring finger     # = proximal phalange
        *    OOOO#       5 = pinky           O = metacarpals
        *     OOO
        * (Hand in the real world -> https://en.wikipedia.org/wiki/Hand)
        *
        * From the structure we can infer the Hand's Hierarchy:
        *	- metacarpals
        *		- thumb_proximal_phalange
        *			- thumb_intermediate_phalange
        *				- thumb_distal_phalange
        *					- thumb_fingertip
        *		- index_proximal_phalange
        *			- index_intermediate_phalange
        *				- index_distal_phalange
        *					- index_fingertip
        *		- middle_proximal_phalange
        *			- middle_intermediate_phalange
        *				- middle_distal_phalange
        *					- middle_fingertip
        *		- ring_proximal_phalange
        *			- ring_intermediate_phalange
        *				- ring_distal_phalange
        *					- ring_fingertip
        *		- pinky_proximal_phalange
        *			- pinky_intermediate_phalange
        *				- pinky_distal_phalange
        *					- pinky_fingertip
        *
        * Notice that modifier["HAND_SECTION"] is a local transformation matrix,
        * where (1, 0, 0) is the bone's direction, and apparently (0, 1, 0) / (0, 0, 1)
        * is perpendicular to the bone.
        * Particularly, (0, 0, 1) is the rotation axis of the nearer joint.
        *
        \**********************************************************************************/

        // Example: Animate the index finger
        // * period = 2.4 seconds
        float period = 2.5f;
        float time_in_period = fmod(passed_time, period);
        // * angle: 0 -> PI/3 -> 0
        // float thumb_angle;
        // float thumb_angle = abs(time_in_period / (period * 0.5f) - 1.0f) * (M_PI /3.0 );
        // * target = proximal phalange of the index
        // * rotation axis = (0, 0, 1)
        if (MotionNum == 1)
        {
            float thumb_angle;
            if (time_in_period <= 0.25 * period)
                thumb_angle = (5.0 * M_PI / 12.0);
            else if (time_in_period > 0.25 * period && time_in_period < 0.75 * period)
                thumb_angle = abs(2.0f - time_in_period / (period * 0.25f)) * (5.0 * M_PI / 12.0);
            else
                thumb_angle = (5.0 * M_PI / 12.0);
            /* 1.  Thumbs Up   */
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.05f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.4f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.15f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["thumb_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["index_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["middle_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["ring_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["pinky_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));
        }
        else if (MotionNum == 2)
        {
            float thumb_angle;
            if (time_in_period <= 0.25 * period)
                thumb_angle = (M_PI / 3.0);
            else if (time_in_period > 0.25 * period && time_in_period < 0.75 * period)
                thumb_angle = abs(2.0f - time_in_period / (period * 0.25f)) * (M_PI / 3.0);
            else
                thumb_angle = (M_PI / 3.0);
            /*2. OK */
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.15f * thumb_angle, glm::fvec3(0.0, 0, 1.0));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.6f * thumb_angle, glm::fvec3(0.0, 0, 1.0));
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.45f * thumb_angle, glm::fvec3(0.0, 0, 1.0));
            // modifier["thumb_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.75f * thumb_angle, glm::fvec3(0.0, -0.15, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, -0.15, 1.0));
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, -0.15, 1.0));
            // modifier["index_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.15f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.15f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.15f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["middle_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.10f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.10f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.10f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["ring_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.05f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.05f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), 0.05f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["pinky_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));
        }
        else if (MotionNum == 3)
        {
            float thumb_angle;
            if (time_in_period <= 0.25 * period)
                thumb_angle = (5.0 * M_PI / 12.0);
            else if (time_in_period > 0.25 * period && time_in_period < 0.75 * period)
                thumb_angle = abs(2.0f - time_in_period / (period * 0.25f)) * (5.0 * M_PI / 12.0);
            else
                thumb_angle = (5.0 * M_PI / 12.0);
            /*3. Shoot */
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.05f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.4f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.15f * thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["thumb_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.10f * thumb_angle, glm::fvec3(0.0, -0.15, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.15f * thumb_angle, glm::fvec3(0.0, -0.15, 1.0));
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -0.10f * thumb_angle, glm::fvec3(0.0, -0.15, 1.0));
            // modifier["index_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["middle_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["ring_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));

            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), thumb_angle, glm::fvec3(0.0, 0.0, 1.0));
            // modifier["pinky_fingertip"]=glm::rotate(glm::identity<glm::mat4>(),thumb_angle,glm::fvec3(0.0,1.0,0.0));
        }
        // --- You may edit above ---

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glClearColor(1.0, 0.5, 1.0, 1.0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glm::fmat4 mvp = glm::ortho(-12.5f * ratio, 12.5f * ratio, -5.f, 20.f, -20.f, 20.f) *
                         glm::lookAt(glm::fvec3(.0f, .0f, -1.f), glm::fvec3(.0f, .0f, .0f), glm::fvec3(.0f, 1.f, .0f));
        glUniformMatrix4fv(glGetUniformLocation(program, "u_mvp"), 1, GL_FALSE, (const GLfloat *)&mvp);
        glUniform1i(glGetUniformLocation(program, "u_diffuse"), SCENE_RESOURCE_SHADER_DIFFUSE_CHANNEL);
        SkeletalMesh::Scene::SkeletonTransf bonesTransf;
        sr.getSkeletonTransform(bonesTransf, modifier);
        if (!bonesTransf.empty())
            glUniformMatrix4fv(glGetUniformLocation(program, "u_bone_transf"), bonesTransf.size(), GL_FALSE,
                               (float *)bonesTransf.data());
        sr.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    SkeletalMesh::Scene::unloadScene("Hand");

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}