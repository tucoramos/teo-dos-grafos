#include "grafos.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <string>

using namespace std;

Grafo::Grafo(int tipoGrafo, ifstream& arquivo) : tipo(tipoGrafo) {
    if (tipo == 0) {
        inicializaLista(arquivo);
    } else if (tipo == 1) {
        inicializaMatriz(arquivo);
    }
    else {
        throw invalid_argument("Tipo de grafo invalido. Use 0 para lista de adjacencia ou 1 para matriz de adjacencia.");
    }
}

// Implementação da inicialização do grafo usando lista de adjacência
void Grafo::inicializaLista(ifstream& arquivo) {

    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo");
        return;
    }

    // Lê o número de vértices do arquivo
    arquivo >> numeroDeVertices;

    // Inicializa a lista de adjacência
    listaAdjacencia = new ListaAdjacencia(numeroDeVertices);
    int vertice, adjacente;

    // Continue a leitura do arquivo para construir a lista de adjacência
    while (arquivo >> vertice >> adjacente) {
        listaAdjacencia->adicionarAresta(vertice, adjacente);
    }
}

ifstream Grafo::saidaGrafo(){
    if (tipo ==0){
        return saidaLista();
    } else if (tipo ==1){
        return saidaMatriz();
    }
} 

// Implementação da iformações do grau dos vértices usando lista de adjacência
void Grafo::infosGrauLista(ostream& arquivo){
    vector<int> graus(numeroDeVertices, 0);

    // Calcula o grau de cada vértice
    for (int i = 0; i < numeroDeVertices; ++i) {
        No* n = listaAdjacencia->estrutura[i];
        while (n != nullptr) {
            graus[i]++;
            n = n->prox;
        }
    }

    // Acha o grau mínimo, máximo
    int grauMinimo = *min_element(graus.begin(), graus.end());
    int grauMaximo = *max_element(graus.begin(), graus.end());

    //Soma todos os graus e divide pelo número de vértices para achar o grau médio
    double grauMedio = 0;
    for (int grau : graus) {
        grauMedio += grau;
    }
    grauMedio /= numeroDeVertices;

    // Ordena todos os graus e calcula a mediana
    sort(graus.begin(), graus.end());
    double mediana;
    if (numeroDeVertices % 2 == 0) {
        mediana = (graus[numeroDeVertices / 2 - 1] + graus[numeroDeVertices / 2]) / 2.0;
    } else {
        mediana = graus[numeroDeVertices / 2];
    }

    // Escreve as informações no arquivo de saída
    arquivo << "Grau minimo: " << grauMinimo << endl;
    arquivo << "Grau maximo: " << grauMaximo << endl;
    arquivo << "Grau medio: " << grauMedio << endl;
    arquivo << "Mediana de grau: " << mediana << endl;
}

// Implementação do calculo do diâmetro do grafo usando lista de adjacência
void Grafo::diametroLista(ostream& arquivo){
    int diametro = 0;
    
    for (int i = 0; i < numeroDeVertices; ++i) {
        // Para cada vértice, realiza uma busca em largura (BFS) para calcular a distância minima dele até os outros vértices
        auto resultadoBFS = implementacaoBFSLista(i);
        vector<int> nivel = resultadoBFS.second;

        // itera sobre os níveis para encontrar a maior distância mínima encontrada ate agora (inclusive de BFS de outros vértices)
        for (int j = 0; j < numeroDeVertices; ++j) {
            if (nivel[j] != -1) {
                diametro = max(diametro, nivel[j]);
            } else {
                // Se algum vértice não for alcançável, o grafo não é conexo
                arquivo << "O grafo não é conexo." << endl;
                return; // Retorna vazio para indicar que o grafo não é conexo
            }
        }
    }

    // retorna o diâmetro do grafo, que é a maior distância mínima entre quaisquer dois vértices
    arquivo << "Diametro do grafo: " << diametro << endl;
}

// Implementação da contagem de componentes conexas usando lista de adjacência
void Grafo::componentesConexasLista(ostream& arquivo){
    // valores que armazenam resultados
    vector<pair<int,vector<int>>> resultado;
    int numeroComponetes = 0;
    int numeroElementos = 0;
    vector<bool> visitado(numeroDeVertices,false);

    for (int i = 0; i < numeroDeVertices; ++i) {
        if(!visitado[i]){
            // Se o vértice não foi visitado, então encontramos uma nova componente conexa
            numeroComponetes++;
            vector<int> componente;
            numeroElementos = 0;

            //Ja que encontramos uma nova componente conexa, realizamos uma BFS para encontrar todos os vértices dessa componente
            auto resultadoBFS = implementacaoBFSLista(i);
            vector<int> nivel = resultadoBFS.second; //salva vetor de níveis do BFS

            // Itera sobre todos os vértices e marca como visitados aqueles que foram alcançados na BFS, adicionando-os à componente atual
            for (int j = 0; j < numeroDeVertices; ++j) {
                if (nivel[j] != -1) {
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
    for (size_t i = 0; i < numeroComponetes; i++)
    {
        arquivo << "Componente " << i + 1 << " (tamanho: " << resultado[i].first << "): \n   [";
        for (int vertice : resultado[i].second) {
            arquivo << vertice << " ";
        }
        arquivo << "]\n";
    }
}

// Implementação da saída do grafo usando lista de adjacência
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
    int numeroDeArestas = 0;
    for (int i = 0; i < numeroDeVertices; ++i) {
        No* n = listaAdjacencia->estrutura[i];
        while (n != nullptr) {
            numeroDeArestas++;
            n = n->prox;
        }
    }
    arquivo << "Numero de arestas: " << numeroDeArestas << endl;

    //Infos sobre grau dos vertices
    infosGrauLista(arquivo);

    //diametro do grafo,
    diametroLista(arquivo);

    //numero de componentes conexas,
    componentesConexasLista(arquivo);

    arquivo.close();
    return ifstream(nomeArquivo);
}

ifstream Grafo::bfs(int vertice){
    if (tipo ==0){
        return bfsLista(vertice);
    } else if (tipo ==1){
        return bfsMatriz(vertice);
    }
} 

// Implementação da busca em largura (BFS) usando lista de adjacência
pair<vector<int>, vector<int>> Grafo::implementacaoBFSLista(int vertice) {
    // Guarda o estado de cada vértice (visitado ou não) 
    vector<int> visitado(numeroDeVertices, 0);
    // Guarda o nível de cada vértice
    vector<int> nivel(numeroDeVertices, -1);
    // Guarda a ordem de visita dos vértices
    vector<int> pai(numeroDeVertices,0);
    // Fila para a BFS
    queue<int> fila;

    // Inicializa o vértice inicial
    visitado[vertice] = 1;
    nivel[vertice] = 0;
    pai[vertice] = -1;
    fila.push(vertice);


    // Enquanto a fila não estiver vazia, continue a BFS
    while (!fila.empty()) {
        // Pega o vértice da frente da fila e salva ele na ordem de visita
        int atual = fila.front();
        fila.pop();

        // Itera sobre os vizinhos do vértice atual caso ele nao tenha sido visitado, marca o nível dele e adiciona na fila
        No* n = listaAdjacencia->estrutura[atual];
        while (n != nullptr) {
            if (!visitado[n->valor]) {
                visitado[n->valor] = 1;
                nivel[n->valor] = nivel[atual] + 1;
                pai[n->valor] = atual;
                fila.push(n->valor);
            }
            n = n->prox;
        }
    }

    return make_pair(pai, nivel);
}

ifstream Grafo::dfs(int vertice){
    if (tipo ==0){
        return dfsLista(vertice);
    } else if (tipo ==1){
        return dfsMatriz(vertice);
    }
} 

ifstream Grafo::distancia(int vertice1, int vertice2){
    if (tipo ==0){
        return distanciaLista(vertice1, vertice2);
    } else if (tipo ==1){
        return distanciaMatriz(vertice1, vertice2);
    }
} 