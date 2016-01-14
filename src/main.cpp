#include "Parser.h"
#include "Query.h"
#include <time.h>

int main(int argc, char const *argv[])
{
	time_t start, end, tempo;
	start = clock();

	Query *query = new Query();

	query->CreateInvertedIndex();

	query->ComputeWeightCollection();

	query->ComputeNormCollection();

	query->ProcessQuery();
		
	delete query;

	end = clock();
	tempo = end - start;
	cout << "Tempo de processamento: " << tempo/CLOCKS_PER_SEC << " segundos" << endl;

	return 0;
}