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

unordered_map<string, string> ReadFileContent(){
/*Essa função ler o arquivo e extrai o conteúdo dos campos do
texto que serão indexados para compor o índice invertido*/

	ifstream arq;
	arq.open("cfc/cf76");

	int flag_tag;
	string tag, line, NUM_DOC;
	string doc_content;
	unordered_map<string, string> buffer;

	while(getline(arq, line)){

		tag = line.substr(0, 2); //retorna os dois primeiros caracteres da linha

		if(tag == "RN"){
			NUM_DOC = line.substr(3);
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

	//unordered_map<string, string>::iterator iti = buffer.begin();

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
	case '-':
	case '<':
	case '>':
	case '=':
	case '&':
	case '?':
	case '/':
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

unordered_map<string, InvertedList> Parser::CreateInvertedIndex(){

	string doc_content;
	unordered_map<string, string> buffer;
	vector<string> terms, terms_clean;
	unordered_map<string, vector<string>> documents;
	
	unordered_map<string, InvertedList> inverted_index; //hash do indice invertido
	InvertedList inverted_list;
	ListCell cell;

	buffer = ReadFileContent(); //recebe o map com o conteúdo de todos os documentos

	unordered_map<string,string>::iterator it = buffer.begin();

	for(it; it != buffer.end(); it++) // esse loop pega todo o conteúdo e separa em termos preenchendo o vetor de termos
	{
		doc_content = it->second;
		istringstream iss(doc_content);
		copy(istream_iterator<string>(iss), istream_iterator<string>(), // função split
			back_inserter<vector<string> >(terms));

		terms_clean = CleanDocument(terms); //retira as palavras que são stopwords, caractere especiais e digitos
		documents[it->first] = terms_clean; //preenche o hash com os ids dos documentos (RN)
									  //com seus respectivos termos limpos
	}

	unordered_map<string, vector<string>>::iterator iti;//pra percorrer os documentos
	vector<string>::iterator it_term; //para percorrer os termos de cada documento
	unordered_map<string, InvertedList>::iterator it_hash;//para percorrer o hash


	//preenche o hash com os termos e cria suas listas invertidas
	for (iti = documents.begin(); iti != documents.end(); iti++){

		for (it_term = iti->second.begin(); it_term != iti->second.end(); ++it_term) {
			cell.id_doc = iti->first;

			if (inverted_index.find(*it_term) == inverted_index.end()) {//o termo não existe no hash
				//caso o termo não exista no hash, insere no hash e incrementa a frequencia e o total de docs em que o termo aparece
				inverted_list.total_docs = 1;
				cell.tf = 1;
				inverted_list.idf = log2(NUM_TOTAL_COLECAO/inverted_list.total_docs);
				cell.weight = cell.tf*inverted_list.idf;
				inverted_list.lista.push(cell);
				inverted_index[*it_term] = inverted_list;
			}else{ //o termo já existe no hash
				it_hash = inverted_index.find(*it_term); //pega posição onde se encontra o termo
				InvertedList list_aux = (*it_hash).second; //pega lista invertida do termo
				ListCell cell_aux = list_aux.lista.top(); //pega o primeiro valor da lista/pilha
				if (cell_aux.id_doc == iti->first) //verifica se o documento já existe na lista invertida
				{//se já tiver: só incrementa o tf e atualiza o peso
					cell_aux.tf++;
					cell_aux.weight = cell_aux.tf * list_aux.idf;					 
				}else{//e não tiver: insere uma nova celula na lista invertida com os valores correspondentes
					list_aux.total_docs++;
					cell.tf = 1;
					list_aux.idf = log2(NUM_TOTAL_COLECAO/inverted_list.total_docs);
					cell_aux.weight = cell.tf * list_aux.idf; 
					list_aux.lista.push(cell);					
				}
			}				
		}
	}

	return inverted_index;
}