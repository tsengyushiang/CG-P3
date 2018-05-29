#version 330

uniform float iTime;
uniform sampler2D iChannel0;
uniform vec2 iResolution; 

in vec2 UV;
vec2 fragCoord=UV*iResolution;

layout(location=0)out vec4 fragColor;

const int nsamples =3;

void main()
{
    vec2 center = vec2(cos(iTime)*iResolution.x/5.+iResolution.x/2.,iResolution.y/5.*sin(iTime)+iResolution.y/2.) /iResolution.xy;
	float blurStart = 1.0;
    float blurWidth = 0.1;

    
	vec2 uv = fragCoord.xy / iResolution.xy;
    
    uv -= center;
    float precompute = blurWidth * (1.0 / float(nsamples - 1));
    
    vec4 color = vec4(0.0);
    for(int i = 0; i < nsamples; i++)
    {
        float scale = blurStart + (float(i)* precompute);
        color += texture(iChannel0, uv * scale + center);
    }
    
    
    color /= float(nsamples);
    
	fragColor = color;
}