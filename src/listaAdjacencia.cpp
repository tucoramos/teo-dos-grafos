#include "listaAdjacencia.h"

#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>

std::size_t ListaAdjacencia::tamanhoValido(int tamanho) {
    if (tamanho < 1 || tamanho == std::numeric_limits<int>::max()) {
        throw std::invalid_argument("Numero de vertices invalido");
    }
    return static_cast<std::size_t>(tamanho) + 1;
}

ListaAdjacencia::ListaAdjacencia(int tam)
    : tamanho(tam), estrutura(tamanhoValido(tam), nullptr) {}

ListaAdjacencia::~ListaAdjacencia() {
    for (int i = 1; i <= tamanho; ++i) {
        No* atual = estrutura[i];
        while (atual != nullptr) {
            No* anterior = atual;
            atual = atual->prox;
            delete anterior;
        }
    }
}

void ListaAdjacencia::validarVertice(int vertice) const {
    if (vertice < 1 || vertice > tamanho) {
        throw std::invalid_argument("Extremidade da aresta fora do intervalo 1..n");
    }
}

void ListaAdjacencia::adicionarAresta(int vertice1, int vertice2, double peso) {
    validarVertice(vertice1);
    validarVertice(vertice2);
    if (!std::isfinite(peso) || peso == 0) {
        throw std::invalid_argument("Peso deve ser finito e diferente de zero");
    }
    // Ambos sao alocados antes da insercao: uma falha nao deixa meia aresta.
    std::unique_ptr<No> primeiro(new No{vertice2, peso, nullptr});
    std::unique_ptr<No> segundo(new No{vertice1, peso, nullptr});
    primeiro->prox = estrutura[vertice1];
    estrutura[vertice1] = primeiro.release();
    segundo->prox = estrutura[vertice2];
    estrutura[vertice2] = segundo.release();
}

const No* ListaAdjacencia::primeiroVizinho(int vertice) const {
    validarVertice(vertice);
    return estrutura[vertice];
}

int ListaAdjacencia::proximoVizinho(const No*& cursor) const {
    double peso;
    return proximoVizinho(cursor, peso);
}

int ListaAdjacencia::proximoVizinho(const No*& cursor, double& peso) const {
    if (cursor == nullptr) return 0;
    int vizinho = cursor->valor;
    peso = cursor->peso;
    cursor = cursor->prox;
    return vizinho;
}
