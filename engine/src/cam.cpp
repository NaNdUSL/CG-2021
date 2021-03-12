#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#define FCR 1 //fixed camera rotation - explorer
#define FPR 2 //fixed point rotation - FPS
#define ZOM 3 //zooming

float invSqrt(float number){
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * (long *) &y;
	i  = 0x5f3759df - (i >> 1);
	y  = * (float *) &i;
	y  = y * (threehalfs - (x2 * y * y));
//	y  = y * (threehalfs - (x2 * y * y));

	return y;
}


class Camera{
	public:
	int moveState;
	int clickX,clickY;
	GLdouble sensi;
	GLdouble sensc;
	
	std::vector<GLfloat> posVec{0.0f,0.0f,10.0f};
	std::vector<GLfloat> centerVec{0.0f,0.0f,-10.0f};
	std::vector<GLfloat> dirVec{1.0f, 1.0f, 1.0f};
	std::vector<GLfloat> dirSide{0.0f, 0.0f, 0.0f};

	std::vector<GLfloat> pos{0.0f, 0.0f, 0.0f};
	std::vector<GLfloat> center{0.0f,0.0f,.0f};

	Camera(){
		
		reset();		
	}
	

	void detectCamMouse(int button, int state, int x, int y){
		if (state == GLUT_UP){
			this->moveState = 0;
			return;
		}
		switch (button){
			case GLUT_RIGHT_BUTTON:
			this->moveState = FCR;
			break;
	
			case GLUT_MIDDLE_BUTTON:
			this->moveState = ZOM;
			break;
	
			case GLUT_LEFT_BUTTON:
			this->moveState = FPR;
			break;
	
			default:
			return;
		}
		this->clickX = x;
		this->clickY = y;
		glutPostRedisplay();
	}

	void controlCamera(int x, int y){
		int xMove = (clickX - x);
		int yMove = (clickY - y);	
		switch (moveState){
			case FCR:
			this->centerVec[0] += xMove*sensi;
			this->centerVec[1] -= yMove*sensi;
			if (abs(centerVec[1] + yMove*sensi) > (M_PI/2)){
				this->centerVec[1] += yMove*sensi;
			}
			calcCenterP();
			break;
	
			case ZOM:
			if (posVec[2] + xMove*sensi > 0){
				this->posVec[2] += xMove*sensi;
			}
			calcPosP();
			break;
	
			case FPR:
			this->posVec[0] += xMove*sensi;
			if (abs(posVec[1] + yMove*sensi) < (M_PI/2)){
				this->posVec[1] += yMove*sensi;
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


	void detectKeyboard(unsigned char key, int x, int y){
		calcDir();
		glutGetModifiers();
		switch (key){
			
			case 'w':
				this->pos[0] += this->dirVec[0]*sensc;
				this->pos[2] += this->dirVec[2]*sensc;
				this->center[0] += this->dirVec[0]*sensc;
				this->center[2] += this->dirVec[2]*sensc;
				break;
	
			case 's':
				this->pos[0] -= this->dirVec[0]*sensc;
				this->pos[2] -= this->dirVec[2]*sensc;
				this->center[0] -= this->dirVec[0]*sensc;
				this->center[2] -= this->dirVec[2]*sensc;
				break;

			case 'a':
				this->pos[0] += this->dirVec[2]*sensc;
				this->pos[2] -= this->dirVec[0]*sensc;
				this->center[0] += this->dirVec[2]*sensc;
				this->center[2] -= this->dirVec[0]*sensc;
				break;

			case 'd':
				this->pos[0] -= this->dirVec[2]*sensc;
				this->pos[2] += this->dirVec[0]*sensc;
				this->center[0] -= this->dirVec[2]*sensc;
				this->center[2] += this->dirVec[0]*sensc;
				break;

			case ' ':
				this->pos[1] += 1*sensc;
				this->center[1] += 1*sensc;
				break;

			case 'c':
				this->pos[1] -= 1*sensc;
				this->center[1] -= 1*sensc;
				break;

			case 'r':
				reset();
				break;

			default:
				break;
		}
		glutPostRedisplay();
	}

	void calcCenterP(){
		this->center[0] = pos[0] + centerVec[2]*cos(centerVec[1])*sin(centerVec[0]);
		this->center[1] = pos[1] + centerVec[2]*sin(centerVec[1]);
		this->center[2] = pos[2] + centerVec[2]*cos(centerVec[1])*cos(centerVec[0]);
		this->posVec[0] = centerVec[0];
		this->posVec[1] = centerVec[1];
		this->posVec[2] = -1*centerVec[2];
	}

	void calcPosP(){
		this->pos[0] = posVec[2]*cos(posVec[1])*sin(posVec[0]) + center[0];
		this->pos[1] = posVec[2]*sin(posVec[1]) + center[1];
		this->pos[2] = posVec[2]*cos(posVec[1])*cos(posVec[0]) + center[2];
		this->centerVec[0] = posVec[0];
		this->centerVec[1] = posVec[1];
		this->centerVec[2] = -posVec[2];
	}

	void calcDir(){
		normalize((center[0] - pos[0]), 0, (center[2] - pos[2]), this->dirVec);
		printf("%f, %f, %f\n", this->dirVec[0], this->dirVec[1], this->dirVec[2]);
	}

	void ortVec(std::vector<GLfloat> vec1, std::vector<GLfloat> vec2, std::vector<GLfloat> &retVec){
		retVec.clear();
		retVec.push_back(vec1[1]*vec2[2] - vec1[2]*vec2[1]);
		retVec.push_back(vec1[2]*vec2[0] - vec1[0]*vec2[2]);
		retVec.push_back(vec1[0]*vec2[1] - vec1[1]*vec2[0]);
	}

	void normalize(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat> &retVec){
		float value = invSqrt((x*x) + (y*y) + (z*z));
		retVec[0] = x * value;
		retVec[1] = y * value;
		retVec[2] = z * value;
	}

	void reset(){

		this->moveState = 0;
		this->clickX = 0;
		this->clickY = 0;
		this->sensi = 0.01f;
		this->sensc = 0.1f;
		this->posVec = {0.0f,0.0f,10.0f};
		this->centerVec = {0.0f,0.0f,-10.0f};
		this->center = {0.0f,0.0f,0.0f};
		calcDir();
		std::vector<GLfloat> yAxis{0.0f,1.0f,0.0f};
		ortVec(this->dirVec, yAxis, this->dirSide);
		calcPosP();
	}

	void place(){
		gluLookAt(
		pos[0],pos[1],pos[2],  
		center[0],center[1],center[2],
		0.0f, 1.0f, 0.0f
		);
	}

};