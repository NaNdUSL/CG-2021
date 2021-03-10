#include <unistd.h>


class Model{
	public:
	std::vector<std::vector<float>> vertexList;
	std::vector<std::vector<int>> facesList;

	Model(){

	}

	Model(std::string fileName){
		//printf("%s\n", fileName);
		readFile(fileName);
	}

	void draw(){
		glBegin(GL_TRIANGLES);
		for(std::vector<int> v : facesList){
			for(int vertex : v){
				//printf("%d\n", vertexList.size());
				//printf("%f %f %f \n", vertexList[vertex][0],vertexList[vertex][1],vertexList[vertex][2]);
				glVertex3f(vertexList[vertex][0],vertexList[vertex][1],vertexList[vertex][2]);
			}
		}
		glEnd();
	}

	void readFile( std::string fileName){
		//printf("FIXE\n");
		//const char* auxname = strdup(fileName);

		//printf("%s\n", auxname);
		//printf("MODEL %d\n", fileName);

		std::ifstream mod;
		//printf("%s\n", fileName);
		mod.open(fileName.c_str());
	
		int numVertices;
		int numFaces;
		float auxf;
		int auxi;

		mod >> numVertices;
		mod >> numFaces;

		
		for(int i = 0; i < numVertices;i++){
			vertexList.push_back(std::vector<float>());
			for(int j = 0; j < 3; j++){
				mod >> auxf;
				//printf("COCO\n"); printf SAGRADO
				//printf("C++ coco\n");
				//if(strcmp(fileName,auxname) == 0);
				vertexList[i].push_back(auxf);
			}
		}

	
		for(int i = 0; i < numFaces; i++){
			facesList.push_back(std::vector<int>());
			for(int j = 0; j < 3; j++){
				mod >> auxi;
				facesList[i].push_back(auxi-1);
			}
		}



		mod.close();
	}
};