#include "grafos.h"

Grafo::Grafo(int tipoGrafo, ifstream& arquivo) : tipo(tipoGrafo) {
    if (tipo == 0) {
        inicializaLista(arquivo);
    } else if (tipo == 1) {
        inicializaMatriz(arquivo);
    }
    else {
        throw std::invalid_argument("Tipo de grafo invalido. Use 0 para lista de adjacencia ou 1 para matriz de adjacencia.");
    }
}

ifstream Grafo::saidaGrafo(){
    if (tipo ==0){
        return saidaLista();
    } else if (tipo ==1){
        return saidaMatriz();
    }
} 

ifstream Grafo::bfs(int vertice){
    if (tipo ==0){
        return bfsLista(vertice);
    } else if (tipo ==1){
        return bfsMatriz(vertice);
    }
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