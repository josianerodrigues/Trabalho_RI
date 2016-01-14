#include "Parser.h"

int NUM_DOC;
bool flag_line = false; //flag que indica qual o bloco de linhas devem ser lidas para serem indexados os termos
float NUM_TOTAL_COLECAO = 1239;

map<string, Vocabulary> inverted_index;

Parser::Parser()
{
	cout << "Iniciando processo de geração do índice invertido!!" << endl;
}

bool IsTag(string line)
{/*Essa função verifica se os  dois primeiros caracteres da linha sao aqueles que devem ser indexados*/

	string tag;
	tag = line.substr(0, 2); //retorna os dois primeiros caracteres da linha

	if (tag == "TI" || tag == "AB" || tag == "EX" || tag == "MJ" || tag == "MN"){
		flag_line = true;
		return true;
	} else if (tag == "RN") {
		//Pega o numero do documento
		NUM_DOC = atoi((line.substr(3, line.length()-1)).c_str());
	} else if (!(line[0] == ' ')) {
		flag_line = false;
		return false;
	}
	return false;
}

string Parser::GetLineContent(string line)
{/* Essa função retorna o conteúdo da linha */
	return line.substr(3, line.length());
}


bool IsCaracter(char c)
{/*Essa função verifica a existência de digitios e caracteres especiais*/

	if (isdigit(c))	{
		return true;
	}

	switch (c){
	case '.':
	case '"':
	case ',':
	case '\'':
	case '(':
	case ')':
	case '[':
	case ']':
	case ':':
	case '%':
	case '+':
	case '<':
	case '>':
	case '=':
	case '&':
	case '?':
	case '/':
	case '-':
	case '$':
	case '#':
	case ';':
		return true;
	default:
		return false;
	}
}

vector<string> Parser::CleanDocument(vector<string> terms)
{ /*Essa função remove todos os termos que são stopwords, dígitos e caracteres especiais*/
	
	vector<string>::iterator it;
	vector<string> terms_clean;

	ifstream stopword_file;
	stopword_file.open("../stopwords.txt");
	unordered_map<string, string> stopwords;
	string line;

	while(getline(stopword_file, line)){ 
		line = line.substr(0, line.length() - 1); //cada linha corresponde a uma stopword (0 -1 é pra retirar o \n)
		stopwords[line];
	}

	for (it = terms.begin(); it != terms.end(); it++) {
		it->erase(remove_if(it->begin(), it->end(), &IsCaracter), it->end()); //remove caracteres especiais e digitos
		transform(it->begin(), it->end(), it->begin(), ::tolower); //transforma em minusculo

		if (!(*it).empty()) {
			if (stopwords.find(*it) == stopwords.end() && (*it).size() > 2){//insere no vector caso não seja uma stopword
				terms_clean.push_back(*it);									//e que seja uma palavra com mais de 2 caracteres
			}					
		}
	}
	return terms_clean;
}

void IndexDocument(vector<string> terms)
{/*Essa função insere os termos no hash criando suas respectivas lista invertidas com informações de tf e total_docs*/
	
	vector<string>::iterator it;
	string word;
	Vocabulary voc;
	ListCell cell;
	Vocabulary voc_aux;
	ListCell cel_aux;
	map<string, Vocabulary>::iterator it_hash;
	map<int, ListCell>::iterator it_cel;

	for (it = terms.begin(); it != terms.end(); it++) {
		word = *it;
		//se o termo não existe no hash, insere no hash com tf=1, total_docs=1 e cria um elemento na lista invertida
		if (inverted_index.find(word) == inverted_index.end()) {
			voc.total_docs = 1;
			cell.tf = 1;
			voc.inverted_list[NUM_DOC] = cell;
			inverted_index[word] = voc;
			//se existe
		} else {
			it_hash = inverted_index.find(word);
			voc_aux = (*it_hash).second;
			//Caso já exista, primeiro verifica se o documento não existe na lista invertida
			if (voc_aux.inverted_list.find(NUM_DOC) == voc_aux.inverted_list.end()) {
			//Se não existir, incrementa o total_docs e cria uma nova celula na lista invertida com tf=1
				voc_aux.total_docs++;
				cell.tf = 1;
				voc_aux.inverted_list[NUM_DOC] = cell;
				
			} else { //Se já existe o documento, incrementa o tf
				//voc_aux.total_docs = (*it_hash).second.total_docs;
				it_cel = voc_aux.inverted_list.find(NUM_DOC);
				cel_aux = (*it_cel).second;
				cel_aux.tf++;
				voc_aux.inverted_list[NUM_DOC] = cel_aux;
			}

			inverted_index[word] = voc_aux;
		}
	}
}

void Parser::PrintHash()
{/*Essa função imprime o índice invertido em um arquivo txt*/

	map<string, Vocabulary>::iterator iti;
	map<int, ListCell>::iterator it_cel;
	float acum = 0;
	int aux = 0;
	char line[255];

	ofstream file("hash.txt");
	for (iti = inverted_index.begin(); iti != inverted_index.end(); ++iti) {
		sprintf(line, "%s ->  totaldocs:%d, idf: %f\n", iti->first.c_str(),
				iti->second.total_docs, (*iti).second.idf);
		file << line;

		for (it_cel = iti->second.inverted_list.begin();
				it_cel != iti->second.inverted_list.end(); ++it_cel) {
			sprintf(line, "(%d, %d)  peso: %f \n", it_cel->first,
					it_cel->second.tf, it_cel->second.weight);

			file << line;
		}
	}
	file.close();
}

void Parser::ReadColection(string file_name) {

	vector<string> terms, terms_clean;
	string line, buffer;
	ifstream arq(file_name.c_str());

	if (!arq.is_open()) {
		cout << "erro ao abrir arquivo " << file_name << endl;
		//return NULL;
	}

	//Faz a leitura de cada linha do arquivo
	while (getline(arq, line)) {
		if ((line.empty()) && (terms.size() > 0)) {
			terms_clean = CleanDocument(terms);
			IndexDocument(terms_clean);
			terms.clear();
			terms_clean.clear();
		}
		//checa se a linha deve ser lida
		if (IsTag(line) || flag_line) {
			buffer = GetLineContent(line);
		 	istringstream iss(buffer);
		 	copy(istream_iterator<string>(iss), istream_iterator<string>(), 
		 		back_inserter<vector<string> >(terms));
		}
	}

	map<string, Vocabulary>::iterator it_hash;
	map<int, ListCell>::iterator it_cel;

	//computa o idf de cada lista invertida e o peso de cada termo na lista invertida
	for (it_hash = inverted_index.begin(); it_hash != inverted_index.end(); ++it_hash){
		(*it_hash).second.idf = log2(NUM_TOTAL_COLECAO/(*it_hash).second.total_docs);

		for (it_cel = it_hash->second.inverted_list.begin(); it_cel != it_hash->second.inverted_list.end(); ++it_cel){
			(*it_cel).second.weight = (*it_hash).second.idf * (*it_cel).second.tf;
		}
	}
}

Parser::~Parser()
{
	cout << "Índice invertido finalizado!!" << endl;
}