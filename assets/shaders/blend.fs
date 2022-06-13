#version 460 core
out vec4 FragColor;

in vec2 voPos;
in vec2 voUV;

uniform sampler2D uTexture1;
uniform sampler2D uTexture2;

void main()
{
    //vec3 color = mix(texture(uTexture1, voUV).rgb, texture(uTexture2, voUV).rgb, vec3(0.5));
    vec3 color = texture(uTexture1, voUV).rgb + texture(uTexture2, voUV).rgb * 0.25;

    FragColor = vec4(vec3(color),1.);
}



