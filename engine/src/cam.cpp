#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#define FCR 1 //fixed camera rotation - explorer
#define FPR 2 //fixed point rotation - FPS
#define ZOM 3 //zooming

// Função que calcula "inverse square root of a floating point number"
// Artigo da wikipédia: https://en.wikipedia.org/wiki/Fast_inverse_square_root
// Usada para efetuar a normalização de vetores no método "normalize" definido mais à frente
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

	int moveState;			// Estado de rotação da câmara (FCR, FPR, ZOM)
	int clickX,clickY;		// Coordenadas do mouse na janela
	
	// Constantes usadas referentes à sensibilidade, quer do mouse, quer do keyboard
	GLdouble sensi;
	GLdouble sensc;
	GLdouble sensz;
	GLfloat raio;

	// Coordenadas iniciais
	std::vector<GLfloat> origPosVec{0.0f,0.0f,10.0f};
	std::vector<GLfloat> origCenter{0.0f,0.0f,0.0f};
	
	// Coordenadas polares da câmera e do centro, respetivamente
	std::vector<GLfloat> posVec{0.0f,0.0f,10.0f};
	std::vector<GLfloat> centerVec{0.0f,0.0f,-10.0f};
	
	// Vetor normal paralelo ao plano XZ que representa o sentido da câmera
	std::vector<GLfloat> dirVec{1.0f, 0.0f, 1.0f};

	// Vetor normal com o mesmo sentido que a câmera
	std::vector<GLfloat> oriVec{1.0f, 0.0f, 1.0f};

	// Coordenadas das posições da câmera e do centro
	std::vector<GLfloat> pos{0.0f, 0.0f, 0.0f};
	std::vector<GLfloat> center{0.0f, 0.0f, 0.0f};

	// Construtor da classe Camara
	Camera(){
		reset();		
	}
	
	// Método que deteta o input do mouse
	void detectCamMouse(int button, int state, int x, int y){
		if (state == GLUT_UP){
			this->moveState = 0;			// moveState = 0 caso seja largado o botão
			return;
		}
		switch (button){
			case GLUT_RIGHT_BUTTON:			// Botão direito do rato define Fixed Camera Rotation
			this->moveState = FCR;
			break;
	
			case GLUT_MIDDLE_BUTTON:		// Botão do centro do rato define Zoom
			this->moveState = ZOM;
			break;
	
			case GLUT_LEFT_BUTTON:			// Botão esquerdo do rato define Fixed Point Rotation
			this->moveState = FPR;
			break;
	
			default:
			return;
		}
		this->clickX = x;
		this->clickY = y;
	}


// Método que após atualizados os valores das coordenadas polares de acordo com o movimento, convertem-nas para coordenadas cartesianas
	void controlCamera(int x, int y){
		int xMove = (clickX - x);
		int yMove = (clickY - y);	
		switch (moveState){
			case FCR:												// Usando coordenadas polares e a constante de sensibilidade do rato calcula os novos valores do "centro"
			this->centerVec[0] += xMove*sensi;
			if (abs(centerVec[1] + yMove*sensi) < (M_PI/2)){		// Limita o ângulo do centro entre -PI/2 e PI/2
				this->centerVec[1] += yMove*sensi;
			}
			calcCenterP();
			break;
	
			case ZOM:												// Usando coordenadas polares e a constante de sensibilidade do rato calcula os novos valores da câmera
			if (posVec[2] + xMove*sensz > 0){
				this->posVec[2] += xMove*sensz;						// Limita o raio (distância da câmera ao centro)
			}
			calcPosP();
			this->sensz = (posVec[2] * sensz) / raio;
			this->sensc = (posVec[2] * sensc) / raio;
			this->raio = posVec[2];
			break;
	
			case FPR:												// Usando coordenadas polares e a constante de sensibilidade do rato calcula os novos valores da câmera
			this->posVec[0] += xMove*sensi;
			if (abs(posVec[1] + yMove*sensi) < (M_PI/2)){			// Limita o ângulo da câmera entre -PI/2 e PI/2
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

	// Deteta a key pressionada e calcula as novas coordenadas da câmera e do centro de acordo com a translação pretendida
	void detectKeyboard(unsigned char key, int x, int y){
		calcDir();
		calcOri();
		glutGetModifiers();
		switch (key){											// Para "WASD" basta atualizar os valores de X e de Z e para space_key e "C" atualiza o valor de Y
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

			case 't':
				this->pos[0] += this->oriVec[0]*sensc;
				this->pos[1] += this->oriVec[1]*sensc;
				this->pos[2] += this->oriVec[2]*sensc;
				this->center[0] += this->oriVec[0]*sensc;
				this->center[1] += this->oriVec[1]*sensc;
				this->center[2] += this->oriVec[2]*sensc;
				break;
	
			case 'g':
				this->pos[0] -= this->oriVec[0]*sensc;
				this->pos[1] -= this->oriVec[1]*sensc;
				this->pos[2] -= this->oriVec[2]*sensc;
				this->center[0] -= this->oriVec[0]*sensc;
				this->center[1] -= this->oriVec[1]*sensc;
				this->center[2] -= this->oriVec[2]*sensc;
				break;

			case 'f':
				this->pos[0] += this->dirVec[2]*sensc;
				this->pos[2] -= this->dirVec[0]*sensc;
				this->center[0] += this->dirVec[2]*sensc;
				this->center[2] -= this->dirVec[0]*sensc;
				break;

			case 'h':
				this->pos[0] -= this->dirVec[2]*sensc;
				this->pos[2] += this->dirVec[0]*sensc;
				this->center[0] -= this->dirVec[2]*sensc;
				this->center[2] += this->dirVec[0]*sensc;
				break;

			case ' ':
				this->pos[1] += sensc;
				this->center[1] += sensc;
				break;

			case 'c':
				this->pos[1] -= sensc;
				this->center[1] -= sensc;
				break;

			case 'r':											// O "r" dá "reset" na câmera
				reset();
				break;

			default:
				break;
		}
		glutPostRedisplay();
	}

// Converte as coordenadas polares do centro para cartesianas
	void calcCenterP(){
		this->center[0] = pos[0] + centerVec[2]*cos(centerVec[1])*sin(centerVec[0]);
		this->center[1] = pos[1] + centerVec[2]*sin(centerVec[1]);
		this->center[2] = pos[2] + centerVec[2]*cos(centerVec[1])*cos(centerVec[0]);
		this->posVec[0] = (M_PI)+centerVec[0];
		this->posVec[1] = - centerVec[1];
		this->posVec[2] = centerVec[2];
	}

// Converte as coordenadas polares da câmera para cartesianas
	void calcPosP(){
		this->pos[0] = posVec[2]*cos(posVec[1])*sin(posVec[0]) + center[0];
		this->pos[1] = posVec[2]*sin(posVec[1]) + center[1];
		this->pos[2] = posVec[2]*cos(posVec[1])*cos(posVec[0]) + center[2];
		this->centerVec[0] = (M_PI)+posVec[0];
		this->centerVec[1] = - posVec[1];
		this->centerVec[2] = posVec[2];
	}

// Calcula e normaliza o vetor dirVec
	void calcDir(){
		normalize((center[0] - pos[0]), 0, (center[2] - pos[2]), this->dirVec);
	}

	void calcOri(){
		normalize((center[0] - pos[0]), (center[1] - pos[1]), (center[2] - pos[2]), this->oriVec);
	}

// Método que calcula o vetor perpendicular a dois vetores que não sejam paralelos entre si
	void ortVec(std::vector<GLfloat> vec1, std::vector<GLfloat> vec2, std::vector<GLfloat> &retVec){
		retVec.clear();
		retVec.push_back(vec1[1]*vec2[2] - vec1[2]*vec2[1]);
		retVec.push_back(vec1[2]*vec2[0] - vec1[0]*vec2[2]);
		retVec.push_back(vec1[0]*vec2[1] - vec1[1]*vec2[0]);
	}

// Método que normaliza o vetor
	void normalize(GLfloat x, GLfloat y, GLfloat z, std::vector<GLfloat> &retVec){
		float value = invSqrt((x*x) + (y*y) + (z*z));
		retVec[0] = x * value;
		retVec[1] = y * value;
		retVec[2] = z * value;
	}

// Dá reset à posição e aos valores da câmera
	void reset(){
		this->moveState = 0;
		this->clickX = 0;
		this->clickY = 0;
		this->sensi = 0.005f;
		this->sensz = 0.5f;
		this->sensc = 0.5f;
		this->posVec.assign(origPosVec.begin(), origPosVec.end());
		this->center.assign(origCenter.begin(), origCenter.end());
		calcDir();
		calcOri();
		calcPosP();
		this->raio = posVec[2];
	}

// Dá "place" da câmera de acordo com as coordenadas desta e do ponto de referência
	void place(){
		gluLookAt(
		pos[0],pos[1],pos[2],  
		center[0],center[1],center[2],
		0.0f, 1.0f, 0.0f
		);
	}

};