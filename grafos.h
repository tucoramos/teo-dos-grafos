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
        ofstream saidaGrafo(); // item 2, item 6 e item 5 (diametro do grafo)
            //numero de vertices, 
            //numero de arestas, 
            //grau minimo, 
            //grau maximo, 
            //grau medio,
            //mediana de grau
            //diametro do grafo,
            //numero de componentes conexas,
        ArvoreBusca bfs(int vertice); // item 4
        ArvoreBusca dfs(int vertice); // item 4
//        int distancia(int vertice1, int vertice2); //item 5 (distancia entre dois vertices)
//        int diametro(); //item 5 (diametro do grafo)
        vector<vector<int>> componentesConexas(); // item 6

    private:
        int tipo; // 0 = lista de adjacencia, 1 = matriz de adjacencia
        const int numVertices;
        vector<vector<bool>> matrizAdjacencia;
        vector<vector<int>> componentesConexasMatriz();

        void inicializaLista(ifstream& arquivo);
        void inicializaMatriz(ifstream& arquivo);

        ofstream saidaLista();
        ofstream saidaMatriz();

        ofstream bfsLista(int vertice);
        ArvoreBusca bfsMatriz(int vertice);

        ofstream dfsLista(int vertice);
        ArvoreBusca dfsMatriz(int vertice);

        
        int distanciaLista(int vertice1, int vertice2);
        int distanciaMatriz(int vertice1, int vertice2);
        int diametroMatriz();
};

#endif