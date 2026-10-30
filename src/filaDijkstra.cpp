#include "filaDijkstra.h"

#include <cmath>
#include <limits>

FilaDijkstra::FilaDijkstra(std::vector<double>& estimativas, bool modoHeap)
    : distancias(estimativas), usarHeap(modoHeap) {}

void FilaDijkstra::atualizarDistancia(int vertice, double novaDistancia) {
    if (!(novaDistancia < distancias[vertice])) return;
    distancias[vertice] = novaDistancia;
    if (usarHeap) heap.emplace(novaDistancia, vertice);
}

int FilaDijkstra::extrairMinimo(const std::vector<bool>& concluidos) {
    if (usarHeap) {
        while (!heap.empty()) {
            const auto entrada = heap.top();
            heap.pop();
            if (!concluidos[entrada.second] && std::isfinite(entrada.first)) {
                return entrada.second;
            }
        }
        return 0;
    }
    int menor = 0;
    double distancia = std::numeric_limits<double>::infinity();
    for (std::size_t v = 1; v < distancias.size(); ++v) {
        if (!concluidos[v] && std::isfinite(distancias[v]) && distancias[v] < distancia) {
            menor = static_cast<int>(v);
            distancia = distancias[v];
        }
    }
    return menor;
}
