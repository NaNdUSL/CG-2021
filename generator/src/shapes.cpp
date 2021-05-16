#include <IL/il.h>

class Mesh{
	/* 
		Class Mesh - Classe abstrata para geração da malha dos shapes;
	*/
	public:
		
		// - variáveis de instância
		std::vector<std::vector<int>> planar; 
		std::vector<float> vertexDescriptor; // posititon[3],UV[3],normal[3]

		std::vector<int> faces;
		std::map<int, std::vector<std::vector<float>>> UVs;
		std::map<int, std::vector<float>> position;
		std::map<int, std::vector<std::vector<float>>> normals;

		int facs,vex;
		const char* fileName;



		
		// - Construtor :: recebe o nome do ficheiro onde o shape será escrito
		Mesh(const char* file){
			this->vex = 0;
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
			this->vex = 0;
			int lins,cols;
			lins = (int)(planar.size());
			cols = (int)(planar[0].size());


			for (int i = 1; i < lins; i++){
				for (int h = 0; h < cols; h++){
					int p = (h+1)%(cols);

					if ((planar[i][h] > 0 && planar[i-1][h]> 0 && planar[i-1][p]> 0)    &&  (planar[i][h] != planar[i-1][h]) && (planar[i][h] != planar[i-1][p]) && (planar[i-1][h]!= planar[i-1][p])){
						faces.push_back(checkVertexProfile(planar[i][h]));
						faces.push_back(checkVertexProfile(planar[i-1][h]));
						faces.push_back(checkVertexProfile(planar[i-1][p]));
						t++;
					}
					
					if ((planar[i][h] > 0 && planar[i-1][p] > 0 && planar[i][p] > 0)   &&  (planar[i][h] != planar[i-1][p]) && (planar[i][h] != planar[i][p]) && (planar[i-1][p]!= planar[i][p])){
						faces.push_back(checkVertexProfile(planar[i][h]));
						faces.push_back(checkVertexProfile(planar[i-1][p]));
						faces.push_back(checkVertexProfile(planar[i][p]));
						t++;
					}
				}
			}

			this -> facs = t;
		}


		int checkVertexProfile(int vx){
			//printf("%d\n",vx );
			std::vector<float> pos = position[vx];
			
			


			for (float i:pos){
				vertexDescriptor.push_back(i);
			}
			

			std::vector<float> uvCoord = UVs[vx][0];
			if (UVs[vx].size() > 1) UVs[vx].erase(UVs[vx].begin());

			std::vector<float> normal = normals[vx][0];
			if (normals[vx].size() > 1) normals[vx].erase(normals[vx].begin());
			
			for (float i:uvCoord){
				vertexDescriptor.push_back(i);
			}
			for (float i:normal){
				vertexDescriptor.push_back(i);
			}
			
			return vex++;
		}


/*		void insertIfMissing(std::map<int, std::vector<std::vector<float>>> m,int key){
			if ( m.find(key) == m.end() ) {
				m[key] = std::vector<std::vector<float>>();
			}
		}*/



		// - Método para escrita dos vértices e índices de vértices em ficheiro
		void write3DFile(){
			std::ofstream File2Wr;
			File2Wr.open(this->fileName);

			File2Wr << vex << "\n" << facs <<"\n";

			for (float point : this->vertexDescriptor){
				File2Wr << point << "\n";
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
			return y;
		}
		
		// Método que normaliza o vetor
		void normalize(float x, float y, float z, std::vector<float> &retVec){
			float value = invSqrt((x*x) + (y*y) + (z*z));
			retVec[0] = x * value;
			retVec[1] = y * value;
			retVec[2] = z * value;
		}

		void cross(std::vector<float> vec1, std::vector<float> vec2, std::vector<float> &retVec){
			retVec.clear();
			retVec.push_back(vec1[1]*vec2[2] - vec1[2]*vec2[1]);
			retVec.push_back(vec1[2]*vec2[0] - vec1[0]*vec2[2]);
			retVec.push_back(vec1[0]*vec2[1] - vec1[1]*vec2[0]);
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
		}

		void shape(){

			float freqalf = (2*M_PI)/slices;
			float freqbet = (M_PI)/(stacks+1);
			int vtxPts = 1;

			float uFreq = 1/(float)(slices);
			float vFreq = 1/(float)(stacks+1);

			std::vector<int> row;

			int st;
			int sl;
			for (st = 0; st < stacks+2; st++){
				row.clear();
				for (sl = 0; sl < slices; sl++){
						std::vector<float> aux;
						std::vector<float> auxN{0,0,0};
						aux.push_back((cos(freqbet*st - (M_PI/2))*radius)*sin(freqalf*sl));
						aux.push_back((float)radius*sin(freqbet*st - (M_PI/2)));
						aux.push_back((cos(freqbet*st - (M_PI/2))*radius)*cos(freqalf*sl));
						
						(this->position).insert({vtxPts,aux});
						normalize(aux[0],aux[1],aux[2],auxN);
						

						//insertIfMissing(this->UVs,vtxPts);
						//insertIfMissing(this->normals,vtxPts);
						this->normals[vtxPts].push_back(auxN);
						

						if(sl==0 && st>1 && st<stacks+1){
							std::vector<float> auxUVA{1.0f,1.0f - (float)st * (vFreq)};
							std::vector<float> auxUVB{0.0f,1.0f - (float)st * (vFreq)};							
							
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);
						}

						else if(sl==0 && st==1){
							std::vector<float> auxUVA{1.0f,1.0f - (float)st * (vFreq)};
							std::vector<float> auxUVB{0.0f,1.0f - (float)st * (vFreq)};
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);							
							this->UVs[vtxPts].push_back(auxUVA);
						}

						else{
							std::vector<float> auxUV{(float) sl* (uFreq), 1 - (float)st * (vFreq)};
							this->UVs[vtxPts].push_back(auxUV);
						}

						row.push_back(vtxPts);
						if (!(st == 0 || st == stacks+1) || sl == slices-1) vtxPts++;
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

			float uFreq = 1/(float)(slices);
			float vFreq = 2/(3*(float)(stacks));

			float baseUVRad = (float)1/6;

			for (int st = 0; st <= stacks+1; st++){
				row.clear();
				for (int sl = 0; sl < slices; sl++){
						aux.clear();
						std::vector<float> auxN{0,0,0};
						inv = (stacks+1 - st)%(stacks+1);
						aux.push_back((rat*inv)*sin(freq*sl));
						aux.push_back((-(height))/2 +(heiStck)*(st-1));                   
						aux.push_back((rat*inv)*cos(freq*sl));
						

						normalize(height*sin(freq*sl), radius , height*cos(freq*sl),auxN);

						//insertIfMissing(this->UVs,vtxPts);
						//insertIfMissing(this->normals,vtxPts);
						
						if (st == 0){
							aux[1] =(-(height))/2;
							auxN[0] = 0;
							auxN[1] = -1;
							auxN[2] = 0;
						}

						(this->position).insert({vtxPts,aux});

						if (st == 1){
							std::vector<float> auxNA{0,-1,0};
							this->normals[vtxPts].push_back(auxNA);
							this->normals[vtxPts].push_back(auxNA);
						}

						this->normals[vtxPts].push_back(auxN);

						if(sl==0 && st>1 && st<stacks+1){
							std::vector<float> auxUVA{0.0f,1.0f -((float)st+1) * (vFreq)};
							std::vector<float> auxUVB{1.0f,1.0f -((float)st+1) * (vFreq)};
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVB);
						}

						else if(sl==0 && st==1){
							std::vector<float> auxUVA{1.0f,1.0f - ((float)st+1) * (vFreq)};
							std::vector<float> auxUVC{baseUVRad + baseUVRad* sin(freq*sl),baseUVRad + baseUVRad* cos(freq*sl)};
							std::vector<float> auxUVB{0.0f,1.0f - ((float)st+1) * (vFreq)};
							this->UVs[vtxPts].push_back(auxUVC);

							this->UVs[vtxPts].push_back(auxUVC);
							
							this->UVs[vtxPts].push_back(auxUVB);
							
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);
							
							this->UVs[vtxPts].push_back(auxUVC);
						}


						else if(st==1){
							std::vector<float> auxUVA{baseUVRad + baseUVRad* sin(freq*sl),baseUVRad + baseUVRad* cos(freq*sl)};
							std::vector<float> auxUVB{(float) sl* (uFreq),1.0f -((float)st+1) * (vFreq)};
							
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);							

							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVB);


							this->UVs[vtxPts].push_back(auxUVB);							
						}



						else if(st==0){
							std::vector<float> auxUVA{baseUVRad, baseUVRad};
							this->UVs[vtxPts].push_back(auxUVA);
						}

						else{
							std::vector<float> auxUV{(float) sl* (uFreq),1.0f -((float)st+1) * (vFreq)};
							this->UVs[vtxPts].push_back(auxUV);
						}


						row.push_back(vtxPts);
						if (!(st == 0 || st == stacks+1) || sl == slices-1) vtxPts++;
					
				}
				(this->planar).push_back(row);
			}
		}
};






/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/








class Plane: public Mesh{
	public:
		float xDim, zDim, initU, initV, maxU, maxV;
		int xSlc, zSlc;
		float xAng = 0;
		float zAng = 0;
		std::vector<float> orig;

		
		Plane(float xDim, float zDim, int xSlc, int zSlc,const char* fileName,std::vector<float> orig = {0,0,0}, float xAng = 0, float zAng = 0, float initU = 0, float initV= 0, float maxU = 1, float maxV= 1): Mesh(fileName){
			(this->xDim) = xDim;
			(this->zDim) = zDim;
			(this->xSlc) = xSlc;
			(this->zSlc) = zSlc;
			
			(this->zAng) = zAng;
			(this->xAng) = xAng;
			(this->orig) = orig;

			this ->initU = initU;
			this ->initV = initV;
			this ->maxU =  maxU;
			this ->maxV =  maxV;
		}

		
		void shape(){
			int vtxPts = 1;
			float xRatio = xDim/xSlc;
			float zRatio = zDim/zSlc;
			
			std::vector<float> rots;
			std::vector<float> aux;
			std::vector<int> row;

			float uFreq = (maxU - initU)/(float)(xSlc);
			float vFreq = (maxV - initV)/(float)(zSlc);

			for (int x = 0;  x < xSlc+1; x++){
				row.clear();
				for (int z = 0; z < zSlc+1; z++){
					aux.clear();
					rots.clear();
					//insertIfMissing(this->UVs,vtxPts);
					//insertIfMissing(this->normals,vtxPts);

					std::vector<float> auxUV{( (float)x * (uFreq)) + initU, ((float)z *(vFreq)) + initV};
					//printf("%f\n", uFreq);
					//printf("%f\n", ( (float)x * (uFreq)) + initU);
					//printf("%f\n", ((float)z *(vFreq)) + initV);


					this->UVs[vtxPts].push_back(auxUV);


					//gera o ponto
					aux.push_back((-(xDim)/2) + x*xRatio);
					aux.push_back(0);
					aux.push_back((-(zDim)/2) + z*zRatio);
					
					std::vector<float> auxN{0,1,0};
					std::vector<float> rotsN;


					//rotate xAx
					rots.push_back(aux[0]);
					rots.push_back(-aux[2]*sin(xAng));
					rots.push_back(aux[2]*cos(xAng));

					rotsN.push_back(0);
					rotsN.push_back(cos(xAng));
					rotsN.push_back(sin(xAng));

					
		
					//rotate zAx
					aux[0] = rots[0]*cos(zAng) - rots[1]*sin(zAng);
					aux[1] = rots[0]*sin(zAng) + rots[1]*cos(zAng);
					aux[2] = rots[2];

					//rotate zAx
					auxN[0] = rotsN[0]*cos(zAng) - rotsN[1]*sin(zAng);
					auxN[1] = rotsN[0]*sin(zAng) + rotsN[1]*cos(zAng);
					auxN[2] = rotsN[2];					


					//translada para centrar na origem desejada
					aux[0] += orig[0];
					aux[1] += orig[1];
					aux[2] += orig[2];
					
					this->normals[vtxPts].push_back(auxN);
					(this->position).insert({vtxPts,aux});
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
			(this->xDim) = xDim;
			(this->yDim) = yDim;
			(this->zDim) = zDim;
			(this->slicesX) = slicesX;
			(this->slicesY) = slicesY;
			(this->slicesZ) = slicesZ;
		}

		void shape(){
			planeFaces.push_back(Plane(yDim,zDim,slicesY,slicesZ,"",{xDim/2,0,0},0,-M_PI/2,0,0,(float)1/3,(float)1/3));
			planeFaces.push_back(Plane(yDim,zDim,slicesY,slicesZ,"",{-xDim/2,0,0},0,M_PI/2,(float)1/3,0,(float)2/3,(float)1/3));
			
			planeFaces.push_back(Plane(xDim,zDim,slicesX,slicesZ,"",{0,yDim/2,0},0,0,0,(float)1/3,(float)1/3,(float)2/3));
			planeFaces.push_back(Plane(xDim,zDim,slicesX,slicesZ,"",{0,-yDim/2,0},0,M_PI,(float)1/3,(float)1/3,(float)2/3,(float)2/3));

			planeFaces.push_back(Plane(xDim,yDim,slicesX,slicesY,"",{0,0,zDim/2},M_PI/2,0,0,(float)2/3,(float)1/3,1));
			planeFaces.push_back(Plane(xDim,yDim,slicesX,slicesY,"",{0,0,-zDim/2},-M_PI/2,0,(float)1/3,(float)2/3,(float)2/3,1));
			
			this->unifyFaces();
		}

		void unifyFaces(){
			int front = 0;
			(this->facs) = 0;
			for (Plane p : (this->planeFaces)){
				p.shape();
				p.trisPolyLine();
				for (float vtx : p.vertexDescriptor){
					(this -> vertexDescriptor ).push_back(vtx);
				}

				for(int vertexRef: p.faces){
					(this->faces).push_back(vertexRef+front);
				}

				front += p.vex;
				(this->facs) += p.facs;
			}

			this->vex = front;
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
		}

		void shape(){	
			int vtxPts = 1;
			float heiStck = height/(stacks);
			float freq = (2*M_PI)/slices;
			
			float uFreq = 1/(float)(slices);
			float vFreq = 2/(3*(float)(stacks));

			float baseUVRad = (float)1/6;
			
			std::vector<int> row;
			std::vector<float> aux;

			std::vector<float> auxN{0,0,0};

			for (int st = 0; st <= stacks+2; st++){
				row.clear();
				for (int sl = 0; sl < slices; sl++){
					aux.clear();

					//insertIfMissing(this->UVs,vtxPts);
					//insertIfMissing(this->normals,vtxPts);
					if ( st > 0 && st < stacks+2) {
						aux.push_back(radius*sin(freq*sl));
						aux.push_back((-(height))/2 +(heiStck)*(st-1));                   
						aux.push_back(radius*cos(freq*sl));
					}

					else if (st == 0){
						aux.push_back(0);
						aux.push_back((-(height))/2);
						aux.push_back(0);
					} 
						
					else if (st == stacks+2){
						aux.push_back(0);
						aux.push_back((height)/2);
						aux.push_back(0);
					}	
					

					if (st>0 && st<stacks+1){
						normalize(aux[0],0,aux[2],auxN);
					}

					
					if (st == 1){
						std::vector<float> auxNA{0,-1,0};
						normalize(aux[0],0,aux[2],auxN);
						this->normals[vtxPts].push_back(auxNA);
						this->normals[vtxPts].push_back(auxNA);
					}





					if(st == 0  || st == stacks+2){
						auxN[0] = 0;
						auxN[1] = (2)*(st%(stacks+1)) - 1;
						auxN[2] = 0;
					}

					
					if (st == stacks+1){
						std::vector<float> auxNA{0,1,0};
						normalize(aux[0],0,aux[2],auxN);
						//vec.insert(vec.begin(), 3);

						this->normals[vtxPts].push_back(auxN);
						this->normals[vtxPts].push_back(auxN);
						this->normals[vtxPts].push_back(auxN);
						this->normals[vtxPts].push_back(auxNA);
					}

					else this->normals[vtxPts].push_back(auxN);



					if(sl==0 && st>1 && st<stacks+1){
						std::vector<float> auxUVA{0.0f,1 - ((float)st+1) * (vFreq)};
						std::vector<float> auxUVB{1.0f,1 - ((float)st+1) * (vFreq)};
						
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVB);
					}

					else if(st==0){
						std::vector<float> auxUVA{baseUVRad,baseUVRad};
						this->UVs[vtxPts].push_back(auxUVA);
					}
					else if(st==stacks+2){
						std::vector<float> auxUVA{1-baseUVRad,baseUVRad};
						this->UVs[vtxPts].push_back(auxUVA);
					}

					else if(sl==0 && st==stacks+1){
						std::vector<float> auxUVA{0.0f,1 - ((float)st+1) * (vFreq)};
						std::vector<float> auxUVB{1.0f,1 - ((float)st+1) * (vFreq)};
						std::vector<float> auxUVC{1-baseUVRad + baseUVRad* sin(freq*sl),baseUVRad + baseUVRad* cos(freq*sl)};
						
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVB);
						this->UVs[vtxPts].push_back(auxUVC);
					}
					
					else if(st==stacks+1){
						std::vector<float> auxUV{(float) sl* (uFreq),1 - ((float)st+1) * (vFreq)};
						std::vector<float> auxUVA{1-baseUVRad + baseUVRad* sin(freq*sl),baseUVRad + baseUVRad* cos(freq*sl)};
						this->UVs[vtxPts].push_back(auxUV);
						this->UVs[vtxPts].push_back(auxUV);
						this->UVs[vtxPts].push_back(auxUV);
						this->UVs[vtxPts].push_back(auxUVA);
					}

					else if(sl==0 && st==1){
						std::vector<float> auxUVA{0.0f,1 - ((float)st+1) * (vFreq)};
						std::vector<float> auxUVB{1.0f,1 - ((float)st+1) * (vFreq)};
						std::vector<float> auxUVC{baseUVRad + baseUVRad* sin(freq*sl),baseUVRad + baseUVRad* cos(freq*sl)};
						
						this->UVs[vtxPts].push_back(auxUVC);
						this->UVs[vtxPts].push_back(auxUVC);
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVB);
						this->UVs[vtxPts].push_back(auxUVB);
						
					}
					
					else if(st==1){
						std::vector<float> auxUV{(float) sl* (uFreq),1 - ((float)st+1) * (vFreq)};
						std::vector<float> auxUVA{baseUVRad + baseUVRad* sin(freq*sl),baseUVRad + baseUVRad* cos(freq*sl)};
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUV);
					}



					
					else{
						std::vector<float> auxUV{(float) sl* (uFreq),1 - ((float)st+1) * (vFreq)};
						this->UVs[vtxPts].push_back(auxUV);
					}

					(this->position).insert({vtxPts,aux});
					row.push_back(vtxPts);
					
					if (sl == slices-1 || (st > 0 && st < stacks+2)){
						vtxPts++;
					}			
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
			float uFreq = 1/(float)(slices);
			float vFreq = 1/(float)(stacks-1);
			float alpha = (2*M_PI)/slices;
			float beta = (2*M_PI)/(stacks-1);
			int vtxPts = 1;


			std::vector<int> row;

			for (int st = 0; st < stacks; st++){
				row.clear();
				for (int sl = 0; sl < slices+1; sl++){
					//insertIfMissing(this->UVs,vtxPts);
					//insertIfMissing(this->normals,vtxPts);
					
					if (sl == 0 && st > 0 && st < stacks){
						std::vector<float> auxUVB{0.0f,1-(float)st * (vFreq)};
						std::vector<float> auxUVA{1.0f,1-(float)st * (vFreq)};
						
						this->UVs[vtxPts].push_back(auxUVB);
						this->UVs[vtxPts].push_back(auxUVB);
						this->UVs[vtxPts].push_back(auxUVA);

						this->UVs[vtxPts].push_back(auxUVB);
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVA);

						
					}

					if (sl > 0 && st == 0){
						std::vector<float> auxUVA{(float)sl * (uFreq), 1.0f};

						this->UVs[vtxPts].push_back(auxUVA);					
					}

					if (sl == 0 && st == 0){
						std::vector<float> auxUVB{1.0f, 0.0f};
						std::vector<float> auxUVA{0.0f, 1.0f};
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVB);
						this->UVs[vtxPts].push_back(auxUVB);
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVA);
						this->UVs[vtxPts].push_back(auxUVB);						
					}

					if (sl > 0 && st > 0 && st < stacks){
						std::vector<float> auxUV{(float) sl* (uFreq),1-(float)st * (vFreq)};
						this->UVs[vtxPts].push_back(auxUV);
					}


					if (st < stacks){
						std::vector<float> auxN{0,0,0};
						std::vector<float> aux;
						aux.push_back(extr*(sin(alpha*sl)) + intr*sin(alpha*sl)*(cos(beta*st)));
						aux.push_back(intr*(sin(beta*st)));
						aux.push_back(extr*cos(alpha*sl) + intr*cos(alpha*sl)*cos(beta*st));

						normalize(aux[0] - extr*(sin(alpha*sl)),aux[1], aux[2] - extr*cos(alpha*sl), auxN);
						
						(this->position).insert({vtxPts,aux});
						this->normals[vtxPts].push_back(auxN);
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



/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/




class Bezier : public Mesh{
	public:
		float tessFact = 0;
		int tesselationLv = 0;
		int patcheNum = 0;
		std::vector<std::vector<std::vector<float>>> curves;

		Bezier(std::string fileName, int tesselation, const char* file):Mesh(file){
			tessFact = 1.0f/((float) tesselation);
			tesselationLv = tesselation;

			readPatches(fileName);
		}

		void readPatches(std::string fileName){
			std::ifstream file(fileName.c_str());
			
			std::vector<std::vector<int>> indexes;
			std::vector<std::vector<float>> points;
			
			file >> patcheNum;
			
			
			std::vector<int> aux;
			std::string lmao;
			for (int i = 0; i < patcheNum; i++){
				aux.clear();
				lmao.clear();
				for (int j = 0; j < 15 ; j++){
					std::getline(file,lmao,',');
					aux.push_back(std::stoi(lmao));
				}
				std::getline(file,lmao);
				aux.push_back(std::stoi(lmao));
				indexes.push_back(aux);
			}



			int pointsNum = 0;
			file >> pointsNum;

			std::vector<float> flAux;
			for (int i = 0; i < pointsNum; i++){
				flAux.clear();
				lmao.clear();
				for (int j = 0; j < 2; j++){
					std::getline(file,lmao,',');
					flAux.push_back(std::stof(lmao));

				}
				std::getline(file,lmao);
				flAux.push_back(std::stof(lmao));

				flAux.push_back(1.0f);
				points.push_back(flAux);
			}


			std::vector<std::vector<float>> curve;
			for(std::vector<int> row : indexes){
				curve.clear();
				int count = 0;
				for (int i : row){
					curve.push_back(points[i]);
					count++;
					if (count > 3){
						curves.push_back(curve);
						curve.clear();
						count = 0;
					}
				}
			}		
		}


		void shape(){

			float u = 0;
			float v = 0;
			int vtxPts = 1;
			std::vector<int> row;

			int UVFact = partitionUv(patcheNum);

/*				printf("%d\n",UVFact );
				printf("%d\n",patcheNum );*/
			
			float initU;
			float initV;
			float maxU;
			float maxV;

			for (int kk = 0; kk < patcheNum; kk++){
				u = 0;
				int aux = kk % (UVFact);

				initU = (float)(aux) * (1/(float)UVFact);
				maxU = initU + (1/(float)UVFact);

				aux = (kk/UVFact);
				initV = (float)(aux)*(1/(float)UVFact);
				maxV = initV + (1/(float)UVFact);

				/*printf("IU %f\n",initU );*/
				/*printf("MU  %f\n",maxU );*/
				/*printf("IV   %f\n",initV );*/
				/*printf("MV    %f\n",maxV );*/



				float uFreq = (maxU - initU)/(float)(tesselationLv);
				float vFreq = (maxV - initV)/(float)(tesselationLv);

				/*printf("%f\n",uFreq);
				printf("%f\n",vFreq );*/



				std::vector<std::vector<float>> interCurve;
				std::vector<std::vector<float>> interDerivU;


				for (int i = 0; i <= tesselationLv; i++){
					interCurve.clear();
					interDerivU.clear();
					row.clear();
					
					interCurve.push_back(getCurrPoint(curves[(kk*4)],u));
					interCurve.push_back(getCurrPoint(curves[(kk*4)+1],u));
					interCurve.push_back(getCurrPoint(curves[(kk*4)+2],u));
					interCurve.push_back(getCurrPoint(curves[(kk*4)+3],u));

					
					interDerivU.push_back(getCurrDeriv(curves[(kk*4)],u));
					interDerivU.push_back(getCurrDeriv(curves[(kk*4)+1],u));
					interDerivU.push_back(getCurrDeriv(curves[(kk*4)+2],u));
					interDerivU.push_back(getCurrDeriv(curves[(kk*4)+3],u));


					
					v = 0;
					for (int j = 0; j <= tesselationLv; j++){

						std::vector<float> newPoint = getCurrPoint(interCurve,v);
						std::vector<float> newDerivU = getCurrPoint(interDerivU,v);
						std::vector<float> newDerivV = getCurrDeriv(interCurve,v);
						std::vector<float> auxN;

						//insertIfMissing(this->UVs,vtxPts);
						//insertIfMissing(this->normals,vtxPts);
						
						newPoint.erase(newPoint.begin() + 3); 
						(this->position).insert({vtxPts,newPoint});

						cross(newDerivU,newDerivV,auxN);
						normalize(auxN[0],auxN[1],auxN[2],auxN);
						this->normals[vtxPts].push_back(auxN);
						
						std::vector<float> auxUV{( (float)j * (uFreq)) + initU, ((float)i *(vFreq)) + initV};
						this->UVs[vtxPts].push_back(auxUV);

						row.push_back(vtxPts++);
						v += tessFact;
					}
					row.push_back((-1));
					
					(this->planar).insert(this->planar.begin(), row);
					u += tessFact;
				}

				row.clear();
				for (int i = 0; i < tesselationLv+2; i++){
					row.push_back(-1);
					
				}
				(this->planar).insert(this->planar.begin(), row);
				
			}


		}


		void matDotVec(std::vector<std::vector<float>> mat, std::vector<float> vec, std::vector<float>*res){
			(*res).clear();
			for (int i = 0; i < (int) mat.size(); i++){
				(*res).push_back(0);
				for (int j = 0; j < (int) mat[0].size(); j++){
					(*res)[i] = (*res)[i] + (vec[j] * (mat[i][j]));
				}
			}
		}

		

		std::vector<float> getCurrPoint(std::vector<std::vector<float>> pts,float t){
			std::vector<float> point;
			std::vector<std::vector<float>>bezierM = {
					{-1.0f,  3.0f, -3.0f,  1.0f},
					{ 3.0f, -6.0f,  3.0f, 0.0f},
					{-3.0f,  3.0f,  0.0f,  0.0f},
					{ 1.0f,  0.0f,  0.0f,  0.0f}
					};

			std::vector<std::vector<float>> a;
			std::vector<float> T = { (float)pow(t,3),(float)pow(t,2),t,1 };
			
			for (int i = 0;  i < 4; i++){
				a.push_back({0});
				std::vector<float> p = { pts[0][i],pts[1][i],pts[2][i],pts[3][i] };
				matDotVec(bezierM, p, &a[i]);
			}

			point.clear();
			matDotVec(a,T,&point);

			return point;
		}

		

		

		std::vector<float> getCurrDeriv(std::vector<std::vector<float>> pts,float t){
			std::vector<float> point;
			std::vector<std::vector<float>>bezierM = {
					{-1.0f,  3.0f, -3.0f,  1.0f},
					{ 3.0f, -6.0f,  3.0f, 0.0f},
					{-3.0f,  3.0f,  0.0f,  0.0f},
					{ 1.0f,  0.0f,  0.0f,  0.0f}
					};

			std::vector<std::vector<float>> a;
			std::vector<float> T = { (float)pow(t,2)*3 ,  2*t, 1, 0 };
			
			for (int i = 0;  i < 4; i++){
				a.push_back({0});
				std::vector<float> p = { pts[0][i],pts[1][i],pts[2][i],pts[3][i] };
				matDotVec(bezierM, p, &a[i]);
			}

			point.clear();
			matDotVec(a,T,&point);

			return point;
		}

		int partitionUv(int patchNum){
			double k = (double) patchNum;
			k = sqrt(k);
			k = floor(k);
			k += 1;
			int r = (int) k;
			return r;
		}
 
};



/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


class HeightPlane : public Mesh{
	public:
		float xDim, zDim;
		int xSlc, zSlc;
		unsigned int hm,hmw,hmh;
		unsigned char *imageData;
		float intensity;
		float xRatio;
		float zRatio;


		HeightPlane(float xDim, float zDim, float intensity, int xSlc, int zSlc,std::string hmapFile,const char* fileName) : Mesh(fileName){
			(this->xDim) = xDim;
			(this->zDim) = zDim;
			(this->xSlc) = xSlc;
			(this->zSlc) = zSlc;


			this->intensity = intensity;
			ilGenImages(1,&hm);
			ilBindImage(hm);
			ilLoadImage((ILstring) hmapFile.c_str());
			ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	
			hmw = ilGetInteger(IL_IMAGE_WIDTH);
			hmh = ilGetInteger(IL_IMAGE_HEIGHT);
	
			imageData = ilGetData();
		}
	
		
		void shape(){
			int vtxPts = 1;
 			xRatio = xDim/xSlc;
			zRatio = zDim/zSlc;
			
			std::vector<float> rots;
			std::vector<int> row;
			std::vector<float> auxN;
			std::vector<float> aux;

			float uFreq = 1/(float)(xSlc);
			float vFreq = 1/(float)(zSlc);

			for (int x = 0;  x < xSlc+1; x++){
				//printf(" XXXXXXXX %d\n",x);
				row.clear();
				for (int z = 0; z < zSlc+1; z++){
					//printf(" ZZZZZZZZZZZZZ %d\n",z);
					aux.clear();
					auxN.clear();
					rots.clear();
					//insertIfMissing(this->UVs,vtxPts);
					//insertIfMissing(this->normals,vtxPts);

					auxN = getNormal(x,z);
					std::vector<float> auxUV{( (float)x * (uFreq)), ((float)z *(vFreq))};
					this->UVs[vtxPts].push_back(auxUV);

					aux = getPoint(x,z);
					this->normals[vtxPts].push_back(auxN);
					(this->position).insert({vtxPts,aux});
					row.push_back(vtxPts++);	
				}
				row.push_back(-1);
				(this->planar).push_back(row);
			}
		}

        //
		std::vector<float> getPoint(int x, int z){
			std::vector<float> aux;
			aux.push_back((-(xDim)/2) + x* (this-> xRatio));
			aux.push_back(getHMPValue(x,z));
			aux.push_back((-(zDim)/2) + z*(this-> zRatio));
			return aux;
		}


		std::vector<float> getNormal(int x, int z){
			std::vector<float> p1 = getPoint(x,z-1);
			std::vector<float> p2 = getPoint(x,z+1);
			std::vector<float> p3 = getPoint(x-1,z);
			std::vector<float> p4 = getPoint(x+1,z);

			std::vector<float> v1{p2[0]-p1[0], p2[1]-p1[1], p2[2]-p1[2]};
			std::vector<float> v2{p4[0]-p3[0], p4[1]-p3[1], p4[2]-p3[2]};

			cross(v1,v2,p1);
			normalize(p1[0],p1[1],p1[2],p1);
			return p1;
		}

		float getHMPValue(int x, int z){
			if ((x > xSlc || x < 0) || (z > zSlc || z < 0)) return 0.0f;
			float lin = ((float)x/(xSlc+1))*hmh;
			float col = ((float)z/(zSlc+1))*hmw;

			//printf("%f,%f\n",((float)x/xSlc),((float)z/zSlc));
			int pix = ((int)col)*(hmw);
			pix += ((int)lin);
			//printf("%d\n",pix );
			return ((float)imageData[pix]/255)*intensity;
		}
};



/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



class HeightSphere : public Mesh{
	public:
		float radius;
		int slices,stacks;
		unsigned int hm,hmw,hmh;
		unsigned char *imageData;
		float intensity;
		float xRatio;
		float zRatio;
		float freqalf;
		float freqbet;
		
		
		HeightSphere(float radius, int slices, int stacks, float intensity, std::string hmapFile, const char* fileName):Mesh(fileName){
			this->radius = radius;
			this->stacks = stacks;
			this->slices = slices;
			this->intensity = intensity;
			ilGenImages(1,&hm);
			ilBindImage(hm);
			ilLoadImage((ILstring) hmapFile.c_str());
			ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	
			hmw = ilGetInteger(IL_IMAGE_WIDTH);
			hmh = ilGetInteger(IL_IMAGE_HEIGHT);
	
			imageData = ilGetData();
		}

		void shape(){

			freqalf = (2*M_PI)/slices;
			freqbet = (M_PI)/(stacks+1);
			int vtxPts = 1;

			float uFreq = 1/(float)(slices);
			float vFreq = 1/(float)(stacks+1);

			std::vector<int> row;

			int st;
			int sl;
			for (st = 0; st < stacks+2; st++){
				row.clear();
				for (sl = 0; sl < slices; sl++){
						//printf("%d\n",st);
						std::vector<float> aux = getPoint(sl,st);
						std::vector<float> auxN = getNormal(st,sl);
						(this->position).insert({vtxPts,aux});
						this->normals[vtxPts].push_back(auxN);
						

						if(sl==0 && st>1 && st<stacks+1){
							std::vector<float> auxUVA{1.0f,1.0f - (float)st * (vFreq)};
							std::vector<float> auxUVB{0.0f,1.0f - (float)st * (vFreq)};							
							
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);
						}

						else if(sl==0 && st==1){
							std::vector<float> auxUVA{1.0f,1.0f - (float)st * (vFreq)};
							std::vector<float> auxUVB{0.0f,1.0f - (float)st * (vFreq)};
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVB);
							this->UVs[vtxPts].push_back(auxUVA);
							this->UVs[vtxPts].push_back(auxUVA);							
							this->UVs[vtxPts].push_back(auxUVA);
						}

						else{
							std::vector<float> auxUV{(float) sl* (uFreq), 1 - (float)st * (vFreq)};
							this->UVs[vtxPts].push_back(auxUV);
						}

						row.push_back(vtxPts);
						if (!(st == 0 || st == stacks+1) || sl == slices-1) vtxPts++;
				}
				(this->planar).push_back(row);
			}
		}


		std::vector<float> getPoint(int sl, int st){
			std::vector<float> geNrm = getGeomNorm(sl,st);

			float heightFact = getHMPValue(st,sl);
			std::vector<float> res{geNrm[0]*(heightFact+radius), geNrm[1]*(heightFact+radius), geNrm[2]*(heightFact+radius)};
			return res;
		}

		std::vector<float> getGeomNorm(int sl, int st){
			std::vector<float> aux;
			aux.push_back((cos(freqbet*st - (M_PI/2)))*sin(freqalf*sl));
			aux.push_back(sin(freqbet*st - (M_PI/2)));
			aux.push_back((cos(freqbet*st - (M_PI/2)))*cos(freqalf*sl));
			return aux;
		}

		std::vector<float> getNormal(int z, int x){
			std::vector<float> p1 = getPoint(x,z-1);
			std::vector<float> p2 = getPoint(x,z+1);
			std::vector<float> p3 = getPoint(x-1,z);
			std::vector<float> p4 = getPoint(x+1,z);

			std::vector<float> v1{p2[0]-p1[0], p2[1]-p1[1], p2[2]-p1[2]};
			std::vector<float> v2{p4[0]-p3[0], p4[1]-p3[1], p4[2]-p3[2]};

			cross(v2,v1,p1);
			normalize(p1[0],p1[1],p1[2],p1);
			return p1;
		}

		float getHMPValue(int st, int sl){
			if ((st < 0) || st > (stacks + 1)) return 0.0f;
			if ((sl < 0) || st > (slices)) return 0.0f; 
			
			int x = st;
			int z = sl;

			
			float lin = ((float)1-(float)x/(stacks+2))*(hmh-1);
			float col = ((float)z/(slices+1))*hmw;

			//printf("%f,%f\n",(float)x/(stacks+1),((float)1-(float)z/(slices+1)));
			//printf("%d,%d\n",((int)lin),((int)col));
			int pix = ((int)lin)*(hmw);
			//printf("%d\n",pix );
			pix += ((int)col);
			//printf("%d\n",pix );
			return ((float)imageData[pix]/255)*intensity;
		}	
};
