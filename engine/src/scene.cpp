class Scene{
	public:
		// camera Object
		Camera cam;
		
		// Models
		std::vector<Group> groups;
		
		// - Engine Runtime Options
		// Mode of polygon (fill / line)
		int polyMode = GL_FILL;
		// Origin Axis
		int oAxisDr = 100;
		// cameraCenter Axis
		int ccAxisDr = 0;

		



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





		// Receive keyboard input and applay options
		void keyboardOpts(unsigned char key, int x, int y){
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


		void drawGroups(){
			for (Group g: groups){
				g.makeGroup();
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
};


class XMLParser{
public:
	std::vector<Group> groups;
	std::map<std::string,Model> modelTable;
	std::string fileName;
	XMLDocument doc;

	XMLParser(std::string fileName, std::vector<Group> &groups){
		this->fileName = fileName;
		this->groups = groups;
	}

	int openXML(){
		(this->doc).LoadFile(this->fileName);
		return doc.ErrorID();
	}

	void parse(){
		XMLElement* iterator;
		for(iterator = doc.FirstChildElement()->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
			std::string tagName(iterator->Value());
			if (!tagName.compare("group")){
				parseGroup(iterator);
			}
		}
	}

	void parseGroup(std::vector<Group> &gps, XMLElement* base){
		Group g;
		gps.push_back(g);

		XMLElement* iterator;
		for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
			std::string tagName(iterator->Value());
			if (!tagName.compare("translate")){
				printf("BRUH1\n");
			}

			else if (!tagName.compare("rotate")){
				printf("BRUH2\n");
			}

			else if (!tagName.compare("scale")){
				printf("BRUH3\n");
			}
			
			else if (!tagName.compare("models")){
				parseModel(g,iterator);
			}

			else if (!tagName.compare("group")){
				parseGroup(g.child,iterator);
			}
		}
	}

	void parseModel(Group &parent,XMLElement* base){
		XMLElement* iterator;
		for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
			std::string fileName(iterator->Attribute("file"));
			
			if ( modelTable.find(fileName) == modelTable.end()){
				modelTable[fileName] = Model(fileName);
			}
			parent.models.push_back(modelTable[fileName]);
		}
	}
};