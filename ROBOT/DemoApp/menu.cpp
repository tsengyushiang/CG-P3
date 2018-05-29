#include "menu.h"

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		char *cstr = new char[faces[i].length() + 1];
		strcpy(cstr, faces[i].c_str());
		unsigned char *data = TextureApp::LoadImageA(cstr, width, height);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0,GL_BGRA, GL_UNSIGNED_BYTE, data
			);
			free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void ReLoadCubeMap(GLuint& textureID, std::string folderName)
{
	if (textureID)glDeleteTextures(1, &textureID);
	std::vector<std::string> faces =
	{
		"right.png",
		"left.png",
		"up.png",
		"down.png",
		"back.png",
		"front.png"
	};

	for (string& str : faces)
	{
		str = folderName + str;
	}

	textureID = loadCubemap(faces);
}

void ReLoadProgram(GLuint& programId, char* vp, char* fp)
{
	if (programId)glDeleteProgram(programId);
	ShaderInfo shaders[3];
	
	shaders[0] = { GL_VERTEX_SHADER, vp };// vertex shader
	shaders[1] = { GL_FRAGMENT_SHADER, fp };//fragment shader
	shaders[2] = { GL_NONE, NULL };

	programId = LoadShaders(shaders);
}

void createMenu()
{
	int ActionMenu, ShaderMenu,TextureMenu,mainMenu, SceneMenu;
	mainMenu = glutCreateMenu(MainmenuEvents);
	ActionMenu = glutCreateMenu(ActionMenuEvents);
	ShaderMenu = glutCreateMenu(ShaderMenuEvents);
	TextureMenu = glutCreateMenu(TextureMenuEvents);
	SceneMenu = glutCreateMenu(SceneMenuEvents);

	glutSetMenu(mainMenu);
	glutAddSubMenu("Action", ActionMenu);
	glutAddSubMenu("Effects", ShaderMenu);
	glutAddSubMenu("RobotEffects", TextureMenu);
	glutAddSubMenu("Scene", SceneMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);	//ªP•k¡‰√ˆ¡p

	glutSetMenu(ActionMenu);
	glutAddMenuEntry("idle", 0);
	glutAddMenuEntry("walk", 1);
	
	
	glutSetMenu(ShaderMenu);
	glutAddMenuEntry("None", 0);
	glutAddMenuEntry("CRT", 9);
	glutAddMenuEntry("Toon", 10);
	glutAddMenuEntry("Flame", 2);
	glutAddMenuEntry("Ether", 4);
	glutAddMenuEntry("Swirl", 7);
	glutAddMenuEntry("LightBall", 5);
	glutAddMenuEntry("RadialBlur", 8);
	glutAddMenuEntry("Chromatical", 3);
	glutAddMenuEntry("NightVision", 6);
	glutAddMenuEntry("Edge Detector", 1);	
	

	glutSetMenu(TextureMenu);
	glutAddMenuEntry("Beam", 4);
	glutAddMenuEntry("Lava", 6);
	glutAddMenuEntry("Colors", 2);
	glutAddMenuEntry("Default", 1);	
	glutAddMenuEntry("Shining", 3);
	glutAddMenuEntry("Reflection", 5);

	glutSetMenu(SceneMenu);
	glutAddMenuEntry("univers", 0);
	glutAddMenuEntry("ocean", 1);
	glutAddMenuEntry("desert", 2);
	glutAddMenuEntry("frozen", 3);

}
void MainmenuEvents(int option) {
}

void SceneMenuEvents(int option) {
	switch (option) {
	case 0:
		ReLoadCubeMap(cubemapTexture, "Media/Texture/universe/");
		glutPostRedisplay();
		break;
	case 1:
		ReLoadCubeMap(cubemapTexture, "Media/Texture/ocean/");
		glutPostRedisplay();
		break;
	case 2:
		ReLoadCubeMap(cubemapTexture, "Media/Texture/desert/");
		glutPostRedisplay();
		break;
	case 3:
		ReLoadCubeMap(cubemapTexture, "Media/Texture/frozen/");
		glutPostRedisplay();
		break;
	}

}
void TextureMenuEvents(int option) {
	switch (option) {	
	case 1:
		robot_texture = StaticTexture1;

		ReLoadProgram(RobotProgram, "Shader/robot.vp", "Shader/robot.fp");
		glutPostRedisplay();
		break;
	case 2:
		robot_texture = shader_texture;

		ReLoadProgram(RobotProgram, "Shader/robot.vp", "Shader/robot.fp");
		ReLoadProgram(ShaderTextureProgram,
			"Shader/DrawTexture.vp",
			"Shader/Texture/Colors.fp");
		glutPostRedisplay();
		break;
	case 3:
		robot_texture = shader_texture;

		ReLoadProgram(RobotProgram, "Shader/robot.vp", "Shader/robot.fp");
		ReLoadProgram(ShaderTextureProgram,
			"Shader/DrawTexture.vp",
			"Shader/Texture/Shining.fp");
		glutPostRedisplay();
		break;
	case 4:
		robot_texture = shader_texture;

		ReLoadProgram(RobotProgram, "Shader/robot.vp", "Shader/robot.fp");
		ReLoadProgram(ShaderTextureProgram,
			"Shader/DrawTexture.vp",
			"Shader/Texture/Beam.fp");
		glutPostRedisplay();
		break;
	case 5:
		ReLoadProgram(RobotProgram, "Shader/robot.vp", "Shader/Texture/Reflection.fp");
		glutPostRedisplay();
		break;
	case 6:
		robot_texture = shader_texture;

		ReLoadProgram(RobotProgram, "Shader/robot.vp", "Shader/robot.fp");
		ReLoadProgram(ShaderTextureProgram,
			"Shader/DrawTexture.vp",
			"Shader/Texture/Lava.fp");
		glutPostRedisplay();
		break;	
	}
}
void ActionMenuEvents(int option) {
	switch (option) {
	case 0:
		action = IDLE;
		break;
	case 1:
		action = WALK;
		break;
	}
}
void ShaderMenuEvents(int option) {	

	switch (option) {
	case 0:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/DrawTexture.fp");
		glutPostRedisplay();
		break;
	case 1:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/EdgeDetector.fp");
		glutPostRedisplay();
		break;
	case 2:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/Flame.fp");
		glutPostRedisplay();
		break;	
	case 3:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/Chromatical.fp");
		glutPostRedisplay();
		break;
	case 4:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/Ether.fp");
		glutPostRedisplay();
		break;
	case 5:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/LightBall.fp");
		glutPostRedisplay();
		break;
	case 6:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/NightVision.fp");
		glutPostRedisplay();
		break;
	case 7:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/Swirl.fp");
		glutPostRedisplay();
		break;
	case 8:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/RadialBlur.fp");
		glutPostRedisplay();
		break;
	case 9:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/CRT.fp");
		glutPostRedisplay();
		break;
	case 10:
		ReLoadProgram(FBOprogram,
			"Shader/DrawTexture.vp",
			"Shader/Effects/Toon.fp");
		glutPostRedisplay();
		break;
	}
}