class LightRef{
	
	
	public:
		static int total;
		int id;
		float intensityMultiplier;

		float vectsUtil[4];

		GLfloat ambt[4];
		GLfloat diff[4];
		GLfloat spec[4];
		
		LightRef(std::vector<float> vals, float isPoint){

			this->id = total++;
			for (int i = 0; i < 3; ++i){
				vectsUtil[i] = vals[i];
			}

			vectsUtil[3] = isPoint;
		}

		void setup(){
			for (int i = 0; i < 3; ++i){
				diff[i] *= intensityMultiplier;
				spec[i] *= intensityMultiplier;
			}

			glEnable(GL_LIGHT0+id); //GL_LIGHT0+i = GL_LIGHTi
			glLightfv(GL_LIGHT0+id, GL_AMBIENT, ambt);
			glLightfv(GL_LIGHT0+id, GL_DIFFUSE, diff);
			glLightfv(GL_LIGHT0+id, GL_SPECULAR, spec);			
		}

		virtual void place() = 0;
};


int LightRef::total = 0;



class PointLight : public LightRef{
	
	public:
		PointLight(std::vector<float> vals):LightRef(vals, 1.0f){
		}
		void place(){
			glLightfv(GL_LIGHT0+id,GL_POSITION, vectsUtil);
		}
};




class DirectLight : public LightRef{
	
	public:
		DirectLight(std::vector<float> vals):LightRef(vals, 0.0f){
		}
		void place(){
			glLightfv(GL_LIGHT0+id,GL_POSITION, vectsUtil);
		}
};






class SpotLight : public LightRef{
	public:
		float directSpot[3];
		float exponent;
		float cutoff;

		SpotLight(std::vector<float> vals, std::vector<float> directs, float expArg, float cutArg):LightRef(vals, 1.0f){
			for (int i = 0; i < 3; ++i){
				directSpot[i] = directs[i];
			}

			if (expArg < 0) expArg = 0;
			if (expArg > 128) expArg = 128;
			exponent = expArg;
			
			if (cutArg < 0) cutArg = 0;
			if (cutArg > 90) cutArg = 180;
			cutoff = cutArg;
		}
		
		void place(){

			//printf("AAA %f, %f, %f\n", directSpot[0],directSpot[1], directSpot[2]);
			//printf("%f\n", cutoff);
			glLightfv(GL_LIGHT0+id,GL_POSITION, vectsUtil);
			glLightfv(GL_LIGHT0+id,GL_SPOT_EXPONENT, &exponent);
			glLightfv(GL_LIGHT0+id,GL_SPOT_CUTOFF, &cutoff);
			glLightfv(GL_LIGHT0+id,GL_SPOT_DIRECTION, directSpot);
			
		}
};

