#ifndef TIPO_LISTA_H
#define TIPO_LISTA_H

#include <vector>
using namespace std;

struct No {
    int valor;
    No* prox;
};

class ListaAdjacencia {
public:
    ListaAdjacencia(int tam): tamanho(tam), estrutura(tam + 1, nullptr) {}

    void adicionarAresta(int vertice1, int vertice2) {
        No* novoNo = new No{vertice2, estrutura[vertice1]};
        estrutura[vertice1] = novoNo;
        No* novoNo2 = new No{vertice1, estrutura[vertice2]};
        estrutura[vertice2] = novoNo2;
    }
    int tamanho;
    vector<No*> estrutura;
};

#endif