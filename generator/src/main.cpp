#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <string.h>
#include "shapes.cpp"


#include <IL/il.h>



#define PLN 1
#define BOX 2
#define SPH 3
#define CON 4
#define CYL 5
#define TOR 6
#define PTC 7
#define HMP 8
#define HMS 9


int checkArgs(int nFloats, int nInts, int nStrings, std::vector<float> &fls, std::vector<int> &ins, std::vector<std::string>&str, int argc, char const *argv[]){
	float argF;
	int argI;
	if (argc < (nStrings+nFloats+nInts+3)) return 0;

	for (int i = 0; i < nStrings; i++){
		str.push_back(std::string(argv[i+2]));
	}

	for (int i = 0; i < nFloats; i++){
		argF = atof(argv[i+(nStrings+2)]);
		if (argF <= 0.1) return 0;
		fls.push_back(argF);
	}

	for (int i = 0; i < nInts; i++){
		argI = atoi(argv[i+(nStrings+nFloats+2)]);
		if (argI < 1)  return 0;
		ins.push_back(argI);
	}

	return 1;
}


int main(int argc, char const *argv[]){
	ilInit();


	int shape = 0;
	std::vector<float> fls;
	std::vector<int> ins;
	std::vector<std::string> str;
	if (argc > 2){
		if(!strcmp(argv[1], "plane")){
			shape += PLN;
			shape *= checkArgs(2,2,0,fls,ins,str,argc,argv);
		}
	
		else if (!strcmp(argv[1], "box")){
			shape += BOX;
			shape *= checkArgs(3,3,0,fls,ins,str,argc,argv);
		}
		
		else if (!strcmp(argv[1], "sphere")){
			shape += SPH;
			shape *= checkArgs(1,2,0,fls,ins,str,argc,argv);		
		}
	
		else if (!strcmp(argv[1], "cone")){
			shape += CON;
			shape *= checkArgs(2,2,0,fls,ins,str,argc,argv);
		}

		else if (!strcmp(argv[1], "cylinder")){
			shape += CYL;
			shape *= checkArgs(2,2,0,fls,ins,str,argc,argv);
		}
        
        else if (!strcmp(argv[1], "torus")){
            shape += TOR;
            shape *= checkArgs(2,2,0,fls,ins,str,argc,argv);
        }
        
        else if (!strcmp(argv[1], "patch")){
            shape += PTC;
            shape *= checkArgs(0,1,1,fls,ins,str,argc,argv);
        }

        else if (!strcmp(argv[1], "heightPlane")){
            shape += HMP;
            shape *= checkArgs(3,2,1,fls,ins,str,argc,argv);
        }
        
        else if (!strcmp(argv[1], "heightSphere")){
            shape += HMS;
            shape *= checkArgs(2,2,1,fls,ins,str,argc,argv);
        }      
	}

	switch(shape){
		case PLN:
		Plane(fls[0],fls[1],ins[0],ins[1],argv[6]).build();
		break;

		case BOX:
		Box(fls[0],fls[1],fls[2],ins[0],ins[1],ins[2],argv[8]).build();
		break;

		case SPH:
		Sphere(fls[0],ins[0],ins[1],argv[5]).build();
		break;

		case CON:
		Cone(fls[0],fls[1],ins[0],ins[1],argv[6]).build();
		break;

		case CYL:
		Cylinder(fls[0],fls[1],ins[0],ins[1],argv[6]).build();
		break;

        case TOR:
        Torus(fls[0],fls[1],ins[0],ins[1],argv[6]).build();
        break;

        case PTC:
        Bezier(str[0],ins[0],argv[4]).build();
        break;

        case HMP:
        HeightPlane(fls[0],fls[1],fls[2],ins[0],ins[1],str[0],argv[8]).build();
        break;

        case HMS:
        HeightSphere(fls[0], ins[0],ins[1], fls[1], str[0],argv[7]).build();
        break;

		default:
		printf("Error: Generator called with invalid arguments!\n");
		return 1;
	}
	return 0;
}
