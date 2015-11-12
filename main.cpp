#include <iostream>
#include "Parser.h"

using namespace std;

int main(int argc, char const *argv[])
{
	Parser *parser = new Parser();
	parser->CreateInvertedIndex(); //é referenciado dessa forma pq parser é um ponteiro

	delete parser;

	return 0;
}