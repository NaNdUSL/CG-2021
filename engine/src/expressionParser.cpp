#define SQTEV 1
#define SINEV 2
#define TANEV 3
#define COSEV 4

class ExprParser{
public:
	std::map<std::string, float>*hash;
	std::string expression;
	int error = 0;
	
	ExprParser(std::string exp,std::map<std::string, float>*map){
		this->expression = exp;
		this->hash= map;
		std::reverse(this->expression.begin(), this->expression.end());  
	}

	void setExpr(std::string exp){
		this->expression = exp;
		std::reverse(this->expression.begin(), this->expression.end()); 
	}

	char popChr(){
		if (expression.empty()) return -1;
		char res = expression.back();
		expression.pop_back();
		return res;
	}


	char peekChr(){
		if (expression.empty()) return -1;
		return expression.back();
	}



	float getValue(){
		std::string res;
		while(isdigit(peekChr())){
			res.push_back(popChr());
		}
		if (peekChr() == '.'){
			res.push_back(popChr());

			while(isdigit(peekChr())){
				res.push_back(popChr());
			}
		}

		try{
			return std::stof(res);
		}
		catch(std::exception& ia){
			error = 1;
			return 1;
		}
	}



	float getVar(){
		std::string res;
		while(isalpha(peekChr())){
			res.push_back(popChr());
		}
		try{
			return hash->at(res.c_str());
		}
		catch(std::exception& ia){
			error = 1;
			return 1;
		}
	}


	float getNextFactor(){
		char check = peekChr();
		float res = 0;
		if(isdigit(check)){
			res = getValue();
		}

		else if(isalpha(check)){
			res = getVar();
		}

		else if(check=='('){
			popChr();
			res = eval();
			popChr();
		}
		else if(check=='-'){
			popChr();
			res = getNextFactor();
			res *= (-1);
		}
		else if(check=='~'){
			popChr();
			res = eval();
			res = !(res);
		}
		else{
			error = 1;
			return 1;
		}
		return res;
	}

	float getNextTerm(){
		float res = getNextFactor();
		char check = peekChr();
		while(check == '*' || check == '/'){
			if (popChr() == '*'){
				res *= getNextFactor();
			}
			else{
				res /= getNextFactor();
			}
			check = peekChr();
		}

		return res;
	}

	float getNextExpr(){
		float res = getNextTerm();
		char check = peekChr();
		while (check == '-' || check=='+'){
			if(popChr() == '+'){
				res += getNextTerm();
				
			}
			else{
				res -= getNextTerm();
			}

			check = peekChr();
		}
		return res;
	}

	float getNextBool(){
		float res = getNextExpr();

		char check = peekChr();
		while (check == '>' || check=='<' || check == '='){
			check = popChr();
			if(check == '>'){
				res = (res > getNextExpr());	
			}
			else if(check == '<') {
				res = (res < getNextExpr());
			}

			else{
				res = (res == getNextExpr());
			}

			check = peekChr();
		}

		return res;		
	}


	float eval(){
		float res = getNextBool();
		char check = peekChr();
		while (check == '&' || check=='|'){
			char check = popChr();
			if(check == '&'){
				res = (res && getNextBool());	
			}
			else if(check == '|') {
				res = (res || getNextBool());
			}
			check = peekChr();
		}
		
		return res;	
	}

	float evalOpts(int opt){
		float res = this->eval();
		error = !(isfinite(res));
		if (!(error))
			switch (opt){
				case SQTEV:
				res = sqrt(res);
				break;
	
				case SINEV:
				res = sin(res);
				break;
	
				case TANEV:
				res = cos(res);
				break;
	
				case COSEV:
				res = cos(res);
				break;
	
				default:
				error = 1;
				break;
			}
		error = !(isfinite(res));
		return res;
	}
};





int main(int argc, char const *argv[]){
	std::map<std::string, float>*hash = new std::map<std::string, float>();
	ExprParser teste(std::string("-200"),hash);
	(*hash)["x"] = 0;
	printf("%f\n",teste.evalOpts(SINEV));
	printf("%d\n",teste.error );
	return 0;
}