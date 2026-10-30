#ifndef TIPO_LISTA_H
#define TIPO_LISTA_H

#include <vector>
#include <limits>
#include <memory>
#include <stdexcept>
using namespace std;

struct No {
    int valor;
    No* prox;
};

class ListaAdjacencia {
public:
    explicit ListaAdjacencia(int tam): tamanho(tam), estrutura(tamanhoValido(tam), nullptr) {}
    ListaAdjacencia(const ListaAdjacencia&) = delete;
    ListaAdjacencia& operator=(const ListaAdjacencia&) = delete;
    ListaAdjacencia(ListaAdjacencia&&) = delete;
    ListaAdjacencia& operator=(ListaAdjacencia&&) = delete;
    ~ListaAdjacencia(){
        for (int i = 1; i <= tamanho; ++i) {
            No* atual = estrutura[i];
            while (atual != nullptr) {
                No* temp = atual;
                atual = atual->prox;
                delete temp;
            }
        }
    }
    void adicionarAresta(int vertice1, int vertice2) {
        if (vertice1 < 1 || vertice1 > tamanho || vertice2 < 1 || vertice2 > tamanho) {
            throw invalid_argument("Extremidade da aresta fora do intervalo 1..n");
        }
        // Aloque ambos antes de alterar a lista: uma falha nao deixa meia aresta.
        unique_ptr<No> novoNo(new No{vertice2, nullptr});
        unique_ptr<No> novoNo2(new No{vertice1, nullptr});
        novoNo->prox = estrutura[vertice1];
        estrutura[vertice1] = novoNo.release();
        novoNo2->prox = estrutura[vertice2];
        estrutura[vertice2] = novoNo2.release();
    }
    int tamanho;
    vector<No*> estrutura;

private:
    static size_t tamanhoValido(int tam) {
        if (tam < 1 || tam == numeric_limits<int>::max()) {
            throw invalid_argument("Numero de vertices invalido");
        }
        return static_cast<size_t>(tam) + 1;
    }
};

#endif
