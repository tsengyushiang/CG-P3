#pragma once

#include "vgl.h"
#include "LoadShaders.h"
#include "TextureApp.h"
#include <string>
#include <vector>
using namespace std;
#define WALK 1
#define IDLE 0

extern GLuint FBOprogram;
extern GLuint ShaderTextureProgram;
extern GLuint StaticTexture1;
extern GLuint robot_texture;
extern GLuint shader_texture;
extern GLuint cubemapTexture;
extern GLuint RobotProgram;
extern int action;

void createMenu();
void MainmenuEvents(int option);
void TextureMenuEvents(int option);
void ActionMenuEvents(int option);
void ShaderMenuEvents(int option);
void SceneMenuEvents(int option);

void ReLoadProgram(GLuint& programId, char* vp, char* fp);
void ReLoadCubeMap(GLuint& textureID,string folderName);

unsigned int loadCubemap(vector<string> faces);