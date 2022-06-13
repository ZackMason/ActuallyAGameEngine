#version 460 core
out vec4 FragColor;

in vec2 voPos;
in vec2 voUV;

uniform sampler2D uTexture;

void main()
{
    vec3 color = texture(uTexture, voUV).rgb;

    FragColor = vec4(vec3(1.0-color),1.);
}



