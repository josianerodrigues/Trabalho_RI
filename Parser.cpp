#include "Parser.h"

Parser::Parser(){
	cout << "Começando o parser!!" << endl;
}

Parser::~Parser(){
	cout << "Finalizando parser!!" << endl;
}

int verificaTag(string tag){

	if (tag == "AB" || tag == "TI" || tag == "SO" || tag == "MJ" || tag == "MN" || tag == "EX"){
		return 1;
	}else if(tag == "  "){
		return 0;
	}else{
		return -1;
	}
}

void Parser::LerArquivo(){
	ifstream arq;
	arq.open("cfc/cf74");

	string line;
	string NUM_DOC, buffer, tag;
	int flag_tag;
	while(getline(arq, line)){

		tag = line.substr(0, 2); //retorna os dois primeiros caracteres da linha

		if(tag == "RN"){
			NUM_DOC = line.substr(3, line.length()-1); //atoi: converte char para int -1 é por causa do espaço no final da linha.
			cout << "RN: " << NUM_DOC << endl;
		}else if (verificaTag(tag) == 1){
			buffer = line.substr(3, line.length()); //substr: pega a linha do caracter da posição 3 até o final da linha
			//cout << tag << ": " << buffer << endl;

			do{
				getline(arq, line);
				tag = line.substr(0,2);
				flag_tag = verificaTag(tag);

				if (flag_tag == 1 || flag_tag == 0){
					buffer = line.substr(3, line.length()); //substr: pega a linha do caracter da posição 3 até o final da linha
					//cout << tag << buffer << endl;
				}
			} while(flag_tag == 0);
		}
	}
		
}
