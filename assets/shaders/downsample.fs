#version 460 core
out vec4 FragColor;

in vec2 voPos;
in vec2 voUV;

uniform sampler2D uTexture;

void main()
{
    vec3 color = textureLod(uTexture, voUV, 2).rgb;

    FragColor = vec4(vec3(color),1.);
}



