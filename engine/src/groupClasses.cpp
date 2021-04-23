class Transform{
	public:
		std::vector<float> axis;

		Transform(std::vector<float> vals){
			this->axis.assign(vals.begin(),vals.end());
		}


		virtual void applyTransform(int timeDelta) = 0;
};


class Rotate : public Transform{
	public:
		float degrees;

		Rotate(std::vector<float> vals, float angle):Transform(vals){
			this->degrees = angle;
		}

		void applyTransform(int timeDelta){
			glRotatef(degrees,axis[0],axis[1],axis[2]);
		}
};


class Translate : public Transform{
	public:
		Translate(std::vector<float> vals):Transform(vals){	
		}

		void applyTransform(int timeDelta){
			glTranslatef(axis[0],axis[1],axis[2]);
		}
};


class Scale : public Transform{
	public:
		Scale(std::vector<float> vals):Transform(vals){	
		}

		void applyTransform(int timeDelta){
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

		void applyTransform(int timeDelta){
			currAngle(timeDelta);
			glRotatef(degrees,axis[0],axis[1],axis[2]);
		}
};


class CatmullTranslate : public Transform{
	public:

		float lastPoint = 0.0f;
		float timeToCycle;
		std::vector<float> point;

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

		void currPoint(int timeDelta){
			float offset = (float) timeDelta/timeToCycle;
			lastPoint += offset;

			float t = lastPoint * (float)(axis.size()/3);
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
			

			

			std::vector<std::vector<float>>catmullM = {
				{-0.5f,  1.5f, -1.5f,  0.5f},
				{ 1.0f, -2.5f,  2.0f, -0.5f},
				{-0.5f,  0.0f,  0.5f,  0.0f},
				{ 0.0f,  1.0f,  0.0f,  0.0f}
				};

			std::vector<std::vector<float>> a;
			std::vector<float> T = { (float)pow(t,3),(float)pow(t,2),t,1 };
			for (int i = 0;  i < 4; i++){
				a.push_back({0});
				std::vector<float> p = { p0[i],p1[i],p2[i],p3[i] };
				
				matDotVec(catmullM, p, &a[i]);
				//printf("%f %f %f\n", a[i][0],a[i][1],a[i][2]);
			}

			point.clear();
			matDotVec(a,T,&point);
		}

		void applyTransform(int timeDelta){
			currPoint(timeDelta);
			//printf("%f %f %f\n", point[0],point[1],point[2]);
			glTranslatef(point[0],point[1],point[2]);
		}
};


/* ---------------------------------------------------------------------------------------------------*/



class Group{
	public:
		std::vector<Transform*> trans;
		std::vector<std::pair<std::vector<float>,Model*>> models;
		std::vector<Group> child;

		int flagTRI = 0;
	
		void applyTransforms(int timeDelta){
			for (Transform*t: trans){
				t->applyTransform(timeDelta);
			}
		}
		
		void makeGroup(int timeDelta){
			glPushMatrix();


			applyTransforms(timeDelta);
			for (std::pair<std::vector<float>,Model*> m: models){
				glColor3f(m.first[0],m.first[1],m.first[2]);
				if (flagTRI) m.second->drawT();
				else m.second->drawVBO();
			}
	
			for (Group grp: child){
				grp.makeGroup(timeDelta);
			}
	
			glPopMatrix();
		}
};


