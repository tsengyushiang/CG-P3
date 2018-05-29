#version 330

uniform float iTime;
uniform sampler2D iChannel0;
uniform vec2 iResolution; 

in vec2 UV;
vec2 fragCoord=UV*iResolution;

layout(location=0)out vec4 fragColor;

void main()
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 centerUV = vec2(0.5);
    float distanceToCenter = abs(distance(uv, centerUV));
    float threshold = 0.5;
    float coefficient = 2.0;
    float speed = 1.0;
    float time = sin(iTime * speed);
    
    
    if (distanceToCenter > threshold) {
    	fragColor = texture(iChannel0, uv);
    }  else {
       
        float theta = min(max(1.0 - (distanceToCenter / threshold ), 0.0), 1.0) * (time * coefficient);
        vec2 diffUV = uv - centerUV;
        float x = diffUV.x * cos(theta) - diffUV.y * sin(theta);
        float y = diffUV.x * sin(theta) + diffUV.y * cos(theta);
        vec2 vortexUV = (vec2(x, y)  + centerUV);
        vec4 textureColor = texture(iChannel0, vortexUV);
    	fragColor = textureColor;
    }    
}