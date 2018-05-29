#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;
//#include <cmath>

#include "vgl.h"
#include "LoadShaders.h"
#include "objloader.hpp"
#include "TextureApp.h"
#include "menu.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#define PARTSNUM 7

void updateModels();

void init();

void ChangeSize(int w,int h);
void display();
void Keyboard(unsigned char key, int x, int y);
void idle(int dummy);

int windowW=500, windowH=500;
int mouseX,mouseY;
float roomSize=5;
float cameraAngleY;
float cameraAngleX;
bool mouseLeftDown;
void mouseWheel(int button, int dir, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int, int);

mat4 translate(float x,float y,float z);
mat4 scale(float x,float y,float z);
mat4 rotate(float angle,float x,float y,float z);

void Obj2Buffer();
void load2Buffer( char* obj,int);
void updateObj(int);
void resetObj(int);



bool isFrame;

GLuint VAO;
GLuint VBO;
GLuint uVBO;
GLuint nVBO;
GLuint mVBO;
GLuint UBO;
GLuint VBOs[PARTSNUM];
GLuint uVBOs[PARTSNUM];
GLuint nVBOs[PARTSNUM];
GLuint RobotProgram;


float angles[PARTSNUM];
float position = 0.0;
float angle = 0.0;
float size = 1;

GLfloat movex,movey;
GLint MatricesIdx;
GLuint ModelID;
GLuint M_KaID;
GLuint M_KdID;
GLuint M_KsID;

int vertices_size[PARTSNUM];
int uvs_size[PARTSNUM];
int normals_size[PARTSNUM];
int materialCount[PARTSNUM];

std::vector<std::string> mtls[PARTSNUM];//use material
std::vector<unsigned int> faces[PARTSNUM];//face count
map<string,vec3> KDs;//mtl-name&Kd
map<string,vec3> KSs;//mtl-name&Ks

mat4 Projection ;
mat4 View;
mat4 Model;
mat4 Models[PARTSNUM];

float currentTime=0;
int mode;

GLuint StaticTexture1;
int pNo;
int action;

GLuint FBO;
GLuint FBO2;
GLuint FVAO;
GLuint FVBOp;
GLuint FVBOuv;
GLuint ebo;
GLuint shader_texture, shader_texture_depth;
GLuint robot_texture;
GLuint robot_Frame,robot_depth;
GLuint FBOprogram;
GLuint ShaderTextureProgram;
const float vertices[] = {
1,1,-1,
-1,1,-1,
-1,-1,-1,
1,-1,-1,
};

const float uvs[] = {
1,1,
0,1,
0,0,
1,0, 
};
const GLushort vertex_indices[] =
{
	0,1,2,0,2,3
};


GLuint cubemapTexture;
GLuint cubemapProgram;
GLuint skyVAO;
GLuint skyVBO;

#define BOXSIZE 6
const float skyboxVertices[] = {
	// positions          
	-BOXSIZE,  BOXSIZE, -BOXSIZE,
	-BOXSIZE, -BOXSIZE, -BOXSIZE,
	BOXSIZE, -BOXSIZE, -BOXSIZE,
	BOXSIZE, -BOXSIZE, -BOXSIZE,
	BOXSIZE,  BOXSIZE, -BOXSIZE,
	-BOXSIZE,  BOXSIZE, -BOXSIZE,

	-BOXSIZE, -BOXSIZE,  BOXSIZE,
	-BOXSIZE, -BOXSIZE, -BOXSIZE,
	-BOXSIZE,  BOXSIZE, -BOXSIZE,
	-BOXSIZE,  BOXSIZE, -BOXSIZE,
	-BOXSIZE,  BOXSIZE,  BOXSIZE,
	-BOXSIZE, -BOXSIZE,  BOXSIZE,

	BOXSIZE, -BOXSIZE, -BOXSIZE,
	BOXSIZE, -BOXSIZE,  BOXSIZE,
	BOXSIZE,  BOXSIZE,  BOXSIZE,
	BOXSIZE,  BOXSIZE,  BOXSIZE,
	BOXSIZE,  BOXSIZE, -BOXSIZE,
	BOXSIZE, -BOXSIZE, -BOXSIZE,

	-BOXSIZE, -BOXSIZE,  BOXSIZE,
	-BOXSIZE,  BOXSIZE,  BOXSIZE,
	BOXSIZE,  BOXSIZE,  BOXSIZE,
	BOXSIZE,  BOXSIZE,  BOXSIZE,
	BOXSIZE, -BOXSIZE,  BOXSIZE,
	-BOXSIZE, -BOXSIZE,  BOXSIZE,

	-BOXSIZE,  BOXSIZE, -BOXSIZE,
	BOXSIZE,  BOXSIZE, -BOXSIZE,
	BOXSIZE,  BOXSIZE,  BOXSIZE,
	BOXSIZE,  BOXSIZE,  BOXSIZE,
	-BOXSIZE,  BOXSIZE,  BOXSIZE,
	-BOXSIZE,  BOXSIZE, -BOXSIZE,

	-BOXSIZE, -BOXSIZE, -BOXSIZE,
	-BOXSIZE, -BOXSIZE,  BOXSIZE,
	BOXSIZE, -BOXSIZE, -BOXSIZE,
	BOXSIZE, -BOXSIZE, -BOXSIZE,
	-BOXSIZE, -BOXSIZE,  BOXSIZE,
	BOXSIZE, -BOXSIZE,  BOXSIZE
};