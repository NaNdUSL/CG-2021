#define FCR 1 //fixed camera rotation - explorer
#define FPR 2 //fixed point rotation - FPS
#define ZOM 3 //zooming

class CamObj{
	public:
	int moveState;
	int clickX,clickY;
	GLdouble sensi = 0.01f;
	
	GLfloat posVec[3] = {0.0f,0.0f,10.0f};
	GLfloat centerVec[3] = {0.0f,0.0f,-10.0f};

	GLfloat pos[3];
	GLfloat center[3];

	CamObj(){
		this->calcPosP();
	}
	

	void detectCamMouse(int button, int state, int x, int y){
		if (state == GLUT_UP){
			moveState = 0;
			return;
		}
		switch (button){
			case GLUT_RIGHT_BUTTON:
			moveState = FCR;
			break;
	
			case GLUT_MIDDLE_BUTTON:
			moveState = ZOM;
			break;
	
			case GLUT_LEFT_BUTTON:
			moveState = FPR;

			break;
	
			default:
			return;
		}
		clickX = x;
		clickY = y;
	}

	void controlCamera(int x, int y){
		int xMove = (clickX - x);
		int yMove = (clickY - y);	
		switch (moveState){
			case FCR:
			centerVec[0] += xMove*sensi;
			centerVec[1] -= yMove*sensi;
			if (abs(centerVec[1] + yMove*sensi) > (M_PI/2)){
				centerVec[1] += yMove*sensi;
			}
			calcCenterP();
			break;
	
			case ZOM:
			if (posVec[2] + xMove*sensi > 0){
				posVec[2] += xMove*sensi;
			}
			calcPosP();
			break;
	
			case FPR:
			posVec[0] += xMove*sensi;
			if (abs(posVec[1] + yMove*sensi) < (M_PI/2)){
				posVec[1] += yMove*sensi;
			}
			calcPosP();
			break;
	
			default:
			break;
		}
		clickX = x;
		clickY = y;
		glutPostRedisplay();
	}

	void calcCenterP(){
		center[0] = pos[0] + centerVec[2]*cos(centerVec[1])*sin(centerVec[0]);
		center[1] = pos[1] + centerVec[2]*sin(centerVec[1]);
		center[2] = pos[2] + centerVec[2]*cos(centerVec[1])*cos(centerVec[0]);
		posVec[0] = centerVec[0];
		posVec[1] = centerVec[1];
		posVec[2] = -centerVec[2];
	}

	void calcPosP(){
		pos[0] = posVec[2]*cos(posVec[1])*sin(posVec[0]) + center[0];
		pos[1] = posVec[2]*sin(posVec[1]) + center[1];
		pos[2] = posVec[2]*cos(posVec[1])*cos(posVec[0]) + center[2];
		centerVec[0] = posVec[0];
		centerVec[1] = posVec[1];
		centerVec[2] = -posVec[2];
	}


	void place(){
		gluLookAt(
		pos[0],pos[1],pos[2],  
		center[0],center[1],center[2],
		0.0f, 1.0f, 0.0f
		);
	}
};














class Cylinder
{
	public:
		GLfloat radius,height;
		int slices;

	void setVars(GLfloat radius,GLfloat height, int slices){
		this->radius = radius;
		this->height = height;
		this->slices = slices;
	}

	void draw(){
		GLfloat points[(1+slices)*2] [3];
		int i;
		glPolygonMode(GL_FRONT, GL_FILL);
		float t = (float) slices;

		for (i = 0; i < slices; i++){
			glBegin(GL_POLYGON);

			glColor3f(0.0f, i/t, 1.0f);
			glVertex3f(radius * sin(((2*M_PI)/slices) * i),(height),radius * cos(((2*M_PI)/slices) * i));
			glVertex3f(radius * sin(((2*M_PI)/slices) * (i+1)),(height),radius * cos(((2*M_PI)/slices) * (i+1)));
			glVertex3f(0,(height),0);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(0.0f, i/t, 1.0f);
			glVertex3f(radius * sin(((2*M_PI)/slices) * (i+1)),0,radius * cos(((2*M_PI)/slices) * (i+1)));
			glVertex3f(radius * sin(((2*M_PI)/slices) * i),0,radius * cos(((2*M_PI)/slices) * i));
			glVertex3f(0,0,0);
			glEnd();


			glBegin(GL_POLYGON);
			glColor3f(i/t, 0.0f, 1.0f);
			
			glVertex3f(radius * sin(((2*M_PI)/slices) * i),(height),radius * cos(((2*M_PI)/slices) * i));
			glVertex3f(radius * sin(((2*M_PI)/slices) * (i+1)),(height),radius * cos(((2*M_PI)/slices) * (i+1)));
			glVertex3f(radius * sin(((2*M_PI)/slices) * i),0,radius * cos(((2*M_PI)/slices) * i));
			glEnd();
			
			glBegin(GL_POLYGON);
			glColor3f(i/t, 0.0f, 1.0f);
			glVertex3f(radius * sin(((2*M_PI)/slices) * i),0,radius * cos(((2*M_PI)/slices) * i));
			glVertex3f(radius * sin(((2*M_PI)/slices) * (i+1)),0,radius * cos(((2*M_PI)/slices) * (i+1)));
			glVertex3f(radius * sin(((2*M_PI)/slices) * (i+1)),height,radius * cos(((2*M_PI)/slices) * (i+1)));
			glEnd();
		}

	}
};