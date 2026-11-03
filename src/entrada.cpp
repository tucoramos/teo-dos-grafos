#include "grafos.h"

#include <cmath>
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
    size_t colunas = 0;
    while (getline(arquivo, linha)) {
        ++numeroLinha;
        if (linha.find_first_not_of(" \t\r\n\f\v") == string::npos) continue;
        istringstream campos(linha);
        vector<string> valores;
        string valor;
        while (campos >> valor) valores.push_back(valor);
        if ((valores.size() != 2 && valores.size() != 3) ||
            (colunas != 0 && valores.size() != colunas)) {
            throw invalid_argument("Aresta malformada na linha " + to_string(numeroLinha));
        }
        colunas = valores.size();
        int vertice1, vertice2;
        istringstream primeiro(valores[0]), segundo(valores[1]);
        if (!(primeiro >> vertice1) || (primeiro >> extra) ||
            !(segundo >> vertice2) || (segundo >> extra)) {
            throw invalid_argument("Aresta malformada na linha " + to_string(numeroLinha));
        }
        if (vertice1 < 1 || vertice1 > numeroDeVertices ||
            vertice2 < 1 || vertice2 > numeroDeVertices) {
            throw invalid_argument("Vertice fora do intervalo 1..n na linha " + to_string(numeroLinha));
        }
        if (numeroDeArestas == numeric_limits<int>::max()) {
            throw overflow_error("Numero de arestas excede o limite de int");
        }
        double peso = 1;
        if (colunas == 3) {
            istringstream campoPeso(valores[2]);
            if (!(campoPeso >> peso) || (campoPeso >> extra) ||
                !isfinite(peso)) {
                throw invalid_argument("Peso invalido na linha " + to_string(numeroLinha) +
                                       ": informe um double finito");
            }
        }
        if (tipo == 0) {
            listaAdjacencia->adicionarAresta(vertice1, vertice2, peso);
        } else {
            matrizAdjacencia->adicionarAresta(vertice1, vertice2, peso);
        }
        if (peso < 0) possuiPesoNegativo = true;
        numeroDeArestas++;
    }
    if (arquivo.bad() || !arquivo.eof()) {
        throw ios_base::failure("Erro durante a leitura das arestas");
    }
}
