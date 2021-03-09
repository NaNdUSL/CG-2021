#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>



class Mesh{
	public:
		std::vector<std::vector<int>> planar;
		std::vector<std::vector<float>> vertex;
		std::vector<int> faces;
		int cols,lins,facs,vex;
		const char* fileName;


		Mesh(const char* file){
			this->fileName = file;

		}

		virtual void shape() = 0;

		void trisPolyLine(){
			int t = 0;

			for (int i = 1; i < this->lins; i++){
				for (int h = 0; h < this->cols; h++){

					int p = (h+1)%(this->cols);
					if ((planar[i][h]*planar[i-1][h]*planar[i-1][p]) > 0   &&  (planar[i][h] != planar[i-1][h]) && (planar[i][h] != planar[i-1][p]) && (planar[i-1][h]!= planar[i-1][p])){
						faces.push_back(planar[i][h]);
						faces.push_back(planar[i-1][h]);
						faces.push_back(planar[i-1][p]);
						t++;
					}
					
					if ((planar[i][h]*planar[i-1][p]*planar[i][p]) > 0  &&  (planar[i][h] != planar[i-1][p]) && (planar[i][h] != planar[i][p]) && (planar[i-1][p]!= planar[i][p])){
						faces.push_back(planar[i][h]);
						faces.push_back(planar[i-1][p]);
						faces.push_back(planar[i][p]);
						t++;
					}
				}
			}
			this -> facs = t;
		}

		int write3DFile(){
			printf("%d\n",facs);
			std::ofstream File2Wr;
			File2Wr.open(this->fileName);
			File2Wr << vex << "\n" << facs <<"\n";
			for (std::vector<float>&v : this->vertex){
				for (float &point : v){
					File2Wr << point << "\n";
				}
			}
			for (int face:faces){
				File2Wr << face << "\n";
			}
		}

		void build(){
          this->shape();
          this->trisPolyLine();
          this->write3DFile();
        }
};



class Sphere : public Mesh{
	public:
		int radius,slices,stacks;
		
  		
  		Sphere(int radius, int slices, int stacks,const char* fileName):Mesh(fileName){
          	this->radius = radius;
          	this->stacks = stacks;
          	this->slices = slices;
          	this->vex = (slices*stacks) + 2;
          	this->facs = (2*(this->vex) - 4);
          	this->lins = stacks+2;
          	this->cols = slices;
		
		}

  		void shape(){


          	float rat = (2*radius)/(stacks+1);
			float freqalf = (2*M_PI)/slices;
			float freqbet = (M_PI)/(stacks+1);
          	int vtxPts = 1;

          	std::vector<int> row;
          	
          	for (int st = 0; st < stacks+2; st++){
          		row.clear();
				for (int sl = 0; sl < slices; sl++){
					if (!(st == 0 || st == stacks+1) || sl == slices-1){
						std::vector<float> aux;
						aux.push_back((cos(freqbet*st - (M_PI/2))*radius)*sin(freqalf*sl));
						aux.push_back((-radius) + st*rat);
						aux.push_back((cos(freqbet*st - (M_PI/2))*radius)*cos(freqalf*sl));
						(this->vertex).push_back(aux);
						vtxPts++;
					}

					row.push_back(vtxPts);
 
				}
				(this->planar).push_back(row);
			}

        }	
};




class Cone : public Sphere{
	public:
		int height;
		Cone(int height,int radius, int slices, int stacks,const char* fileName) :Sphere(radius,slices,stacks,fileName){
			(this->height) = height;
		}

		void shape(){	
          	int vtxPts = 1;
			float rat = (radius)/stacks;
			float heiStck = (this->height)/(stacks+1);
			float freq = (2*M_PI)/slices;
			int inv;
			
			std::vector<int> row;
			for (int st = 0; st <= stacks+1; st++){
				row.clear();
				for (int sl = 0; sl < slices; sl++){
					if (!(st == 0 || st == stacks+1) || sl == slices-1){
						std::vector<float> aux;
						inv = (stacks+1 - st)%(stacks+1);
						aux.push_back((inv*rat)*sin(freq*sl));
						aux.push_back((-(this->height))/2 + st*(heiStck));                   
						aux.push_back((inv*rat)*cos(freq*sl));
						(this->vertex).push_back(aux);
						vtxPts++;
					}
					row.push_back(vtxPts);
				}
				(this->planar).push_back(row);
			}
		}
};


class Plane: public Mesh{
	public:
		float xDim, zDim;
		int xSlc, zSlc;
		float xAng = 0;
		float zAng = 0;
		std::vector<float> orig = {0,0,0};

		
		Plane(float xDim, float zDim, int xSlc, int zSlc,const char* fileName): Mesh(fileName){
			(this->xDim) = xDim;
			(this->zDim) = zDim;
			(this->xSlc) = xSlc;
			(this->zSlc) = zSlc;
		}

		void shape(){
			int totalVtx = (xSlc+1)*(zSlc+1);
			int totalFaces = xSlc*zSlc*2;
			

		}


};












int main(int argc, char const *argv[]){
	Cone(20,20,4,1,"efera.3D").build();
	return 0;
}