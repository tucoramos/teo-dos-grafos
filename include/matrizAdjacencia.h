#ifndef MATRIZ_ADJACENCIA_H
#define MATRIZ_ADJACENCIA_H

#include <vector>

class MatrizAdjacencia {
public:
    explicit MatrizAdjacencia(int tamanho);
    MatrizAdjacencia(const MatrizAdjacencia&) = delete;
    MatrizAdjacencia& operator=(const MatrizAdjacencia&) = delete;
    MatrizAdjacencia(MatrizAdjacencia&&) = delete;
    MatrizAdjacencia& operator=(MatrizAdjacencia&&) = delete;

    void adicionarAresta(int vertice1, int vertice2);
    bool existeAresta(int vertice1, int vertice2) const;
    // Colunas 1..n em ordem crescente; atualiza o cursor e retorna zero no fim.
    int proximoVizinho(int vertice, int& coluna) const;

private:
    int tamanho;
    std::vector<std::vector<bool>> estrutura;
    static std::size_t tamanhoValido(int tamanho);
    void validarVertice(int vertice) const;
};

#endif
