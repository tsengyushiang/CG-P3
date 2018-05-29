#version 330

uniform float iTime;
uniform sampler2D iChannel0;
uniform vec2 iResolution; 

in vec2 UV;
vec2 fragCoord=UV*iResolution;

layout(location=0)out vec4 fragColor;

//	Choose one of these to change the style of the crt
//#define X_ONLY
//#define Y_ONLY
#define X_AND_Y

// Will return a value of 1 if the 'x' is < 'value'
float Less(float x, float value)
{
	return 1.0 - step(value, x);
}

// Will return a value of 1 if the 'x' is >= 'lower' && < 'upper'
float Between(float x, float  lower, float upper)
{
    return step(lower, x) * (1.0 - step(upper, x));
}

//	Will return a value of 1 if 'x' is >= value
float GEqual(float x, float value)
{
    return step(value, x);
}

void main()
{
    float brightness = 1.25;
	vec2 uv = fragCoord.xy / iResolution.xy;
    uv.y = uv.y;
    //uv = uv * 0.05;
    
    vec2 uvStep;
    uvStep.x = uv.x / (1.0 / iResolution.x);
    uvStep.x = mod(uvStep.x, 3.0);
    uvStep.y = uv.y / (1.0 / iResolution.y);
    uvStep.y = mod(uvStep.y, 3.0);
    
    vec4 newColour = texture(iChannel0, uv);
    
#ifdef X_ONLY
    newColour.r = newColour.r * Less(uvStep.x, 1.0);
    newColour.g = newColour.g * Between(uvStep.x, 1.0, 2.0);
    newColour.b = newColour.b * GEqual(uvStep.x, 2.0);
#endif
    
#ifdef Y_ONLY
    newColour.r = newColour.r * Less(uvStep.y, 1.0);
    newColour.g = newColour.g * Between(uvStep.y, 1.0, 2.0);
    newColour.b = newColour.b * GEqual(uvStep.y, 2.0);
#endif
    
#ifdef X_AND_Y
    newColour.r = newColour.r * step(1.0, (Less(uvStep.x, 1.0) + Less(uvStep.y, 1.0)));
    newColour.g = newColour.g * step(1.0, (Between(uvStep.x, 1.0, 2.0) + Between(uvStep.y, 1.0, 2.0)));
    newColour.b = newColour.b * step(1.0, (GEqual(uvStep.x, 2.0) + GEqual(uvStep.y, 2.0)));
#endif
    
	fragColor = newColour * brightness;
}
