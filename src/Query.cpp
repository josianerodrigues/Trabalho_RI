#include "Query.h"
#include "Parser.h"

int NUM_QUERY;
map<int, map<string, float> > matrix_weight;
map<string, Consulta> hash_query;
map<int, float> norm_collection;
vector<int> ranking;
Parser *parser = new Parser();
float acum_map = 0; //soma do map de todas as consultas
float acum_p10 = 0; //soma do p@10 de todas as consultas

Query::Query()
{
	cout << "Iniciando processamento das consultas!!" << endl;
}

void Query::CreateInvertedIndex()
{/*Essa função chama a função que ler a coleção (documento a documento) e gera o índice invertido*/	
	string files[6] = {"../cfc/cf74","../cfc/cf75","../cfc/cf76",
					   "../cfc/cf77","../cfc/cf78","../cfc/cf79"};

	parser->ReadColection(files[0]);
	parser->ReadColection(files[1]);
	parser->ReadColection(files[2]);
	parser->ReadColection(files[3]);
	parser->ReadColection(files[4]);
	parser->ReadColection(files[5]);

	parser->PrintHash();

	//delete parser;
}

void ComputeWeightQuery()
{/*Essa função calcula o peso dos termos de cada consulta*/

	map<string, Consulta>::iterator it;
	map<string, Vocabulary>::iterator it_hash;
	Vocabulary voc;
	string term;

	for (it = hash_query.begin(); it != hash_query.end(); it++) {
		term = (*it).first;

		//Caso não exista a palavra no hash o peso é zero
		if (inverted_index.find(term) == inverted_index.end()) {
			hash_query[term].weight = 0;
		} else {
			it_hash = inverted_index.find(term);
			voc = (*it_hash).second;
			//cout << "idf " << voc.idf << endl;
			(*it).second.weight = voc.idf * (*it).second.tf;
			hash_query[term].weight = (*it).second.weight;
		}
	}
}

void PrintRelevants(vector<int> relevantes)
{/*Essa função imprime todos os documentos relevantes da consulta*/
	vector<int>::iterator i;
	cout << "relevantes (" << relevantes.size() << "): ";

	for (i = relevantes.begin(); i != relevantes.end(); i++) {
		cout << (*i) << " ";
	}
	cout << endl << endl;
}

void PrintRanking(map<float, int> sim_total)
{/*Essa função ordena o vetor de ranking do mais similar ao menos similar e imprime na tela*/
	map<float, int>::iterator it_sim;
	map<float, int>::reverse_iterator rit_sim;
	vector<int>::iterator it;

	for (rit_sim = sim_total.rbegin(); rit_sim != sim_total.rend(); ++rit_sim) {
		//cout << "sim [" << rit_sim->second << "] =" << rit_sim->first;
		ranking.push_back(rit_sim->second);
	}

	cout << "Ranking da consulta (" << ranking.size() << "): ";

	//imprime documentos do ranking na tela
	for (it = ranking.begin(); it != ranking.end(); it++) {
		cout << (*it) << " ";
	}

	cout << endl <<  endl;
}

void ComputeSimilarityTotal(map<int, float> acum) {
	map<int, float>::iterator it_acum; //_colecao
	map<int, float>::iterator it_norm;
	int num_doc;
	map<float, int> sim_total;

	for (it_acum = acum.begin(); it_acum != acum.end(); it_acum++) {
		num_doc = it_acum->first;
		it_norm = norm_collection.find(num_doc);

		if (((*it_acum).second != 0) || ((*it_norm).second != 0))
			//sim_total[num_doc] = (*it_acum).second / (*it_norm).second;
			sim_total[(*it_acum).second / (*it_norm).second] = num_doc;
		else {
			//sim_total[num_doc] = 0.0;
			sim_total[0.0] = num_doc;
		}
	}
	PrintRanking(sim_total);
}

void ComputeSimilarity() {
	map<string, Consulta>::iterator it_cons;  //consulta
	map<string, Vocabulary>::iterator it_hash;  //hash
	map<int, ListCell>::iterator it_lista; //lista invertida
	map<int, float> acum; //acumulador
	float sim_parcial; //similaridade
	Vocabulary vocabulary;
	int num_doc;

	string termo_cons; //termo consulta

	for (it_cons = hash_query.begin(); it_cons != hash_query.end(); it_cons++) {
		termo_cons = (*it_cons).first;

		//Caso exista a palavra no hash
		if (inverted_index.find(termo_cons) != inverted_index.end()) {
			it_hash = inverted_index.find(termo_cons);
			vocabulary = (*it_hash).second;
			//cout << "\n" <<(*it_hash).first <<endl;
			for (it_lista = vocabulary.inverted_list.begin();
					it_lista != vocabulary.inverted_list.end(); ++it_lista) {
				num_doc = (*it_lista).first;

				sim_parcial = (*it_lista).second.weight * (*it_cons).second.weight; //peso do doc/termo * peso termo/consulta

				if (acum.find(num_doc) == acum.end()) {
					acum[num_doc] = sim_parcial;
				} else {
					acum[num_doc] += sim_parcial;
				}
			}
		}
	}
	ComputeSimilarityTotal(acum);
}

int BinarySearch(int qtd, vector<int> relevants, int element)
{/*Essa função verifica se o documento está na lista de documentos relevantes para a consulta, por meio da busca binária*/
	int start = 0, end = qtd - 1, medium;

	while (start <= end) {
		medium = (start + end) / 2;
		if (element == relevants[medium])
			return medium;
		else if (element < relevants[medium])
			end = medium - 1;
		else
			start = medium + 1;
	}
	return -1;
}

void ComputeMetrics(vector<int> relevantes)
{/*Essa função calcula as métricas de avaliação MAP e P@10 para cada consulta*/
	vector<int>::iterator it;
	vector<double>::iterator j;
	int indice, doc_atual, k;
	vector<int> docs_found;
	vector<double> recall, precision;

	//Quantidade de acertos
	double acerto = 0, p = 0;
	for (it = ranking.begin(); it != ranking.end(); it++) {
		doc_atual = *it;
		//indica posicao no relevantes[]
		indice = BinarySearch(relevantes.size(), relevantes, doc_atual);

		if (indice != -1) {
			docs_found.push_back(doc_atual);
			recall.push_back(((acerto + 1) / relevantes.size()) * 100);
			precision.push_back(((acerto + 1) / (p + 1)) * 100);
			acerto++;
		}
		p++;
	}

	cout << "Documentos relevantes encontrados (" << docs_found.size() << "): ";

	for (it = docs_found.begin(); it != docs_found.end(); it++) {
		cout << (*it) << " ";
	}
	cout << endl << endl;

	//Interpolação
	float PR_interpolada[11], maximo;
	int flag, i;

	// procura os 11 pontos de interpolacao da precisao
	for (i = 0; i < 11; i++)
		PR_interpolada[i] = 999;
		indice = acerto - 1;
	//ultimo ponto de revocacao
	if (recall[indice] == 100) {
		PR_interpolada[10] = precision[indice];
		indice--;
	} else
		PR_interpolada[10] = 0;
	for (i = 9; i >= 0; i--) {
		maximo = 999;
		while (i * 10 <= recall[indice] && indice >= 0) {
			if (maximo == 999)
				maximo = precision[indice];
			else if (maximo < precision[indice])
				maximo = precision[indice];
			indice--;
		}
		PR_interpolada[i] = maximo;
	}
	maximo = PR_interpolada[10];
	for (i = 9; i >= 0; i--)
		if (PR_interpolada[i] == 999)
			PR_interpolada[i] = maximo;
		else
			maximo = PR_interpolada[i];
	for (i = 0; i < 11; i++){
		if (PR_interpolada[i] < PR_interpolada[i+1])
		{
			PR_interpolada[i] = PR_interpolada[i+1];
		}
	}

	for(i=0;i<11;i++){
		if (PR_interpolada[i] < PR_interpolada[i+1]){
			PR_interpolada[i] = PR_interpolada[i+1];
		}
	    cout << "Revocacao = " << i*10 << "\%  " << " -> " << "Precisao = " << PR_interpolada[i] << "\%" << endl;
	}

	cout << endl;

	float map = 0;
	for (int i = 0; i < 11; ++i){
		map+= PR_interpolada[i];
	}

	acum_map += map/11;
	cout << "MAP interpolado = " << map/11 << "\%" << endl;

	float count_relevante = 0;
	for (int i = 0; i < 10; ++i){
		if(BinarySearch(relevantes.size(), relevantes, ranking[i]) != -1){
			count_relevante++;
		}
	}

	acum_p10 += (count_relevante/10)*100;
	cout << "P@10 = " << (count_relevante/10)*100 << "\%" << endl << endl;
}

void CalculationsQuery(vector<string> terms_query, vector<int> relevants, int qtd_relev)
{/*Essa função monta o hash da consulta e computa todos os valores necessarios para processar a consulta*/
	Consulta consulta;
	Consulta consulta_aux;
	vector<string>::iterator it;
	map<string, Consulta>::iterator it_cons;
	string term;

	for (it = terms_query.begin(); it != terms_query.end(); it++) {
		term = *it;

		if (hash_query.find(term) == hash_query.end()) { // nao existe
			consulta.tf = 1;
			consulta.num_query = NUM_QUERY;
			consulta.qtd_relevantes = qtd_relev;
			consulta.relevantes = relevants;
			hash_query[term] = consulta;
		} else {
			it_cons = hash_query.find(term);
			consulta = (*it_cons).second;
			consulta.tf++;
			hash_query[term] = consulta;
		}

	}

	cout << "----------------------------------- Consulta " << NUM_QUERY << 
	" -----------------------------------" << endl << endl;

	ComputeWeightQuery();

	PrintRelevants(relevants);

	ComputeSimilarity();

	ComputeMetrics(relevants);

	ranking.clear();
	hash_query.clear();
}


void Query::ProcessQuery() {
	string line;
	string arq_query =
			"../cfc/cfquery";
	ifstream arq(arq_query.c_str());
	int qtd_relev = 0;

	vector<string> consulta, consulta_clean;
	vector<int> relevantes;

	float map_final;
	float p10_final;

	if (!arq.is_open()) {
		cout << "erro ao abrir arquivo " << arq << endl;
	}

	for (int i = 0; i < 100; i++) {
		if (line.substr(0, 2).compare("QN") != 0) {
			getline(arq, line);
		}
		NUM_QUERY = atoi(line.substr(3, line.length()).c_str()); //numero da consulta

		getline(arq, line);
		while (line.substr(0, 2).compare("NR") != 0) {
			string buffer = parser->GetLineContent(line);
			istringstream iss(buffer);
			copy(istream_iterator<string>(iss), istream_iterator<string>(),
					back_inserter<vector<string> >(consulta));
			getline(arq, line);
		}

		qtd_relev = atoi(line.substr(3, line.length()).c_str());

		getline(arq, line);

		bool flag; //flag para manipular documentos relevantes
		line = line.substr(2, line.length());
		while ((line.substr(0, 2) != "QN") && (!arq.eof())) {
			flag = true;
			stringstream ss;
			ss << line;
			while (ss >> line) {
				if (flag) {
					relevantes.push_back(atoi(line.c_str()));
					flag = false;
				} else {
					flag = true;
				}
			}

			getline(arq, line);
		}

		consulta_clean = parser->CleanDocument(consulta);
		CalculationsQuery(consulta_clean, relevantes, qtd_relev);
		consulta.clear();
		consulta_clean.clear();
		relevantes.clear();
	}

	map_final = acum_map/100;
	p10_final = acum_p10/100;

	cout << "--------------------- Resultado final das métricas de avaliação ---------------------" << endl;
	cout << "Map final: " << map_final << "\%" << endl;
	cout << "P@10 final: " << p10_final << "\%" << endl << endl;
}

void Query::ComputeWeightCollection() {
	map<string, Vocabulary>::iterator iti; //termo
	map<int, ListCell>::iterator it_cel; //doc - lista invertida
	map<string, float> vetor_termo;
	map<int, map<string, float> >::iterator find_mat; //termo

	//int aux=0;
	for (iti = inverted_index.begin(); iti != inverted_index.end(); iti++) {
		for (it_cel = iti->second.inverted_list.begin();
				it_cel != iti->second.inverted_list.end(); it_cel++) {

			(*iti).second.idf = log2(NUM_TOTAL_COLECAO / (*iti).second.total_docs);
			(*it_cel).second.weight = (*iti).second.idf * (*it_cel).second.tf;

			inverted_index[(*iti).first].idf = (*iti).second.idf;

			if (matrix_weight.find((*it_cel).first) == matrix_weight.end()) {
				vetor_termo.clear();
				vetor_termo[(*iti).first] = (*iti).second.idf
						* (*it_cel).second.tf; //doc com seu peso
				matrix_weight[(*it_cel).first] = vetor_termo;
			} else {
				find_mat = matrix_weight.find(NUM_DOC);
				vetor_termo = (*find_mat).second;
				vetor_termo[(*iti).first] = (*iti).second.idf * (*it_cel).second.tf; //doc com seu peso
				matrix_weight[(*it_cel).first] = vetor_termo;
			}
		}
	}
}

void Query::ComputeNormCollection() {
	map<int, map<string, float> >::iterator i; //documento
	map<string, float>::iterator j; //termo
	float acum = 0;
	int aux = 0;
	for (i = matrix_weight.begin(); i != matrix_weight.end(); i++) {
		acum = 0;
		for (j = (*i).second.begin(); j != (*i).second.end(); j++) {
			acum += powf((*j).second, 2); //eleva ao quadrado e acumula
		}
		norm_collection[(*i).first] = sqrt(acum);	
	}
}

Query::~Query()
{
	cout << "Processamento das consultas finalizado!!" << endl;
}