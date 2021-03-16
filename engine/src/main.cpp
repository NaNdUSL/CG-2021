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
#include "scene.cpp"

// Main Scene
Scene scene;


void changeSize(int w, int h);
void detectMouseButs(int button, int state, int x, int y);
void control(int x, int y);
void keyInput(unsigned char key, int x, int y);



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



// - callBack Registers
void detectMouseButs(int button, int state, int x, int y){
	scene.cam.detectCamMouse(button,state,x,y);
}

void control(int x, int y){
	scene.cam.controlCamera(x,y);
}

void keyInput(unsigned char key, int x, int y){
	scene.cam.detectKeyboard(key,x,y);// this reservers the keys "W A S D C [space] R"
	scene.keyboardOpts(key,x,y); // this reserves " M O I"
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc,  char **argv) {	
	if (argc < 2 || scene.loadXML(argv[1])){ ///////////////////////////////////////////// $$
		printf("Error on Engine Startup - Check the XML File before Starting...\n");
		return 2;
	}

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	
	std::string name = std::string("Engine Scene - ") + argv[1];
	glutCreateWindow(name.c_str());
		
	
	// Required callback registry
	glutDisplayFunc(scene.render);
	glutReshapeFunc(changeSize);

	
	// registration of the keyboard callbacks
	glutMouseFunc(detectMouseButs);
	glutMotionFunc(control);
	glutKeyboardFunc(keyInput);

	// Start Glew and read Models for memory and VBO
	glewInit();
	glEnableClientState(GL_VERTEX_ARRAY);
	scene.loadModels(); ///////////////////////////////////////////// $$

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
} 
