// // https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
// https://blog.csdn.net/cgsmalcloud/article/details/120470810
// https://zhuanlan.zhihu.com/p/194239682

#ifndef HAND_PHONG_SHADER_H
#define HAND_PHONG_SHADER_H


namespace phong_shading {

const char *vertex_shader_330 =
        "#version 330 core\n"
        "const int MAX_BONES = 100;\n"
        "uniform mat4 u_bone_transf[MAX_BONES];\n"
        "uniform mat4 u_view_projection;\n"
        "layout(location = 0) in vec3 in_position;\n"
        "layout(location = 1) in vec2 in_texcoord;\n"
        "layout(location = 2) in vec3 in_normal;\n"
        "layout(location = 3) in ivec4 in_bone_index;\n"
        "layout(location = 4) in vec4 in_bone_weight;\n"
        "out vec2 pass_texcoord;\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        ""
        "void main() {\n"
        "    float adjust_factor = 0.0;\n"
        "    for (int i = 0; i < 4; i++) adjust_factor += in_bone_weight[i] * 0.25;\n"
        "    mat4 bone_transform = mat4(1.0);\n"
        "    if (adjust_factor > 1e-3) {\n"
        "        bone_transform -= bone_transform;\n"
        "        for (int i = 0; i < 4; i++)\n"
        "            bone_transform += u_bone_transf[in_bone_index[i]] * in_bone_weight[i] / adjust_factor;\n"
        "	 }\n"
        "    vec4 pos = bone_transform * vec4(in_position, 1.0);\n"
        "    FragPos = vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);\n"
        "    Normal = mat3(transpose(inverse(bone_transform))) * in_normal;\n"
        "    gl_Position = u_view_projection * pos;\n"
        "    pass_texcoord = in_texcoord;\n"
        "}\n";

const char *fragment_shader_330 =
        "#version 330 core\n"
        "uniform sampler2D u_diffuse;\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 viewPos;\n"
        "uniform vec3 lightColor;\n"
        "in vec2 pass_texcoord;\n"
        "in vec3 FragPos;\n"
        "in vec3 Normal;\n"
        "out vec4 out_color;\n"
        "void main() {\n"
        #ifdef DIFFUSE_TEXTURE_MAPPING
            "out_color = vec4(texture(u_diffuse, pass_texcoord).xyz, 1.0);\n"
        #else
            "vec3 objectColor = vec3(pass_texcoord, 0.0);\n"

            "float ambientStrength = 0.1;\n"
            "vec3 ambient = ambientStrength * lightColor;\n"

            "vec3 norm = normalize(Normal);\n"
            "vec3 lightDir = normalize(lightPos - FragPos);\n"
            "float diff = max(dot(norm, lightDir), 0.0);\n"
            "vec3 diffuse = diff * lightColor;\n"

            "float specularStrength = 0.5;\n"
            "vec3 viewDir = normalize(viewPos - FragPos);\n"
            "vec3 reflectDir = reflect(-lightDir, norm);\n"
            "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
            "vec3 specular = specularStrength * spec * lightColor;\n"

            "vec3 result = (ambient + diffuse + specular) * objectColor;\n"
            "out_color = vec4(result, 1.0);\n"
        #endif
        "}\n";

}

# endif  // HAND_PHONG_SHADER_H