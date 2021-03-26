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
			std::vector<float> color;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				color.clear();
				std::string fileName(iterator->Attribute("file"));
				color.push_back(getParsAtt(iterator,"R", 0.5f));
				color.push_back(getParsAtt(iterator,"G", 0.5f));
				color.push_back(getParsAtt(iterator,"B", 0.5f));


				if ( (*(currentScene->modelTable)).find(fileName) == (*(currentScene->modelTable)).end()){
					(*(currentScene->modelTable))[fileName] = new Model(fileName);
				}
				tris += (int)((*(currentScene->modelTable))[fileName])->facesList.size();
				parent.models.push_back( std::pair<std::vector<float>, Model*>(color,(*(currentScene->modelTable))[fileName]));
			}
		}


		void parseTranslate(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(getParsAtt(base,"X"));
			v.push_back(getParsAtt(base,"Y"));
			v.push_back(getParsAtt(base,"Z"));
			parent.trans.push_back(new Translate(v));
		}

		void parseRotate(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(getParsAtt(base,"axisX"));
			v.push_back(getParsAtt(base,"axisY"));
			v.push_back(getParsAtt(base,"axisZ"));
			parent.trans.push_back(new Rotate(v,getParsAtt(base,"angle")));
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