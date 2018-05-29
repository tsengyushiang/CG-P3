#include "main.h"

vec3 camera = vec3(0,0,10);

GLuint createTexture(int w, int h, bool isDepth = false)
{

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		!isDepth ? GL_RGBA : GL_DEPTH_COMPONENT,
		w,
		h,
		0,
		isDepth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_FLOAT,
		0
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return texture;
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitContextVersion(4,3);//以OpenGL version4.3版本為基準
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);//是否向下相容,GLUT_FORWARD_COMPATIBLE不支援(?
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//multisample for golygons smooth
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(windowW, windowH);
	glutCreateWindow("OpenGL 4.3 - Robot");

	glewExperimental = GL_TRUE; //置於glewInit()之前
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	
	init();
	createMenu();
	
	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutMouseWheelFunc(mouseWheel);
	glutTimerFunc (100, idle, 0); 
	glutMainLoop();

	return 0;
}
void ChangeSize(int w,int h){
	windowW = w;
	windowH = h;
	if(h == 0) windowH = 1;

	glViewport(0,0, windowW, windowH);
	Projection = perspective(80.0f,(float)1,0.1f,1000.0f);

	if (robot_Frame)glDeleteTextures(1, &robot_Frame);
	if (robot_depth)glDeleteTextures(1, &robot_depth);
	robot_Frame = createTexture(windowW, windowH);
	robot_depth = createTexture(windowW, windowH, true);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, robot_Frame, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, robot_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void idle(int dummy){

	currentTime+=0.3;

	isFrame = true;
	int out = 0;
	if(action == WALK){
		updateObj(dummy);
		out = dummy+1;
		if(out > 12) out = 1;
	}
	else if(action == IDLE){
		resetObj(dummy);
		out = 0;
	}
	glutPostRedisplay();
	
	glutTimerFunc (150, idle, out); 
}
void resetObj(int f){
	for(int i = 0 ; i < PARTSNUM;i++){
		angles[i] = 0.0f;
	}	
}
void updateObj(int frame){
	switch(frame){
	case 0:
		//左手
		angles[3] = -45;
		//右手

		//腿
		angles[6] = 45;	
		
		break;
	case 1:
	case 2:
	case 3:
		angles[1] +=10;
		angles[5] -=15;
		position += 0.05;
		break;
	case 4:
	case 5:
	case 6:
		angles[1] -=10;
		angles[5] +=15;
		angles[6] -= 15;
		position -= 0.05;
		break;
	case 7:
	case 8:
	case 9:
		angles[1] -=10;
		angles[5] +=15;
		angles[6] = 0;
		position += 0.05;
		break;
	case 10:
	case 11:
	case 12:
		angles[1] +=10;
		angles[5] -=15;
		angles[6] += 15;
		position -= 0.05;
		break;
	}
}

void init(){
	isFrame = false;
	pNo = 0;
	for(int i = 0;i<PARTSNUM;i++)//初始化角度陣列
		angles[i] = 0.0;

	//VAO
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	ReLoadProgram(RobotProgram, "Shader/robot.vp", "Shader/robot.fp");

	glUseProgram(RobotProgram);//uniform參數數值前必須先use shader
	MatricesIdx = glGetUniformBlockIndex(RobotProgram,"MatVP");
	ModelID =  glGetUniformLocation(RobotProgram,"Model");
    M_KaID = glGetUniformLocation(RobotProgram,"Material.Ka");
	M_KdID = glGetUniformLocation(RobotProgram,"Material.Kd");
	M_KsID = glGetUniformLocation(RobotProgram,"Material.Ks");

	Projection = glm::perspective(80.0f,4.0f/3.0f,0.1f,1000.0f);

	// Camera matrix
	View       = glm::lookAt(
		glm::vec3(0,0,10) , // Camera is at (0,10,25), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,1,0 to look upside-down)
		);

	Obj2Buffer();

	//UBO
	glGenBuffers(1,&UBO);
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferData(GL_UNIFORM_BUFFER,sizeof(mat4)*2,NULL,GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(RobotProgram, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);  
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER,0,UBO,0,UBOsize);
	glUniformBlockBinding(RobotProgram, MatricesIdx,0);
	/*=========================================================TEXTURE*/	
	StaticTexture1 = TextureApp::GenTexture("Media/Texture/iron.png");

	robot_texture = StaticTexture1;


	shader_texture = createTexture(windowW, windowH);
	shader_texture_depth = createTexture(windowW, windowH, true);

	ReLoadProgram(ShaderTextureProgram, "Shader/DrawTexture.vp", "Shader/Texture/Colors.fp");

	glGenFramebuffers(1, &FBO2);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shader_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shader_texture_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*==============================================================FBO*/

	robot_Frame = createTexture(windowW,windowH);
	robot_depth = createTexture(windowW, windowH,true);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, robot_Frame,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, robot_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	/*===================================================================texture for FBO*/
	
	ReLoadProgram(FBOprogram, "Shader/DrawTexture.vp", "Shader/Effects/DrawTexture.fp");
	

	glGenVertexArrays(1, &FVAO);
	glBindVertexArray(FVAO);
	
	glGenBuffers(1, &FVBOp);
	glBindBuffer(GL_ARRAY_BUFFER, FVBOp);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &FVBOuv);
	glBindBuffer(GL_ARRAY_BUFFER, FVBOuv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
	
	/*============================================================================SkyBox*/
	ReLoadCubeMap(cubemapTexture, "Media/Texture/universe/");

	ReLoadProgram(cubemapProgram, "Shader/skybox/skybox.vp", "Shader/skybox/skybox.fp");

	glGenVertexArrays(1, &skyVAO);
	glBindVertexArray(skyVAO);

	glGenBuffers(1, &skyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*===================================================================================*/

	glClearColor(0.2,0.2,0.2,1);
}

#define DOR(angle) (angle*3.1415/180)
void display(){

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*=======================================================================Robot Texture*/

	glUseProgram(ShaderTextureProgram);
	glBindVertexArray(FVAO);

	View = glm::lookAt(
		glm::vec3(0, 0, 0.1), // Camera is at (0,10,25), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
	);

	float resolution[] = { windowW,windowH };
	glUniform1f(glGetUniformLocation(ShaderTextureProgram, "iTime"), currentTime);
	glUniformMatrix4fv(glGetUniformLocation(ShaderTextureProgram, "View"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(ShaderTextureProgram, "Projection"), 1, GL_FALSE, &Projection[0][0]);
	glUniform2f(glGetUniformLocation(ShaderTextureProgram, "iResolution"), resolution[0], resolution[1]);
	glUniformMatrix4fv(glGetUniformLocation(ShaderTextureProgram, "Model"), 1, GL_FALSE, &(scale(1, 1, 1)[0][0]));

	glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
	/*=======================================================================skyBox*/
	
	float anglex = DOR(cameraAngleX) / 3;
	float angley = cameraAngleY / 36;

	View = glm::lookAt(
		glm::vec3(roomSize*sin(anglex), angley, roomSize*cos(anglex)), // Camera is at (0,10,25), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
	);

	glDepthMask(GL_FALSE);
	glUseProgram(cubemapProgram);

	glUniformMatrix4fv(glGetUniformLocation(ShaderTextureProgram, "View"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(ShaderTextureProgram, "Projection"), 1, GL_FALSE, &Projection[0][0]);

	glBindVertexArray(skyVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_TRUE);
	// ... draw rest of the scene

	/*=======================================================================RenderRobot*/
	glUseProgram(RobotProgram);
	
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, robot_texture);
	glUniform1f(glGetUniformLocation(RobotProgram,"time"),currentTime);	
	glUniform3f(glGetUniformLocation(RobotProgram, "cameraPos"), roomSize*sin(anglex), angley, roomSize*cos(anglex));

	updateModels();
	//update data to UBO for MVP
	glBindBuffer(GL_UNIFORM_BUFFER,UBO);
	glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(mat4),&View);
	glBufferSubData(GL_UNIFORM_BUFFER,sizeof(mat4),sizeof(mat4),&Projection);
	glBindBuffer(GL_UNIFORM_BUFFER,0);

	GLuint offset[3] = {0,0,0};//offset for vertices , uvs , normals
	for(int i = 0;i < PARTSNUM ;i++){
		glUniformMatrix4fv(ModelID,1,GL_FALSE,&Models[i][0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,				//location
							  3,				//vec3
							  GL_FLOAT,			//type
							  GL_FALSE,			//not normalized
							  0,				//strip
							  (void*)offset[0]);//buffer offset
		//(location,vec3,type,固定點,連續點的偏移量,buffer point)
		offset[0] +=  vertices_size[i]*sizeof(vec3);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);//location 1 :vec2 UV
		glBindBuffer(GL_ARRAY_BUFFER, uVBO);
		glVertexAttribPointer(1, 
							  2, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[1]);
		//(location,vec2,type,固定點,連續點的偏移量,point)
		offset[1] +=  uvs_size[i]*sizeof(vec2);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);//location 2 :vec3 Normal
		glBindBuffer(GL_ARRAY_BUFFER, nVBO);
		glVertexAttribPointer(2,
							  3, 
							  GL_FLOAT, 
							  GL_FALSE, 
							  0,
							  (void*)offset[2]);
		//(location,vec3,type,固定點,連續點的偏移量,point)
		offset[2] +=  normals_size[i]*sizeof(vec3);

		int vertexIDoffset = 0;//glVertexID's offset 
		string mtlname;//material name
		vec3 Ks = vec3(1,1,1);//because .mtl excluding specular , so give it here.
		for(int j = 0;j <mtls[i].size() ;j++){//
			mtlname = mtls[i][j];	
			//find the material diffuse color in map:KDs by material name.
			glUniform3fv(M_KdID,1,&KDs[mtlname][0]);
			glUniform3fv(M_KsID,1,&Ks[0]);
			//          (primitive   , glVertexID base , vertex count    )
	
			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
			glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[i][j + 1] * 3);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);			

			vertexIDoffset += faces[i][j+1]*3;//glVertexID's base offset is face count*3
		}//end for loop for draw one part of the robot	
		
	}//end for loop for updating and drawing model
	/*=====================================================================RenderFBO*/

	glUseProgram(FBOprogram);
	glBindVertexArray(FVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, robot_Frame);

	View = glm::lookAt(
		glm::vec3(0,0,0.1), // Camera is at (0,10,25), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
	);
		
	glUniform1f(glGetUniformLocation(FBOprogram, "iTime"), currentTime);
	glUniformMatrix4fv(glGetUniformLocation(FBOprogram, "View"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(FBOprogram, "Projection"), 1, GL_FALSE , &Projection[0][0]);
	glUniform2f(glGetUniformLocation(FBOprogram, "iResolution"), resolution[0], resolution[1]);
	glUniformMatrix4fv(glGetUniformLocation(FBOprogram, "Model"), 1, GL_FALSE, &(scale(1,1,1)[0][0]));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	/*==============================================================================*/

	glFlush();//強制執行上次的OpenGL commands
	glutSwapBuffers();//調換前台和後台buffer ,當後臺buffer畫完和前台buffer交換使我們看見它
}

void Obj2Buffer(){
	std::vector<vec3> Kds;
	std::vector<vec3> Kas;
	std::vector<vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::string texture;
	loadMTL("Media/Obj/robot/whole.mtl",Kds,Kas,Kss,Materials,texture);
	//printf("%d\n",texture);
	for(int i = 0;i<Materials.size();i++){
		string mtlname = Materials[i];
		//  name            vec3
		KDs[mtlname] = Kds[i];
	}

	
	load2Buffer("Media/Obj/robot/upbody.obj", 0);

	load2Buffer("Media/Obj/robot/lefthand.obj",1);
	
	load2Buffer("Media/Obj/robot/head.obj", 2);

	load2Buffer("Media/Obj/robot/righthand.obj",3);

	load2Buffer("Media/Obj/robot/downbody.obj",4);	

	load2Buffer("Media/Obj/robot/leftleg.obj",5);

	load2Buffer("Media/Obj/robot/rightleg.obj",6);	
	
	GLuint totalSize[3] = {0,0,0};
	GLuint offset[3] = {0,0,0};
	for(int i = 0;i < PARTSNUM ;i++){
		totalSize[0] += vertices_size[i]*sizeof(vec3);
		totalSize[1] += uvs_size[i] * sizeof(vec2);
		totalSize[2] += normals_size[i] * sizeof(vec3);
	}
	//generate vbo
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&uVBO);
	glGenBuffers(1,&nVBO);
	//bind vbo ,第一次bind也同等於 create vbo 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[0],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[1],NULL,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nVBO);//VBO的target是GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER,totalSize[2],NULL,GL_STATIC_DRAW);
	
	
	for(int i = 0;i < PARTSNUM ;i++){
		glBindBuffer(GL_COPY_WRITE_BUFFER,VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[0],vertices_size[i]*sizeof(vec3));
		offset[0] += vertices_size[i]*sizeof(vec3);
		glInvalidateBufferData(VBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[1],uvs_size[i]*sizeof(vec2));
		offset[1] += uvs_size[i]*sizeof(vec2);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);

		glBindBuffer(GL_COPY_WRITE_BUFFER,nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,0,offset[2],normals_size[i]*sizeof(vec3));
		offset[2] += normals_size[i] * sizeof(vec3);
		glInvalidateBufferData(uVBOs[i]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);
	}
	glBindBuffer(GL_COPY_WRITE_BUFFER,0);


}

void updateModels(){	

	mat4 Rotatation[PARTSNUM];
	mat4 Translation[PARTSNUM];
	for(int i = 0 ; i < PARTSNUM;i++){
		Models[i] = mat4(1.0f);
		Rotatation[i] = mat4(1.0f);
		Translation[i] = mat4(1.0f); 
	}
	float r,pitch,yaw,roll;
	float alpha, beta ,gamma;

	//Body
	beta = angle;
	Rotatation[0] = rotate(beta,0,1,0);
	Translation[0] = translate(0.0,-1+position,0.1);
	Models[0] = Translation[0]*Rotatation[0];
	//左手=======================================================
	//左上手臂
	yaw = DOR(beta);r = 3.7;
	alpha = angles[1];
	gamma = 10;
	Rotatation[1] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);//向前旋轉*向右旋轉
	Translation[1] = translate(0.9,0.6,0.25);

	Models[1] = Models[0]*Translation[1]*Rotatation[1];
	//============================================================
	//頭==========================================================
	Translation[2] = translate(-0.1,0.5,0.35);
	Rotatation[2] = rotate(0, 0, 0, 0);
	Models[2] = Models[0]*Translation[2]*Rotatation[2];
	//============================================================
	//右手=========================================================
	gamma = -10;alpha = angles[3] = -angles[1];
	Rotatation[3] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	Translation[3] = translate(-0.9,0.62,0.25);
	Models[3] = Models[0]*Translation[3]*Rotatation[3];	
	//=============================================================
	//back&DBody===================================================	
	Translation[4] =translate(0.1,-1.5+position,0.3);
	Models[4] = Translation[4]*Rotatation[4];
	//=============================================================
	//左腳
	alpha = angles[5];gamma = 10;
	Rotatation[5] = rotate(alpha,1,0,0)*rotate(gamma,0,0,1);
	Translation[5] =translate(0.2,1.4,-0.3);
	Models[5] = Models[4]*Translation[4]*Translation[5]*Rotatation[5];
	//=============================================================
	//右腳
	alpha = angles[6] = -angles[5];
	gamma = -10;
	Rotatation[6] = rotate(alpha ,1,0,0)*rotate(gamma ,0,0,1);
	Translation[6] =translate(-0.6,1.4,-0.3);
	Models[6] = Models[4]*Translation[4]*Translation[6]*Rotatation[6];
	//=============================================================
}

void load2Buffer(char* obj,int i){
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals; // Won't be used at the moment.
	std::vector<unsigned int> materialIndices;

	bool res = loadOBJ(obj, vertices, uvs, normals,faces[i],mtls[i]);
	if(!res) printf("load failed\n");

	//glUseProgram(RobotProgram);

	glGenBuffers(1,&VBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(vec3),&vertices[0],GL_STATIC_DRAW);
	vertices_size[i] = vertices.size();

	//(buffer type,data起始位置,data size,data first ptr)
	//vertices_size[i] = glm_model->numtriangles;
	
	//printf("vertices:%d\n",vertices_size[);

	glGenBuffers(1,&uVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	uvs_size[i] = uvs.size();

	glGenBuffers(1,&nVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	normals_size[i] = normals.size();
}
mat4 translate(float x,float y,float z){
	vec4 t = vec4(x,y,z,1);//w = 1 ,則x,y,z=0時也能translate
	vec4 c1 = vec4(1,0,0,0);
	vec4 c2 = vec4(0,1,0,0);
	vec4 c3 = vec4(0,0,1,0);
	mat4 M = mat4(c1,c2,c3,t);
	return M;
} 
mat4 scale(float x,float y,float z){
	vec4 c1 = vec4(x,0,0,0);
	vec4 c2 = vec4(0,y,0,0);
	vec4 c3 = vec4(0,0,z,0);
	vec4 c4 = vec4(0,0,0,1);
	mat4 M = mat4(c1,c2,c3,c4);
	return M;
}

mat4 rotate(float angle,float x,float y,float z){
	float r = DOR(angle);
	mat4 M = mat4(1);

	vec4 c1 = vec4(cos(r)+(1-cos(r))*x*x,(1-cos(r))*y*x+sin(r)*z,(1-cos(r))*z*x-sin(r)*y,0);
	vec4 c2 = vec4((1-cos(r))*y*x-sin(r)*z,cos(r)+(1-cos(r))*y*y,(1-cos(r))*z*y+sin(r)*x,0);
	vec4 c3 = vec4((1-cos(r))*z*x+sin(r)*y,(1-cos(r))*z*y-sin(r)*x,cos(r)+(1-cos(r))*z*z,0);
	vec4 c4 = vec4(0,0,0,1);
	M = mat4(c1,c2,c3,c4);
	return M;
}
void Keyboard(unsigned char key, int x, int y){
	switch(key){
	
	}
	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{	
	if (dir > 0)
	{
		roomSize ++;
	}
	else
	{
		
		roomSize --;
		if (roomSize < 1)roomSize = 0.1;
	}
	return;
}

void mouse(int button, int state, int x, int y)//camera
{
	mouseX = x;
	mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}
}

void mouseMotion(int x, int y)//camera
{
	if (mouseLeftDown)
	{
		cameraAngleX -= (x - mouseX);		
		cameraAngleY += (y - mouseY);

		mouseX = x;
		mouseY = y;
	}
}