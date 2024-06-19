#version 330 core

out vec4 frag_color;

in vec2 v2f_tex_coords;

//gbuffer
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

uniform sampler2D ssao;

struct Light {
    
    vec3 position;
    vec3 color;
    
    float linear;
    float quad;
};
uniform Light light;

void main()
{             
    
    vec3 frag_pos = texture(g_position, v2f_tex_coords).rgb;
    vec3 normal = texture(g_normal, v2f_tex_coords).rgb;
    vec3 albedo = texture(g_albedo, v2f_tex_coords).rgb;
    
    float occlusion = texture(ssao, v2f_tex_coords).r;
    


    vec3 ambient = vec3(0.3 * albedo * occlusion);
    vec3 lighting  = ambient; 
    vec3 view_dir  = normalize(-frag_pos);//view space
    // diffuse
    vec3 light_dir = normalize(light.position - frag_pos);
    vec3 diffuse = max(dot(normal, light_dir), 0.0) * albedo * light.color;
    // specular
    vec3 half_dir = normalize(light_dir + view_dir);  
    float spec = pow(max(dot(normal, half_dir), 0.0), 8.0);
    vec3 specular = light.color * spec;
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quad * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    
    lighting += diffuse + specular;

    frag_color = vec4(lighting, 1.0);
}
