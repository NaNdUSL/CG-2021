class Model{
	public:
	
	std::vector<float> vertexList;
	std::vector<unsigned int> facesList;
	unsigned int indexNumber;

	GLuint vertex;
	GLuint indices;

	Model(){
	}

	Model(std::string fileName){
		readFile(fileName);
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
};


class Group{
	public:
		std::vector<Transform> trans;
		std::vector<Model> models;
		std::vector<Group> child;
		int flagTRI = 0;
	
		void applyTransforms(){
			for (Transform t: trans){
				t.applyTransform();
			}
		}
		
		void makeGroup(){
			glPushMatrix();

			for (Model mod : models){
				if (flagTRI) mod.drawT();
				else mod.drawVBO();
			}
	
			for (Group grp: child){
				grp.makeGroupVBO()
			}
	
			glPopMatrix();
		}
};



class Transform{
	public:
		std::vector<float> axis;

		Transform(std::vector<float> vals){
			this->values.assign(vals.begin(),vals.end());
		}

		virtual void applyTransform();
};


class Rotation : public Transform{
	public:
		float degrees;

		Rotation(std::vector<float> vals, float angle):Transform(vals){
			this->degrees = angle;
		}

		void applyTransform(){
			glRotatef(degrees,axis[0],axis[1],axis[2]);
		}
};


class Translate : public Transform{
	public:
		Translate(std::vector<float> vals):Transform(vals){	
		}

		void applyTransform(){
			glTranslatef(axis[0],axis[1],axis[2]);
		}	
};


class Scale : public Transform{
	public:
		Translate(std::vector<float> vals):Transform(vals){	
		}

		void applyTransform(){
			glSclalef(axis[0],axis[1],axis[2]);
		}
	
};

