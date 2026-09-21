#include "memoria.h"
#include "grafos.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "uso: estudo <arquivo> <matriz|lista>\n";
        return 1;
    }

    double antes = memoriaMB();

    ifstream arquivo(argv[1]);
    int tipo = (string(argv[2]) == "matriz") ? 1 : 0;
    Grafo g(tipo, arquivo);

    double depois = memoriaMB();
    cout << "Memoria do grafo: " << (depois - antes) << " MB\n";
    cout << "Total do processo: " << depois << " MB\n";
    cout << "Vertices: " << g.vertices() << "\n";
    cout << "Arestas: "  << g.arestas()  << "\n";
}