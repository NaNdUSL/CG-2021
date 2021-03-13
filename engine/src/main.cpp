#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include "string.h"

using namespace tinyxml2;
 

// Source Files
#include "cam.cpp"
#include "model.cpp"


// camera Object of the scene
Camera cam;

// Models of the scene
std::vector<Model> models;


// - Engine Runtime Options
// Mode of polygon (fill / line)
int polyMode = GL_FILL;
// Origin Axis
int oAxisDr = 100;
// cameraCenter Axis
int ccAxisDr = 0;


void changeSize(int w, int h);
void axis();
void renderScene(void);
void detectMouseButs(int button, int state, int x, int y);
void control(int x, int y);
void keyInput(unsigned char key, int x, int y);
int readXML(const char* xmlName, std::vector<std::string> &listXML);
void drawModelsVBO();
void drawModelsTRI();




void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


// Draw the axis centered on Origin
void axis(){
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f( oAxisDr, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f,0.0f, 0.0f);
	glVertex3f(0.0f, oAxisDr, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f,0.0f);
	glVertex3f(0.0f, 0.0f, oAxisDr);
	glEnd();
}


// Draww the axis centered on CameraCenter
void ccAxis(){
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(cam.center[0], cam.center[1], cam.center[2]);
	glVertex3f(cam.center[0]+0.5f, cam.center[1], cam.center[2]);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(cam.center[0], cam.center[1], cam.center[2]);
	glVertex3f(cam.center[0], cam.center[1]+0.5f, cam.center[2]);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(cam.center[0], cam.center[1], cam.center[2]);
	glVertex3f(cam.center[0], cam.center[1], cam.center[2]+0.5f);
	glEnd();
}



void renderScene(void) {
	// - Scene Runtime options
	glClearColor(0.2f,0.2f,0.3f,0.2f);
	glPolygonMode(GL_FRONT_AND_BACK, polyMode);
		

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	// places the camera on the scene and draw Axis
	cam.place();
	if (oAxisDr) axis();
	if (ccAxisDr) ccAxis();

	// Draw Scene
	glColor3f(0.5f,0.5f,0.5f);
	drawModelsVBO();

	// End of frame
	glutSwapBuffers();
}



// - callBack Registers

void detectMouseButs(int button, int state, int x, int y){
	cam.detectCamMouse(button,state,x,y);
}

void control(int x, int y){
	cam.controlCamera(x,y);
}

void keyInput(unsigned char key, int x, int y){
	cam.detectKeyboard(key,x,y); // this reservers the keys "W A S D C [space] R"
	
	switch (key){
		case 'm':
		if (polyMode == GL_FILL) polyMode = GL_LINE;
		else polyMode = GL_FILL;
		break;

		case 'o':
		if (oAxisDr == 10) oAxisDr = 0;
		else if (oAxisDr == 100) oAxisDr = 10;
		else oAxisDr = 100;
		break;

		case 'i':
		if (ccAxisDr) ccAxisDr = 0;
		else ccAxisDr = 1;
		break;
		
		default:
		break;
	}
}





// - Leitura do ficheiro XML passado como argumento na Engine
int readXML(const char* xmlName, std::vector<std::string> &listXML){
	//Variável XMLDocument armazena o ficheiro XML estruturadamente
	XMLDocument doc;
	doc.LoadFile(xmlName);

	//Verifica se ocorreu algum erro durante o loading do ficheiro
	// 0- bem sucedido
	// 3- ficheiro não encontrado
	// 7- erro no XML
	int y = doc.ErrorID();
	if(y != 0){
		printf("ERROR ON XML FILE READ, error code: %d\n", y);
		return y;
	}

	//O método FirstChildElement procura um elemento com o nome especificado e "entra" nesse elemento (altera o nível de profundidade)
	// da travessia. Neste caso executamos o método 2 vezes pq temos <scene> -> <model>
	//XMLElement* nomeFile = doc.FirstChildElement()->FirstChildElement();

	//Vamos buscar o valor do atributo file para obtermos o nome do ficheiro
	//const char* str = nomeFile->Attribute("file");
	XMLElement* iterator;
	for(iterator = doc.FirstChildElement()->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
		const char* file_name = iterator->Attribute("file");
		listXML.push_back(std::string(file_name));
	}
	
	return 0;
}



// - Model Drawing Options

void drawModelsVBO(){
	// use of VBO
	for(Model m : models){
		m.drawVBO();
	}
}

void drawModelsTRI(){
	// use of glvertex3f
	for(Model m : models){
		m.drawT();
	}
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc,  char **argv) {
	std::vector<std::string> listXML = std::vector<std::string> ();
	
	if (argc < 2 || readXML(argv[1],listXML)){
		printf("Error on Engine Startup - Check the XML File before Starting...\n");
		return 2;
	}

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	std::string name = std::string("Engine Scene - ");
	name += argv[1];
	glutCreateWindow(name.c_str());
		
	// Required callback registry
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
	// registration of the keyboard callbacks
	glutMouseFunc(detectMouseButs);
	glutMotionFunc(control);
	glutKeyboardFunc(keyInput);

	// Start Glew and read Models for memory and VBO
	glewInit();
	glEnableClientState(GL_VERTEX_ARRAY);
	for(std::string fileName : listXML){
		models.push_back(Model(fileName));
	}

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
} 
