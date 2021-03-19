class XMLParser{
	public:
		std::vector<Group>*groups;
		std::map<std::string,Model*>*modelTable;
		std::string fileName;
		XMLDocument doc;

		XMLParser(std::string fileName, std::vector<Group>*groups, std::map<std::string,Model*> *modelTable){
			this->fileName = fileName;
			this->groups = groups;
			this->modelTable = modelTable;
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
			for(iterator = doc.FirstChildElement()->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				std::string tagName(iterator->Value());
				if (!tagName.compare("group")){
					parseGroup(groups,iterator);
				}
			}
		}

		void parseGroup(std::vector<Group>*gps, XMLElement* base){
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
			}

			(*gps).push_back(g);
		}


		void parseModel(Group &parent,XMLElement* base){
			XMLElement* iterator;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				std::string fileName(iterator->Attribute("file"));
				
				if ( (*modelTable).find(fileName) == (*modelTable).end()){
					(*modelTable)[fileName] = new Model(fileName);
				}
				parent.models.push_back((*modelTable)[fileName]);
			}
			
		}


		void parseTranslate(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(base-> FloatAttribute("X"));
			v.push_back(base-> FloatAttribute("Y"));
			v.push_back(base-> FloatAttribute("Z"));
			parent.trans.push_back(new Translate(v));
		}

		void parseRotate(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(base-> FloatAttribute("axisX"));
			v.push_back(base-> FloatAttribute("axisY"));
			v.push_back(base-> FloatAttribute("axisZ"));
			parent.trans.push_back(new Rotate(v,base->FloatAttribute("angle")));
		}

		void parseScale(Group &parent,XMLElement* base){
			std::vector<float> v;
			v.push_back(base-> FloatAttribute("X",1));
			v.push_back(base-> FloatAttribute("Y",1));
			v.push_back(base-> FloatAttribute("Z",1));
			parent.trans.push_back(new Scale(v));
		}
};