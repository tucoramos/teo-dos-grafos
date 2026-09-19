#include "grafos.h"
#include <iostream>
#include <algorithm>
#include <queue>
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