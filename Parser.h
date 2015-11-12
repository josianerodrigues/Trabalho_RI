#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <fstream>
#include <cstring> //para usar strcat
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <stack>
#define NUM_TOTAL_COLECAO 1239

using namespace std;

struct ListCell
{
	string id_doc; //numero do documento
	int tf; //frequencia que o termo ocorre no documento
	double weight;
};

struct InvertedList
{ // lista invertida
	double idf; //importância do termo na coleção
	int total_docs;
	stack <ListCell> lista;
};

class Parser
{
public:
	Parser(); //construtor da classe
	~Parser(); //destrutor da classe
	//vector<string> CleanDocument(vector<string> terms);
	unordered_map<string, InvertedList> CreateInvertedIndex();
private:

};


#endif
