#ifndef GRAFOS_H
#define GRAFOS_H

using namespace std;

#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>

struct ArvoreBusca{
    int raiz;
    vector<int> pai;
    vector<int> nivel;
    int nivelMaximo;
    int maisDistante;
};

class Grafo{
    public:
        Grafo(int tipoGrafo, ifstream& arquivo); // item 1 e item 3
        ifstream saidaGrafo(); // item 2, item 6 e item 5 (diametro do grafo)
            //numero de vertices, 
            //numero de arestas, 
            //grau minimo, 
            //grau maximo, 
            //grau medio,
            //mediana de grau
            //diametro do grafo,
            //numero de componentes conexas,
        ifstream bfs(int vertice); // item 4
        ifstream dfs(int vertice); // item 4
        ifstream distancia(int vertice1, int vertice2); //item 5 (distancia entre dois vertices)
        vector<vector<int>> componentesConexas(); // item 6

    private:
        int tipo; // 0 = lista de adjacencia, 1 = matriz de adjacencia
        int numVertices;
        vector<vector<bool>> matrizAdjacencia;
        vector<vector<int>> componentesConexasMatriz();

        void inicializaLista(ifstream& arquivo);
        void inicializaMatriz(ifstream& arquivo);

        ifstream saidaLista();
        ifstream saidaMatriz();

        ifstream bfsLista(int vertice);
        ArvoreBusca implementacaobfsMatriz(int vertice);
        ifstream bfsMatriz(int vertice);

        ifstream dfsLista(int vertice);
        ArvoreBusca implementacaodfsMatriz(int vertice);
        ifstream dfsMatriz(int vertice);

        
        ifstream distanciaLista(int vertice1, int vertice2);
        ifstream distanciaMatriz(int vertice1, int vertice2);
        int diametroMatriz();
};

#endif