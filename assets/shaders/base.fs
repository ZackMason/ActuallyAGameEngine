#version 460 core
out vec4 FragColor;

in vec3 voPos;
in vec3 voNorm;
in vec2 voUV;

uniform vec3 uColor;

void main()
{
    vec3 color = uColor.rgb;

    FragColor = vec4(vec3(color),1.);
}



