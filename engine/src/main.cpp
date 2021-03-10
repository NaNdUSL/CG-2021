#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "tinyxml2.h"
#include "tinyxml2.cpp"
#include "string.h"

using namespace tinyxml2;
//using namespace std;
// cam movement
#include "cam.cpp"
#include "model.cpp"
#include <unistd.h>

//vector<vector<float>> vertex_list;
//vector<vector<int>> faces_list;

Camera cam;
std::vector<Model> models;

//void draw_model(vector<vector<float>> &vertex_list, vector<vector<int>> &faces_list);
void readXML(const char* xmlName, std::vector<std::string> &listXML);
void printModels();
//void read_file(const char* file_name, vector<vector<float>> &vertex_list, vector<vector<int>> &faces_list);

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






void axis(){

	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f( 10.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f,0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f,0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	cam.place();


	axis();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//printf("tudo fixe\n");
	printModels();
	//printf("tudo fixe!\n");

	
	// End of frame
	glutSwapBuffers();
}




void detectMouseButs(int button, int state, int x, int y){
	cam.detectCamMouse(button,state,x,y);
}


void control(int x, int y){
	cam.controlCamera(x,y);
}






int main(int argc,  char **argv) {
	//Model model;
	std::vector<std::string> listXML = std::vector<std::string> ();
	readXML("te.xml",listXML);

	int i = 0;
	for(std::string fileName : listXML){
		//printf("MAIN %d\n", fileName);
		models.push_back(Model(fileName));
		i++;
	}

	//printf("i: %d\n", i);

	/*for(Model m : models){
		printf("FILE NAME: %s\n", m.file_name);
	} */

	//printf("MODELS SIZE%d\n", models.size());

	//clindro.setVars(3.0f,3.0f,1000);
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("my window");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMouseFunc(detectMouseButs);
	glutMotionFunc(control);
	
// put here the registration of the keyboard callbacks
	//glutMouseFunc(detectMouseButs);
	//glutMotionFunc(control);
	//glutKeyboardFunc(keyInput);
	//glutSpecialFunc(specialInput);


//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
} 


void readXML(const char* xmlName, std::vector<std::string> &listXML){
	//Variável XMLDocument armazena o ficheiro XML estruturadamente
	XMLDocument doc;
	doc.LoadFile(xmlName);

	//Verifica se ocorreu algum erro durante o loading do ficheiro
	// 0- bem sucedido
	// 3- ficheiro não encontrado
	// 7- erro no XML
	int y = doc.ErrorID();

	if(y != 0){
		printf(" ERRO: %d\n", y);
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

}

void printModels(){
	for(Model m : models){
		m.draw();
	}
}

/*void read_file(const char* file_name, std::vector<vector<float>> &vertex_list, vector<vector<int>> &faces_list){
	ifstream model;
	model.open(file_name);
	int num_vertices;
	int num_faces;
	float aux;

	model >> num_vertices;
	model >> num_faces;

	for(int i = 0; i < num_vertices;i++){
		vertex_list.push_back(vector<float>());
		for(int j = 0; j < 3; j++){
			model >> aux;

			vertex_list[i].push_back(aux);
		}
	}

	aux = (int) aux;
	for(int i = 0; i < num_faces; i++){
		faces_list.push_back(vector<int>());
		for(int j = 0; j < 3; j++){
			model >> aux;
			faces_list[i].push_back(aux-1);
		}
	}
}

void draw_model(vector<vector<float>> &vertex_list, vector<vector<int>> &faces_list){
	glBegin(GL_TRIANGLES);
	for(vector<int> v : faces_list){
		for(int vertex : v){
			glVertex3f(vertex_list[vertex][0],vertex_list[vertex][1],vertex_list[vertex][2]);
		}
	}
	glEnd();

} */