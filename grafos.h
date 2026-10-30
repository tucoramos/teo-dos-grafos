#ifndef GRAFOS_H
#define GRAFOS_H

#include "tipoLista.h"
using namespace std;

#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>

// Contrato comum para lista e matriz:
// pai[v] e nivel[v] usam os vertices 1..n; a posicao 0 nao e utilizada.
// A raiz tem pai 0 e nivel 0; vertices inalcancaveis tem pai e nivel -1.
// maisDistante e o menor vertice no nivelMaximo da arvore de busca.
struct ArvoreBusca{
    int raiz = 0;
    vector<int> pai;
    vector<int> nivel;
    int nivelMaximo = 0;
    int maisDistante = 0;
};

class Grafo{
    public:
        // Exige n > 0 e uma aresta valida por linha. Entradas invalidas
        // lancam invalid_argument; falhas de leitura lancam ios_base::failure.
        Grafo(int tipoGrafo, ifstream& arquivo); // item 1 e item 3
        Grafo(const Grafo&) = delete;
        Grafo& operator=(const Grafo&) = delete;
        Grafo(Grafo&&) = delete;
        Grafo& operator=(Grafo&&) = delete;
        ifstream saidaGrafo(); // item 2, item 6 e item 5 (diametro do grafo)
            //numero de vertices, 
            //numero de arestas, 
            //grau minimo, 
            //grau maximo, 
            //grau medio,
            //mediana de grau
            //diametro do grafo,
            //diametro aproximado do grafo,
            //numero de componentes conexas,
        ifstream bfs(int vertice); // item 4
        ifstream dfs(int vertice); // item 4
        ifstream distancia(int vertice1, int vertice2); //item 5 (distancia entre dois vertices)

        
        int vertices() const { return numeroDeVertices; }
        int arestas() const { return numeroDeArestas; }        

        ~Grafo() = default;
        // tornados publicos para fazer os estudo de caso
        ArvoreBusca implementacaoBFSLista(int vertice);
        ArvoreBusca implementacaoBFSMatriz(int vertice);
        ArvoreBusca implementacaoDFSLista(int vertice);
        ArvoreBusca implementacaoDFSMatriz(int vertice);

        
        private:
        int tipo; // 0 = lista de adjacencia, 1 = matriz de adjacencia
        int numeroDeVertices = 0;
        int numeroDeArestas = 0;
        unique_ptr<ListaAdjacencia> listaAdjacencia; // propriedade exclusiva, para tipo 0
        vector<vector<bool>> matrizAdjacencia;
        
        void lerEntrada(ifstream& arquivo);
        void validarVertice(int vertice) const;
        void validarBusca(int vertice, int representacao) const;
        
        
        ifstream saidaLista();
            void infosGrauLista(ostream& arquivo);
            void diametroLista(ostream& arquivo);
            void diametroAproximadoLista(ostream& arquivo);
            void componentesConexasLista(ostream& arquivo);
        ifstream saidaMatriz();
            vector<vector<int>> componentesConexasMatriz();
            int diametroMatriz();
            int diametroAproximadoMatriz();
        
        ifstream bfsLista(int vertice);
        ifstream bfsMatriz(int vertice);
        
        ifstream dfsLista(int vertice);
        ifstream dfsMatriz(int vertice);
        
        ifstream distanciaLista(int vertice1, int vertice2);
        ifstream distanciaMatriz(int vertice1, int vertice2);

};

#endif
