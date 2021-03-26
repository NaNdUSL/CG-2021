class Mesh{
	/* 
		Class Mesh - Classe abstrata para geração da malha dos shapes;
	*/
	public:
		
		// - variáveis de instância
		std::vector<std::vector<int>> planar; 
		std::vector<std::vector<float>> vertex;
		std::vector<int> faces;
		int facs,vex;
		const char* fileName;



		
		// - Construtor :: recebe o nome do ficheiro onde o shape será escrito
		Mesh(const char* file){
			this->fileName = file;
		}


		
		// - Método abstrato que gera a posição dos vértices da malha
		virtual void shape() = 0;


		

		// - Método de geração da malha do shape:
			/*	Tendo a devida planificação dos vértices em uma matriz bidimensional (v.i. "planar"),
				gera a ordem pela qual os vértices devem ser apresentados à engine para criar cada uma das faces do shape.
				-1  -1   -1  -1   -1

				1----2   -1  6    -1
				| \  |       | \            -> na planificação, produziria um resultado [1,3,4,1,4,2,6,5,7]
				3----4   -1  5----7  
			*/
		void trisPolyLine(){
			int t = 0;
			int lins,cols;
			lins = (int)(planar.size());
			cols = (int)(planar[0].size());


			for (int i = 1; i < lins; i++){
				for (int h = 0; h < cols; h++){
					int p = (h+1)%(cols);

					if ((planar[i][h] > 0 && planar[i-1][h]> 0 && planar[i-1][p]> 0)    &&  (planar[i][h] != planar[i-1][h]) && (planar[i][h] != planar[i-1][p]) && (planar[i-1][h]!= planar[i-1][p])){
						faces.push_back(planar[i][h]);
						faces.push_back(planar[i-1][h]);
						faces.push_back(planar[i-1][p]);
						t++;
					}
					
					if ((planar[i][h] > 0 && planar[i-1][p] > 0 && planar[i][p] > 0)   &&  (planar[i][h] != planar[i-1][p]) && (planar[i][h] != planar[i][p]) && (planar[i-1][p]!= planar[i][p])){
						faces.push_back(planar[i][h]);
						faces.push_back(planar[i-1][p]);
						faces.push_back(planar[i][p]);
						t++;
					}
				}
			}
			this -> facs = t;
		}



		// - Método para escrita dos vértices e índices de vértices em ficheiro
		void write3DFile(){
			std::ofstream File2Wr;
			File2Wr.open(this->fileName);
			File2Wr << vex << "\n" << facs <<"\n";
			for (std::vector<float> &v : this->vertex){
				for (float &point : v){
					File2Wr << point << "\n";
				}
			}
			for (int face:faces){
				File2Wr << face << "\n";
			}
			File2Wr.close();
		}


		// - Método que gera o shape, a malha e escreve em ficheiro
		void build(){
			this->shape();
			this->trisPolyLine();
			this->write3DFile();
		}
};






/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/






class Sphere : public Mesh{
	public:
		float radius;
		int slices,stacks;
		
		
		Sphere(float radius, int slices, int stacks,const char* fileName):Mesh(fileName){
			this->radius = radius;
			this->stacks = stacks;
			this->slices = slices;
			this->vex = (slices*stacks) + 2;
		}

		void shape(){

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
						aux.push_back((float)radius*sin(freqbet*st - (M_PI/2)));
						aux.push_back((cos(freqbet*st - (M_PI/2))*radius)*cos(freqalf*sl));
						(this->vertex).push_back(aux);
						row.push_back(vtxPts++);
					}

					else row.push_back(vtxPts);
 
				}
				(this->planar).push_back(row);
			}
		}	
};






/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/






class Cone : public Sphere{
	public:
		float height;
		Cone(float height,float radius, int slices, int stacks,const char* fileName) :Sphere(radius,slices,stacks,fileName){
			(this->height) = height;
		}

		void shape(){	
			int vtxPts = 1;
			float rat = (radius)/stacks;
			float heiStck = height/(stacks);
			float freq = (2*M_PI)/slices;
			int inv;
			
			std::vector<int> row;
			std::vector<float> aux;

			for (int st = 0; st <= stacks+1; st++){
				row.clear();
				for (int sl = 0; sl < slices; sl++){
					if (!(st == 0 || st == stacks+1) || sl == slices-1){
						aux.clear();
						inv = (stacks+1 - st)%(stacks+1);
						aux.push_back((rat*inv)*sin(freq*sl));
						aux.push_back((-(height))/2 +(heiStck)*(st-1));                   
						aux.push_back((rat*inv)*cos(freq*sl));
						
						if (st == 0)aux[1] =(-(height))/2;

						(this->vertex).push_back(aux);
						row.push_back(vtxPts++);
					}
					else row.push_back(vtxPts);
					
				}
				(this->planar).push_back(row);
			}
		}
};






/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/








class Plane: public Mesh{
	public:
		float xDim, zDim;
		int xSlc, zSlc;
		float xAng = 0;
		float zAng = 0;
		std::vector<float> orig;

		
		Plane(float xDim, float zDim, int xSlc, int zSlc,const char* fileName,std::vector<float> orig = {0,0,0}, float xAng = 0, float zAng = 0): Mesh(fileName){
			(this->xDim) = xDim;
			(this->zDim) = zDim;
			(this->xSlc) = xSlc;
			(this->zSlc) = zSlc;
			
			(this->zAng) = zAng;
			(this->xAng) = xAng;
			(this->orig) = orig;

			(this->vex) = (xSlc+1)*(zSlc+1);
		}

		
		void shape(){
			int vtxPts = 1;
			float xRatio = xDim/xSlc;
			float zRatio = zDim/zSlc;
			
			std::vector<float> rots;
			std::vector<float> aux;
			std::vector<int> row;

			for (int x = 0;  x < xSlc+1; x++){
				row.clear();
				for (int z = 0; z < zSlc+1; z++){
					aux.clear();
					rots.clear();
					//gera o ponto
					aux.push_back((-(xDim)/2) + x*xRatio);
					aux.push_back(0);
					aux.push_back((-(zDim)/2) + z*zRatio);
					
					//rotate xAx
					rots.push_back(aux[0]);
					rots.push_back(-aux[2]*sin(xAng));
					rots.push_back(aux[2]*cos(xAng));
					
		
					//rotate zAx
					aux[0] = rots[0]*cos(zAng) - rots[1]*sin(zAng);
					aux[1] = rots[0]*sin(zAng) + rots[1]*cos(zAng);
					aux[2] = rots[2];

					//translada para centrar na origem desejada
					aux[0] += orig[0];
					aux[1] += orig[1];
					aux[2] += orig[2];
					
					(this->vertex).push_back(aux);
					row.push_back(vtxPts++);	
				}
				row.push_back(-1);
				(this->planar).push_back(row);
			}
		}
};






/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/






class Box : public Mesh{
	public:
		float xDim, yDim, zDim;
		int slicesX, slicesY, slicesZ;
		std::vector<Plane> planeFaces;

		Box(float xDim, float yDim, float zDim,int slicesX, int slicesY, int slicesZ,const char* fileName): Mesh(fileName){
			(this->vex) = 2*(slicesX+1)*(slicesZ+1) + 2*(slicesX+1)*(slicesY+1) + 2*(slicesY+1)*(slicesZ+1);
			(this->xDim) = xDim;
			(this->yDim) = yDim;
			(this->zDim) = zDim;
			(this->slicesX) = slicesX;
			(this->slicesY) = slicesY;
			(this->slicesZ) = slicesZ;
		}

		void shape(){
			planeFaces.push_back(Plane(yDim,zDim,slicesY,slicesZ,"",{xDim/2,0,0},0,-M_PI/2));
			planeFaces.push_back(Plane(yDim,zDim,slicesY,slicesZ,"",{-xDim/2,0,0},0,M_PI/2));
			
			planeFaces.push_back(Plane(xDim,zDim,slicesX,slicesZ,"",{0,yDim/2,0},0,0));
			planeFaces.push_back(Plane(xDim,zDim,slicesX,slicesZ,"",{0,-yDim/2,0},0,M_PI));

			planeFaces.push_back(Plane(xDim,yDim,slicesX,slicesY,"",{0,0,zDim/2},M_PI/2,0));
			planeFaces.push_back(Plane(xDim,yDim,slicesX,slicesY,"",{0,0,-zDim/2},-M_PI/2,0));
			
			this->unifyFaces();
		}

		void unifyFaces(){
			int front = 0;
			(this->facs) = 0;
			for (Plane p : (this->planeFaces)){
				p.shape();
				p.trisPolyLine();
				for (std::vector<float> vtx : p.vertex){
					(this -> vertex).push_back(vtx);
				}

				for(int vertexRef: p.faces){
					(this->faces).push_back(vertexRef+front);
				}

				front += p.vex;
				(this->facs) += p.facs;
			}
		}

		void build(){
		  this->shape();
		  this->write3DFile();
		}
};






/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/





class Cylinder : public Cone{
	public:

		Cylinder(float radius, float height, int slices, int stacks,const char* fileName):Cone(radius,height,slices, stacks,fileName){
			this->vex = (slices*(stacks+1)) + 2;
		}

		void shape(){	
			int vtxPts = 1;
			float heiStck = height/(stacks);
			float freq = (2*M_PI)/slices;
			
			std::vector<int> row;
			std::vector<float> aux;

			for (int st = 0; st <= stacks+2; st++){
				row.clear();
				for (int sl = 0; sl < slices; sl++){
					aux.clear();
					if ( st > 0 && st < stacks+2) {
						aux.push_back(radius*sin(freq*sl));
						aux.push_back((-(height))/2 +(heiStck)*(st-1));                   
						aux.push_back(radius*cos(freq*sl));
						
						(this->vertex).push_back(aux);
						vtxPts++;
					}

					else if (st == 0 && sl == slices-1){
						aux.push_back(0);
						aux.push_back((-(height))/2);
						aux.push_back(0);
						(this->vertex).push_back(aux);
					} 
						
					else if (st == stacks+2 && sl == 0){
						aux.push_back(0);
						aux.push_back((height)/2);
						aux.push_back(0);
						vtxPts++;
						(this->vertex).push_back(aux);
					}	
					row.push_back(vtxPts);			
				}
				(this->planar).push_back(row);
			}
		}	
};






/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/





class Torus : public Mesh{
	public:
		float intr,extr;
		int slices,stacks;

		Torus(float internRadius, float externRadius, int slices, int stacks, const char* fileName):Mesh(fileName){
			this->vex = (slices*(stacks));
			this->slices = slices;
			this->stacks = stacks;
			this->intr = internRadius;
			this->extr = externRadius;
		}

		void shape(){
			float alpha = (2*M_PI)/slices;
			float beta = (2*M_PI)/(stacks-1);
			  int vtxPts = 1;

			  std::vector<int> row;

			  for (int st = 0; st < stacks+1; st++){
				 row.clear();
				for (int sl = 0; sl < slices+1; sl++){
					if (vtxPts < this->vex){
						std::vector<float> aux;
						aux.push_back(extr*(sin(alpha*sl)) + intr*sin(alpha*sl)*(cos(beta*st)));
						aux.push_back(intr*(sin(beta*st)));
						aux.push_back(extr*cos(alpha*sl) + intr*cos(alpha*sl)*cos(beta*st));
						
						(this->vertex).push_back(aux);
						row.push_back(vtxPts++);
					}

					else{
						row.push_back((vtxPts % this->vex)+1);
						vtxPts++;
					}

				}
				(this->planar).push_back(row);
			}
		}
};