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
#include <list>
#define NUM_TOTAL_COLECAO 1239

using namespace std;

struct ListCell
{
	int tf; //frequencia que o termo ocorre no documento
	double weight; //peso do termo no documento
};

struct InvertedList // lista invertida
{ 
	double idf; //importância do termo na coleção
	int total_docs;
	unordered_map<double,ListCell> lista;
};

class Parser
{
public:
	Parser(); //construtor da classe
	~Parser(); //destrutor da classe
	//vector<string> CleanDocument(vector<string> terms);
	void PrintHash();
	void IndexDocument(double id_doc, vector<string> terms_clean);
	void CreateInvertedIndex(string file_name);
	void ReadColection();
private:
	unordered_map<string, InvertedList> inverted_index; //hash do indice invertido
};

#endif
