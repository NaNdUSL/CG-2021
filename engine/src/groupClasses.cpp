class Transform{
	public:
		std::vector<float> axis;

		Transform(std::vector<float> vals){
			this->axis.assign(vals.begin(),vals.end());
		}

		virtual void applyTransform()=0;
};


class Rotate : public Transform{
	public:
		float degrees;

		Rotate(std::vector<float> vals, float angle):Transform(vals){
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
		Scale(std::vector<float> vals):Transform(vals){	
		}

		void applyTransform(){
			glScalef(axis[0],axis[1],axis[2]);
		}	
};

/* ---------------------------------------------------------------------------------------------------*/


class Group{
	public:
		std::vector<Transform*> trans;
		std::vector<Model*> models;
		std::vector<Group> child;
		int flagTRI = 0;
	
		void applyTransforms(){
			for (Transform*t: trans){
				t->applyTransform();
			}
		}
		
		void makeGroup(){
			glPushMatrix();


			applyTransforms();
			for (Model* mod : models){
				if (flagTRI) mod->drawT();
				else mod->drawVBO();
			}
	
			for (Group grp: child){
				grp.makeGroup();
			}
	
			glPopMatrix();
		}
};





