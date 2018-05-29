// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
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
   
    // Dot product gives us diffuse intensity
    float diff = max(0.0, dot(normalize(vVaryingNormal),
					normalize(vVaryingLightDir)));

    // Multiply intensity by diffuse color, force alpha to 1.0
    vFragColor = diff * diffuseColor*vec4(Material.Kd,1);

    // Add in ambient light
    vFragColor += ambientColor;

	

    // Specular Light
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),
								normalize(vVaryingNormal)));//反射角
    float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));
    if(diff != 0) {
		spec = pow(spec, Shininess);
		vFragColor += specularColor*vec4(Material.Ka,1)*spec;
    }
	
    vec4 texcolor=texture(tex,UV);
   vFragColor=mix(vFragColor,texcolor,0.6); 
}
	
    