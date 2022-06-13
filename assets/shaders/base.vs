#version 460 core

layout ( location = 0 ) in vec3 aPos;
layout ( location = 1 ) in vec3 aNormal;
layout ( location = 2 ) in vec2 aUV;

out vec3 voPos;
out vec3 voNorm;
out vec2 voUV;

uniform mat4 uVP;
uniform mat4 uM;

void main()
{
    voUV = aUV;
    voPos = (uM * vec4(aPos,1.)).xyz;
    voNorm = normalize(mat3(transpose(inverse(uM))) * aNormal);
    
    gl_Position = (uVP * uM * vec4(aPos,1));
}
