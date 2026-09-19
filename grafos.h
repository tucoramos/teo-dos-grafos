#ifndef GRAFOS_H
#define GRAFOS_H

#include "tipoLista.h"
using namespace std;

#include <fstream>

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

    private:
        int tipo; // 0 = lista de adjacencia, 1 = matriz de adjacencia
        int numeroDeVertices = 0;
        ListaAdjacencia* listaAdjacencia = nullptr; // para tipo 0

        void inicializaLista(ifstream& arquivo);
        void inicializaMatriz(ifstream& arquivo);

        ifstream saidaLista();
        ifstream saidaMatriz();

        ifstream bfsLista(int vertice);
        ifstream bfsMatriz(int vertice);

        ifstream dfsLista(int vertice);
        ifstream dfsMatriz(int vertice);

        ifstream distanciaLista(int vertice1, int vertice2);
        ifstream distanciaMatriz(int vertice1, int vertice2);
};

#endif