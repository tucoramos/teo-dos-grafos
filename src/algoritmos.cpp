#include "grafos.h"
#include "filaDijkstra.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <stack>
#include <stdexcept>

ResultadoDijkstra Grafo::dijkstra(int origem, bool usarHeap) const {
    validarVertice(origem);
    if (possuiPesoNegativo) {
        throw invalid_argument("Dijkstra nao aceita grafos com pesos negativos");
    }
    ResultadoDijkstra resultado;
    resultado.origem = origem;
    resultado.distancia.assign(numeroDeVertices + 1, numeric_limits<double>::infinity());
    resultado.pai.assign(numeroDeVertices + 1, -1);
    vector<bool> concluidos(numeroDeVertices + 1, false);
    FilaDijkstra fila(resultado.distancia, usarHeap);
    resultado.pai[origem] = 0;
    fila.atualizarDistancia(origem, 0);
    for (int atual = fila.extrairMinimo(concluidos); atual != 0;
         atual = fila.extrairMinimo(concluidos)) {
        concluidos[atual] = true;
        auto cursor = iniciarVizinhos(atual);
        double peso;
        for (int vizinho = proximoVizinho(atual, cursor, peso); vizinho != 0;
             vizinho = proximoVizinho(atual, cursor, peso)) {
            if (concluidos[vizinho]) continue;
            // Subtrair o maior operando tambem detecta excesso que arredondaria
            // para DBL_MAX em vez de produzir infinito na soma.
            const double maior = max(resultado.distancia[atual], peso);
            const double menor = min(resultado.distancia[atual], peso);
            if (menor > numeric_limits<double>::max() - maior) {
                throw overflow_error("Soma dos pesos excede o intervalo finito de double");
            }
            double candidata = resultado.distancia[atual] + peso;
            if (!isfinite(candidata)) {
                throw overflow_error("Soma dos pesos excede o intervalo finito de double");
            }
            if (candidata < resultado.distancia[vizinho]) {
                fila.atualizarDistancia(vizinho, candidata);
                resultado.pai[vizinho] = atual;
            }
        }
    }
    return resultado;
}

vector<int> Grafo::caminhoMinimo(const ResultadoDijkstra& resultado, int destino) const {
    validarVertice(destino);
    validarResultadoDijkstra(resultado);
    const double distancia = resultado.distancia[destino];
    if (distancia == numeric_limits<double>::infinity() && resultado.pai[destino] == -1) {
        return {};
    }
    vector<int> caminho;
    int atual = destino;
    for (int passos = 0; passos < numeroDeVertices; ++passos) {
        if (atual < 1 || atual > numeroDeVertices ||
            !isfinite(resultado.distancia[atual]) || resultado.distancia[atual] < 0) {
            throw invalid_argument("Cadeia de pais invalida em ResultadoDijkstra");
        }
        caminho.push_back(atual);
        if (atual == resultado.origem) {
            reverse(caminho.begin(), caminho.end());
            return caminho;
        }
        atual = resultado.pai[atual];
    }
    throw invalid_argument("Ciclo na cadeia de pais de ResultadoDijkstra");
}

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
