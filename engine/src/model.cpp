class Model{
	public:
	
	std::vector<float> vertexList;
	std::vector<unsigned int> facesList;
	unsigned int indexNumber;

	GLuint vertex;
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

	void drawVBO(){
		glBindBuffer(GL_ARRAY_BUFFER,vertex);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glDrawElements(GL_TRIANGLES, indexNumber, GL_UNSIGNED_INT, 0);
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
		
		for(int i = 0; i < 3*numVertices;i++){
			mod >> auxf;
			vertexList.push_back(auxf);
		}

		for(int i = 0; i < 3*numFaces; i++){
			mod >> auxi;
			facesList.push_back(auxi-1);
		}
		mod.close();
	}


	void prepareDataVBO(){
		glGenBuffers(1,&vertex);
		glBindBuffer(GL_ARRAY_BUFFER, vertex);
		glBufferData(GL_ARRAY_BUFFER,(unsigned int)((sizeof(float))*vertexList.size()), vertexList.data(), GL_STATIC_DRAW);

		glGenBuffers(1,&indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,(unsigned int)((sizeof(unsigned int))*facesList.size()), facesList.data(), GL_STATIC_DRAW);

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


