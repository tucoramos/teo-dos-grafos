#include "grafos.h"

#include <algorithm>
#include <queue>
#include <stack>

ArvoreBusca Grafo::calcularBFS(int vertice) const {
    ArvoreBusca arv;
    arv.raiz = vertice;
    arv.pai.assign(numeroDeVertices + 1, -1);
    arv.nivel.assign(numeroDeVertices + 1, -1);
    arv.pai[vertice] = 0;
    arv.nivel[vertice] = 0;
    arv.maisDistante = vertice;
    queue<int> fila;
    fila.push(vertice);
    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();
        auto cursor = iniciarVizinhos(atual);
        for (int vizinho = proximoVizinho(atual, cursor); vizinho != 0;
             vizinho = proximoVizinho(atual, cursor)) {
            if (arv.nivel[vizinho] != -1) continue;
            arv.pai[vizinho] = atual;
            arv.nivel[vizinho] = arv.nivel[atual] + 1;
            if (arv.nivel[vizinho] > arv.nivelMaximo ||
                (arv.nivel[vizinho] == arv.nivelMaximo && vizinho < arv.maisDistante)) {
                arv.nivelMaximo = arv.nivel[vizinho];
                arv.maisDistante = vizinho;
            }
            fila.push(vizinho);
        }
    }
    return arv;
}

ArvoreBusca Grafo::calcularDFS(int vertice) const {
    ArvoreBusca arv;
    arv.raiz = vertice;
    arv.pai.assign(numeroDeVertices + 1, -1);
    arv.nivel.assign(numeroDeVertices + 1, -1);
    arv.pai[vertice] = 0;
    arv.nivel[vertice] = 0;
    arv.maisDistante = vertice;
    vector<CursorVizinhos> proximo(numeroDeVertices + 1);
    stack<int> pilha;
    proximo[vertice] = iniciarVizinhos(vertice);
    pilha.push(vertice);
    while (!pilha.empty()) {
        int atual = pilha.top();
        int vizinho = proximoVizinho(atual, proximo[atual]);
        if (vizinho == 0) {
            pilha.pop();
            continue;
        }
        if (arv.nivel[vizinho] != -1) continue;
        arv.pai[vizinho] = atual;
        arv.nivel[vizinho] = arv.nivel[atual] + 1;
        if (arv.nivel[vizinho] > arv.nivelMaximo ||
            (arv.nivel[vizinho] == arv.nivelMaximo && vizinho < arv.maisDistante)) {
            arv.nivelMaximo = arv.nivel[vizinho];
            arv.maisDistante = vizinho;
        }
        proximo[vizinho] = iniciarVizinhos(vizinho);
        pilha.push(vizinho);
    }
    return arv;
}

vector<int> Grafo::reconstruirCaminho(const ArvoreBusca& arvore, int destino) const {
    vector<int> caminho;
    if (arvore.nivel[destino] == -1) return caminho;
    for (int v = destino; v != 0; v = arvore.pai[v]) caminho.push_back(v);
    reverse(caminho.begin(), caminho.end());
    return caminho;
}

vector<int> Grafo::calcularGraus() const {
    vector<int> graus(numeroDeVertices, 0);
    for (int v = 1; v <= numeroDeVertices; ++v) {
        auto cursor = iniciarVizinhos(v);
        while (proximoVizinho(v, cursor) != 0) ++graus[v - 1];
    }
    return graus;
}

void Grafo::calcularEstatisticasGrau(const vector<int>& graus, int& minimo, int& maximo,
                                    double& medio, double& mediana) const {
    minimo = *min_element(graus.begin(), graus.end());
    maximo = *max_element(graus.begin(), graus.end());
    medio = 0;
    for (int grau : graus) medio += grau;
    medio /= numeroDeVertices;
    vector<int> ordenados = graus;
    sort(ordenados.begin(), ordenados.end());
    if (numeroDeVertices % 2 == 0) {
        mediana = (ordenados[numeroDeVertices / 2 - 1] + ordenados[numeroDeVertices / 2]) / 2.0;
    } else {
        mediana = ordenados[numeroDeVertices / 2];
    }
}

vector<vector<int>> Grafo::calcularComponentesConexas() const {
    vector<bool> visitado(numeroDeVertices + 1, false);
    vector<vector<int>> componentes;
    for (int raiz = 1; raiz <= numeroDeVertices; ++raiz) {
        if (visitado[raiz]) continue;
        auto arvore = calcularBFS(raiz);
        vector<int> componente;
        // Preserva os vertices em ordem crescente dentro de cada componente.
        for (int v = 1; v <= numeroDeVertices; ++v) {
            if (arvore.nivel[v] != -1) {
                visitado[v] = true;
                componente.push_back(v);
            }
        }
        componentes.push_back(move(componente));
    }
    sort(componentes.begin(), componentes.end(),
         [](const vector<int>& a, const vector<int>& b) { return a.size() > b.size(); });
    return componentes;
}

int Grafo::calcularDiametro() const {
    auto primeira = calcularBFS(1);
    for (int v = 1; v <= numeroDeVertices; ++v) {
        if (primeira.nivel[v] == -1) return -1;
    }
    int diametro = primeira.nivelMaximo;
    for (int v = 2; v <= numeroDeVertices; ++v) {
        diametro = max(diametro, calcularBFS(v).nivelMaximo);
    }
    return diametro;
}

int Grafo::calcularDiametroAproximado() const {
    auto primeira = calcularBFS(1);
    for (int v = 1; v <= numeroDeVertices; ++v) {
        if (primeira.nivel[v] == -1) return -1;
    }
    return calcularBFS(primeira.maisDistante).nivelMaximo;
}
