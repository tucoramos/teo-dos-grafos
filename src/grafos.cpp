#include "grafos.h"

#include <stdexcept>

Grafo::Grafo(int tipoGrafo, ifstream& arquivo) : tipo(tipoGrafo) {
    if (tipo != 0 && tipo != 1) {
        throw invalid_argument("Tipo de grafo invalido. Use 0 para lista de adjacencia ou 1 para matriz de adjacencia.");
    }
    lerEntrada(arquivo);
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

Grafo::CursorVizinhos Grafo::iniciarVizinhos(int vertice) const {
    CursorVizinhos cursor;
    if (tipo == 0) cursor.proximo = listaAdjacencia->primeiroVizinho(vertice);
    return cursor;
}

int Grafo::proximoVizinho(int vertice, CursorVizinhos& cursor) const {
    if (tipo == 0) return listaAdjacencia->proximoVizinho(cursor.proximo);
    return matrizAdjacencia->proximoVizinho(vertice, cursor.coluna);
}

ArvoreBusca Grafo::implementacaoBFSLista(int vertice) {
    validarBusca(vertice, 0);
    return calcularBFS(vertice);
}

ArvoreBusca Grafo::implementacaoBFSMatriz(int vertice) {
    validarBusca(vertice, 1);
    return calcularBFS(vertice);
}

ArvoreBusca Grafo::implementacaoDFSLista(int vertice) {
    validarBusca(vertice, 0);
    return calcularDFS(vertice);
}

ArvoreBusca Grafo::implementacaoDFSMatriz(int vertice) {
    validarBusca(vertice, 1);
    return calcularDFS(vertice);
}

ifstream Grafo::bfs(int vertice) {
    validarVertice(vertice);
    return escreverArvore(calcularBFS(vertice), "BFS", "bfs");
}

ifstream Grafo::dfs(int vertice) {
    validarVertice(vertice);
    return escreverArvore(calcularDFS(vertice), "DFS", "dfs");
}

ifstream Grafo::distancia(int vertice1, int vertice2) {
    validarVertice(vertice1);
    validarVertice(vertice2);
    auto arvore = calcularBFS(vertice1);
    return escreverDistancia(vertice1, vertice2, arvore.nivel[vertice2],
                             reconstruirCaminho(arvore, vertice2));
}

ifstream Grafo::saidaGrafo() {
    auto graus = calcularGraus();
    int minimo, maximo;
    double medio, mediana;
    calcularEstatisticasGrau(graus, minimo, maximo, medio, mediana);
    return escreverGrafo(minimo, maximo, medio, mediana, calcularDiametro(),
                         calcularDiametroAproximado(), calcularComponentesConexas());
}
