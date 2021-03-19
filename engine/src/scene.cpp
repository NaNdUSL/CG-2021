class Scene{
	public:
		//SceneFile
		XMLParser* sceneFile;

		// camera Object
		Camera cam;
		
		// Models
		std::vector<Group> groups;
		std::map<std::string,Model*> modelTable;
		
		// - Engine Runtime Options
		// Mode of polygon (fill / line)
		int polyMode = GL_FILL;
		// Origin Axis
		int oAxisDr = 100;
		// cameraCenter Axis
		int ccAxisDr = 0;

		Scene(){
			sceneFile = NULL;
		}
		
		Scene(std::string fileName){
			(this->sceneFile) = new XMLParser(fileName,groups,modelTable);
		}

		int checkFile(){
			return (this->sceneFile)-> openXML();
		}

		void load(){
			(this->sceneFile)-> parse();
			printf("%d\n",groups.size());
		}


		void drawGroups(){

			for (Group g: groups){

				g.makeGroup();
			}
		}

		void setBackColor(){
			glClearColor(0.2f,0.2f,0.3f,0.2f);
		}

		void setDeafultMeshColor(){
			glColor3f(0.5f,0.5f,0.5f);
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
};



