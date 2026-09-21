#ifndef GRAFOS_H
#define GRAFOS_H

#include "tipoLista.h"
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
            //diametro aproximado do grafo,
            //numero de componentes conexas,
        ifstream bfs(int vertice); // item 4
        ifstream dfs(int vertice); // item 4
        ifstream distancia(int vertice1, int vertice2); //item 5 (distancia entre dois vertices)
        
        //Metodos que tiveram que virar public para fazer as analises de tempo no main.cpp
        ArvoreBusca implementacaoBFSLista(int vertice);
        ArvoreBusca implementacaoBFSMatriz(int vertice);
        ArvoreBusca implementacaoDFSMatriz(int vertice);
        ArvoreBusca implementacaoDFSLista(int vertice);
        ~Grafo();        
        

    private:
        int tipo; // 0 = lista de adjacencia, 1 = matriz de adjacencia
        int numeroDeVertices = 0;
        ListaAdjacencia* listaAdjacencia = nullptr; // para tipo 0
        vector<vector<bool>> matrizAdjacencia;

        void inicializaLista(ifstream& arquivo);
        void inicializaMatriz(ifstream& arquivo);
        

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

        void liberarListaAdjacencia();
};

#endif