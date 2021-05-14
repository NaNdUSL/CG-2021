class Transform{
	public:
		std::vector<float> axis;

		Transform(std::vector<float> vals){
			this->axis.assign(vals.begin(),vals.end());
		}


		virtual void applyTransform(int timeDelta, int toDraw) = 0;

		void draw(){
			return;
		}
};


class Rotate : public Transform{
	public:
		float degrees;

		Rotate(std::vector<float> vals, float angle):Transform(vals){
			this->degrees = angle;
		}

		void applyTransform(int timeDelta, int toDraw){
			glRotatef(degrees,axis[0],axis[1],axis[2]);
		}
};


class Translate : public Transform{
	public:
		Translate(std::vector<float> vals):Transform(vals){	
		}

		void applyTransform(int timeDelta, int toDraw){
			glTranslatef(axis[0],axis[1],axis[2]);
		}
};


class Scale : public Transform{
	public:
		Scale(std::vector<float> vals):Transform(vals){	
		}

		void applyTransform(int timeDelta, int toDraw){
			glScalef(axis[0],axis[1],axis[2]);
		}	
};



class TimedRotation : public Transform{
	public:
		float timeToCycle; //in miliseconds
		float degrees;


		TimedRotation(float time,std::vector<float> vals):Transform(vals){
			degrees = 0.0f;
			timeToCycle = time*1000; // se recebernos em segundos podemos mudar aqui
			if (timeToCycle <= 0) timeToCycle = 1000;
		}

		void currAngle(int timeDelta){
			degrees += (360/timeToCycle)* ((float) timeDelta); // depende da unidade de timeToCycle
		}

		void applyTransform(int timeDelta, int toDraw){
			currAngle(timeDelta);
			glRotatef(degrees,axis[0],axis[1],axis[2]);
		}
};


class CatmullTranslate : public Transform{
	public:

		float lastPoint = 0.0f;
		float timeToCycle;
		std::vector<float> point;
		std::vector<float> deriv;
		float rot[4][4];
		std::vector<float> lastY = {0,1,0,0};
		std::vector<float> lastX = {0,0,0,0};
		std::vector<float> lastZ = {0,0,0,0};

		CatmullTranslate(float time,std::vector<float> controlPts):Transform(controlPts){
			timeToCycle = time*1000;
			if (timeToCycle <= 0) timeToCycle = 1000;

		}


		void matDotVec(std::vector<std::vector<float>> mat, std::vector<float> vec, std::vector<float>*res){
			(*res).clear();
			for (int i = 0; i < mat.size(); i++){
				(*res).push_back(0);
				for (int j = 0; j < mat[0].size(); j++){
					(*res)[i] = (*res)[i] + (vec[j] * (mat[i][j]));
				}
			}
		}

		
		
		void getRotationMat(){
			normalize(deriv);
			normalize(lastY);
			lastZ = crossProd(deriv,lastY);
			lastY = crossProd(lastZ,deriv);
			rot[0][0] = deriv[0]; rot[0][1] = lastY[0]; rot[0][2] = lastZ[0]; rot[0][3] =        0;  
			rot[1][0] = deriv[1]; rot[1][1] = lastY[1]; rot[1][2] = lastZ[1]; rot[1][3] =        0;
			rot[2][0] = deriv[2]; rot[2][1] = lastY[2]; rot[2][2] = lastZ[2]; rot[2][3] =        0;
			rot[3][0] =        0; rot[3][1] =        0; rot[3][2] =        0; rot[3][3] =        1;
		}


		void currPoint(int timeDelta){
			float offset = (float) timeDelta/timeToCycle;
			lastPoint += offset;

			float t = lastPoint * (float)(axis.size()/3);
			std::vector<float> vec = getPointCurve(t,1);
			point.clear();


			for (int i=0; i<vec.size(); i++)
        		point.push_back(vec[i]);
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

		void normalize(std::vector<float> &retVec){
			float value = invSqrt((retVec[0]*retVec[0]) + (retVec[1]*retVec[1]) + (retVec[2]*retVec[2]));
			retVec[0] = retVec[0] * value;
			retVec[1] = retVec[1] * value;
			retVec[2] = retVec[2] * value;
		}


		std::vector<float> crossProd(std::vector<float> a, std::vector<float> b){
			std::vector<float> res;
			res.push_back(a[1]*b[2] - a[2]*b[1]);
			res.push_back(a[2]*b[0] - a[0]*b[2]);
			res.push_back(a[0]*b[1] - a[1]*b[0]);
			normalize(res);

			return res;
		}





		std::vector<float> getPointCurve(float t, int rot=0){
			int index = floor(t);
			t = t - index;

			int indices[4]; 
			indices[0] = (index + (axis.size()/3)-1)%(axis.size()/3);
			indices[1] = (indices[0]+1)%(axis.size()/3);
			indices[2] = (indices[1]+1)%(axis.size()/3);
			indices[3] = (indices[2]+1)%(axis.size()/3);


			std::vector<float> p0 = { axis[(indices[0]*3)],axis[(indices[0]*3)+1],axis[(indices[0]*3)+2],1.0f };
			std::vector<float> p1 = { axis[(indices[1]*3)],axis[(indices[1]*3)+1],axis[(indices[1]*3)+2],1.0f };
			std::vector<float> p2 = { axis[(indices[2]*3)],axis[(indices[2]*3)+1],axis[(indices[2]*3)+2],1.0f };
			std::vector<float> p3 = { axis[(indices[3]*3)],axis[(indices[3]*3)+1],axis[(indices[3]*3)+2],1.0f };
			

			std::vector<float> res;

			std::vector<std::vector<float>>catmullM = {
				{-0.5f,  1.5f, -1.5f,  0.5f},
				{ 1.0f, -2.5f,  2.0f, -0.5f},
				{-0.5f,  0.0f,  0.5f,  0.0f},
				{ 0.0f,  1.0f,  0.0f,  0.0f}
				};

			std::vector<std::vector<float>> a;
			std::vector<float> T = { (float)pow(t,3),(float)pow(t,2),t,1 };
			std::vector<float> Tlinha = { 3*(float)(pow(t,2)), 2*t, 1, 0 };
			for (int i = 0;  i < 4; i++){
				a.push_back({0});
				std::vector<float> p = { p0[i],p1[i],p2[i],p3[i] };
				
				matDotVec(catmullM, p, &a[i]);
			}

			res.clear();
			matDotVec(a,T,&res); // 

			if (rot){
				matDotVec(a,Tlinha,&deriv);
			}
			return res;
		}


		void applyTransform(int timeDelta, int toDraw){
			if (toDraw)	{
				this->draw();
			}

			currPoint(timeDelta);
			getRotationMat();
			glTranslatef(point[0],point[1],point[2]);
			glMultMatrixf(&rot[0][0]);
		}

		void draw(){
			float tesselation = 0.01f;
			float k;

			glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			for (float i = 0; i < 1; i= i + tesselation){
				k = i * (float)(axis.size()/3);
				std::vector<float> vec = getPointCurve(k);	
				glVertex3f(vec[0], vec[1], vec[2]);
			}
			glEnd();
		}
};


/* ---------------------------------------------------------------------------------------------------*/





class Group{
	public:
		std::vector<Transform*> trans;
		std::vector<std::pair<Material*,Model*>> models;
		std::vector<Group> child;

		int flagTRI = 0;
	
		void applyTransform(int timeDelta, int toDraw){
			for (Transform*t: trans){
				t->applyTransform(timeDelta, toDraw);
			}
		}
		
		void makeGroup(int timeDelta, int toDraw, int drawNormals, Material*UVCheck, int checkUV){
			glPushMatrix();


			applyTransform(timeDelta, toDraw);
			for (std::pair<Material*,Model*> m: models){

				if (checkUV) UVCheck->setup();
				else m.first->setup(); 
				

				if (flagTRI) m.second->drawT();
				else m.second->drawVBO(drawNormals);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
	
			for (Group grp: child){
				grp.makeGroup(timeDelta, toDraw, drawNormals, UVCheck, checkUV);
			}
	
			glPopMatrix();
		}
};

