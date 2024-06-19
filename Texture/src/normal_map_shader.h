// Ref: https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/
// Ref: https://learnopengl.com/Advanced-Lighting/Normal-Mapping

#ifndef HAND_PHONG_SHADER_H
#define HAND_PHONG_SHADER_H


namespace normal_map {

const char *vertex_shader_330 =
        "#version 330 core\n"
        "uniform mat4 u_view_projection;\n"
        "uniform mat4 u_model;\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 viewPos;\n"
        "layout(location = 0) in vec3 in_position;\n"
        "layout(location = 1) in vec3 in_normal;\n"
        "layout(location = 2) in vec2 in_texcoord;\n"
        "layout(location = 3) in vec3 tangent;\n"
        "layout(location = 4) in vec3 bitangent;\n"
        "out vec2 pass_texcoord;\n"
        "out vec3 FragPos;\n"
        "out vec3 TangentLightPos;\n"
        "out vec3 TangentViewPos;\n"
        "out vec3 TangentFragPos;\n"
        ""
        "void main() {\n"
        "    FragPos = in_position;\n"
        "    pass_texcoord = in_texcoord;\n"
        "    mat3 normalMatrix = transpose(inverse(mat3(u_model)));\n"
        "    vec3 T = normalize(normalMatrix * tangent);\n"
        "    vec3 N = normalize(normalMatrix * in_normal);\n"
        "    T = normalize(T - dot(T, N) * N);\n"
        "    vec3 B = cross(N, T);\n"
        "    mat3 TBN = transpose(mat3(T, B, N));\n"
        "    TangentLightPos = TBN * lightPos;\n"
        "    TangentViewPos  = TBN * viewPos;\n"
        "    TangentFragPos  = TBN * FragPos;\n"
        "    gl_Position = u_view_projection * u_model * vec4(in_position, 1.0);\n"

        "}\n";

const char *fragment_shader_330 =
        "#version 330 core\n"
        "uniform sampler2D diffuseMap;\n"
        "uniform sampler2D normalMap;\n"
        "uniform vec3 lightColor;\n"
        "in vec2 pass_texcoord;\n"
        "in vec3 FragPos;\n"
        "in vec3 TangentLightPos;\n"
        "in vec3 TangentViewPos;\n"
        "in vec3 TangentFragPos;\n"
        "out vec4 out_color;\n"
        "void main() {\n"
            "vec3 norm = texture(normalMap, pass_texcoord).rgb;\n"
            "norm = normalize(norm * 2.0 - 1.0);\n"
            "vec3 objectColor = texture(diffuseMap, pass_texcoord).rgb;\n"

            "float ambientStrength = 0.2;\n"
            "vec3 ambient = ambientStrength * lightColor;\n"

            "vec3 lightDir = normalize(TangentLightPos - TangentFragPos);\n"
            "float diff = max(dot(norm, lightDir), 0.0);\n"
            "vec3 diffuse = diff * lightColor;\n"

            "float specularStrength = 0.5;\n"
            "vec3 viewDir = normalize(TangentViewPos - TangentFragPos);\n"
            "vec3 reflectDir = reflect(-lightDir, norm);\n"
            "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
            "vec3 specular = specularStrength * spec * lightColor;\n"

            "vec3 result = (ambient + diffuse + specular) * objectColor;\n"
            "out_color = vec4(result, 1.0);\n"
        "}\n";

}

# endif  