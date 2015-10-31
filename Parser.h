#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <fstream>
#include <cstring> //para usar strcat
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Parser
{
public:
	Parser(); //construtor da classe
	~Parser(); //destrutor da classe
	void LerArquivo();
};


#endif
