#ifndef PARSER_H
#define PARSER_H

#include "includes.h"

struct ListCell{
	int tf;
	double weight;
};

struct Vocabulary{
	double idf;
	int total_docs;
	map<int, ListCell> inverted_list;
};//lista invertida de cada termo

extern struct ListCell list_cell;
extern struct Vocabulary vocabulary;
extern map<string, Vocabulary> inverted_index; //hash do índice invertido
extern int NUM_DOC;
extern float NUM_TOTAL_COLECAO;

class Parser
{
public:
	Parser(); //construtor da classe
	~Parser(); //destrutor da classe
	void ReadColection(string file);
	void PrintHash();
	vector<string> CleanDocument(vector<string> terms);
	string GetLineContent(string line);
};

#endif