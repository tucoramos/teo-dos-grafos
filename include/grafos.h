#ifndef GRAFOS_H
#define GRAFOS_H

#include "listaAdjacencia.h"
#include "matrizAdjacencia.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// Indices 1..n; indice zero reservado com -1. Raiz: pai 0, nivel 0.
// Inalcancaveis: pai e nivel -1. maisDistante: menor vertice do nivel maximo.
struct ArvoreBusca {
    int raiz = 0;
    vector<int> pai;
    vector<int> nivel;
    int nivelMaximo = 0;
    int maisDistante = 0;
};

class Grafo {
public:
    // 0 = lista, 1 = matriz. Exige 1 <= n < INT_MAX e arestas validas.
    // Entradas invalidas lancam invalid_argument; falhas de leitura, ios_base::failure.
    Grafo(int tipoGrafo, ifstream& arquivo);
    Grafo(const Grafo&) = delete;
    Grafo& operator=(const Grafo&) = delete;
    Grafo(Grafo&&) = delete;
    Grafo& operator=(Grafo&&) = delete;
    ~Grafo() = default;

    int vertices() const { return numeroDeVertices; }
    int arestas() const { return numeroDeArestas; }
    ifstream saidaGrafo();
    ifstream bfs(int vertice);
    ifstream dfs(int vertice);
    ifstream distancia(int vertice1, int vertice2);

    // Compatibilidade com os programas de estudo de caso.
    ArvoreBusca implementacaoBFSLista(int vertice);
    ArvoreBusca implementacaoBFSMatriz(int vertice);
    ArvoreBusca implementacaoDFSLista(int vertice);
    ArvoreBusca implementacaoDFSMatriz(int vertice);

private:
    int tipo;
    int numeroDeVertices = 0;
    int numeroDeArestas = 0;
    // Somente o ponteiro da representacao escolhida possui um objeto.
    unique_ptr<ListaAdjacencia> listaAdjacencia;
    unique_ptr<MatrizAdjacencia> matrizAdjacencia;

    // Cursor pequeno, sem propriedade da memoria nem copia de vizinhos.
    // Na lista guarda o proximo no; na matriz, a proxima coluna (1..n).
    // A DFS conserva um cursor por vertice para retomar depois de explorar um filho.
    struct CursorVizinhos {
        const No* proximo = nullptr;
        int coluna = 1;
    };
    CursorVizinhos iniciarVizinhos(int vertice) const;
    int proximoVizinho(int vertice, CursorVizinhos& cursor) const;

    void lerEntrada(ifstream& arquivo);
    void validarVertice(int vertice) const;
    void validarBusca(int vertice, int representacao) const;

    ArvoreBusca calcularBFS(int vertice) const;
    ArvoreBusca calcularDFS(int vertice) const;
    vector<int> reconstruirCaminho(const ArvoreBusca& arvore, int destino) const;
    vector<int> calcularGraus() const;
    void calcularEstatisticasGrau(const vector<int>& graus, int& minimo, int& maximo,
                                 double& medio, double& mediana) const;
    vector<vector<int>> calcularComponentesConexas() const;
    int calcularDiametro() const;
    int calcularDiametroAproximado() const;

    string nomeArquivoSaida(const string& operacao) const;
    ifstream escreverArvore(const ArvoreBusca& arvore, const string& busca,
                            const string& operacao) const;
    ifstream escreverDistancia(int origem, int destino, int distancia,
                               const vector<int>& caminho) const;
    ifstream escreverGrafo(int minimo, int maximo, double medio, double mediana,
                           int diametro, int aproximado,
                           const vector<vector<int>>& componentes) const;
};

#endif
