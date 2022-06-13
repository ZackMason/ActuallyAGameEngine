#version 460 core
out vec4 FragColor;

in vec3 voPos;
in vec3 voNorm;
in vec2 voUV;

uniform vec3 uColor;
uniform vec3 uSunDir = normalize(vec3(1,1,1));

const float Br = 0.0025;
const float Bm = 0.0003;
const float g =  0.9800;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

vec3 sky_color(vec2 uv) {
    float atmo = sqrt(1.0 - uv.y);
    vec3 color = vec3(0.2, 0.4, 0.8);

    float scatter = pow(uv.y/1.0, 1.0/15.0);
    scatter = 1.0 - clamp(scatter, 0.8, 1.0);

    vec3 scatter_color = mix(vec3(1.0), vec3(1.0, 0.3, 0.0) * 1.5, scatter);
    return mix(color, vec3(scatter_color), atmo/1.3);
}

vec2 rand2(vec2 p)
{
    p = vec2(dot(p, vec2(12.9898,78.233)), dot(p, vec2(26.65125, 83.054543))); 
    return fract(sin(p) * 43758.5453);
}

float rand(vec2 p)
{
    return fract(sin(dot(p.xy ,vec2(54.90898,18.233))) * 4337.5453);
}

float stars(in vec2 x, float numCells, float size, float br)
{
    vec2 n = x * numCells;
    vec2 f = floor(n);

	float d = 1.0e10;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            vec2 g = f + vec2(float(i), float(j));
			g = n - g - rand2(mod(g, numCells)) + rand(g);
            // Control size
            g *= 1. / (numCells * size);
			d = min(d, dot(g, g));
        }
    }

    return br * (smoothstep(.95, 1., (1. - sqrt(d))));
}

void main()
{
    vec3 pos = normalize(voPos);
    
    vec3 sun = normalize(uSunDir);
    float mu = dot(normalize(voPos), sun);
    float rayleigh = 3.0 / (8.0 * 3.1415) * (1.0 + mu * mu);
    vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);
    
    vec3 day_extinction = exp(-exp(-((pos.y + sun.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
    vec3 night_extinction = vec3(1.0 - exp(sun.y)) * 0.2;
    vec3 extinction = mix(day_extinction, night_extinction, -sun.y * 0.2 + 0.5);
    vec3 color = vec3(1);
    color.rgb = rayleigh * mie * extinction;

    //if (dot(color.rgb,color.rgb) < 0.15)
    {

        vec2 uv = vec2(
            atan(pos.y, pos.x),
            atan(length(pos.xy)/pos.z)
        );

        color.rgb += max(0.0, 1.0-pow(dot(color.rgb,color.rgb),.1250)) * stars(uv, 32, 0.04510, max(0.0, 2.0-length(extinction)));
    }

    if (pos.y < 0)
        color = mix(vec3(1.0), vec3(0.3412, 0.1569, 0.1569), -pos.y);
    
    color = pow(color * 1.0, vec3(2.2));
    FragColor = vec4(color, 1.0);
}



