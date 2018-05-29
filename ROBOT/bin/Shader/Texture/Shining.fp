#version 330

uniform float iTime;
uniform sampler2D iChannel0;
uniform vec2 iResolution; 

in vec2 UV;
vec2 iMouse=vec2(250,300);
vec2 fragCoord=UV*iResolution;

layout(location=0)out vec4 fragColor;

#define iterations 17
#define formuparam 0.53

#define volsteps 20
#define stepsize 0.1

#define zoom   0.800
#define tile   0.850
#define speed  0.010 

// variant of https://shadertoy.com/view/4scXWB


float flare( vec2 U )                              // rotating hexagon 
{	vec2 A = sin(vec2(0, 1.57) + 1.0);
    U = abs( U * mat2(A, -A.y, A.x) ) * mat2(2,0,1,1.7); 
    return .05/max(U.x,U.y);                        // glowing-spiky approx of step(max,.2)
  //return .02*pow(max(U.x,U.y), -2.);
 
}

#define r(x)     fract(1e4*sin((x)*541.17))        // rand, signed rand   in 1, 2, 3D.
#define sr2(x)   ( r(vec2(x,x+.1)) *2.-1. )
#define sr3(x)   ( r(vec4(x,x+.1,x+.2,0)) *2.-1. )

void main()
{

    vec2 R = iResolution.xy;
    fragCoord =  (fragCoord+fragCoord - R) / R.y;
	fragColor -= fragColor+.3;
    for (float i=0.; i<4e2; i++)
        fragColor += flare (fragCoord - sr2(i)*R/R.y )             // rotating flare at random location
              * r(i+.2)                            // random scale
              * pow(.5 + .5*sin( (10.+10.*r(i+.07)) *iTime + r(i+.35)*6.), 3.)*.4 // time pulse
           // * pow(1. + sin( (10.+10.*r(i+.07)) *iTime + r(i+.35)*6. ), 1.5) *.1 // time pulse
           // * .005/(1. + sin( (5.+5.*r(i+.07)) *iTime + r(i+.35)*6. ) ) *.1     // flashes
              * (1.+.1*sr3(i+.4));                 // random color - uncorrelated
}