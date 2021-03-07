#include <stdio.h>
#include <stdlib.h>
#define L 20
#include <math.h>


int trisPolyLine(int N, int K,int vtx[N][K], int res[]){
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

int circleVertex(float radius, int slices, int stacks, int totalVtx, float vtx[totalVtx][3], int totalFaces, int vtxNb[]){
	int vtxPts = 1; // NUNCA PASSE ISSO PARA 0
	int aux[stacks+2][slices];
	float rat = (2*radius)/(stacks+1);
	float freqalf = (2*M_PI)/slices;
	float freqbet = (M_PI)/(stacks+1);

	for (int st = 0; st < stacks+2; st++){
		for (int sl = 0; sl < slices; sl++){
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
	int vtxNb[totalFaces*3];
	float vtx[totalVtx][3];
	int h = circleVertex(radius,slices,stacks,totalVtx,vtx,totalFaces,vtxNb);
	for (int i = 0; i < totalVtx; i++){
		printf("%d: %f , %f , %f\n",i+1,vtx[i][0],vtx[i][1],vtx[i][2] ); 	
	}

	for (int i = 0; i < h; i+=3){
		printf("%d %d %d \n", vtxNb[i],vtxNb[i+1],vtxNb[i+2]);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









////////////////////////////////////////////////////////////// CONE //////////////////////////////////////////////////////////////////////////

int coneVertex(float radius, float height, int slices, int stacks, int totalVtx, float vtx[totalVtx][3], int totalFaces, int vtxNb[]){
	int vtxPts = 1; // NUNCA PASSE ISSO PARA 0
	int aux[stacks+2][slices];
	float rat = (radius)/stacks;
	float heiStck = (height)/(stacks+1);
	float freq = (2*M_PI)/slices;
	int inv;

	for (int st = 0; st <= stacks+1; st++){
		for (int sl = 0; sl < slices; sl++){

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
	int vtxNb[totalFaces*3];
	float vtx[totalVtx][3];
	int h = coneVertex(radius, height, slices,stacks,totalVtx,vtx,totalFaces,vtxNb);
	for (int i = 0; i < totalVtx; i++){
		printf("%d: %f , %f , %f\n",i+1,vtx[i][0],vtx[i][1],vtx[i][2] ); 	
	}

	for (int i = 0; i < h; i+=3){
		printf("%d %d %d \n", vtxNb[i],vtxNb[i+1],vtxNb[i+2]);
	}
	return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int boxVertex(float xDim, float yDim, float zDim,int slicesX, int slicesY, int slicesZ,int totalVtx, float vtx[totalVtx][3], int totalFaces, int vtxNb[]){
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










int main(int argc, char const *argv[]){
	//int vtx[4][3] = {{-1,1,-1},{1,3,4},{5,6,7},{8,8,8}};
	//int res[L*3];
	//for (int i = 0; i < L*3; ++i){
	//	res[i] = -1;
	//}
	//trisPolyLine(4,3,vtx,res);
	//for (int i = 0; i < L*3; ++i){
	//	printf("%d ",res[i]);
	//}
	//boxDr(10,10,10,2,4,2);
	//circleDr(30,4,2);
	coneDr(30,20,4,10);
	return 0;
}