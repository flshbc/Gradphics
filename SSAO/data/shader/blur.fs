#version 330 core

out float frag_color;

in vec2 v2f_tex_coords;


uniform sampler2D ssao;

void main() 
{
    
    vec2 texel_size = 1.0 / vec2(textureSize(ssao, 0));

    float result = 0.0;
    for (int x = -2; x < 2; x++) 
    {
        for (int y = -2; y < 2; y++) 
        {
            
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            
            result += texture(ssao, v2f_tex_coords + offset).r;
        }
    }

    
    frag_color = result / (4.0 * 4.0);

    
}  