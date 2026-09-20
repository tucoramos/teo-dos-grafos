#include "grafos.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <string>
#include <stack>

using namespace std;
// Metodo que chama a implementação da inicialização do grafo dependendo do tipo de grafo
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

// Metodo que chama a implementação da saída do grafo dependendo do tipo de grafo
ifstream Grafo::saidaGrafo(){
    if (tipo ==0){
        return saidaLista();
    } else if (tipo ==1){
        return saidaMatriz();
    }
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
    int diametro = 0;
    
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        // Para cada vértice, realiza uma busca em largura (BFS) para calcular a distância minima dele até os outros vértices
        auto resultadoBFS = implementacaoBFSLista(i);
        vector<int> nivel = resultadoBFS.second;

        // itera sobre os níveis para encontrar a maior distância mínima encontrada ate agora (inclusive de BFS de outros vértices)
        for (int j = 1; j < numeroDeVertices + 1; ++j) {
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
    vector<bool> visitado(numeroDeVertices + 1, false);

    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        if(!visitado[i]){
            // Se o vértice não foi visitado, então encontramos uma nova componente conexa
            numeroComponetes++;
            vector<int> componente;
            numeroElementos = 0;

            //Ja que encontramos uma nova componente conexa, realizamos uma BFS para encontrar todos os vértices dessa componente
            auto resultadoBFS = implementacaoBFSLista(i);
            vector<int> nivel = resultadoBFS.second; //salva vetor de níveis do BFS

            // Itera sobre todos os vértices e marca como visitados aqueles que foram alcançados na BFS, adicionando-os à componente atual
            for (int j = 1; j < numeroDeVertices + 1; ++j) {
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
    int numeroDeArestas = 0;
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        No* n = listaAdjacencia->estrutura[i];
        while (n != nullptr) {
            numeroDeArestas++;
            n = n->prox;
        }
    }
    arquivo << "Numero de arestas: " << numeroDeArestas/2 << endl;

    //Infos sobre grau dos vertices
    infosGrauLista(arquivo);

    //diametro do grafo,
    diametroLista(arquivo);

    //numero de componentes conexas,
    componentesConexasLista(arquivo);

    arquivo.close();
    return ifstream(nomeArquivo);
}

// Metodo que chama a implementação da BFS dependendo do tipo de grafo
ifstream Grafo::bfs(int vertice){
    if (tipo ==0){
        return bfsLista(vertice);
    } else if (tipo ==1){
        return bfsMatriz(vertice);
    }
} 

// Implementação da saida da BFS usando lista de adjacência
ifstream Grafo::bfsLista(int vertice){
    // Implementação da saída da BFS
    string nomeArquivo = "bfs_lista.txt";
    ofstream arquivo(nomeArquivo);


    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    // Chama a implementação da BFS e recebe os vetores de pai e nível
    auto resultado = implementacaoBFSLista(vertice);
    vector<int> pai = resultado.first;
    vector<int> nivel = resultado.second;

    // Escreve os resultados da BFS no arquivo de saída
    arquivo << "BFS a partir do vertice: " << vertice << endl;
    arquivo << "Vertice | Pai | Nivel\n";
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        if (nivel[i] != -1){
            arquivo << i << " | " << pai[i] << " | " << nivel[i] << "\n";
        }
    }
    arquivo.close();

    // Retorna o arquivo de saída em modo de leitura
    return ifstream(nomeArquivo);
}

// Implementação da busca em largura (BFS) usando lista de adjacência
pair<vector<int>, vector<int>> Grafo::implementacaoBFSLista(int vertice) {
    // Guarda o estado de cada vértice (visitado ou não) 
    vector<int> visitado(numeroDeVertices + 1, 0);
    // Guarda o nível de cada vértice
    vector<int> nivel(numeroDeVertices + 1, -1);
    // Guarda a ordem de visita dos vértices
    vector<int> pai(numeroDeVertices + 1, -1);
    // Fila para a BFS
    queue<int> fila;

    // Inicializa o vértice inicial
    visitado[vertice] = 1;
    nivel[vertice] = 0;
    pai[vertice] = 0; // O vértice inicial não tem pai
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

// Metodo que chama a implementação da DFS dependendo do tipo de grafo
ifstream Grafo::dfs(int vertice){
    if (tipo ==0){
        return dfsLista(vertice);
    } else if (tipo ==1){
        return dfsMatriz(vertice);
    }
} 

// Implementação da saida da DFS usando lista de adjacência
ifstream Grafo::dfsLista(int vertice){
    // Implementação da saída da DFS
    string nomeArquivo = "dfs_lista.txt";
    ofstream arquivo(nomeArquivo);


    // Verifica se o arquivo abre corretamente
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo de saída");
    }

    // Chama a implementação da DFS e recebe os vetores de pai e nívelq
    auto resultado = implementacaoDFSLista(vertice);
    vector<int> pai = resultado.first;
    vector<int> nivel = resultado.second;

    // Escreve os resultados da DFS no arquivo de saída
    arquivo << "DFS a partir do vertice: " << vertice << endl;
    arquivo << "Vertice | Pai | Nivel\n";
    for (int i = 1; i < numeroDeVertices + 1; ++i) {
        if (nivel[i] != -1){
            arquivo << i << " | " << pai[i] << " | " << nivel[i] << "\n";
        }
    }
    arquivo.close();

    // Retorna o arquivo de saída em modo de leitura
    return ifstream(nomeArquivo);
}

// Implementação da busca em profundidade (DFS) usando lista de adjacência
pair<vector<int>, vector<int>> Grafo::implementacaoDFSLista(int vertice){

    // Guarda o estado de cada vértice (visitado ou não) 
    vector<int> visitado(numeroDeVertices + 1, 0);
    // Guarda o nível de cada vértice
    vector<int> nivel(numeroDeVertices + 1, -1);
    // Guarda a ordem de visita dos vértices
    vector<int> pai(numeroDeVertices + 1, -1);
    // pilha para a DFS
    stack<int> pilha;


    pilha.push(vertice);
    visitado[vertice] = 1;
    nivel[vertice] = 0;
    pai[vertice] = 0; // O vértice inicial não tem pai
    
    vector<No*> proximo = listaAdjacencia->estrutura;
    while (!pilha.empty()) {
        int atual = pilha.top();
        No* n = proximo[atual];

        // Pula os vizinhos que já foram visitados.
        while (n != nullptr && visitado[n->valor]) {
            n = n->prox;
        }

        // Terminou todos os vizinhos: volta ao vértice anterior.
        if (n == nullptr) {
            pilha.pop();
            continue;
        }

        // Guarda onde retomar quando voltar para este vértice.
        proximo[atual] = n->prox;

        int vizinho = n->valor;
        visitado[vizinho] = 1;
        pai[vizinho] = atual;
        nivel[vizinho] = nivel[atual] + 1;

        // Explora esse vizinho antes de continuar os demais.
        pilha.push(vizinho);
    }

    return make_pair(pai, nivel);
}

// Metodo que chama a implementação da distância entre dois vértices dependendo do tipo de grafo
ifstream Grafo::distancia(int vertice1, int vertice2){
    if (tipo ==0){
        return distanciaLista(vertice1, vertice2);
    } else if (tipo ==1){
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
    vector<int> pai = resultado.first;
    vector<int> nivel = resultado.second;

    // Verifica se o vértice2 foi alcançado
    if (nivel[vertice2] == -1) {
        arquivo << "Nao ha caminho entre os vertices " << vertice1 << " e " << vertice2 << endl;
    } else {
        // Reconstrói o caminho do vértice1 até o vértice2 usando o vetor de pai
        vector<int> caminho;
        for (int v = vertice2; v != 0; v = pai[v]) {
            caminho.push_back(v);
        }
        reverse(caminho.begin(), caminho.end());

        // Escreve a distância e o caminho no arquivo de saída
        arquivo << "Distancia entre os vertices " << vertice1 << " e " << vertice2 << ": " << nivel[vertice2] << endl;
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

Grafo::~Grafo() {
    if (tipo == 0) {
        liberarListaAdjacencia();
    } else if (tipo == 1) {
        // Se houver alocação dinâmica para matriz de adjacência, libere-a aqui
    }
}

void Grafo::liberarListaAdjacencia() {
    if (listaAdjacencia != nullptr) {
        delete listaAdjacencia;
    }
}
