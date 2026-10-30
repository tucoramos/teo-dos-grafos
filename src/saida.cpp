#include "grafos.h"

#include <iomanip>
#include <limits>
#include <stdexcept>

void Grafo::escreverDijkstra(const ResultadoDijkstra& resultado, const string& nomeArquivo) const {
    validarResultadoDijkstra(resultado);
    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) throw invalid_argument("Erro ao abrir o arquivo de saida");
    arquivo << setprecision(numeric_limits<double>::max_digits10);
    arquivo << "Origem: " << resultado.origem << '\n';
    arquivo << "Vertice Distancia Pai\n";
    for (int v = 1; v <= numeroDeVertices; ++v) {
        arquivo << v << ' ';
        if (resultado.distancia[v] == numeric_limits<double>::infinity()) {
            arquivo << "inalcancavel";
        } else {
            arquivo << resultado.distancia[v];
        }
        arquivo << ' ' << resultado.pai[v] << '\n';
    }
    arquivo.close();
    if (!arquivo) throw ios_base::failure("Erro ao gravar o resultado de Dijkstra");
}

string Grafo::nomeArquivoSaida(const string& operacao) const {
    return operacao + (tipo == 0 ? "_lista.txt" : "_matriz.txt");
}

ifstream Grafo::escreverArvore(const ArvoreBusca& arvore, const string& busca,
                              const string& operacao) const {
    string nome = nomeArquivoSaida(operacao);
    ofstream arquivo(nome);
    if (!arquivo.is_open()) throw invalid_argument("Erro ao abrir o arquivo de saída");
    arquivo << busca << " a partir do vertice: " << arvore.raiz << '\n';
    arquivo << "Vertice | Pai | Nivel\n";
    for (int v = 1; v <= numeroDeVertices; ++v) {
        arquivo << v << " | " << arvore.pai[v] << " | " << arvore.nivel[v] << '\n';
    }
    arquivo.close();
    return ifstream(nome);
}

ifstream Grafo::escreverDistancia(int origem, int destino, int distancia,
                                 const vector<int>& caminho) const {
    string nome = nomeArquivoSaida("distancia");
    ofstream arquivo(nome);
    if (!arquivo.is_open()) throw invalid_argument("Erro ao abrir o arquivo de saída");
    if (distancia == -1) {
        arquivo << "Nao ha caminho entre os vertices " << origem << " e " << destino << '\n';
    } else {
        arquivo << "Distancia entre os vertices " << origem << " e " << destino << ": " << distancia << '\n';
        arquivo << "Caminho: ";
        for (size_t i = 0; i < caminho.size(); ++i) {
            if (i > 0) arquivo << " -> ";
            arquivo << caminho[i];
        }
        arquivo << '\n';
    }
    arquivo.close();
    return ifstream(nome);
}

ifstream Grafo::escreverGrafo(int minimo, int maximo, double medio, double mediana,
                             int diametro, int aproximado,
                             const vector<vector<int>>& componentes) const {
    string nome = nomeArquivoSaida("saida");
    ofstream arquivo(nome);
    if (!arquivo.is_open()) throw invalid_argument("Erro ao abrir o arquivo de saída");
    arquivo << "Dados sobre o grafo:\n";
    arquivo << "Formato salvo na memoria: " << (tipo == 0 ? "lista" : "matriz") << " de Adjacencia\n";
    arquivo << "Numero de vertices: " << numeroDeVertices << '\n';
    arquivo << "Numero de arestas: " << numeroDeArestas << '\n';
    arquivo << "Grau minimo: " << minimo << '\n';
    arquivo << "Grau maximo: " << maximo << '\n';
    arquivo << "Grau medio: " << medio << '\n';
    arquivo << "Mediana de grau: " << mediana << '\n';
    if (diametro == -1) arquivo << "O grafo não é conexo.\n";
    else arquivo << "Diametro do grafo: " << diametro << '\n';
    if (aproximado == -1) arquivo << "O grafo não é conexo [diametro aproximado].\n";
    else arquivo << "Diametro aproximado do grafo: " << aproximado << '\n';
    arquivo << "Numero de componentes conexas: " << componentes.size() << '\n';
    for (size_t i = 0; i < componentes.size(); ++i) {
        arquivo << "Componente " << i + 1 << " (tamanho: " << componentes[i].size() << "): \n   [";
        for (int vertice : componentes[i]) arquivo << vertice << ' ';
        arquivo << "]\n";
    }
    arquivo.close();
    return ifstream(nome);
}
