#include "grafos.h"

#include <cmath>
#include <limits>
#include <stdexcept>

Grafo::Grafo(int tipoGrafo, ifstream& arquivo, bool orientado)
    : tipo(tipoGrafo), direcionado(orientado) {
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

int Grafo::proximoVizinho(int vertice, CursorVizinhos& cursor, double& peso) const {
    if (tipo == 0) return listaAdjacencia->proximoVizinho(cursor.proximo, peso);
    return matrizAdjacencia->proximoVizinho(vertice, cursor.coluna, peso);
}

void Grafo::validarResultadoDijkstra(const ResultadoDijkstra& resultado) const {
    validarVertice(resultado.origem);
    const auto tamanho = static_cast<size_t>(numeroDeVertices) + 1;
    if (resultado.distancia.size() != tamanho || resultado.pai.size() != tamanho) {
        throw invalid_argument("ResultadoDijkstra incompativel com o numero de vertices");
    }
    if (resultado.pai[0] != -1 || resultado.pai[resultado.origem] != 0 ||
        resultado.distancia[resultado.origem] != 0) {
        throw invalid_argument("ResultadoDijkstra com origem ou indice reservado invalido");
    }
}

void Grafo::validarResultadoBellmanFord(const ResultadoBellmanFord& resultado) const {
    validarVertice(resultado.destino);
    if (resultado.possuiCicloNegativo) {
        if (!resultado.distancia.empty() || !resultado.proximo.empty()) {
            throw invalid_argument("ResultadoBellmanFord com ciclo negativo deve ter vetores vazios");
        }
        return;
    }
    const auto tamanho = static_cast<size_t>(numeroDeVertices) + 1;
    const double infinito = numeric_limits<double>::infinity();
    if (resultado.distancia.size() != tamanho || resultado.proximo.size() != tamanho) {
        throw invalid_argument("ResultadoBellmanFord incompativel com o numero de vertices");
    }
    if (resultado.distancia[0] != infinito || resultado.proximo[0] != -1 ||
        resultado.distancia[resultado.destino] != 0 || resultado.proximo[resultado.destino] != 0) {
        throw invalid_argument("ResultadoBellmanFord com destino ou indice reservado invalido");
    }
    for (int v = 1; v <= numeroDeVertices; ++v) {
        const double distancia = resultado.distancia[v];
        const int proximo = resultado.proximo[v];
        if (distancia == infinito && proximo == -1) continue;
        if (!isfinite(distancia) || (v != resultado.destino &&
            (proximo < 1 || proximo > numeroDeVertices))) {
            throw invalid_argument("Distancia ou proximo invalido em ResultadoBellmanFord");
        }
    }
}

void Grafo::validarResultadoFloydWarshall(const ResultadoFloydWarshall& resultado,
                                        bool completo) const {
    if (resultado.possuiCicloNegativo) {
        if (!resultado.distancia.empty() || !resultado.pai.empty()) {
            throw invalid_argument("ResultadoFloydWarshall com ciclo negativo deve ter matrizes vazias");
        }
        return;
    }
    const auto tamanho = static_cast<size_t>(numeroDeVertices) + 1;
    const double infinito = numeric_limits<double>::infinity();
    if (resultado.distancia.size() != tamanho || resultado.pai.size() != tamanho) {
        throw invalid_argument("ResultadoFloydWarshall incompativel com o numero de vertices");
    }
    // Confere todas as dimensoes antes de indexar qualquer linha.
    for (size_t u = 0; u < tamanho; ++u) {
        if (resultado.distancia[u].size() != tamanho || resultado.pai[u].size() != tamanho) {
            throw invalid_argument("Linha incompativel em ResultadoFloydWarshall");
        }
    }
    for (int u = 0; u <= numeroDeVertices; ++u) {
        if (resultado.distancia[0][u] != infinito || resultado.distancia[u][0] != infinito ||
            resultado.pai[0][u] != -1 || resultado.pai[u][0] != -1 ||
            (u != 0 && (resultado.distancia[u][u] != 0 || resultado.pai[u][u] != 0))) {
            throw invalid_argument("Origem ou indice reservado invalido em ResultadoFloydWarshall");
        }
        if (!completo || u == 0) continue;
        for (int v = 1; v <= numeroDeVertices; ++v) {
            const double distancia = resultado.distancia[u][v];
            const int pai = resultado.pai[u][v];
            if (distancia == infinito && pai == -1) continue;
            if (!isfinite(distancia) || (u != v && (pai < 1 || pai > numeroDeVertices))) {
                throw invalid_argument("Distancia ou pai invalido em ResultadoFloydWarshall");
            }
        }
    }
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
    if (direcionado) {
        throw logic_error("saidaGrafo exige grafo nao direcionado: componentes e diametros nao definidos neste relatorio");
    }
    auto graus = calcularGraus();
    int minimo, maximo;
    double medio, mediana;
    calcularEstatisticasGrau(graus, minimo, maximo, medio, mediana);
    return escreverGrafo(minimo, maximo, medio, mediana, calcularDiametro(),
                         calcularDiametroAproximado(), calcularComponentesConexas());
}
