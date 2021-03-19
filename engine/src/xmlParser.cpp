class XMLParser{
	public:
		Scene* currentScene;

		std::string fileName;
		XMLDocument doc;

		XMLParser(std::string fileName, Scene* scene){
			this->fileName = fileName;
			this->currentScene = scene;
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
					parseGroup(currentScene->groups,iterator);
				}
				else if(!tagName.compare("settings")){
					parseSettings(iterator);
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
			std::vector<float> color;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				color.clear();
				std::string fileName(iterator->Attribute("file"));
				color.push_back( iterator -> FloatAttribute("R", 0.5f));
				color.push_back( iterator -> FloatAttribute("G", 0.5f));
				color.push_back( iterator -> FloatAttribute("B", 0.5f));


				if ( (*(currentScene->modelTable)).find(fileName) == (*(currentScene->modelTable)).end()){
					(*(currentScene->modelTable))[fileName] = new Model(fileName);
				}
				parent.models.push_back( std::pair<std::vector<float>, Model*>(color,(*(currentScene->modelTable))[fileName]));
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

		void parseSettings(XMLElement*base){
			XMLElement* iterator;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				std::string tagName(iterator->Value());
				if (!tagName.compare("camera")){
					parseCamera(iterator);
				}

				else if (!tagName.compare("background")){
					currentScene->background[0] = iterator-> FloatAttribute("R");
					currentScene->background[1] = iterator-> FloatAttribute("G");
					currentScene->background[2] = iterator-> FloatAttribute("B");
					currentScene->background[3] = iterator-> FloatAttribute("A");
				}
			}
		}

		void parseCamera(XMLElement*base){
			XMLElement* iterator;
			for(iterator = base->FirstChildElement(); iterator != NULL; iterator = iterator->NextSiblingElement()){
				std::string tagName(iterator->Value());
				if (!tagName.compare("center")){
					currentScene->cam.origCenter[0] = iterator->FloatAttribute("X");
					currentScene->cam.origCenter[1] = iterator->FloatAttribute("Y");
					currentScene->cam.origCenter[2] = iterator->FloatAttribute("Z");
				}

				else if (!tagName.compare("position")){
					currentScene->cam.origPosVec[0] = iterator->FloatAttribute("A");
					currentScene->cam.origPosVec[1] = fmod((iterator->FloatAttribute("B") + M_PI/2),M_PI)-(M_PI/2);
					currentScene->cam.origPosVec[2] = iterator->FloatAttribute("R",10.0f);
				}
			}

			currentScene->cam.reset();
		}
};