#ifndef FILA_DIJKSTRA_H
#define FILA_DIJKSTRA_H

#include <functional>
#include <queue>
#include <utility>
#include <vector>

// Auxiliar interno: os vetores usam indices 1..n e pertencem ao calculo.
class FilaDijkstra {
public:
    FilaDijkstra(std::vector<double>& distancias, bool usarHeap);
    void atualizarDistancia(int vertice, double novaDistancia);
    int extrairMinimo(const std::vector<bool>& concluidos);

private:
    std::vector<double>& distancias;
    bool usarHeap;
    // A prioridade e uma copia imutavel: distancia, depois numero do vertice.
    using Entrada = std::pair<double, int>;
    std::priority_queue<Entrada, std::vector<Entrada>, std::greater<Entrada>> heap;
};

#endif
