#version 330

uniform float iTime;
uniform sampler2D iChannel0;
uniform vec2 iResolution; 

in vec2 UV;
vec2 fragCoord=UV*iResolution;

layout(location=0)out vec4 fragColor;


#ifdef GL_ES
precision lowp float;
#endif

#define resolution iResolution
#define time iTime
#define gl_FragColor fragColor
#define gl_FragCoord fragCoord
#define surfacePosition UV

float ti = time + 200.;

void main(){
 vec2 pos = surfacePosition-vec2(0.5,0.5);

 const float pi = 3.14159;
 const float n = 32.0;
 
 float radius = length(pos) * 2.0;
 float t = sin(pos.y) * sin(pos.x * ti);
 
 float color = 0.20;
 
 for (float i = 1.0; i <= n; i++){
  color += 0.003 / abs(0.77 * sin(
   3. * (t + i/n * ti * 0.0288888888888)
      ) - radius
  );
 }

 gl_FragColor = texture(iChannel0,UV)+vec4(vec3(0.3, 0.3, 0.5) * color, 1.);
 
}