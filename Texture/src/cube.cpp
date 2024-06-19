
// Ref: https://learnopengl.com/Advanced-Lighting/Normal-Mapping

#include "cube.h"
#include <iostream>
#include <vector>
#include <stb_image.h>


#include "gl_env.h"
#include "normal_map_shader.h"

#include <config.h>


static unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

static void ComputeTBN(const glm::vec3 &pos1, const glm::vec3 &pos2, const glm::vec3 &pos3,
                       const glm::vec2 &uv1,  const glm::vec2 &uv2,  const glm::vec2 &uv3,
                       glm::vec3 *tangent, glm::vec3 *bitangent) {
    glm::vec3 edge1 = pos2 - pos1, edge2 = pos3 - pos1;
    glm::vec2 delta_uv1 = uv2 - uv1, delta_uv2 = uv3 - uv1;
    float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);

    tangent->x = f * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x);
    tangent->y = f * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y);
    tangent->z = f * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z);

    bitangent->x = f * (-delta_uv2.x * edge1.x + delta_uv1.x * edge2.x);
    bitangent->y = f * (-delta_uv2.x * edge1.y + delta_uv1.x * edge2.y);
    bitangent->z = f * (-delta_uv2.x * edge1.z + delta_uv1.x * edge2.z);
}

Cube::Cube(glm::vec3 position, float scale): MovableObject(position), scale_(scale) {
    glm::vec3 tangent, bitangent;
    for (int i = 0; i < NUM_VERTICES; i += 3) {  // iter through triangles
        int base[3] = {i * NUM_ATTRIB, (i + 1) * NUM_ATTRIB, (i + 2) * NUM_ATTRIB};
        std::vector<glm::vec3> pos;
        std::vector<glm::vec2> uv;
        for (int b : base) {  // iter through vertices
            pos.emplace_back(vertices_[b], vertices_[b + 1], vertices_[b + 2]);
            uv.emplace_back(vertices_[b + 6], vertices_[b + 7]);
        }
        ComputeTBN(pos[0], pos[1], pos[2], uv[0], uv[1], uv[2], &tangent, &bitangent);
        float tmp[] = {tangent.x, tangent.y, tangent.z, bitangent.x, bitangent.y, bitangent.z};
        int nbase[3] = {i * NUM_ATTRIB_W_TBN, (i + 1) * NUM_ATTRIB_W_TBN, (i + 2) * NUM_ATTRIB_W_TBN}; // new base
        for (int j = 0; j < 3; j++) {
            int b = base[j], nb = nbase[j];
            std::copy(vertices_ + b, vertices_ + b + NUM_ATTRIB, vertices_w_tbn_ + nb);
            std::copy(tmp, tmp + 6, vertices_w_tbn_ + nb + NUM_ATTRIB);
        }
    }

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    unsigned int vertex_vbo;
    glGenBuffers(1, &vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_w_tbn_), vertices_w_tbn_, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);   // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NUM_ATTRIB_W_TBN * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);   // norm
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, NUM_ATTRIB_W_TBN * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);   // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, NUM_ATTRIB_W_TBN * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);   // tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, NUM_ATTRIB_W_TBN * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);   // bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, NUM_ATTRIB_W_TBN * sizeof(float), (void*)(11 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int vertex_shader, fragment_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &normal_map::vertex_shader_330, NULL);
    glCompileShader(vertex_shader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &normal_map::fragment_shader_330, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    shader_ = glCreateProgram();
    glAttachShader(shader_, vertex_shader);
    glAttachShader(shader_, fragment_shader);
    glLinkProgram(shader_);
    glUseProgram(shader_);
    glUniform1i(glGetUniformLocation(shader_, "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(shader_, "normalMap"), 1);

    diffuse_map_ = loadTexture(DATA_DIR"/texture.bmp");
    normal_map_  = loadTexture(DATA_DIR"/normal.bmp");
}

void Cube::Draw(const glm::mat4 &view_projection, const glm::vec3 camera_pos,
               const glm::vec3 &light_color, const glm::vec3 &light_pos) {
    glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), position_);
    glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), glm::vec3(scale_, scale_, scale_));
    glm::mat4 model = translate * scale;

    glUseProgram(shader_);
    glUniformMatrix4fv(glGetUniformLocation(shader_, "u_view_projection"), 1, GL_FALSE, (const GLfloat*) &view_projection);
    glUniformMatrix4fv(glGetUniformLocation(shader_, "u_model"), 1, GL_FALSE, (const float*) &model);
    glUniform3f(glGetUniformLocation(shader_, "lightPos"), light_pos.x, light_pos.y, light_pos.z);
    glUniform3f(glGetUniformLocation(shader_, "lightColor"), light_color.x, light_color.y, light_color.z);
    glUniform3f(glGetUniformLocation(shader_, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map_);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_map_);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
    glBindVertexArray(0);
}