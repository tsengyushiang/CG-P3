#version 330

uniform float iTime;
uniform sampler2D iChannel0;
uniform vec2 iResolution; 

in vec2 UV;
vec2 fragCoord=UV*iResolution;

layout(location=0)out vec4 fragColor;

void main(void)
{ 			
	// distance from center of image, used to adjust blur
	vec2 uv = fragCoord.xy / iResolution.xy;
	float d = length(uv - vec2(0.5,0.5));
	
	// blur amount
	float blur = 0.0;	
	blur = (1.0 + sin(iTime*6.0)) * 0.5;
	blur *= 1.0 + sin(iTime*16.0) * 0.5;
	blur = pow(blur, 3.0);
	blur *= 0.05;
	// reduce blur towards center
	blur *= d;
	
	// final color
    vec3 col;
    col.r = texture( iChannel0, vec2(uv.x+blur,uv.y) ).r;
    col.g = texture( iChannel0, uv ).g;
    col.b = texture( iChannel0, vec2(uv.x-blur,uv.y) ).b;
		
	
	col *= 1.0 - d * 0.5;
	
    fragColor = vec4(col,1.0);
}
	
    