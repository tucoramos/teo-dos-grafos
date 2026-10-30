#include "grafos.h"

#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

void Grafo::lerEntrada(ifstream& arquivo) {
    if (!arquivo.is_open()) {
        throw invalid_argument("Erro ao abrir o arquivo");
    }
    string linha;
    if (!getline(arquivo, linha)) {
        if (arquivo.bad()) throw ios_base::failure("Erro ao ler o cabecalho");
        throw invalid_argument("Cabecalho ausente: informe o numero de vertices");
    }
    istringstream cabecalho(linha);
    string extra;
    if (!(cabecalho >> numeroDeVertices) || numeroDeVertices < 1 ||
        numeroDeVertices == numeric_limits<int>::max() || (cabecalho >> extra)) {
        throw invalid_argument("Cabecalho invalido: informe apenas um inteiro n, com 1 <= n < INT_MAX");
    }

    if (tipo == 0) {
        listaAdjacencia.reset(new ListaAdjacencia(numeroDeVertices));
    } else {
        matrizAdjacencia.reset(new MatrizAdjacencia(numeroDeVertices));
    }
    size_t numeroLinha = 1;
    while (getline(arquivo, linha)) {
        ++numeroLinha;
        if (linha.find_first_not_of(" \t\r\n\f\v") == string::npos) continue;
        istringstream campos(linha);
        int vertice1, vertice2;
        if (!(campos >> vertice1 >> vertice2) || (campos >> extra)) {
            throw invalid_argument("Aresta malformada na linha " + to_string(numeroLinha));
        }
        if (vertice1 < 1 || vertice1 > numeroDeVertices ||
            vertice2 < 1 || vertice2 > numeroDeVertices) {
            throw invalid_argument("Vertice fora do intervalo 1..n na linha " + to_string(numeroLinha));
        }
        if (numeroDeArestas == numeric_limits<int>::max()) {
            throw overflow_error("Numero de arestas excede o limite de int");
        }
        if (tipo == 0) {
            listaAdjacencia->adicionarAresta(vertice1, vertice2);
        } else {
            matrizAdjacencia->adicionarAresta(vertice1, vertice2);
        }
        numeroDeArestas++;
    }
    if (arquivo.bad() || !arquivo.eof()) {
        throw ios_base::failure("Erro durante a leitura das arestas");
    }
}
