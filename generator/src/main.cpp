#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include "shapes.cpp"

#define PLN 1
#define BOX 2
#define SPH 3
#define CON 4
#define CYL 5



int checkArgs(int nFloats, int nInts, std::vector<float> &fls, std::vector<int> &ins, int argc, char const *argv[]){
	int r = 0;
	float argF;
	int argI;
	if (argc < (nFloats+nInts+3)) return 0;

	for (int i = 0; i < nFloats; i++){
		argF = atof(argv[i+2]);
		if (argF <= 0.1) return 0;
		fls.push_back(argF);
	}

	for (int i = 0; i < nInts; i++){
		argI = atoi(argv[i+(nFloats+2)]);
		if (argI < 1)  return 0;
		ins.push_back(argI);
	}

	return 1;
}


int main(int argc, char const *argv[]){
	int shape = 0;
	std::vector<float> fls;
	std::vector<int> ins;

	if (argc > 2){
		if(!strcmp(argv[1], "plane")){
			shape += PLN;
			shape *= checkArgs(2,2,fls,ins,argc,argv);
		}
	
		else if (!strcmp(argv[1], "box")){
			shape += BOX;
			shape *= checkArgs(3,3,fls,ins,argc,argv);
		}
		
		else if (!strcmp(argv[1], "sphere")){
			shape += SPH;
			shape *= checkArgs(1,2,fls,ins,argc,argv);		
		}
	
		else if (!strcmp(argv[1], "cone")){
			shape += CON;
			shape *= checkArgs(2,2,fls,ins,argc,argv);
		}

		else if (!strcmp(argv[1], "cylinder")){
			shape += CYL;
			shape *= checkArgs(2,2,fls,ins,argc,argv);
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

		default:
		printf("Error: Generator called with invalid arguments!\n");
		return 1;
	}
	return 0;
}
