#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#define FCR 1 //fixed camera rotation - explorer
#define FPR 2 //fixed point rotation - FPS
#define ZOM 3 //zooming
#define W 4	//forward
#define A 5	//left
#define D 6	//right
#define S 7	//backwards

float invSqrt(float number){
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * (long *) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - (i >> 1);               // what the fuck? 
	y  = * (float *) &i;
	y  = y * (threehalfs - (x2 * y * y));   // 1st iteration
//	y  = y * (threehalfs - (x2 * y * y));   // 2nd iteration, this can be removed

	return y;
}


class Camera{
	public:
	int moveState;
	int clickX,clickY;
	GLdouble sensi;
	GLdouble sensc;
	
	std::vector<GLfloat> posVec{std::vector<GLfloat> (3)};
	std::vector<GLfloat> centerVec{std::vector<GLfloat> (3)};
	std::vector<GLfloat> dirVec{std::vector<GLfloat> (3)};
	
	std::vector<GLfloat> pos{std::vector<GLfloat> (3)};
	std::vector<GLfloat> center{std::vector<GLfloat> (3)};

	CamObj(){
		this->moveState = 0;
		this->clickX = 0;
		this->clickY = 0;
		this->sensi = 0.01f;
		this->sensc = 0.1f;
		this->posVec = {0.0f,0.0f,10.0f};
		this->centerVec = {0.0f,0.0f,-10.0f};
		this->dirVec = {1.0f, 1.0f, 1.0f};
		calcPosP();
		
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
			calcDir();
			break;
	
			case ZOM:
			if (posVec[2] + xMove*sensi > 0){
				this->posVec[2] += xMove*sensi;
			}
			calcPosP();
			calcDir();
			break;
	
			case FPR:
			this->posVec[0] += xMove*sensi;
			if (abs(posVec[1] + yMove*sensi) < (M_PI/2)){
				this->posVec[1] += yMove*sensi;
			}
			calcPosP();
			calcDir();
			break;
	
			default:
			break;
		}
		clickX = x;
		clickY = y;
		glutPostRedisplay();
	}

// se o vetor for paralelao ao y pode dar asneira e anular-se
// o mesmo deve acontecer para as special keys por isso ter isso em conta


	void detectKeyboard(unsigned char key, int x, int y){
		calcDir();
		glutGetModifiers();
		switch (key){
			
			case 'w':
				this->pos[0] += dirVec[0]*sensc;
				this->pos[2] += dirVec[2]*sensc;
				this->center[0] += dirVec[0]*sensc;
				this->center[2] += dirVec[2]*sensc;
				break;
	
			case 's':
				this->pos[0] -= dirVec[0]*sensc;
				this->pos[2] -= dirVec[2]*sensc;
				this->center[0] -= dirVec[0]*sensc;
				this->center[2] -= dirVec[2]*sensc;
				break;

			case 'a':
				this->pos[0] += dirVec[2]*sensc;
				this->pos[2] -= dirVec[0]*sensc;
				this->center[0] += dirVec[2]*sensc;
				this->center[2] -= dirVec[0]*sensc;
				break;

			case 'd':
				this->pos[0] -= dirVec[2]*sensc;
				this->pos[2] += dirVec[0]*sensc;
				this->center[0] -= dirVec[2]*sensc;
				this->center[2] += dirVec[0]*sensc;
				break;

			case ' ':
				this->pos[1] += 1*sensc;
				this->center[1] += 1*sensc;
				break;

			case 'c':
				this->pos[1] -= 1*sensc;
				this->center[1] -= 1*sensc;
				break;

			default:
				break;

		glutPostRedisplay();
		}
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
		float value = invSqrt((center[0] - pos[0])*(center[0] - pos[0]) + (center[1] - pos[1])*(center[1] - pos[1]) + (center[2] - pos[2])*(center[2] - pos[2]));
		this->dirVec[0] = (center[0] - pos[0]) * value;
		this->dirVec[1] = (center[1] - pos[1]) * value;
		this->dirVec[2] = (center[2] - pos[2]) * value;
	}

	void place(){
		gluLookAt(
		pos[0],pos[1],pos[2],  
		center[0],center[1],center[2],
		0.0f, 1.0f, 0.0f
		);
	}
};
