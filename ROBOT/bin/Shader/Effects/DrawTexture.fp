#version 330

uniform float iTime;
uniform sampler2D iChannel0;
uniform vec2 iResolution; 

in vec2 UV;
vec2 fragCoord=UV*iResolution;

layout(location=0)out vec4 fragColor;



void main(void)
{ 			
	fragColor = texture(iChannel0,UV);
}
	
    