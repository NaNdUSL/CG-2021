#include <stdio.h>
#include <stdlib.h>
#define L 20
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
ofstream MyFile("filename.txt");




int trisPolyLine(int N, int K, vector<vector<int>> &vtx,  vector<int> &res){
	int t = 0;

	for (int i = 1; i < N; i++){
		for (int h = 0; h < K; h++){
			int p = (h+1)%K;
			if ((vtx[i][h]*vtx[i-1][h]*vtx[i-1][p]) > 0   &&  (vtx[i][h] != vtx[i-1][h]) && (vtx[i][h] != vtx[i-1][p]) && (vtx[i-1][h]!= vtx[i-1][p])){
				res[t++] = vtx[i][h];
				res[t++] = vtx[i-1][h];
				res[t++] = vtx[i-1][p];
			}

			if ((vtx[i][h]*vtx[i-1][p]*vtx[i][p]) > 0  &&  (vtx[i][h] != vtx[i-1][p]) && (vtx[i][h] != vtx[i][p]) && (vtx[i-1][p]!= vtx[i][p])){
				res[t++] = vtx[i][h];
				res[t++] = vtx[i-1][p];
				res[t++] = vtx[i][p];
			}
		}
	}
	return t;
}




/////////////////////////////////////////////////////////////////////// ESFERA /////////////////////////////////////////////////////////////////

int circleVertex(float radius, int slices, int stacks, vector<vector <float>> &vtx, vector<int>&vtxNb){
	int vtxPts = 1; // NUNCA PASSE ISSO PARA 0
	vector<vector<int>> aux (
		stacks+2,
		vector<int>(slices)
	);
	float rat = (2*radius)/(stacks+1);
	float freqalf = (2*M_PI)/slices;
	float freqbet = (M_PI)/(stacks+1);

	for (int st = 0; st < stacks+2; st++){
		for (int sl = 0; sl < slices; sl++){
			vtx.push_back({0,0,0});
			vtx[vtxPts-1][0] = (cos(freqbet*st - (M_PI/2))*radius)*sin(freqalf*sl);
			vtx[vtxPts-1][1] = (-radius) + st*rat;
			vtx[vtxPts-1][2] = (cos(freqbet*st - (M_PI/2))*radius)*cos(freqalf*sl);
			aux[st][sl] = vtxPts;
			if (!(st == 0 || st == stacks+1) || sl == slices-1) vtxPts++; 
		}
	}

	return trisPolyLine(stacks+2,slices,aux,vtxNb);
}


int circleDr(float radius, int slices, int stacks){
	int totalVtx = (slices*stacks) + 2;
	int totalFaces = (2*totalVtx - 4);
	vector<int> vtxNb(totalFaces*3);
	vector<vector<float>> vtx;
	int h = circleVertex(radius,slices,stacks,vtx,vtxNb);
	for (int i = 0; i < totalVtx; i++){
		printf("%d: %f , %f , %f\n",i+1,vtx[i][0],vtx[i][1],vtx[i][2] ); 	
	}

	for (int i = 0; i < h; i+=3){
		printf("%d %d %d \n", vtxNb[i],vtxNb[i+1],vtxNb[i+2]);
	}
	MyFile << totalVtx;
	MyFile << "\n";
	MyFile << totalFaces;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////// CONE //////////////////////////////////////////////////////////////////////////

int coneVertex(float radius, float height, int slices, int stacks, vector<vector<float>> &vtx, vector<int> &vtxNb){
	int vtxPts = 1; // NUNCA PASSE ISSO PARA 0
	vector<vector<int>> aux (
		stacks+2,
		vector<int>(slices)
	);
	float rat = (radius)/stacks;
	float heiStck = (height)/(stacks+1);
	float freq = (2*M_PI)/slices;
	int inv;

	for (int st = 0; st <= stacks+1; st++){
		for (int sl = 0; sl < slices; sl++){
			vtx.push_back({0,0,0});
			inv = (stacks+1 - st)%(stacks+1);
			vtx[vtxPts-1][0] = (inv*rat)*sin(freq*sl);
			vtx[vtxPts-1][1] = (-height)/2 + st*(heiStck);                   
			vtx[vtxPts-1][2] = (inv*rat)*cos(freq*sl);

			aux[st][sl] = vtxPts;
			if (!(st == 0 || st == stacks+1) || sl == slices-1) vtxPts++; 
		}
	}


	return trisPolyLine(stacks+2,slices,aux,vtxNb);
}


int coneDr(float radius,float height, int slices, int stacks){
	int totalVtx = (slices*stacks) + 2;
	int totalFaces = (2*totalVtx - 4);
	vector<int> vtxNb(totalFaces*3);
	vector<vector<float>> vtx;
	int h = coneVertex(radius, height, slices,stacks,vtx,vtxNb);
	for (int i = 0; i < totalVtx; i++){
		printf("%d: %f , %f , %f\n",i+1,vtx[i][0],vtx[i][1],vtx[i][2] ); 	
	}

	for (int i = 0; i < h; i+=3){
		printf("%d %d %d \n", vtxNb[i],vtxNb[i+1],vtxNb[i+2]);
	}
	MyFile << totalVtx;
	MyFile << "\n";
	MyFile << totalFaces;
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////// PLANO //////////////////////////////////////////////////////////////////////////

int planeVertex(vector<float>&Orig, float xAng, float zAng, float xDim, float zDim, int xSlc, int zSlc, int*vtxPts, vector< vector<float>>&vtx, vector<int> &vtxNb, int next){
	vector<vector<int>> aux (
		xSlc+1,
		vector<int>(zSlc+2)
	);
	float xRatio = xDim/xSlc;
	float zRatio = zDim/zSlc;
	vector<float> rots = {0,0,0};
	int x,z;

	for (x = 0;  x < xSlc+1; x++){
		for (z = 0; z < zSlc+1; z++){
			vtx.push_back({0,0,0});
			//gera o ponto
			vtx[*vtxPts-1+next][0] = (-(xDim)/2) + x*xRatio;
			vtx[*vtxPts-1+next][1] = 0;
			vtx[*vtxPts-1+next][2] = (-(zDim)/2) + z*zRatio;

			//rotate xAx
			rots[0] = vtx[*vtxPts-1+next][0];
			rots[1] = - vtx[*vtxPts-1+next][2]*sin(xAng);
			rots[2] = vtx[*vtxPts-1+next][2]*cos(xAng);

			//rotate zAx
			vtx[*vtxPts-1+next][0] = rots[0]*cos(zAng) - rots[1]*sin(zAng);
			vtx[*vtxPts-1+next][1] = rots[0]*sin(zAng) + rots[1]*cos(zAng);
			vtx[*vtxPts-1+next][2] = rots[2];
			


			//translada para centrar na origem desejada
			vtx[*vtxPts-1+next][0] += Orig[0];
			vtx[*vtxPts-1+next][1] += Orig[1];
			vtx[*vtxPts-1+next][2] += Orig[2];
			
			aux[x][z] = (*vtxPts)++;	
		}
		aux[x][z] = -1;
	}

	return trisPolyLine(xSlc+1,zSlc+2,aux,vtxNb);
}



int planeDr(float xDim, float zDim, int xSlc, int zSlc){
	int totalVtx = (xSlc+1)*(zSlc+1);
	int totalFaces = xSlc*zSlc*2;
	vector<int> vtxNb(totalFaces*3);
	vector<vector<float>> vtx;
	vector<float> orig = {0,0,0};
	int vtxPts = 1;

	int h = planeVertex(orig, 0, 0,xDim,zDim,xSlc,zSlc,&vtxPts,vtx,vtxNb,0);
	
	for (int i = 0; i < totalVtx; i++){
		printf("%d: %f , %f , %f\n",i+1,vtx[i][0],vtx[i][1],vtx[i][2] ); 	
	}

	for (int i = 0; i < h; i+=3){
		printf("%d %d %d \n", vtxNb[i],vtxNb[i+1],vtxNb[i+2]);
	}

	MyFile << totalVtx;
	MyFile << "\n";
	MyFile << totalFaces;

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








/////////////////////////////////////////////////////////////// BOX //////////////////////////////////////////////////////////////////////////////


int boxDr(float xDim, float yDim, float zDim,int slicesX, int slicesY, int slicesZ){
	int vtxPts = 1;
	int totalVtx = 2*(slicesX+1)*(slicesZ+1) + 2*(slicesX+1)*(slicesY+1) + 2*(slicesY+1)*(slicesZ+1); //número de vértices dos 6 planos que vamos gerar
	int totalFaces = 2*(slicesX*slicesY*2) + 2*(slicesX*slicesZ*2) + 2*(slicesZ*slicesY*2); //removemos as faces dos vértices repetidos
	
	vector<int> vtxNb(totalFaces*3);
	vector<vector<float>>vtx;
	
	vector<float> orig= {0,0,0};
	int done = 0;

	
	//faces do eixo x
	orig[0] = xDim/2;
	orig[1] = 0;
	orig[2] = 0;
	done += planeVertex(orig,0,M_PI/2,zDim,yDim,slicesZ,slicesY,&vtxPts,vtx,vtxNb,done); // z e y podem estar ao contr[ario]
	orig[0]*=-1;
	done += planeVertex(orig,0,-M_PI/2,zDim,yDim,slicesZ,slicesY,&vtxPts,vtx,vtxNb,done);
	
	//faces do eixo y
	orig[0] = 0;
	orig[1] = yDim/2;
	orig[2] = 0;
	done += planeVertex(orig,0,0,xDim,zDim,slicesX,slicesZ,&vtxPts,vtx,vtxNb,done);
	orig[1]*=-1;
	done += planeVertex(orig,0,0,xDim,zDim,slicesX,slicesZ,&vtxPts,vtx,vtxNb,done);


	
	orig[0] = 0;
	orig[1] = 0;
	orig[2] = zDim/2;
	done += planeVertex(orig,M_PI/2,0,xDim,yDim,slicesX,slicesY,&vtxPts,vtx,vtxNb,done);
	orig[2] *= -1;
	done += planeVertex(orig,-M_PI/2,0,xDim,yDim,slicesX,slicesY,&vtxPts,vtx,vtxNb,done);





	for (int i = 0; i < totalVtx; i++){
		printf("%d: %f , %f , %f\n",i+1,vtx[i][0],vtx[i][1],vtx[i][2] ); 	
	}
	for (int i = 0; i < done; i+=3){
		printf("%d %d %d \n", vtxNb[i],vtxNb[i+1],vtxNb[i+2]);
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////// MAIN //////////////////////////////////////////////////////////
int main(int argc, char const *argv[]){
	boxDr(10,20,10,1,1,1);
	//circleDr(30,4,1);
	//coneDr(30,20,4,1);
	
	//planeDr(20,20,2,2);
	return 0;
}















/*
nV
nF
x1
y1
z1
1
2
3
*/


















/*int boxVertex(float xDim, float yDim, float zDim,int slicesX, int slicesY, int slicesZ,int totalVtx, float vtx[totalVtx][3], int totalFaces, int vtxNb[]){
	int vtxPts = 1;
	int aux[((slicesZ-1)*2) + (slicesY-1) + 4][((slicesX-1)*2) + ((slicesZ-1)*2) + 4];
	float xSZ = xDim/slicesX;
	float ySZ = yDim/slicesY;
	float zSZ = zDim/slicesZ;

	for (int y = 0; y < slicesY+1; y++){
		for (int x = 0; x < slicesX+1; x++){
			for (int z = 0; z < slicesZ+1; z++){
				if (!(y%slicesY) || !(z%slicesZ) || !(x%slicesX)){
					vtx[vtxPts-1][0] = (xDim/2) - xSZ*x;
					vtx[vtxPts-1][1] = (yDim/2) - ySZ*y;
					vtx[vtxPts-1][2] = (zDim/2) - zSZ*z;
					vtx++;
				}		
			}
		}
	}


	//return trisPolyLine(slicesY+2,slicesX+slicesZ+2,aux,vtxNb);
}


int boxDr(float xDim, float yDim, float zDim,int slicesX, int slicesY, int slicesZ){ //o número de slices deve variar de 1 para frente
	int totalVtx = (4*(slicesX))+(4*(slicesY))+(4*(slicesZ-1))+(2*(slicesX-1)*(slicesZ-1))+(2*(slicesZ-1)*(slicesY-1))+(2*(slicesY-1)*(slicesX-1));
	int totalFaces = (2*totalVtx - 4);
	int vtxNb[totalFaces*3];
	float vtx[totalVtx][3];
	int h = boxVertex(xDim,yDim,zDim,slicesX,slicesY,slicesZ,totalVtx,vtx,totalFaces,vtxNb);
	for (int i = 0; i < totalVtx; i++){
		printf("%d: %f , %f , %f\n",i+1,vtx[i][0],vtx[i][1],vtx[i][2] ); 	
	}

	//for (int i = 0; i < h; i+=3){
	//	printf("%d %d %d \n", vtxNb[i],vtxNb[i+1],vtxNb[i+2]);
	//}
}
*/