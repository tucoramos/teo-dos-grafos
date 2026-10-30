#ifndef LISTA_ADJACENCIA_H
#define LISTA_ADJACENCIA_H

#include <vector>

struct No {
    int valor;
    double peso;
    No* prox;
};

class ListaAdjacencia {
public:
    explicit ListaAdjacencia(int tamanho);
    ListaAdjacencia(const ListaAdjacencia&) = delete;
    ListaAdjacencia& operator=(const ListaAdjacencia&) = delete;
    ListaAdjacencia(ListaAdjacencia&&) = delete;
    ListaAdjacencia& operator=(ListaAdjacencia&&) = delete;
    ~ListaAdjacencia();

    void adicionarAresta(int vertice1, int vertice2, double peso = 1);
    const No* primeiroVizinho(int vertice) const;
    // Consome um no do cursor; zero indica fim (vertices comecam em 1).
    int proximoVizinho(const No*& cursor) const;
    int proximoVizinho(const No*& cursor, double& peso) const;

private:
    int tamanho;
    std::vector<No*> estrutura;
    static std::size_t tamanhoValido(int tamanho);
    void validarVertice(int vertice) const;
};

#endif
