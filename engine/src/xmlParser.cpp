#include "tinyxml2.h"
#include "expressionParser.cpp"

using namespace tinyxml2;

class XMLParser{
	public:
		Scene* currentScene;
		ExprParser pars;

		std::string fileName;
		tinyxml2::XMLDocument doc;

		int tris = 0;

		XMLParser(std::string fileName, Scene* scene){
			this->fileName = fileName;
			this->currentScene = scene;
			this->currentScene->name += fileName;
		}

		int openXML(){
			//Verifica se ocorreu algum erro durante o loading do ficheiro
			// 0- bem sucedido
			// 3- ficheiro não encontrado
			// 7- erro no XML
			(this->doc).LoadFile((this->fileName).c_str());
			return doc.ErrorID();
		}

		void parse(){
			XMLElement* iterator;
			parseFromTag(currentScene->groups,doc.FirstChildElement());
			this->currentScene->name += " | Tris: " + std::to_string(tris);
		}

		void parseFromTag(std::vector<Group>*gps,XMLElement* base){
			if (base){
				XMLElement* iterator;
				for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
					std::string tagName(iterator->Value());
					if (!tagName.compare("group")){
						parseGroup(gps,iterator);
					}
					else if(!tagName.compare("settings")){
						parseSettings(iterator);
					}
					else if(!tagName.compare("lights")){
						parseLight(iterator);
					}
					
					else parseStatement(gps,iterator,tagName);
				}
			}
		}

		void parseGroup(std::vector<Group>*gps, XMLElement* base){// Retorna o número de triângulos lidos no grupo
			Group g;
			XMLElement* iterator;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				std::string tagName(iterator->Value());
				if (!tagName.compare("translate")){
					parseTranslate(g,iterator);
				}

				else if (!tagName.compare("rotate")){
					parseRotate(g,iterator);
				}

				else if (!tagName.compare("scale")){
					parseScale(g,iterator);
				}
				
				else if (!tagName.compare("models")){
					parseModel(g,iterator);
				}

				else if (!tagName.compare("group")){
					parseGroup(&(g.child),iterator);
				}

				else parseStatement(&(g.child),iterator,tagName);
			}

			(*gps).push_back(g);
		}


		void parseModel(Group &parent,XMLElement* base){
			XMLElement* iterator;
			std::vector<float> diff;
			std::vector<float> ambt;
			std::vector<float> spec;
			std::vector<float> emsv;

			Material* aux;

			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				diff.clear();
				ambt.clear();
				spec.clear();
				emsv.clear();


				std::string fileName(iterator->Attribute("file"));
				
				

				diff.push_back(getParsAtt(iterator,"diffR", 1.0f));
				ambt.push_back(getParsAtt(iterator,"ambtR", diff[0]));
				spec.push_back(getParsAtt(iterator,"specR", 0.0f));
				emsv.push_back(getParsAtt(iterator,"emsvR", 0.0f));

				diff.push_back(getParsAtt(iterator,"diffG", 1.0f));
				ambt.push_back(getParsAtt(iterator,"ambtG", diff[1]));
				spec.push_back(getParsAtt(iterator,"specG", 0.0f));
				emsv.push_back(getParsAtt(iterator,"emsvG", 0.0f));

				diff.push_back(getParsAtt(iterator,"diffB", 1.0f));
				ambt.push_back(getParsAtt(iterator,"ambtB", diff[2]));
				spec.push_back(getParsAtt(iterator,"specB", 0.0f));
				emsv.push_back(getParsAtt(iterator,"emsvB", 0.0f));

				diff.push_back(getParsAtt(iterator,"diffA", 1.0f));
				ambt.push_back(getParsAtt(iterator,"ambtA", diff[3]));
				spec.push_back(getParsAtt(iterator,"specA", 0.0f));
				emsv.push_back(getParsAtt(iterator,"emsvA", 0.0f));


				

				if ( (*(currentScene->modelTable)).find(fileName) == (*(currentScene->modelTable)).end()){
					(*(currentScene->modelTable))[fileName] = new Model(fileName);
				}

				if (iterator->Attribute("texture")){
					std::string textureName(iterator->Attribute("texture"));
					aux = new Material(textureName,diff,spec,ambt,emsv);
				}

				else{
					aux = new Material(diff,spec,ambt,emsv);
				}


				tris += (int)(((((*(currentScene->modelTable))[fileName])->facesList.size()))/3);
				parent.models.push_back( std::pair<Material*, Model*>(aux,(*(currentScene->modelTable))[fileName]));
			}
		}


		void parseLight(XMLElement* base){
			XMLElement* iterator;
			LightRef* lightAdd;
			std::vector<float> postSpec{0,0,0};
			std::vector<float> spotSpec{0,0,0};
			float exponent;
			float cutoff;

			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				postSpec[0] = getParsAtt(iterator,"X", 0.0f);
				postSpec[1] = getParsAtt(iterator,"Y", 0.0f);
				postSpec[2] = getParsAtt(iterator,"Z", 0.0f);

				std::string lightType(iterator->Attribute("type"));
				
				if (!lightType.compare("DIRECTIONAL")){
					lightAdd = new DirectLight(postSpec);
				}
				
				else if (!lightType.compare("SPOT")){
					printf("bruh\n");
					exponent = getParsAtt(iterator,"exp", 0.0f);
					cutoff = getParsAtt(iterator,"cOff", 180.0f);

					spotSpec[0] = getParsAtt(iterator,"dsX", 0.0f);
					spotSpec[1] = getParsAtt(iterator,"dsY", 0.0f);
					spotSpec[2] = getParsAtt(iterator,"dsZ", -1.0f);
					
					lightAdd = new SpotLight(postSpec,spotSpec,exponent,cutoff);
				}

				else{
					lightAdd = new PointLight(postSpec);
				}

				lightAdd->intensityMultiplier = getParsAtt(iterator,"I", 1.0f);

				lightAdd->ambt[0] = getParsAtt(iterator,"aR", 0.0f);
				lightAdd->ambt[1] = getParsAtt(iterator,"aG", 0.0f);
				lightAdd->ambt[2] = getParsAtt(iterator,"aB", 0.0f);
				lightAdd->ambt[3] = getParsAtt(iterator,"aA", 1.0f);

				lightAdd->diff[0] = getParsAtt(iterator,"dR", 1.0f);
				lightAdd->diff[1] = getParsAtt(iterator,"dG", 1.0f);
				lightAdd->diff[2] = getParsAtt(iterator,"dB", 1.0f);
				lightAdd->diff[3] = getParsAtt(iterator,"dA", 1.0f);

				lightAdd->spec[0] = getParsAtt(iterator,"sR", lightAdd->diff[0]);
				lightAdd->spec[1] = getParsAtt(iterator,"sG", lightAdd->diff[1]);
				lightAdd->spec[2] = getParsAtt(iterator,"sB", lightAdd->diff[2]);
				lightAdd->spec[3] = getParsAtt(iterator,"sA", lightAdd->diff[3]);
				currentScene->lightConfigs.push_back(lightAdd);
			}
		}

		void parseTranslate(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(getParsAtt(base,"X"));
			v.push_back(getParsAtt(base,"Y"));
			v.push_back(getParsAtt(base,"Z"));
			float t = getParsAtt(base,"time",-1);
			if (t > 0){
				std::vector<float> points;
				XMLElement* iterator;
				for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
					points.push_back(getParsAtt(iterator,"X"));
					points.push_back(getParsAtt(iterator,"Y"));
					points.push_back(getParsAtt(iterator,"Z"));
				}
				parent.trans.push_back(new CatmullTranslate(t,points));
			}
			else{
				parent.trans.push_back(new Translate(v));
			}
		}

		void parseRotate(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(getParsAtt(base,"axisX"));
			v.push_back(getParsAtt(base,"axisY"));
			v.push_back(getParsAtt(base,"axisZ"));
			float t = getParsAtt(base,"time",-1);
			if (t > 0){
				parent.trans.push_back(new TimedRotation(t,v));
			}
			else{
				parent.trans.push_back(new Rotate(v,getParsAtt(base,"angle")));
			}
		}

		void parseScale(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(getParsAtt(base,"X",1));
			v.push_back(getParsAtt(base,"Y",1));
			v.push_back(getParsAtt(base,"Z",1));
			parent.trans.push_back(new Scale(v));
		}



		// - processamento de settings da engine

		void parseSettings(XMLElement*base){
			XMLElement* iterator;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				std::string tagName(iterator->Value());
				if (!tagName.compare("camera")){
					parseCamera(iterator);
				}

				else if (!tagName.compare("background")){
					currentScene->background[0] = getParsAtt(iterator,"R");
					currentScene->background[1] = getParsAtt(iterator,"G");
					currentScene->background[2] = getParsAtt(iterator,"B");
					currentScene->background[3] = getParsAtt(iterator,"A");
				}
			}
		}

		void parseCamera(XMLElement*base){
			XMLElement* iterator;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				std::string tagName(iterator->Value());
				if (!tagName.compare("center")){
					currentScene->cam.origCenter[0] = getParsAtt(iterator,"X");
					currentScene->cam.origCenter[1] = getParsAtt(iterator,"Y");
					currentScene->cam.origCenter[2] = getParsAtt(iterator,"Z");
				}

				else if (!tagName.compare("position")){
					currentScene->cam.origPosVec[0] = getParsAtt(iterator,"A");
					currentScene->cam.origPosVec[1] = fmod((getParsAtt(iterator,"B") + M_PI/2),M_PI)-(M_PI/2);
					currentScene->cam.origPosVec[2] = getParsAtt(iterator,"R",10.0f);
				}
			}

			currentScene->cam.reset();
		}


		// - Processamento de statements


		void parseStatement(std::vector<Group>*gps, XMLElement*base, std::string tagName){
			if (!tagName.compare("set")) setParsAtt(base,IDNEV);
			else if (!tagName.compare("pi")) setParsAtt(base,MPIEV);
			else if (!tagName.compare("rand")) setParsAtt(base,RNDEV);
			else if (!tagName.compare("cos")) setParsAtt(base,COSEV);
			else if (!tagName.compare("sin")) setParsAtt(base,SINEV);
			else if (!tagName.compare("tan")) setParsAtt(base,TANEV);
			else if (!tagName.compare("sqrt")) setParsAtt(base,SQTEV);

			else parseFlowStm(gps,base,tagName);
		}


		void parseFlowStm(std::vector<Group>*gps, XMLElement*base, std::string tagName){
			float cond = getParsAtt(base,"exp");

			if (!tagName.compare("if")){
				XMLElement*iterator;
				if (cond){
					for(iterator = base->FirstChildElement(); iterator != NULL && strcmp(iterator->Value(),"then"); iterator =  iterator->NextSiblingElement());
				}
				else{
					for(iterator = base->FirstChildElement(); iterator != NULL && strcmp(iterator->Value(),"else"); iterator = iterator->NextSiblingElement());
				}
				parseFromTag(gps,iterator);
			}

			if (!tagName.compare("while")){
				while (cond){
					parseFromTag(gps,base);
					cond = getParsAtt(base,"exp");
				}
			}
		}

		

		// interação com o parser
		float getParsAtt(XMLElement*base,const char* name,float deft = 0){
			const char* val = base->Attribute(name);
			if (!val) return deft;
			pars.setExpr(val);
			float res = pars.eval();
			if (pars.error){
				printf("Warning! Something went wrong parsing XML!\n");
				return deft;
			}
			return res;
		}

		void setParsAtt(XMLElement*base, int opt){
			float evaluation;

			const char* target = base->Attribute("tgt");
			if (!target || !target[0]) return;
			std::string varTgt(target);

			const char* value = base->Attribute("exp");
			if (!(value)) value = "";

			pars.setExpr(value);
			evaluation = pars.evalOpts(opt);
			if (pars.error) printf("Warning! Something went wrong parsing XML!\n");

			pars.hash[varTgt] = evaluation;
		}
};