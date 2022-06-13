#version 460 core
out vec4 FragColor;

in vec2 voPos;
in vec2 voUV;

uniform sampler2D uTexture;

uniform int uTonemap;
uniform float uExposure = 1.0;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(uTexture, voUV).rgb;
    
    switch (uTonemap) {
        case 0:
            color = ACESFilm(color);
            break;
        case 1:
            color = color / (color + vec3(1.0));
            break;
        case 2:
            color = vec3(1.0) - exp(-color * uExposure);
    }
    
    //color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(vec3(color),1.);
}



