#include "grafos.h"
#include "somaPesos.h"

#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>

ResultadoBellmanFord Grafo::bellmanFord(int destino) const {
    validarVertice(destino);
#ifdef GRAFOS_TESTES
    estatisticasBF = {};
#endif
    ResultadoBellmanFord resultado;
    resultado.destino = destino;
    const auto tamanho = static_cast<size_t>(numeroDeVertices) + 1;
    const double infinito = numeric_limits<double>::infinity();

    // Original u -> v: melhorias em v propagam para seus predecessores u.
    vector<vector<pair<int, double>>> entradas(tamanho);
    for (int u = 1; u <= numeroDeVertices; ++u) {
        auto cursor = iniciarVizinhos(u);
        double peso;
        for (int v = proximoVizinho(u, cursor, peso); v != 0;
             v = proximoVizinho(u, cursor, peso)) {
            entradas[v].emplace_back(u, peso);
        }
    }

    auto executar = [&](vector<double>& distancias, vector<int>* proximos,
                        vector<int> ativos, int limite, bool deteccao) {
        vector<bool> agendado(tamanho, false);
        vector<pair<int, double>> fronteira;
        for (int rodada = 1; rodada <= limite && !ativos.empty(); ++rodada) {
            // Valores imutaveis durante a rodada: cada propagacao acrescenta
            // exatamente uma aresta, mesmo que outro ativo seja melhorado agora.
            fronteira.clear();
            for (int v : ativos) fronteira.emplace_back(v, distancias[v]);
            vector<int> seguintes;
#ifdef GRAFOS_TESTES
            if (deteccao) {
                ++estatisticasBF.rodadasDeteccao;
                estatisticasBF.verticesDeteccao += fronteira.size();
            } else {
                ++estatisticasBF.rodadasConsulta;
                estatisticasBF.verticesConsulta += fronteira.size();
            }
#endif
            for (const auto& atual : fronteira) {
                for (const auto& aresta : entradas[atual.first]) {
#ifdef GRAFOS_TESTES
                    if (deteccao) ++estatisticasBF.arestasDeteccao;
                    else ++estatisticasBF.arestasConsulta;
#endif
                    const int u = aresta.first;
                    const double candidata = detalhes::somarPesos(atual.second, aresta.second);
                    if (candidata >= distancias[u]) continue;
                    distancias[u] = candidata;
                    if (proximos) (*proximos)[u] = atual.first;
                    if (!agendado[u]) {
                        agendado[u] = true;
                        seguintes.push_back(u);
                    }
                }
            }
            if (deteccao && rodada == numeroDeVertices && !seguintes.empty()) return true;
            for (int v : seguintes) agendado[v] = false;
            ativos = move(seguintes); // Fronteira vazia encerra sem novas varreduras.
        }
        return false;
    };

    // Superorigem implicita: alcanca todas as componentes com custo zero.
    // Inverter todas as arestas preserva os ciclos e seus pesos.
    vector<double> deteccao(tamanho, 0);
    vector<int> todos(numeroDeVertices);
    iota(todos.begin(), todos.end(), 1);
    if (executar(deteccao, nullptr, move(todos), numeroDeVertices, true)) {
        resultado.possuiCicloNegativo = true;
        return resultado;
    }

    resultado.distancia.assign(tamanho, infinito);
    resultado.proximo.assign(tamanho, -1);
    resultado.distancia[destino] = 0;
    resultado.proximo[destino] = 0;
    executar(resultado.distancia, &resultado.proximo, {destino}, numeroDeVertices - 1, false);
    return resultado;
}

vector<int> Grafo::caminhoMinimo(const ResultadoBellmanFord& resultado, int origem) const {
    validarVertice(origem);
    validarResultadoBellmanFord(resultado);
    if (resultado.possuiCicloNegativo) {
        throw logic_error("Caminhos minimos indisponiveis: grafo possui ciclo negativo");
    }
    if (resultado.distancia[origem] == numeric_limits<double>::infinity()) return {};
    vector<int> caminho;
    int atual = origem;
    for (int passos = 0; passos < numeroDeVertices; ++passos) {
        if (atual < 1 || atual > numeroDeVertices || !isfinite(resultado.distancia[atual])) {
            throw invalid_argument("Cadeia de proximos invalida em ResultadoBellmanFord");
        }
        caminho.push_back(atual);
        if (atual == resultado.destino) return caminho;
        atual = resultado.proximo[atual];
    }
    throw invalid_argument("Ciclo na cadeia de proximos de ResultadoBellmanFord");
}
