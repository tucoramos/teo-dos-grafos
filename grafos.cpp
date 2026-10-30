#include "grafos.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <string>
#include <stack>
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;
// Metodo que chama a implementação da inicialização do grafo dependendo do tipo de grafo
Grafo::Grafo(int tipoGrafo, ifstream& arquivo) : tipo(tipoGrafo) {
    if (tipo != 0 && tipo != 1) {
        throw invalid_argument("Tipo de grafo invalido. Use 0 para lista de adjacencia ou 1 para matriz de adjacencia.");
    }
    lerEntrada(arquivo);
}

// A leitura e a validacao sao identicas para as duas representacoes.
void Grafo::lerEntrada(ifstream& arquivo) {
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo");
    }
    string linha;
    if (!getline(arquivo, linha)) {
        if (arquivo.bad()) throw ios_base::failure("Erro ao ler o cabecalho");
        throw invalid_argument("Cabecalho ausente: informe o numero de vertices");
    }
    istringstream cabecalho(linha);
    string extra;
    if (!(cabecalho >> numeroDeVertices) || numeroDeVertices < 1 ||
        numeroDeVertices == numeric_limits<int>::max() || (cabecalho >> extra)) {
        throw invalid_argument("Cabecalho invalido: informe apenas um inteiro n, com 1 <= n < INT_MAX");
    }

    if (tipo == 0) {
        listaAdjacencia.reset(new ListaAdjacencia(numeroDeVertices));
    } else {
        matrizAdjacencia.assign(numeroDeVertices, vector<bool>(numeroDeVertices, false));
    }
    size_t numeroLinha = 1;
    while (getline(arquivo, linha)) {
        ++numeroLinha;
        if (linha.find_first_not_of(" \t\r\n\f\v") == string::npos) continue;
        istringstream campos(linha);
        int vertice1, vertice2;
        if (!(campos >> vertice1 >> vertice2) || (campos >> extra)) {
            throw invalid_argument("Aresta malformada na linha " + to_string(numeroLinha));
        }
        if (vertice1 < 1 || vertice1 > numeroDeVertices ||
            vertice2 < 1 || vertice2 > numeroDeVertices) {
            throw invalid_argument("Vertice fora do intervalo 1..n na linha " + to_string(numeroLinha));
        }
        if (numeroDeArestas == numeric_limits<int>::max()) {
            throw overflow_error("Numero de arestas excede o limite de int");
        }
        if (tipo == 0) {
            listaAdjacencia->adicionarAresta(vertice1, vertice2);
        } else {
            matrizAdjacencia[vertice1-1][vertice2-1] = true;
            matrizAdjacencia[vertice2-1][vertice1-1] = true;
        }
        numeroDeArestas++;
    }
    if (arquivo.bad() || !arquivo.eof()) {
        throw ios_base::failure("Erro durante a leitura das arestas");
    }
}

void Grafo::validarVertice(int vertice) const {
    if (vertice < 1 || vertice > numeroDeVertices) {
        throw invalid_argument("Vertice fora do intervalo 1..n");
    }
}

void Grafo::validarBusca(int vertice, int representacao) const {
    validarVertice(vertice);
    if (tipo != representacao) {
        throw logic_error("A busca solicitada nao corresponde a representacao do grafo");
    }
}

// Metodo que chama a implementação da saída do grafo dependendo do tipo de grafo
ifstream Grafo::saidaGrafo(){
    if (tipo ==0){
        return saidaLista();
    } else {
        return saidaMatriz();
    }
} 

ifstream Grafo::saidaMatriz(){
    // Cria um arquivo de saída para a matriz de adjacência
    string nomeArquivo = "saida_matriz.txt";
    ofstream arquivo(nomeArquivo);

    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    arquivo << "Dados sobre o grafo:\n";
    arquivo << "Formato salvo na memoria: matriz de Adjacencia\n";

    //numero de vertices,
    arquivo << "Numero de vertices: " << numeroDeVertices << endl;

    //numero de arestas,
/*    int numeroDeArestas = 0;
    for (int i = 0; i < numeroDeVertices; ++i) {
        for (int j = i + 1; j < numeroDeVertices; ++j) {
            if (matrizAdjacencia[i][j]) {
                numeroDeArestas++;
            }
        }
    }
*/

    arquivo << "Numero de arestas: " << numeroDeArestas << endl;

    vector<int> graus(numeroDeVertices, 0);
    for (int i = 0; i < numeroDeVertices; ++i) {
        for (int j = 0; j < numeroDeVertices; ++j) {
            if (matrizAdjacencia[i][j]) {
                graus[i]++;
            }
        }
    }

    //grau minimo,
    int grauMinimo = *min_element(graus.begin(), graus.end());
    arquivo << "Grau minimo: " << grauMinimo << endl;

    //grau maximo, 
    int grauMaximo = *max_element(graus.begin(), graus.end());
    arquivo << "Grau maximo: " << grauMaximo << endl;
    
    //grau medio,
    double grauMedio = 0;
    for (int grau : graus) {
        grauMedio += grau;
    }
    grauMedio /= numeroDeVertices;
    arquivo << "Grau medio: " << grauMedio << endl;

    //mediana de grau
    sort(graus.begin(), graus.end());
    double mediana;
    if (numeroDeVertices % 2 == 0) {
        mediana = (graus[numeroDeVertices / 2 - 1] + graus[numeroDeVertices / 2]) / 2.0;
    } else {
        mediana = graus[numeroDeVertices / 2];
    }
    arquivo << "Mediana de grau: " << mediana << endl;

    //diametro do grafo,
    int diametro = diametroMatriz();
    if (diametro == -1) {
        arquivo << "O grafo não é conexo." << endl;
    } else {
        arquivo << "Diametro do grafo: " << diametro << endl;
    }

    //diametro aproximado do grafo,
    int diametroAproximado = diametroAproximadoMatriz();
    if (diametroAproximado == -1) {
        arquivo << "O grafo não é conexo [diametro aproximado]." << endl;
    } else {
        arquivo << "Diametro aproximado do grafo: " << diametroAproximado << endl;
    }

    //numero de componentes conexas,
    vector<vector<int>> componentes = componentesConexasMatriz();
    size_t tam = componentes.size();
    arquivo << "Numero de componentes conexas: " << tam << endl;
    
    for (size_t i = 0; i < tam; ++i) {
        arquivo << "Componente " << i + 1 << " (tamanho: " << componentes[i].size() << "): \n   [";
        for (int vertice : componentes[i]) {
            arquivo << vertice << " "; 
        }
        arquivo << "]\n";
    }

    arquivo.close();
    return ifstream(nomeArquivo);
}

// Implementação da iformações do grau dos vértices usando lista de adjacência
void Grafo::infosGrauLista(ostream& arquivo){
    vector<int> graus(numeroDeVertices + 1, 0);

    // Calcula o grau de cada vértice
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        No* n = listaAdjacencia->estrutura[i];
        while (n != nullptr) {
            graus[i]++;
            n = n->prox;
        }
    }

    // Acha o grau mínimo, máximo
    if (numeroDeVertices == 0) {
        arquivo << "Grau minimo: 0" << endl;
        arquivo << "Grau maximo: 0" << endl;
        arquivo << "Grau medio: 0" << endl;
        arquivo << "Mediana de grau: 0" << endl;
        return;
    }
    int grauMinimo = *min_element(graus.begin() + 1, graus.end());
    int grauMaximo = *max_element(graus.begin() + 1, graus.end());

    //Soma todos os graus e divide pelo número de vértices para achar o grau médio
    double grauMedio = 0;
    for (int grau : graus) {
        grauMedio += grau;
    }
    grauMedio /= numeroDeVertices;

    // Ordena todos os graus e calcula a mediana
    sort(graus.begin() + 1, graus.end());
    double mediana;
    if (numeroDeVertices % 2 == 0) {
        mediana = (graus[numeroDeVertices / 2] + graus[numeroDeVertices / 2 + 1]) / 2.0;
    } else {
        mediana = graus[numeroDeVertices / 2 + 1];
    }

    // Escreve as informações no arquivo de saída
    arquivo << "Grau minimo: " << grauMinimo << endl;
    arquivo << "Grau maximo: " << grauMaximo << endl;
    arquivo << "Grau medio: " << grauMedio << endl;
    arquivo << "Mediana de grau: " << mediana << endl;
}

// Implementação do calculo do diâmetro do grafo usando lista de adjacência
void Grafo::diametroLista(ostream& arquivo){
    // Realiza uma busca em largura (BFS) a partir do vértice 1 para calcular a distância mínima de cada vértice até o vértice 1
    ArvoreBusca primeira = implementacaoBFSLista(1);

    for (int i = 1; i < numeroDeVertices + 1; i++) {
        // grafo desconexo: diametro infinito
        if (primeira.nivel[i] == -1){
            // Se algum vértice não for alcançável, o grafo não é conexo
                arquivo << "O grafo não é conexo." << endl;
                return; // Retorna vazio para indicar que o grafo não é conexo
        }  
    }

    int diametro = primeira.nivelMaximo;

    for (int i = 2; i < numeroDeVertices + 1; ++i) {
        // Para cada vértice, realiza uma busca em largura (BFS) para calcular a distância minima dele até os outros vértices
        auto resultadoBFS = implementacaoBFSLista(i);
        // Atualiza o diâmetro se a distância máxima encontrada na BFS for maior que o diâmetro atual
        diametro = max(diametro, resultadoBFS.nivelMaximo);     
    }

    // retorna o diâmetro do grafo, que é a maior distância mínima entre quaisquer dois vértices
    arquivo << "Diametro do grafo: " << diametro << endl;
}

// Implementação do calculo do diâmetro aproximado do grafo usando lista de adjacência
void Grafo::diametroAproximadoLista(ostream& arquivo){
    
    ArvoreBusca primeira = implementacaoBFSLista(1);

    for (int i = 1; i < numeroDeVertices + 1; i++) {
        // grafo desconexo: diametro infinito
        if (primeira.nivel[i] == -1){
            // Se algum vértice não for alcançável, o grafo não é conexo
                arquivo << "O grafo não é conexo [diametro aproximado]." << endl;
                return; // Retorna vazio para indicar que o grafo não é conexo
        }  
    }

    ArvoreBusca distante = implementacaoBFSLista(primeira.maisDistante);
    arquivo << "Diametro aproximado do grafo: " << distante.nivelMaximo << endl;
}

// Implementação da contagem de componentes conexas usando lista de adjacência
void Grafo::componentesConexasLista(ostream& arquivo){
    // valores que armazenam resultados
    vector<pair<int,vector<int>>> resultado;
    int numeroComponetes = 0;
    int numeroElementos = 0;
    vector<bool> visitado(numeroDeVertices + 1, false);

    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        if(!visitado[i]){
            // Se o vértice não foi visitado, então encontramos uma nova componente conexa
            numeroComponetes++;
            vector<int> componente;
            numeroElementos = 0;

            //Ja que encontramos uma nova componente conexa, realizamos uma BFS para encontrar todos os vértices dessa componente
            auto resultadoBFS = implementacaoBFSLista(i);

            // Itera sobre todos os vértices e marca como visitados aqueles que foram alcançados na BFS, adicionando-os à componente atual
            for (int j = 1; j < numeroDeVertices + 1; ++j) {
                if (resultadoBFS.nivel[j] != -1) {
                    visitado[j] = true;
                    componente.push_back(j);
                    numeroElementos++;
                }
            }

            // Adiciona a componente atual e seu tamanho ao resultado
            resultado.push_back(make_pair(numeroElementos,componente));
        }
    }

    // Ordena as componentes conexas pelo tamanho em ordem decrescente
    sort(resultado.begin(),resultado.end(),[](const pair<int,vector<int>>& a,const pair<int,vector<int>>& b){
        return a.first > b.first;
    });

    // Escreve o número de componentes conexas no arquivo de saída
    arquivo << "Numero de componentes conexas: " << numeroComponetes << endl;

    // Itera e escreve cada componente conexa e seus vértices no arquivo de saída
    for (size_t i = 0; i < resultado.size(); i++)
    {
        arquivo << "Componente " << i + 1 << " (tamanho: " << resultado[i].first << "): \n   [";
        for (int vertice : resultado[i].second) {
            arquivo << vertice << " ";
        }
        arquivo << "]\n";
    }
}

// Implementação da saída padrão do grafo usando lista de adjacência
ifstream Grafo::saidaLista(){
    // Cria um arquivo de saída para a lista de adjacência
    string nomeArquivo = "saida_lista.txt";
    ofstream arquivo(nomeArquivo);


    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }


    arquivo << "Dados sobre o grafo:\n";
    arquivo << "Formato salvo na memoria: lista de Adjacencia\n";

    //numero de vertices,
    arquivo << "Numero de vertices: " << numeroDeVertices << endl;

    //numero de arestas,
/*    int numeroDeArestas = 0;
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        No* n = listaAdjacencia->estrutura[i];
        while (n != nullptr) {
            numeroDeArestas++;
            n = n->prox;
        }
    }
*/
    arquivo << "Numero de arestas: " << numeroDeArestas << endl;

    //Infos sobre grau dos vertices
    infosGrauLista(arquivo);

    //diametro do grafo,
    diametroLista(arquivo);

    //diametro aproximado do grafo,
    diametroAproximadoLista(arquivo);

    //numero de componentes conexas,
    componentesConexasLista(arquivo);

    arquivo.close();
    return ifstream(nomeArquivo);
}

// Metodo que chama a implementação da BFS dependendo do tipo de grafo
ifstream Grafo::bfs(int vertice){
    validarVertice(vertice);
    if (tipo ==0){
        return bfsLista(vertice);
    } else {
        return bfsMatriz(vertice);
    }
}

ArvoreBusca Grafo::implementacaoBFSMatriz(int vertice) {
    validarBusca(vertice, 1);
    ArvoreBusca arv;
    arv.raiz = vertice;
    arv.pai.assign(numeroDeVertices + 1, -1);
    arv.nivel.assign(numeroDeVertices + 1, -1);
    arv.maisDistante = vertice;
    queue<int> fila;
    fila.push(vertice);
    arv.nivel[vertice] = 0;
    arv.pai[vertice] = 0;

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();
        for (int vizinho = 1; vizinho <= numeroDeVertices; ++vizinho) {
            if (matrizAdjacencia[atual-1][vizinho-1] && arv.nivel[vizinho] == -1) {
                arv.pai[vizinho] = atual;
                arv.nivel[vizinho] = arv.nivel[atual] + 1;
                if (arv.nivel[vizinho] > arv.nivelMaximo ||
                    (arv.nivel[vizinho] == arv.nivelMaximo && vizinho < arv.maisDistante)) {
                    arv.nivelMaximo = arv.nivel[vizinho];
                    arv.maisDistante = vizinho;
                }
                fila.push(vizinho);
            }
        }
    }
    return arv;
}

ArvoreBusca Grafo::implementacaoDFSMatriz(int vertice) {
    validarBusca(vertice, 1);
    ArvoreBusca arv;
    arv.raiz = vertice;
    arv.pai.assign(numeroDeVertices + 1, -1);
    arv.nivel.assign(numeroDeVertices + 1, -1);
    arv.maisDistante = vertice;
    vector<int> proximo(numeroDeVertices + 1, 1);
    stack<int> pilha;
    pilha.push(vertice);
    arv.pai[vertice] = 0;
    arv.nivel[vertice] = 0;

    while (!pilha.empty()) {
        int atual = pilha.top();
        int& candidato = proximo[atual];
        while (candidato <= numeroDeVertices &&
               (!matrizAdjacencia[atual-1][candidato-1] || arv.nivel[candidato] != -1)) {
            ++candidato;
        }
        if (candidato > numeroDeVertices) {
            pilha.pop();
            continue;
        }
        int vizinho = candidato++;
        arv.pai[vizinho] = atual;
        arv.nivel[vizinho] = arv.nivel[atual] + 1;
        if (arv.nivel[vizinho] > arv.nivelMaximo ||
            (arv.nivel[vizinho] == arv.nivelMaximo && vizinho < arv.maisDistante)) {
            arv.nivelMaximo = arv.nivel[vizinho];
            arv.maisDistante = vizinho;
        }
        pilha.push(vizinho);
    }
    return arv;
}

int Grafo::diametroMatriz() {
    ArvoreBusca primeira = implementacaoBFSMatriz(1);

    for (int i = 1; i <= numeroDeVertices; i++) {
        if (primeira.nivel[i] == -1) return -1;   // grafo desconexo: diametro infinito
    }

    int diametro = primeira.nivelMaximo;
    for (int i = 2; i <= numeroDeVertices; i++) {      // o vertice 1 ja foi feito
        int temp = implementacaoBFSMatriz(i).nivelMaximo;
        if (diametro < temp) {
            diametro = temp;
        }
    }
    return diametro;
}

int Grafo::diametroAproximadoMatriz(){
    ArvoreBusca primeira = implementacaoBFSMatriz(1);

    for (int i = 1; i <= numeroDeVertices; i++) {
        if (primeira.nivel[i] == -1) return -1;   // grafo desconexo: diametro infinito
    }

    ArvoreBusca distante = implementacaoBFSMatriz(primeira.maisDistante);
    return distante.nivelMaximo;
}

vector<vector<int>> Grafo::componentesConexasMatriz() {
    vector<bool> visitado(numeroDeVertices, false);
    vector<vector<int>> componentes;

    for (int raiz = 1; raiz <= numeroDeVertices; raiz++) {
        if (visitado[raiz-1]) continue;        // ja pertence a uma componente anterior

        ArvoreBusca arv = implementacaoBFSMatriz(raiz);     // alcanca exatamente a componente de raiz

        vector<int> vertices;
        for (int i = 1; i <= numeroDeVertices; i++) {
            if (arv.nivel[i] != -1) {          // nivel != -1 == foi alcancado
                visitado[i-1] = true;
                vertices.push_back(i);
            }
        }
        componentes.push_back(move(vertices));
    }

    // ordem decrescente de tamanho, como pede o enunciado
    sort(componentes.begin(), componentes.end(),
         [](const vector<int>& a, const vector<int>& b) { return a.size() > b.size(); });

    return componentes;
}

// Implementação da saida da BFS usando lista de adjacência
ifstream Grafo::bfsLista(int vertice){
    // Implementação da saída da BFS
    if(vertice < 1 || vertice > numeroDeVertices) {
        throw invalid_argument("Vertice inicial invalido");
    }

    string nomeArquivo = "bfs_lista.txt";
    ofstream arquivo(nomeArquivo);


    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    // Chama a implementação da BFS e recebe os vetores de pai e nível
    ArvoreBusca arv = implementacaoBFSLista(vertice);

    // Escreve os resultados da BFS no arquivo de saída
    arquivo << "BFS a partir do vertice: " << vertice << endl;
    arquivo << "Vertice | Pai | Nivel\n";
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        arquivo << i << " | " << arv.pai[i] << " | " << arv.nivel[i] << "\n";
    }
    arquivo.close();

    // Retorna o arquivo de saída em modo de leitura
    return ifstream(nomeArquivo);
}

ifstream Grafo::bfsMatriz(int vertice){
    if(vertice < 1 || vertice > numeroDeVertices) {
        throw invalid_argument("Vertice inicial invalido");
    }
    string nomeArquivo = "bfs_matriz.txt";
    ofstream arquivoSaida(nomeArquivo);

    if (!arquivoSaida.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    ArvoreBusca arv = implementacaoBFSMatriz(vertice);
    
    arquivoSaida << "BFS a partir do vertice: " << vertice << "\n";
    arquivoSaida << "Vertice | Pai | Nivel\n";

    for (int i = 1; i <= numeroDeVertices; i++) {
        arquivoSaida << i << " | " << arv.pai[i] << " | " << arv.nivel[i] << "\n";
    }

    arquivoSaida.close();
    return ifstream(nomeArquivo);
}

// Implementação da busca em largura (BFS) usando lista de adjacência
ArvoreBusca Grafo::implementacaoBFSLista(int vertice) {
    validarBusca(vertice, 0);
    // Guarda o estado de cada vértice (visitado ou não) 
    vector<int> visitado(numeroDeVertices + 1, 0);

    // Guarda o nível de cada vértice
    vector<int> nivel(numeroDeVertices + 1, -1);

    // Guarda o pai de cada vértice
    vector<int> pai(numeroDeVertices + 1, -1);

    // Fila para a BFS
    queue<int> fila;

    // Inicializa o vértice inicial
    visitado[vertice] = 1;
    nivel[vertice] = 0;
    pai[vertice] = 0;
    fila.push(vertice);

    int nivelMaximo = 0;
    int maisDistante = vertice;

    // Enquanto a fila não estiver vazia, continue a BFS
    while (!fila.empty()) {

        int atual = fila.front();
        fila.pop();

        No* n = listaAdjacencia->estrutura[atual];

        while (n != nullptr) {

            if (!visitado[n->valor]) {

                visitado[n->valor] = 1;
                nivel[n->valor] = nivel[atual] + 1;
                pai[n->valor] = atual;

                if (nivel[n->valor] > nivelMaximo ||
                    (nivel[n->valor] == nivelMaximo && n->valor < maisDistante)) {
                    nivelMaximo = nivel[n->valor];
                    maisDistante = n->valor;
                }

                fila.push(n->valor);
            }

            n = n->prox;
        }
    }

    return ArvoreBusca{
        vertice,
        pai,
        nivel,
        nivelMaximo,
        maisDistante
    };
}

// Metodo que chama a implementação da DFS dependendo do tipo de grafo
ifstream Grafo::dfs(int vertice){
    validarVertice(vertice);
    if (tipo ==0){
        return dfsLista(vertice);
    } else {
        return dfsMatriz(vertice);
    }
} 

// Implementação da saida da DFS usando lista de adjacência
ifstream Grafo::dfsLista(int vertice){
    // Checa se o vértice inicial é válido
    if(vertice < 1 || vertice > numeroDeVertices) {
        throw invalid_argument("Vertice inicial invalido");
    }
    // Implementação da saída da DFS
    string nomeArquivo = "dfs_lista.txt";
    ofstream arquivo(nomeArquivo);


    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    // Chama a implementação da DFS e recebe os vetores de pai e nívelq
    ArvoreBusca arv = implementacaoDFSLista(vertice);

    // Escreve os resultados da DFS no arquivo de saída
    arquivo << "DFS a partir do vertice: " << vertice << endl;
    arquivo << "Vertice | Pai | Nivel\n";
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        arquivo << i << " | " << arv.pai[i] << " | " << arv.nivel[i] << "\n";
    }
    arquivo.close();

    // Retorna o arquivo de saída em modo de leitura
    return ifstream(nomeArquivo);
}

ifstream Grafo::dfsMatriz(int vertice){
    // Checa se o vértice inicial é válido
    if(vertice < 1 || vertice > numeroDeVertices) {
        throw invalid_argument("Vertice inicial invalido");
    }
    string nomeArquivo = "dfs_matriz.txt";
    ofstream arquivoSaida(nomeArquivo);

    if (!arquivoSaida.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    ArvoreBusca arv = implementacaoDFSMatriz(vertice);
    
    arquivoSaida << "DFS a partir do vertice: " << vertice << "\n";
    arquivoSaida << "Vertice | Pai | Nivel\n";

    for (int i = 1; i <= numeroDeVertices; i++) {
        arquivoSaida << i << " | " << arv.pai[i] << " | " << arv.nivel[i] << "\n";
    }

    arquivoSaida.close();
    return ifstream(nomeArquivo);
}

// Implementação da busca em profundidade (DFS) usando lista de adjacência
ArvoreBusca Grafo::implementacaoDFSLista(int vertice){
    validarBusca(vertice, 0);

    // Guarda o estado de cada vértice (visitado ou não) 
    vector<int> visitado(numeroDeVertices + 1, 0);

    // Guarda o nível de cada vértice
    vector<int> nivel(numeroDeVertices + 1, -1);

    // Guarda o pai de cada vértice
    vector<int> pai(numeroDeVertices + 1, -1);

    // Pilha para a DFS
    stack<int> pilha;

    pilha.push(vertice);
    visitado[vertice] = 1;
    nivel[vertice] = 0;
    pai[vertice] = 0;

    int nivelMaximo = 0;
    int maisDistante = vertice;

    vector<No*> proximo = listaAdjacencia->estrutura;

    while (!pilha.empty()) {

        int atual = pilha.top();
        No* n = proximo[atual];

        // Pula os vizinhos que já foram visitados
        while (n != nullptr && visitado[n->valor]) {
            n = n->prox;
        }

        // Terminou todos os vizinhos: volta ao vértice anterior
        if (n == nullptr) {
            pilha.pop();
            continue;
        }

        // Guarda onde retomar quando voltar para este vértice
        proximo[atual] = n->prox;

        int vizinho = n->valor;

        visitado[vizinho] = 1;
        pai[vizinho] = atual;
        nivel[vizinho] = nivel[atual] + 1;

        if (nivel[vizinho] > nivelMaximo ||
            (nivel[vizinho] == nivelMaximo && vizinho < maisDistante)) {
            nivelMaximo = nivel[vizinho];
            maisDistante = vizinho;
        }

        // Explora esse vizinho antes de continuar os demais
        pilha.push(vizinho);
    }

    return ArvoreBusca{
        vertice,
        pai,
        nivel,
        nivelMaximo,
        maisDistante
    };
}

// Metodo que chama a implementação da distância entre dois vértices dependendo do tipo de grafo
ifstream Grafo::distancia(int vertice1, int vertice2){
    validarVertice(vertice1);
    validarVertice(vertice2);
    if (tipo ==0){
        return distanciaLista(vertice1, vertice2);
    } else {
        return distanciaMatriz(vertice1, vertice2);
    }
}

ifstream Grafo::distanciaLista(int vertice1, int vertice2){
    // Implementação da saída da distância entre dois vértices
    string nomeArquivo = "distancia_lista.txt";
    ofstream arquivo(nomeArquivo);

    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    // Chama a implementação da BFS e recebe os vetores de pai e nível
    auto resultado = implementacaoBFSLista(vertice1);

    // Verifica se o vértice2 foi alcançado
    if (resultado.nivel[vertice2] == -1) {
        arquivo << "Nao ha caminho entre os vertices " << vertice1 << " e " << vertice2 << endl;
    } else {
        // Reconstrói o caminho do vértice1 até o vértice2 usando o vetor de pai
        vector<int> caminho;
        for (int v = vertice2; v != 0; v = resultado.pai[v]) {
            caminho.push_back(v);
        }
        reverse(caminho.begin(), caminho.end());

        // Escreve a distância e o caminho no arquivo de saída
        arquivo << "Distancia entre os vertices " << vertice1 << " e " << vertice2 << ": " << resultado.nivel[vertice2] << endl;
        arquivo << "Caminho: ";
        for (size_t i = 0; i < caminho.size(); ++i) {
            arquivo << caminho[i];
            if (i < caminho.size() - 1) {
                arquivo << " -> ";
            }
        }
        arquivo << endl;
    }

    arquivo.close();
    return ifstream(nomeArquivo);
}

ifstream Grafo::distanciaMatriz(int vertice1, int vertice2){
    string nomeArquivo = "distancia_matriz.txt";
    ofstream arquivoSaida(nomeArquivo);

    if (!arquivoSaida.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    ArvoreBusca arv = implementacaoBFSMatriz(vertice1);
    if (arv.nivel[vertice2] == -1) {
        arquivoSaida << "Nao ha caminho entre os vertices " << vertice1 << " e " << vertice2 << endl;
    } else {    
        vector<int> caminho;
        for (int v = vertice2; v != 0; v = arv.pai[v]) {
            caminho.push_back(v);
        }
        reverse(caminho.begin(), caminho.end());

        arquivoSaida << "Distancia entre os vertices " << vertice1 << " e " << vertice2 << ": " << arv.nivel[vertice2] << endl;
        arquivoSaida << "Caminho: ";
        for (size_t i = 0; i < caminho.size(); ++i) {
            arquivoSaida << caminho[i];
            if (i < caminho.size() - 1) {
                arquivoSaida << " -> ";
            }
        }
        arquivoSaida << endl;
    }

    arquivoSaida.close();
    return ifstream(nomeArquivo);
}
