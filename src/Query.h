#ifndef QUERY_H
#define QUERY_H

#include "includes.h"

struct Consulta{
	string num_query;
	float tf;
	float weight;
	vector<int> relevantes;
	int qtd_relevantes;
};//estrutura para armazenar valores da consulta

class Query
{
public:
	Query(); //construtor da classe
	~Query(); //destrutor da classe
	void CreateInvertedIndex();	
	void ComputeWeightCollection();
	void ComputeNormCollection();
	void ProcessQuery();
};

#endif