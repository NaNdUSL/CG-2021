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
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include "string.h"

using namespace tinyxml2;
 

// Source Files
#include "cam.cpp"
#include "model.cpp"
#include "groupClasses.cpp"
#include "scene.cpp"
#include "xmlParser.cpp"


// Main Scene
Scene* scene = new Scene();


void changeSize(int w, int h);
void detectMouseButs(int button, int state, int x, int y);
void control(int x, int y);
void keyInput(unsigned char key, int x, int y);
void renderScene(void);



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
	scene->cam.detectCamMouse(button,state,x,y);
}

void control(int x, int y){
	scene->cam.controlCamera(x,y);
}

void keyInput(unsigned char key, int x, int y){
	scene->cam.detectKeyboard(key,x,y);// this reservers the keys "W A S D C [space] R"
	scene->keyboardOpts(key,x,y); // this reserves " M O I"
}





// RenderScene
void renderScene(void){
	// - Scene Runtime options
	scene->setBackColor();
	glPolygonMode(GL_FRONT_AND_BACK, scene->polyMode);



	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	// places the camera on the scene and draw Axis
	scene->cam.place();
	if (scene->oAxisDr) scene->axis();
	if (scene->ccAxisDr) scene->ccAxis();

	// Draw Scene
	float fps = scene->getFPS();
	if (!(fps < 0)) glutSetWindowTitle(((scene->name) + " - FPS: " + std::to_string(fps)).c_str());
	scene->drawGroups();


	// End of frame
	glutSwapBuffers();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc,  char **argv) {	
	if (argc < 2){
		printf("Error on Engine Startup - No XML File Specified...\n");
		return 2;
	}

	XMLParser parser (std::string(argv[1]),scene);

	int y = parser.openXML();
	if (y){
		printf("ERROR ON XML FILE READ, error code: %d\n", y);
		return y;
	}
	

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	
	glutCreateWindow((scene->name).c_str());
		
	
	// Required callback registry
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene); // FPS COUNTER
	glutReshapeFunc(changeSize);

	
	// registration of the keyboard callbacks
	glutMouseFunc(detectMouseButs);
	glutMotionFunc(control);
	glutKeyboardFunc(keyInput);

	// Start Glew and read Models for memory and VBO
	glewInit();
	glEnableClientState(GL_VERTEX_ARRAY);
	parser.parse();


	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
} 