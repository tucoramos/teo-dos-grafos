// Estudos de caso da Parte 3 (Bellman-Ford e Floyd-Warshall).
//
// Uso:
//   estudo_p3 <arquivo> [opcoes]
//
// Opcoes:
//   --rodadas R            rodadas para os tempos medios (padrao 10)
//   --amostra K            destinos sorteados para estimar o Bellman-Ford de
//                          todos os pares (padrao 10)
//   --todos-pares-completo executa de fato o Bellman-Ford para todos os n
//                          destinos, R vezes (pode levar horas ou dias)
//   --fw auto|sim|nao      Floyd-Warshall: "auto" (padrao) executa somente se as
//                          matrizes couberem no limite de memoria
//   --memoria-fw-gb G      limite de memoria do modo auto, em GB (padrao 2)
//   --somente-fw           executa apenas o Floyd-Warshall (ignora o limite de
//                          memoria): tabela dos pares e tempo medio das rodadas
//   --nao-direcionado      le o grafo como nao direcionado (padrao: direcionado)
//   --semente S            semente do sorteio dos destinos (padrao 2026)
//
// Estudo 1: distancia dos vertices 10, 20 e 30 ate o vertice 100 (Bellman-Ford).
// Estudo 2: distancias entre os pares de {10, 20, 30, 40, 50}. Com Floyd-Warshall
//           quando executado; a mesma tabela tambem e calculada com cinco
//           execucoes do Bellman-Ford (uma por destino), para conferencia ou
//           para substituir o Floyd-Warshall quando ele nao for executado.
// Estudo 3: tempo medio (s) de cada algoritmo em R rodadas, sem leitura/escrita.
// Estudo 4: Bellman-Ford para todos os pares (n execucoes) x Floyd-Warshall.
//
// Os tempos medem apenas as chamadas dos algoritmos da biblioteca.

#include "grafos.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <limits>
#include <random>
#include <set>
#include <sstream>

using namespace std;
using namespace std::chrono;

static string formatarDistancia(double distancia) {
    if (isinf(distancia)) return "inf";
    ostringstream saida;
    saida << fixed << setprecision(2) << distancia;
    return saida.str();
}

static string formatarCaminho(const vector<int>& caminho) {
    if (caminho.empty()) return "sem caminho";
    ostringstream saida;
    for (size_t i = 0; i < caminho.size(); ++i) {
        if (i > 0) saida << " -> ";
        saida << caminho[i];
    }
    return saida.str();
}

static string formatarTempo(double segundos) {
    ostringstream saida;
    if (segundos >= 3600) {
        saida << fixed << setprecision(4) << segundos << " s (~" << setprecision(1)
              << segundos / 3600 << " h)";
    } else {
        saida << fixed << setprecision(4) << segundos << " s";
    }
    return saida.str();
}

static double segundosDesde(steady_clock::time_point inicio) {
    return duration<double>(steady_clock::now() - inicio).count();
}

struct Opcoes {
    string arquivo;
    int rodadas = 10;
    int amostra = 10;
    bool todosParesCompleto = false;
    bool somenteFW = false;
    string fw = "auto";
    double memoriaFwGB = 2.0;
    bool direcionado = true;
    unsigned semente = 2026u;
};

static Opcoes lerOpcoes(int argc, char* argv[]) {
    Opcoes op;
    op.arquivo = argv[1];
    for (int i = 2; i < argc; ++i) {
        const string a = argv[i];
        auto valor = [&]() -> string {
            if (i + 1 >= argc) throw invalid_argument("Falta o valor de " + a);
            return argv[++i];
        };
        if (a == "--rodadas") op.rodadas = stoi(valor());
        else if (a == "--amostra") op.amostra = stoi(valor());
        else if (a == "--todos-pares-completo") op.todosParesCompleto = true;
        else if (a == "--fw") op.fw = valor();
        else if (a == "--memoria-fw-gb") op.memoriaFwGB = stod(valor());
        else if (a == "--somente-fw") op.somenteFW = true;
        else if (a == "--nao-direcionado") op.direcionado = false;
        else if (a == "--semente") op.semente = static_cast<unsigned>(stoul(valor()));
        else throw invalid_argument("Opcao desconhecida: " + a);
    }
    if (op.rodadas < 1 || op.amostra < 1) throw invalid_argument("rodadas e amostra devem ser >= 1");
    if (op.fw != "auto" && op.fw != "sim" && op.fw != "nao") {
        throw invalid_argument("--fw aceita auto, sim ou nao");
    }
    if (op.somenteFW && op.fw == "nao") {
        throw invalid_argument("--somente-fw nao pode ser usado com --fw nao");
    }
    return op;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "uso: estudo_p3 <arquivo> [--rodadas R] [--amostra K] [--todos-pares-completo]\n"
             << "                 [--fw auto|sim|nao] [--memoria-fw-gb G] [--nao-direcionado]\n"
             << "                 [--semente S] [--somente-fw]\n";
        return 1;
    }
    try {
        const Opcoes op = lerOpcoes(argc, argv);
        ifstream arquivo(op.arquivo);
        if (!arquivo.is_open()) {
            cerr << "Erro ao abrir " << op.arquivo << '\n';
            return 1;
        }
        cout << "Lendo " << op.arquivo << " ("
             << (op.direcionado ? "direcionado" : "nao direcionado") << ", lista)...\n";
        auto inicioLeitura = steady_clock::now();
        Grafo g(0, arquivo, op.direcionado);
        const int n = g.vertices();
        cout << "n = " << n << ", m = " << g.arestas() << " (leitura: "
             << fixed << setprecision(2) << segundosDesde(inicioLeitura) << " s)\n";
        cout << "Rodadas: " << op.rodadas << "\n\n";

        const vector<int> conjunto = {10, 20, 30, 40, 50};
        for (int v : conjunto) {
            if (v > n) throw invalid_argument("O grafo nao possui o vertice " + to_string(v));
        }
        const size_t k = conjunto.size();
        auto imprimirTabela = [&](const vector<vector<double>>& d) {
            cout << "origem\\destino";
            for (int j : conjunto) cout << " | " << j;
            cout << '\n';
            for (size_t i = 0; i < k; ++i) {
                cout << conjunto[i];
                for (size_t j = 0; j < k; ++j) cout << " | " << formatarDistancia(d[i][j]);
                cout << '\n';
            }
        };

        if (op.somenteFW) {
            // Somente Floyd-Warshall: R rodadas; a tabela usa a primeira.
            double totalFW = 0;
            bool cicloFW = false;
            vector<vector<double>> tabelaFW(k, vector<double>(k, 0));
            for (int r = 1; r <= op.rodadas; ++r) {
                const time_t agora = time(nullptr);
                cerr << "  Floyd-Warshall rodada " << r << " iniciada em " << ctime(&agora);
                auto inicio = steady_clock::now();
                ResultadoFloydWarshall fw = g.floydWarshall();
                const double t = segundosDesde(inicio);
                totalFW += t;
                cerr << "  Floyd-Warshall rodada " << r << ": " << formatarTempo(t) << '\n';
                if (r == 1) {
                    cicloFW = fw.possuiCicloNegativo;
                    if (!cicloFW) {
                        for (size_t i = 0; i < k; ++i)
                            for (size_t j = 0; j < k; ++j)
                                tabelaFW[i][j] = fw.distancia[conjunto[i]][conjunto[j]];
                    }
                }
            }
            cout << "Floyd-Warshall - distancias entre os pares de {10, 20, 30, 40, 50}\n";
            if (cicloFW) {
                cout << "O grafo possui ciclo negativo: distancias minimas indefinidas.\n";
            } else {
                cout << "(linha = origem, coluna = destino)\n";
                imprimirTabela(tabelaFW);
            }
            cout << "\nTempo medio do Floyd-Warshall (" << op.rodadas << " rodadas): "
                 << formatarTempo(totalFW / op.rodadas) << '\n';
            cout.flush();
            exit(0);
        }

        // ------------------------------------------------------------------
        // Estudos 1 e 3 (Bellman-Ford): R rodadas com destino 100; a tabela
        // usa o resultado da primeira rodada.
        // ------------------------------------------------------------------
        const int destino = 100;
        if (destino > n) throw invalid_argument("O grafo nao possui o vertice 100");
        ResultadoBellmanFord bf;
        double totalBF = 0;
        for (int r = 1; r <= op.rodadas; ++r) {
            auto inicio = steady_clock::now();
            ResultadoBellmanFord atual = g.bellmanFord(destino);
            const double t = segundosDesde(inicio);
            totalBF += t;
            cerr << "  Bellman-Ford rodada " << r << ": " << formatarTempo(t) << '\n';
            if (r == 1) bf = move(atual);
        }
        const double mediaBF = totalBF / op.rodadas;
        const bool cicloNegativo = bf.possuiCicloNegativo;

        cout << "Estudo 1 - Bellman-Ford, distancia ate o vertice " << destino << '\n';
        if (cicloNegativo) {
            cout << "O grafo possui ciclo negativo: distancias minimas indefinidas.\n";
        } else {
            cout << "Origem | Distancia | Caminho\n";
            for (int origem : {10, 20, 30}) {
                if (origem > n) {
                    cout << origem << " | vertice inexistente | -\n";
                    continue;
                }
                cout << origem << " | " << formatarDistancia(bf.distancia[origem]) << " | "
                     << formatarCaminho(g.caminhoMinimo(bf, origem)) << '\n';
            }
        }
        cout << '\n';

        // ------------------------------------------------------------------
        // Estudo 2: pares de {10, 20, 30, 40, 50}.
        // ------------------------------------------------------------------
        // Cinco execucoes do Bellman-Ford: a de destino j da d(i -> j) para todo i.
        vector<vector<double>> tabelaBF(k, vector<double>(k, 0));
        if (!cicloNegativo) {
            for (size_t j = 0; j < k; ++j) {
                ResultadoBellmanFord r = g.bellmanFord(conjunto[j]);
                for (size_t i = 0; i < k; ++i) tabelaBF[i][j] = r.distancia[conjunto[i]];
            }
        }

        // Floyd-Warshall: duas matrizes (n+1)x(n+1), double e int.
        const double tamanho = static_cast<double>(n) + 1;
        const double memoriaFwGB = tamanho * tamanho * (sizeof(double) + sizeof(int)) / 1e9;
        const bool executarFW = op.fw == "sim" || (op.fw == "auto" && memoriaFwGB <= op.memoriaFwGB);

        double mediaFW = -1;
        bool cicloFW = false;
        vector<vector<double>> tabelaFW(k, vector<double>(k, 0));
        if (executarFW) {
            double totalFW = 0;
            for (int r = 1; r <= op.rodadas; ++r) {
                auto inicio = steady_clock::now();
                ResultadoFloydWarshall fw = g.floydWarshall();
                const double t = segundosDesde(inicio);
                totalFW += t;
                cerr << "  Floyd-Warshall rodada " << r << ": " << formatarTempo(t) << '\n';
                if (r == 1) {
                    cicloFW = fw.possuiCicloNegativo;
                    if (!cicloFW) {
                        for (size_t i = 0; i < k; ++i)
                            for (size_t j = 0; j < k; ++j)
                                tabelaFW[i][j] = fw.distancia[conjunto[i]][conjunto[j]];
                    }
                }
                // fw sai de escopo aqui e libera as matrizes antes da proxima rodada.
            }
            mediaFW = totalFW / op.rodadas;
        }

        cout << "Estudo 2 - distancias entre os pares de {10, 20, 30, 40, 50}\n";
        if (cicloNegativo) {
            cout << "O grafo possui ciclo negativo: distancias minimas indefinidas.\n";
            if (executarFW) {
                cout << "Floyd-Warshall: " << (cicloFW ? "tambem detectou o ciclo negativo"
                                                       : "NAO detectou ciclo negativo (DIVERGENCIA!)")
                     << '\n';
            }
        } else if (executarFW) {
            if (cicloFW) {
                cout << "Floyd-Warshall detectou ciclo negativo, mas o Bellman-Ford nao (DIVERGENCIA!)\n";
            } else {
                cout << "Floyd-Warshall (linha = origem, coluna = destino):\n";
                imprimirTabela(tabelaFW);
                bool iguais = true;
                for (size_t i = 0; i < k; ++i)
                    for (size_t j = 0; j < k; ++j) {
                        const double a = tabelaFW[i][j], b = tabelaBF[i][j];
                        if (isinf(a) != isinf(b) || (!isinf(a) && fabs(a - b) > 1e-6)) iguais = false;
                    }
                cout << "Conferencia com Bellman-Ford: "
                     << (iguais ? "tabelas iguais" : "DIFERENCA!") << '\n';
            }
        } else {
            if (op.fw == "nao") {
                cout << "Floyd-Warshall nao executado (--fw nao).\n";
            } else {
                cout << "Floyd-Warshall nao executado: as matrizes exigiriam ~" << fixed
                     << setprecision(1) << memoriaFwGB << " GB (limite " << op.memoriaFwGB
                     << " GB; use --fw sim para forcar).\n";
            }
            cout << "Mesma tabela calculada com 5 execucoes do Bellman-Ford "
                    "(linha = origem, coluna = destino):\n";
            imprimirTabela(tabelaBF);
        }
        cout << '\n';

        // ------------------------------------------------------------------
        // Estudo 3: tempos medios.
        // ------------------------------------------------------------------
        cout << "Estudo 3 - tempo medio de execucao (" << op.rodadas << " rodadas)\n";
        cout << "Bellman-Ford (destino 100): " << formatarTempo(mediaBF) << '\n';
        if (executarFW) {
            cout << "Floyd-Warshall: " << formatarTempo(mediaFW) << '\n';
        } else {
            cout << "Floyd-Warshall: N/E (~" << fixed << setprecision(1) << memoriaFwGB
                 << " GB de memoria; n^3 = " << scientific << setprecision(2)
                 << tamanho * tamanho * tamanho << " iteracoes do laco interno)\n" << fixed;
        }
        cout << '\n';

        // ------------------------------------------------------------------
        // Estudo 4: Bellman-Ford para todos os pares (um destino por vez).
        // ------------------------------------------------------------------
        cout << "Estudo 4 - Bellman-Ford para todos os pares (n = " << n << " execucoes)\n";
        if (op.todosParesCompleto) {
            double total = 0;
            for (int r = 1; r <= op.rodadas; ++r) {
                auto inicio = steady_clock::now();
                double controle = 0;
                for (int d = 1; d <= n; ++d) {
                    ResultadoBellmanFord res = g.bellmanFord(d);
                    controle += res.possuiCicloNegativo ? 1 : res.proximo[d];
                }
                const double t = segundosDesde(inicio);
                if (controle < 0) cerr << controle;
                total += t;
                cerr << "  Todos os pares rodada " << r << ": " << formatarTempo(t) << '\n';
            }
            cout << "Tempo medio medido: " << formatarTempo(total / op.rodadas) << '\n';
            if (executarFW) {
                cout << "Floyd-Warshall: " << formatarTempo(mediaFW) << '\n';
            }
        } else {
            // Destinos distintos sorteados; a media por execucao multiplicada por n
            // estima o tempo de uma rodada completa de todos os pares.
            mt19937 gerador(op.semente);
            uniform_int_distribution<int> sorteio(1, n);
            set<int> escolhidos;
            vector<int> destinos;
            while (static_cast<int>(destinos.size()) < min(op.amostra, n)) {
                const int v = sorteio(gerador);
                if (escolhidos.insert(v).second) destinos.push_back(v);
            }
            double total = 0, controle = 0;
            for (int d : destinos) {
                auto inicio = steady_clock::now();
                ResultadoBellmanFord res = g.bellmanFord(d);
                total += segundosDesde(inicio);
                controle += res.possuiCicloNegativo ? 1 : res.proximo[d];
            }
            if (controle < 0) cerr << controle;
            const double porExecucao = total / destinos.size();
            cout << "Media por execucao (amostra de " << destinos.size()
                 << " destinos, semente " << op.semente << "): " << formatarTempo(porExecucao) << '\n';
            cout << "Estimativa de uma rodada completa (media x n): "
                 << formatarTempo(porExecucao * n) << '\n';
            cout << "(use --todos-pares-completo para medir de fato)\n";
            if (executarFW) {
                cout << "Floyd-Warshall (medido): " << formatarTempo(mediaFW) << '\n';
            }
        }
    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << '\n';
        return 1;
    }
    // Encerra sem destruir o grafo: liberar milhoes de nos um a um pode demorar,
    // e o sistema operacional recupera a memoria de qualquer forma.
    cout.flush();
    exit(0);
}
