#include "matrizAdjacencia.h"

#include <limits>
#include <stdexcept>

std::size_t MatrizAdjacencia::tamanhoValido(int tamanho) {
    if (tamanho < 1 || tamanho == std::numeric_limits<int>::max()) {
        throw std::invalid_argument("Numero de vertices invalido");
    }
    return static_cast<std::size_t>(tamanho);
}

MatrizAdjacencia::MatrizAdjacencia(int tam)
    : tamanho(tam), estrutura(tamanhoValido(tam), std::vector<bool>(tamanhoValido(tam), false)) {}

void MatrizAdjacencia::validarVertice(int vertice) const {
    if (vertice < 1 || vertice > tamanho) {
        throw std::invalid_argument("Extremidade da aresta fora do intervalo 1..n");
    }
}

void MatrizAdjacencia::adicionarAresta(int vertice1, int vertice2) {
    validarVertice(vertice1);
    validarVertice(vertice2);
    estrutura[vertice1 - 1][vertice2 - 1] = true;
    estrutura[vertice2 - 1][vertice1 - 1] = true;
}

bool MatrizAdjacencia::existeAresta(int vertice1, int vertice2) const {
    validarVertice(vertice1);
    validarVertice(vertice2);
    return estrutura[vertice1 - 1][vertice2 - 1];
}

int MatrizAdjacencia::proximoVizinho(int vertice, int& coluna) const {
    validarVertice(vertice);
    if (coluna < 1 || coluna > tamanho + 1) {
        throw std::invalid_argument("Coluna fora do intervalo 1..n+1");
    }
    while (coluna <= tamanho) {
        int vizinho = coluna++;
        if (estrutura[vertice - 1][vizinho - 1]) return vizinho;
    }
    return 0;
}
