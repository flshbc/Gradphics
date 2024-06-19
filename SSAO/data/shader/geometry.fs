#version 330 core
// gbuffer info
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 albedo;

in vec3 v2f_pos;
in vec2 v2f_tex_coords;
in vec3 v2f_normal;

void main()
{   
    
    position = v2f_pos;
    normal = normalize(v2f_normal);
    
   
    albedo = vec3(0.95);
}