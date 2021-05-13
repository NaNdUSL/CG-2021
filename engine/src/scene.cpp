class Scene{
	public:
		std::string name = std::string("Engine Scene - ");

		// Time based variables
		int lastRefresh = glutGet(GLUT_ELAPSED_TIME);
		float frames = 0;


		// camera Object
		Camera cam;

		// lights
		std::vector<LightRef*> lightConfigs;
		int lightsOn = 1;
		int drawnormals = 0;
		
		
		// Models
		std::vector<Group>*groups = new std::vector<Group>();
		std::map<std::string,Model*>*modelTable = new std::map<std::string,Model*>();
		
		// - Engine Runtime Options
		// Mode of polygon (fill / line)
		int polyMode = GL_FILL;
		// Origin Axis
		int oAxisDr = 100;
		// cameraCenter Axis
		int ccAxisDr = 0;
		// draw Transforms
		int drawTrans = 0;
		
		// if the animation controls
		int speedUp = 1;
		int notPause = 1;
		int lastMeasure = glutGet(GLUT_ELAPSED_TIME);


		//- Engine Starter Options
		std::vector<float> background{0.2f,0.2f,0.3f,0.2f};


		void drawGroups(unsigned int texID){
			int now = glutGet(GLUT_ELAPSED_TIME);
			int elapsed = ((now - lastMeasure)*speedUp*notPause);
			lastMeasure = now; 
			for (Group g: (*groups)){
				g.makeGroup(elapsed, drawTrans, drawnormals, texID);
			}
		}

		void setBackColor(){
			glClearColor(background[0],background[1],background[2],background[3]);
		}

		void setupLights(){
			for (LightRef* light : lightConfigs){
				light->setup();
			}
		}

		void placeLights(){
			for (LightRef* light : lightConfigs){
				light->place();
			}			
		}
		
		float getFPS(){
			float fps = -1;
			frames ++;
			int time = glutGet(GLUT_ELAPSED_TIME);
			if ((time-lastRefresh)>1000){
				fps = ((frames*1000)/(time-lastRefresh));
				lastRefresh = time;
				frames = 0;
			}
			return fps;
		}


		// Draw the axis centered on Origin
		void axis(){
			glBegin(GL_LINES);
			

			GLfloat red[] = {1.1f, 0.0f, 0.0f, 1.0f};
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
			glMaterialfv(GL_FRONT, GL_EMISSION, red);
			// X axis in red
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f( oAxisDr, 0.0f, 0.0f);
			// Y Axis in Green
			

			GLfloat Green[] = {0.0f, 1.0f, 0.0f, 1.0f};
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Green);
			glMaterialfv(GL_FRONT, GL_EMISSION, Green);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.0f,0.0f, 0.0f);
			glVertex3f(0.0f, oAxisDr, 0.0f);
			// Z Axis in Blue
			

			GLfloat Blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blue);
			glMaterialfv(GL_FRONT, GL_EMISSION, Blue);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f,0.0f);
			glVertex3f(0.0f, 0.0f, oAxisDr);


			GLfloat noneV[] = {0.0f, 0.0f, 0.0f, 0.0f};
			glMaterialfv(GL_FRONT, GL_EMISSION, noneV);
			glEnd();
		}
		

		// Draww the axis centered on CameraCenter
		void ccAxis(){
			glBegin(GL_LINES);
			

			// X axis in red
			GLfloat red[] = {1.0f, 0.0f, 0.0f, 1.0f};
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
			glMaterialfv(GL_FRONT, GL_EMISSION, red);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(cam.center[0], cam.center[1], cam.center[2]);
			glVertex3f(cam.center[0]+sqrt(cam.raio), cam.center[1], cam.center[2]);
			

			// Y Axis in Green
			GLfloat Green[] = {0.0f, 1.0f, 0.0f, 1.0f};
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Green);
			glMaterialfv(GL_FRONT, GL_EMISSION, Green);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(cam.center[0], cam.center[1], cam.center[2]);
			glVertex3f(cam.center[0], cam.center[1]+sqrt(cam.raio), cam.center[2]);
			

			// Z Axis in Blue
			GLfloat Blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Blue);
			glMaterialfv(GL_FRONT, GL_EMISSION, Blue);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(cam.center[0], cam.center[1], cam.center[2]);
			glVertex3f(cam.center[0], cam.center[1], cam.center[2]+sqrt(cam.raio));
			

			GLfloat noneV[] = {0.0f, 0.0f, 0.0f, 0.0f};
			glMaterialfv(GL_FRONT, GL_EMISSION, noneV);

			glEnd();
		}


		// Receive keyboard input and applay options
		void keyboardOpts(unsigned char key, int x, int y){
			switch (key){
				case '1':
				if (polyMode == GL_FILL) polyMode = GL_LINE;
				else polyMode = GL_FILL;
				break;
		
				case '2':
				if (oAxisDr == 10) oAxisDr = 0;
				else if (oAxisDr == 100) oAxisDr = 10;
				else oAxisDr = 100;
				break;
		
				case '3':
				if (ccAxisDr) ccAxisDr = 0;
				else ccAxisDr = 1;
				break;
				
				case '4':
				if (speedUp++ > 50) speedUp = 50;
				break;

				case '5':
				if (--speedUp < 1) speedUp = 1;
				break;

				case '6':
				notPause = !(notPause);
				break;

				case '7':
				turnLights(1);
				drawTrans = !(drawTrans);
				break;

				case '8':
				turnLights();
				break;

				case '9':
				turnLights(1);
				drawnormals = !(drawnormals);
				break;				

				default:
				break;
			}
		}


		void turnLights(int toOff=0){
			if(lightsOn){
				glDisable(GL_LIGHTING);
				lightsOn = 0;
			}
			else if (!toOff){
				drawnormals = 0;
				drawTrans = 0;
				glEnable(GL_LIGHTING);
				lightsOn = 1;
			};			
		}
};





