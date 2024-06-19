#version 330 core

out float frag_color;


in vec2 v2f_tex_coords;

//gbuffer
uniform sampler2D g_position;
uniform sampler2D g_normal;
//random rotation
uniform sampler2D noise_tex;

uniform vec3 samples[64];

// SSAO settings
int kernel_num = 64;
float radius = 0.5;
float bias = 0.025;


const vec2 noise_scale = vec2(800.0/4.0, 800.0/4.0); 

uniform mat4 projection;

void main()
{
    
    vec3 frag_pos = texture(g_position, v2f_tex_coords).xyz;
    vec3 normal = normalize(texture(g_normal, v2f_tex_coords).rgb);
    vec3 noise = normalize(texture(noise_tex, v2f_tex_coords * noise_scale).xyz);

    // TBN mat
    vec3 tangent = normalize(noise - normal * dot(noise, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // calc occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernel_num; i++)
    {
        
        vec3 sample_pos = TBN * samples[i];
        sample_pos = frag_pos + sample_pos * radius; 
        
       // screen space
        vec4 offset = vec4(sample_pos, 1.0);
        offset = projection * offset; 
        offset.xyz /= offset.w; 
        offset.xyz = offset.xyz * 0.5 + 0.5; //into [0,1]
        
        
        float sample_depth = texture(g_position, offset.xy).z; 
        
        
        // 当检测一个靠近表面边缘的片段时,将考虑测试表面之下的表面的深度值,这些值将会错误计算遮蔽因子
        // 只有被测深度值才在取样半径内时影响遮蔽因子,光滑插值
        float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));

        // 样本的深度值是否大于当前片段存储的深度值
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check;           
    }
    // 直接利用该结果可以缩放环境光照量
    occlusion = 1.0 - (occlusion / kernel_num);
    
    frag_color = occlusion;
    //frag_color = vec4(vec3(occlusion),1.0);//SSAO texture
}
