#version 460 core

layout(local_size_x = 1, local_size_y = 1) in;


layout(rgba16f, binding = 0) uniform image2D uInput;
layout(rgba16f, binding = 1) uniform image2D uOutput;

vec3 BlurPixels(vec3 a, vec3 b, vec3 c, vec3 d, vec3 e, vec3 f, vec3 g, vec3 h, vec3 i)
{
    return 0.27343750 * (e    )
         + 0.21875000 * (d + f)
         + 0.10937500 * (c + g)
         + 0.03125000 * (b + h)
         + 0.00390625 * (a + i);
}


void main(){

    float kernel[] = {
        1.0, 2.0, 1.0,
        2.0, 4.0, 2.0,
        1.0, 2.0, 1.0
    };


    vec4 color = vec4(0);

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j);
            vec4 c = imageLoad(uInput, pixel_coords) * kernel[j * 3 + i] / 16.0;
            color += c ;//* step(dot(c.xyz, vec3(0.2126, 0.7152, 0.0722)), 1.0);
        }
    }
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    imageStore(uOutput, pixel_coords, color);
}