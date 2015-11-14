#include "Parser.h"

Parser::Parser(){
	cout << "Começando o parser!!" << endl;
}

Parser::~Parser(){
	cout << "Finalizando parser!!" << endl;
}

int VerifyTag(string tag){
/*Essa função verifica se a tag lida é um dos campos a ser indexado*/

	if (tag == "AU" || tag == "TI" || tag == "AB" || tag == "EX"){
		return 1;
	}else if(tag == "  "){ //linhas que começam com espaço
		return 0;
	}else{
		return -1;
	}
}

unordered_map<double, string> ReadFileContent(string file_name){
/*Essa função ler o arquivo e extrai o conteúdo dos campos do
texto que serão indexados para compor o índice invertido*/

	ifstream arq(file_name.c_str());

	if (!arq.is_open()) {
		cout << "erro ao abrir arquivo " << arq << endl;
		//return NULL;
	}

	int flag_tag;
	string tag, line;
	double NUM_DOC;
	string doc_content;
	unordered_map<double, string> buffer;

	while(getline(arq, line)){

		tag = line.substr(0, 2); //retorna os dois primeiros caracteres da linha

		if(tag == "RN"){
			NUM_DOC = atof(line.substr(3).c_str());
			doc_content = "";
		}else if (VerifyTag(tag) == 1){
			doc_content.append(line.substr(3)); //substr: pega a linha do caracter da posição 3 até o final da linha
			doc_content.append(" "); //coloca um espaço entre uma linha e outra.

			do{
				getline(arq, line); // ler a proxima linha
				tag = line.substr(0,2);
				flag_tag = VerifyTag(tag);

				if (flag_tag == 1 || flag_tag == 0){
					doc_content.append(line.substr(3));
					doc_content.append(" ");
				}
			} while(flag_tag == 0 || flag_tag == 1);
		}
		if(doc_content != "")//faz essa verificação pq a primeira tag é PN e o conteudo do doc vai retornar fazio.
			buffer[NUM_DOC] = doc_content;
	}

	// unordered_map<string, string>::iterator iti = buffer.begin();

	// for (iti; iti != buffer.end(); iti++){
	// 	cout << iti->first << ": " << iti->second << endl;
	// }
	cout << "Numero de documento processados: " << buffer.size() << endl;

	return buffer;		
}

bool IsCaracter(char c) {
/*Essa função verifica a existência digitios e caracteres especiais*/

	if (isdigit(c)) {
		return true;
	}

	switch (c) {
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
	case '\\':
	case ' ':
	case ';':
		return true;
	default:
		return false;
	}
}

vector<string> CleanDocument(vector<string> terms){
/*Essa função remove todos os termos que são stopwords, dígitos e caracteres especiais*/
	vector<string>::iterator it;
	vector<string> terms_clean;

	ifstream stopword_file;
	stopword_file.open("stopwords_en.txt");
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
			if (stopwords.find(*it) == stopwords.end()){//insere no vector caso não seja uma stopword
				terms_clean.push_back(*it);
			}					
		}
	}

	 // for (int i = 0; i < terms_clean.size(); ++i) {
	 // 	cout << terms_clean[i] << endl;
	 // }

	return terms_clean;
}

void Parser::IndexDocument(double id_doc, vector<string> terms_clean){

	InvertedList inverted_list;
	InvertedList list_aux;
	ListCell cell;
	ListCell cell_aux;

	vector<string>::iterator it_term; //para percorrer os termos de cada documento
	unordered_map<string, InvertedList>::iterator it_hash;//para percorrer o hash
	unordered_map<double,ListCell>::iterator it_cell;

//preenche o hash com os termos e cria suas listas invertidas
	for (it_term = terms_clean.begin(); it_term != terms_clean.end(); ++it_term) {
		inverted_list.total_docs = 0;
		list_aux.total_docs = 0;
		cell.tf = 0;
		cell_aux.tf = 0;
		inverted_list.idf = 0;
		list_aux.idf = 0;
		cell.weight = 0;
		cell_aux.weight = 0;			

		if (inverted_index.find(*it_term) == inverted_index.end()) {//o termo não existe no hash
			//caso o termo não exista no hash, insere no hash e incrementa a frequencia e o total de docs em que o termo aparece
			inverted_list.total_docs = 1;
			cell.tf = 1;
			inverted_list.idf = log2(NUM_TOTAL_COLECAO/inverted_list.total_docs);
			cell.weight = cell.tf * inverted_list.idf;
			inverted_list.lista[id_doc] = cell;
			inverted_index[*it_term] = inverted_list;

			//cout << *it_term << ":" << id_doc << endl;

		}else{ //o termo já existe no hash
			it_hash = inverted_index.find(*it_term); //pega posição onde se encontra o termo
			list_aux = (*it_hash).second; //pega lista invertida do termo				
			
			if (list_aux.lista.find(id_doc) == list_aux.lista.end()) //verifica se o documento já existe na lista invertida
			{//se não tiver: insere uma nova celula na lista invertida com os valores correspondentes
				list_aux.total_docs++;
				cell.tf = 1;
				list_aux.idf = log2(NUM_TOTAL_COLECAO/list_aux.total_docs);
				cell.weight = cell.tf * list_aux.idf; 
				list_aux.lista[id_doc] = cell;
				//cout << *it_term << ":" << id_doc << endl;
								 
			}else{//se já tiver: só incrementa o tf e atualiza o peso e idf
				it_cell = list_aux.lista.find(id_doc);
				cell_aux = (*it_cell).second;
				cell_aux.tf++;
				//list_aux.idf = log2(NUM_TOTAL_COLECAO / list_aux.total_docs);
				cell_aux.weight = cell_aux.tf * list_aux.idf;
				list_aux.lista[id_doc] = cell_aux;
				//cout << *it_term << ":" << id_doc << endl;		
			}
			inverted_index[*it_term] = list_aux;
		}				
	}

}

void Parser::PrintHash()
{
	unordered_map<string, InvertedList>::iterator iti;
	unordered_map<double,ListCell>::iterator it_list;

	for (iti = inverted_index.begin(); iti != inverted_index.end(); ++iti)
	{
		cout << iti->first << "-> " << "total de docs: " << iti->second.total_docs << "-> " << endl;

		for (it_list = iti->second.lista.begin(); it_list != iti->second.lista.end(); ++it_list)
		{
			cout << it_list->first << ":" << it_list->second.tf << " | " ;
		}

		cout << endl;
	}
}


void Parser::CreateInvertedIndex(string file_name){

	string doc_content;
	unordered_map<double, string> buffer;
	vector<string> terms, terms_clean;

	buffer = ReadFileContent(file_name); //recebe o map com o conteúdo de todos os documentos

	unordered_map<double,string>::iterator it = buffer.begin();

	for(it; it != buffer.end(); it++) // esse loop pega todo o conteúdo e separa em termos preenchendo o vetor de termos
	{
		double id_doc = it->first;

		doc_content = it->second;
		istringstream iss(doc_content);
		copy(istream_iterator<string>(iss), istream_iterator<string>(), // função split
			back_inserter<vector<string> >(terms));

		terms_clean = CleanDocument(terms); //retira as palavras que são stopwords, caractere especiais e digitos
		IndexDocument(id_doc, terms_clean);//indexa documento
		terms.clear();
		terms_clean.clear();
	}

	// unordered_map<string, vector<string>>::iterator d = documents.begin();
	// vector<string>::iterator v;

	// for (d; d != documents.end(); ++d)
	// {
	// 	cout << d->first << "-> ";
	// 	for (v = d->second.begin(); v != d->second.end(); ++v)
	// 	{
	// 		cout << *v << ", ";
	// 	}
	// 	cout <<  endl;
	// }
}

void Parser::ReadColection(){
	string file_name;

	for (int i = 4; i < 10; ++i)
	{
		file_name = "./cfc/cf7";
		file_name+= to_string(i);
		CreateInvertedIndex(file_name);
		//cout << file_name << endl;
	}
}

