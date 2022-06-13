#version 460 core
out vec4 FragColor;

const float PI = 3.14159;

in vec3 voPos;
in vec3 voNorm;
in vec2 voUV;

uniform float uMetallic = 0.0;
uniform float uRoughness = 0.5;
uniform float uAO = 0.1;

uniform sampler2D uTexture;
uniform vec3 uLightDir = normalize(vec3(1,1,1));
uniform vec3 uLightColor = normalize(vec3(0.8431, 0.8784, 0.8471));
uniform vec3 uCameraPos;

#include <pbr.glsl>

void main()
{
    vec3 N = normalize(voNorm);
    vec3 V = normalize(uCameraPos - voPos);
    vec3 albedo = texture(uTexture, (voUV)).rgb;
    float metallic = uMetallic;
    float roughness = uRoughness;
    float ao = uAO;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	vec3 Lo = vec3(0.0);

    vec3 L = normalize(uLightDir);
    vec3 H = normalize(V + L);
    float distance = .4310;
    float attenuation = 1.0 / (distance*distance);
    vec3 radiance = uLightColor * attenuation;

    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; 

    vec3 ambient = albedo * ao;
    vec3 color = ambient + Lo;


    color = pow(color,vec3(2.2));
    FragColor = vec4(vec3(color),1.);
}



