// Estudos de caso da Parte 2 (Dijkstra).
//
// Uso:
//   estudo_dijkstra grafo <arquivo> [k] [kVetor] [semente]
//       Estudo 1: distancia e caminho minimo de 10 ate 20, 30, 40, 50 e 60.
//       Estudo 2: tempo medio do Dijkstra com vetor e com heap, a partir de k
//                 origens aleatorias distintas (as mesmas para as duas versoes).
//       k padrao = 100. kVetor = numero de origens para a versao com vetor;
//       padrao: 10 se n <= 100000, senao 0 (versao O(n^2) nao executada: N/E).
//
//   estudo_dijkstra rede <arquivo_grafo> <arquivo_vertices>
//       Rede de colaboracao: distancia e caminho minimo de "Edsger W. Dijkstra"
//       ate os pesquisadores pedidos no enunciado.
//       arquivo_vertices: uma linha por vertice no formato "indice,nome".

#include "grafos.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <sstream>

using namespace std;
using namespace std::chrono;

static string formatarCaminho(const vector<int>& caminho, const vector<string>* nomes = nullptr) {
    if (caminho.empty()) return "sem caminho";
    ostringstream saida;
    for (size_t i = 0; i < caminho.size(); ++i) {
        if (i > 0) saida << " -> ";
        if (nomes != nullptr) saida << (*nomes)[caminho[i]];
        else saida << caminho[i];
    }
    return saida.str();
}

static string formatarDistancia(double distancia) {
    if (isinf(distancia)) return "inf";
    ostringstream saida;
    saida << fixed << setprecision(4) << distancia;
    return saida.str();
}

// Tempo medio (ms) de uma execucao do Dijkstra para as origens dadas.
static double tempoMedio(const Grafo& g, const vector<int>& origens, bool usarHeap) {
    double total = 0;
    double controle = 0; // usa o resultado para o compilador nao descartar a chamada
    for (int origem : origens) {
        auto inicio = steady_clock::now();
        ResultadoDijkstra r = g.dijkstra(origem, usarHeap);
        auto fim = steady_clock::now();
        total += duration<double, milli>(fim - inicio).count();
        controle += r.pai[origem];
    }
    if (controle < 0) cerr << controle;
    return total / origens.size();
}

static int estudoGrafo(int argc, char* argv[]) {
    const string nomeArquivo = argv[2];
    const int k = argc > 3 ? stoi(argv[3]) : 100;
    int kVetor = argc > 4 ? stoi(argv[4]) : -1;
    const unsigned semente = argc > 5 ? static_cast<unsigned>(stoul(argv[5])) : 2026u;

    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir " << nomeArquivo << '\n';
        return 1;
    }
    cout << "Lendo " << nomeArquivo << "...\n";
    auto inicioLeitura = steady_clock::now();
    Grafo g(0, arquivo); // lista de adjacencia
    auto fimLeitura = steady_clock::now();
    const int n = g.vertices();
    cout << "n = " << n << ", m = " << g.arestas() << " (leitura: "
         << duration<double>(fimLeitura - inicioLeitura).count() << " s)\n\n";

    // Estudo 1: caminhos minimos a partir do vertice 10.
    ResultadoDijkstra r = g.dijkstra(10, true);
    cout << "Estudo 1 - origem 10\n";
    cout << "Destino | Distancia | Caminho\n";
    for (int destino : {20, 30, 40, 50, 60}) {
        if (destino > n) {
            cout << destino << " | vertice inexistente | -\n";
            continue;
        }
        cout << destino << " | " << formatarDistancia(r.distancia[destino]) << " | "
             << formatarCaminho(g.caminhoMinimo(r, destino)) << '\n';
    }

    // Conferencia: as duas versoes devem produzir as mesmas distancias.
    if (n <= 100000) {
        ResultadoDijkstra rv = g.dijkstra(10, false);
        bool iguais = true;
        for (int v = 1; v <= n && iguais; ++v) {
            if (isinf(r.distancia[v]) != isinf(rv.distancia[v]) ||
                (!isinf(r.distancia[v]) && fabs(r.distancia[v] - rv.distancia[v]) > 1e-9)) {
                iguais = false;
            }
        }
        cout << "Conferencia vetor x heap: " << (iguais ? "distancias iguais" : "DIFERENCA!") << '\n';
    }

    // Estudo 2: k origens aleatorias e distintas, iguais para vetor e heap.
    mt19937 gerador(semente);
    uniform_int_distribution<int> sorteio(1, n);
    set<int> escolhidas;
    vector<int> origens;
    while (static_cast<int>(origens.size()) < min(k, n)) {
        int v = sorteio(gerador);
        if (escolhidas.insert(v).second) origens.push_back(v);
    }
    if (kVetor < 0) kVetor = (n <= 100000) ? 10 : 0;
    kVetor = min(kVetor, static_cast<int>(origens.size()));

    cout << "\nEstudo 2 - " << origens.size() << " origens (semente " << semente << ")\n";
    double mediaHeap = tempoMedio(g, origens, true);
    cout << "Heap:  " << fixed << setprecision(2) << mediaHeap << " ms (media de "
         << origens.size() << ")\n";
    if (kVetor > 0) {
        vector<int> origensVetor(origens.begin(), origens.begin() + kVetor);
        double mediaVetor = tempoMedio(g, origensVetor, false);
        cout << "Vetor: " << mediaVetor << " ms (media de " << kVetor << ")\n";
    } else {
        cout << "Vetor: N/E (O(n^2) impraticavel para n = " << n
             << "; informe kVetor para forcar)\n";
    }
    // Encerra sem destruir o grafo: liberar dezenas de milhoes de nos, um a um,
    // pode levar muito tempo, e o sistema operacional recupera a memoria de qualquer forma.
    exit(0);
}

static string aparar(const string& s) {
    const char* espacos = " \t\r\n\"";
    size_t inicio = s.find_first_not_of(espacos);
    if (inicio == string::npos) return "";
    size_t fim = s.find_last_not_of(espacos);
    return s.substr(inicio, fim - inicio + 1);
}

static int estudoRede(char* argv[]) {
    ifstream arquivoVertices(argv[3]);
    if (!arquivoVertices.is_open()) {
        cerr << "Erro ao abrir " << argv[3] << '\n';
        return 1;
    }
    ifstream arquivoGrafo(argv[2]);
    if (!arquivoGrafo.is_open()) {
        cerr << "Erro ao abrir " << argv[2] << '\n';
        return 1;
    }
    Grafo g(0, arquivoGrafo);
    const int n = g.vertices();

    // Mapeamento nome <-> indice. Cada linha: "indice,nome" (o nome pode ter virgulas).
    vector<string> nomes(n + 1);
    map<string, int> indicePorNome;
    string linha;
    while (getline(arquivoVertices, linha)) {
        size_t virgula = linha.find(',');
        if (virgula == string::npos) continue;
        int indice;
        try {
            indice = stoi(aparar(linha.substr(0, virgula)));
        } catch (const exception&) {
            continue; // cabecalho ou linha invalida
        }
        if (indice < 1 || indice > n) continue;
        string nome = aparar(linha.substr(virgula + 1));
        nomes[indice] = nome;
        indicePorNome[nome] = indice;
    }

    const string origemNome = "Edsger W. Dijkstra";
    auto it = indicePorNome.find(origemNome);
    if (it == indicePorNome.end()) {
        cerr << "Pesquisador nao encontrado: " << origemNome << '\n';
        return 1;
    }
    ResultadoDijkstra r = g.dijkstra(it->second, true);

    cout << "Origem: " << origemNome << " (vertice " << it->second << ")\n";
    cout << "Pesquisador | Distancia | Caminho\n";
    // "Éva Tardos" tambem e procurada sem acento, caso o arquivo use outra grafia.
    const vector<vector<string>> destinos = {
        {"Alan M. Turing"}, {"J. B. Kruskal"}, {"Jon M. Kleinberg"},
        {"Éva Tardos", "Eva Tardos"}, {"Daniel R. Figueiredo"}};
    for (const auto& grafias : destinos) {
        const string& destinoNome = grafias[0];
        auto d = indicePorNome.end();
        for (const string& grafia : grafias) {
            d = indicePorNome.find(grafia);
            if (d != indicePorNome.end()) break;
        }
        if (d == indicePorNome.end()) {
            cout << destinoNome << " | nao encontrado no arquivo de vertices | -\n";
            continue;
        }
        cout << destinoNome << " | " << formatarDistancia(r.distancia[d->second]) << " | "
             << formatarCaminho(g.caminhoMinimo(r, d->second), &nomes) << '\n';
    }
    return 0;
}

int main(int argc, char* argv[]) {
    try {
        if (argc >= 3 && string(argv[1]) == "grafo") return estudoGrafo(argc, argv);
        if (argc >= 4 && string(argv[1]) == "rede") return estudoRede(argv);
    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << '\n';
        return 1;
    }
    cerr << "uso:\n"
         << "  estudo_dijkstra grafo <arquivo> [k] [kVetor] [semente]\n"
         << "  estudo_dijkstra rede <arquivo_grafo> <arquivo_vertices>\n";
    return 1;
}
