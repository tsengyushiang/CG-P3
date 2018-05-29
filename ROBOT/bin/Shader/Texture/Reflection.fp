
#version 330

struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

uniform MaterialInfo Material;
layout(location=0)out vec4 vFragColor;
uniform float time;

//lighting color
vec4    ambientColor = vec4(0.1,0.1,0.1,1);
vec4    diffuseColor = vec4(1,1,1,1);   
vec4    specularColor = vec4(1,1,1,1);
float Shininess = 100.0;//for material specular

in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec2 UV;
uniform sampler2D tex;

//===============================Cubemap
in vec3 Normal;
in vec3 Position;
uniform vec3 cameraPos;
uniform samplerCube skybox;

void main(void)
{    
   vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vFragColor = vec4(texture(skybox, vec3(R.x,R.y*-1,R.z)).rgb, 1.0);
}
	
    