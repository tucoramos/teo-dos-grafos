#include "grafos.h"
#include "somaPesos.h"

#include <limits>
#include <stdexcept>

ResultadoFloydWarshall Grafo::floydWarshall() const {
    ResultadoFloydWarshall resultado;
    const auto tamanho = static_cast<size_t>(numeroDeVertices) + 1;
    const double infinito = numeric_limits<double>::infinity();
    resultado.distancia.assign(tamanho, vector<double>(tamanho, infinito));
    resultado.pai.assign(tamanho, vector<int>(tamanho, -1));
    for (int u = 1; u <= numeroDeVertices; ++u) {
        resultado.distancia[u][u] = 0;
        resultado.pai[u][u] = 0;
        auto cursor = iniciarVizinhos(u);
        double peso;
        for (int v = proximoVizinho(u, cursor, peso); v != 0;
             v = proximoVizinho(u, cursor, peso)) {
            if (peso < resultado.distancia[u][v]) {
                resultado.distancia[u][v] = peso;
                resultado.pai[u][v] = u;
            }
        }
    }
    auto detectarCiclo = [&]() {
        for (int v = 1; v <= numeroDeVertices; ++v) {
            if (resultado.distancia[v][v] < 0) {
                resultado.possuiCicloNegativo = true;
                // swap tambem libera a capacidade das matrizes descartadas.
                vector<vector<double>>().swap(resultado.distancia);
                vector<vector<int>>().swap(resultado.pai);
                return true;
            }
        }
        return false;
    };
    if (detectarCiclo()) return resultado;
    // k externo permite atualizar as mesmas duas matrizes, com memoria O(V^2).
    for (int k = 1; k <= numeroDeVertices; ++k) {
        for (int i = 1; i <= numeroDeVertices; ++i) {
            if (resultado.distancia[i][k] == infinito) continue;
            for (int j = 1; j <= numeroDeVertices; ++j) {
                if (resultado.distancia[k][j] == infinito) continue;
                const double candidata = detalhes::somarPesos(resultado.distancia[i][k],
                                                               resultado.distancia[k][j]);
                if (candidata < resultado.distancia[i][j]) {
                    resultado.distancia[i][j] = candidata;
                    resultado.pai[i][j] = resultado.pai[k][j];
                }
            }
        }
        if (detectarCiclo()) return resultado;
    }
    return resultado;
}

vector<int> Grafo::caminhoMinimo(const ResultadoFloydWarshall& resultado,
                               int origem, int destino) const {
    validarVertice(origem);
    validarVertice(destino);
    validarResultadoFloydWarshall(resultado);
    if (resultado.possuiCicloNegativo) {
        throw logic_error("Caminhos minimos indisponiveis: grafo possui ciclo negativo");
    }
    if (resultado.distancia[origem][destino] == numeric_limits<double>::infinity()) {
        if (resultado.pai[origem][destino] != -1) {
            throw invalid_argument("Pai de inalcancavel invalido em ResultadoFloydWarshall");
        }
        return {};
    }
    vector<int> caminho;
    int atual = destino;
    for (int passos = 0; passos < numeroDeVertices; ++passos) {
        if (atual < 1 || atual > numeroDeVertices || !isfinite(resultado.distancia[origem][atual])) {
            throw invalid_argument("Cadeia de pais invalida em ResultadoFloydWarshall");
        }
        caminho.push_back(atual);
        if (atual == origem) {
            reverse(caminho.begin(), caminho.end());
            return caminho;
        }
        atual = resultado.pai[origem][atual];
    }
    throw invalid_argument("Ciclo na cadeia de pais de ResultadoFloydWarshall");
}
