#ifndef HAND_SIMPLE_SHADING_H
#define HAND_SIMPLE_SHADING_H

namespace particle_shader {

const char *vertex_shader_330 =
        "#version 330 core\n"
        "uniform mat4 mvp;\n"
        "layout(location = 0) in vec3 in_position;\n"
        "layout(location = 1) in vec3 pos_offset;\n"
        "layout(location = 2) in vec4 in_color;\n"
        "out vec4 frag_color;\n"
        ""
        "void main() {\n"
        "    gl_Position = mvp * vec4(in_position + pos_offset, 1.0);\n"
        "    frag_color = in_color;\n"
        "}\n";

const char *fragment_shader_330 =
        "#version 330 core\n"
        "in vec4 frag_color;\n"
        "out vec4 out_color;\n"
        "void main() {\n"
            "out_color = frag_color;\n"
        "}\n";

}

#endif  // HAND_SIMPLE_SHADING_H