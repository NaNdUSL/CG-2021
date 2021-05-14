class Model{
public:
	
	std::vector<float> vertexList;
	std::vector<float> normalList;
	std::vector<float> uvList;
	std::vector<unsigned int> facesList;
	unsigned int indexNumber;

	GLuint vertex;
	GLuint normals;
	GLuint uvs;
	GLuint indices;

	unsigned int hm,hmw,hmh;
	unsigned char *imageData;

	float intensity;

	Model(){
	}

	Model(std::string fileName){
		readFile(fileName);
		prepareDataVBO();
	}

	Model(std::string fileName,std::string hmapFile,float inten){
		readFile(fileName);
		loadImage(hmapFile);
		intensity = inten;
		setHeightMap();
		prepareDataVBO();
	}

	void drawT(){
		glBegin(GL_TRIANGLES);
		for(unsigned int i: facesList){
			glVertex3f(vertexList[(3*i)],vertexList[3*i+1],vertexList[3*i+2]);
		}
		glEnd();
	}

	void drawVBO(int drawNormals=1){


		glBindBuffer(GL_ARRAY_BUFFER,vertex);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER,normals);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER,uvs);
		glTexCoordPointer(2,GL_FLOAT,0,0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glDrawElements(GL_TRIANGLES, indexNumber, GL_UNSIGNED_INT, 0);

		if (drawNormals){
			glBindTexture(GL_TEXTURE_2D, 0);
			for (int i = 0; i < (int)vertexList.size(); i+=3 ){
				glBegin(GL_LINES);
				GLfloat purple[] = {1.0f, 0.0f, 1.0f, 1.0f};
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, purple);
				glColor3f(1.0f, 0.0f, 1.0f);
				glVertex3f(vertexList[i], vertexList[i+1], vertexList[i+2]);				
				glVertex3f(vertexList[i]+normalList[i], vertexList[i+1]+normalList[i+1], vertexList[i+2]+normalList[i+2]);
				glEnd();				
			}
		}
	}

	
	void readFile( std::string fileName){
		std::ifstream mod;
		mod.open(fileName.c_str());
		
		int numVertices;
		int numFaces;
		float auxf;
		unsigned int auxi;


		mod >> numVertices;
		mod >> numFaces;
		
		for (int j = 0; j < numVertices; ++j){
			for(int i = 0; i < 3;i++){
				mod >> auxf;
				vertexList.push_back(auxf);
				//printf("%f\n", auxf);
			}
			for(int i = 0; i < 2;i++){
				mod >> auxf;
				uvList.push_back(auxf);
			}		
			for(int i = 0; i < 3;i++){
				mod >> auxf;
				normalList.push_back(auxf);
			}
		}

		for(int i = 0; i < 3*numFaces; i++){
			mod >> auxi;
			facesList.push_back(auxi);
		}



		mod.close();
	}


	void prepareDataVBO(){
		
		glGenBuffers(1,&indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,(unsigned int)((sizeof(unsigned int))*facesList.size()), facesList.data(), GL_STATIC_DRAW);

		glGenBuffers(1,&vertex);
		glBindBuffer(GL_ARRAY_BUFFER, vertex);
		glBufferData(GL_ARRAY_BUFFER,(unsigned int)((sizeof(float))*vertexList.size()), vertexList.data(), GL_STATIC_DRAW);

		glGenBuffers(1,&normals);
		glBindBuffer(GL_ARRAY_BUFFER, normals);
		glBufferData(GL_ARRAY_BUFFER,(unsigned int)((sizeof(unsigned int))*normalList.size()), normalList.data(), GL_STATIC_DRAW);

		glGenBuffers(1,&uvs);
		glBindBuffer(GL_ARRAY_BUFFER, uvs);
		glBufferData(GL_ARRAY_BUFFER,(unsigned int)((sizeof(unsigned int))*uvList.size()), uvList.data(), GL_STATIC_DRAW);
		

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		indexNumber = (unsigned int) facesList.size();
	}


	void loadImage(std::string hmapFile){
		ilGenImages(1,&hm);
		ilBindImage(hm);
		ilLoadImage((ILstring) hmapFile.c_str());
		ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);

		hmw = ilGetInteger(IL_IMAGE_WIDTH);
		hmh = ilGetInteger(IL_IMAGE_HEIGHT);

		imageData = ilGetData();
	}

	void setHeightMap(float nLins=0,float nCols=0){
		unsigned int lastVx = vertexList.size()/3;

		if (!(nLins)) nLins = sqrt((float)lastVx); 
		if (!(nCols)) nCols = sqrt((float)lastVx);
		
		//printf("%f\n",nLins);

		for(int h = 0; h < (vertexList.size()/3); h++){
			float pixelH = (float)h/ (float)lastVx;


			float lin = floor(((float)h)/nCols);
			float col = floor((float)h - lin*nCols);

			lin = (lin/nLins)*hmh;
			col = (col/nCols)*hmw;


			int pix = ((int)lin)*hmw;
			pix += ((int)col);
			vertexList[(h*3)+1] += ((float)imageData[pix]/255)*intensity;
		}
	}
};


class Material{
public:

	std::vector<float> diff;
	std::vector<float> spec;
	std::vector<float> ambt;
	std::vector<float> emsv;
	
	unsigned int t = 5, tw, th;
	unsigned int texSet = 0;
	unsigned char *texData;
	
	unsigned int texID = 0;

	Material(std::vector<float> diff, std::vector<float> spec, std::vector<float> ambt, std::vector<float> emsv){
		
		this->diff.assign(diff.begin(),diff.end());
		this->spec.assign(spec.begin(),spec.end());
		this->ambt.assign(ambt.begin(),ambt.end());
		this->emsv.assign(emsv.begin(),emsv.end());
		texSet = 0;
		texID = 0;

	}

	Material(std::string fileName, std::vector<float> diff, std::vector<float> spec, std::vector<float> ambt, std::vector<float> emsv){
		//printf("%s\n",fileName.c_str());

		unsigned int k;

		this->diff.assign(diff.begin(),diff.end());
		this->spec.assign(spec.begin(),spec.end());
		this->ambt.assign(ambt.begin(),ambt.end());
		this->emsv.assign(emsv.begin(),emsv.end());

		ilGenImages(1,&(k));
		this-> t = k;
		ilBindImage(t);
		ilLoadImage((ILstring)fileName.c_str());
		tw = ilGetInteger(IL_IMAGE_WIDTH);
		th = ilGetInteger(IL_IMAGE_HEIGHT);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		texData = ilGetData();
		glGenTextures(1,&texID);
		texSet = 1;

	}

	void setup(){

		GLfloat colD[] = {diff[0], diff[1], diff[2], diff[3]};
		GLfloat colS[] = {spec[0], spec[1], spec[2], spec[3]};
		GLfloat colA[] = {ambt[0], ambt[1], ambt[2], ambt[3]};
		GLfloat colE[] = {emsv[0], emsv[1], emsv[2], emsv[3]};
		
		if (texSet){
			glBindTexture(GL_TEXTURE_2D,texID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		}


		if (diff[3]>= 0) glColor3f(diff[0], diff[1], diff[2]);
		if (ambt[3]>= 0) glMaterialfv(GL_FRONT, GL_AMBIENT, colA);
		if (spec[3]>= 0) glMaterialfv(GL_FRONT, GL_SPECULAR, colS);
		if (diff[3]>= 0) glMaterialfv(GL_FRONT, GL_DIFFUSE, colD);
		if (emsv[3]>= 0) glMaterialfv(GL_FRONT, GL_EMISSION, colE);
		
	}
};