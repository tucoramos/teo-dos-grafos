#include "grafos.h"
#include <chrono>
#include <numeric>
#include <string>

using namespace std;
using namespace chrono;

int main(void){
    vector<string> nomeArquivo = {"grafo_1.txt","grafo_2.txt", "grafo_3.txt", "grafo_4.txt", "grafo_5.txt", "grafo_6.txt"};
    for (const auto& arquivo : nomeArquivo) {
        int numeroDeVertices;

        ifstream pegar(arquivo);
        pegar >> numeroDeVertices;
        pegar.close();

        ifstream entrada(arquivo);
        Grafo grafoLista(0, entrada);
        entrada.close();

        vector<int> vertices(numeroDeVertices,-1);
        vector<int> diametros = {};
        for (int i = 0; i < numeroDeVertices; ++i) {
            if (vertices[i] == -1) {
                auto bfsResult = grafoLista.implementacaoBFSLista(i+1);
                auto distante = grafoLista.implementacaoBFSLista(bfsResult.maisDistante);
                diametros.push_back(distante.nivelMaximo);
                for (int j = 1; j < numeroDeVertices+1; ++j) {
                    if (bfsResult.nivel[j] != -1) {
                        vertices[j-1] = 1;
                    }
                }
                cout << i << "Diametro do grafo " << arquivo << ": " << distante.nivelMaximo << endl;
            }
        }
        cout << "Média dos diâmetros do grafo " << arquivo << ": " << accumulate(diametros.begin(), diametros.end(), 0.0) / diametros.size() << endl;




    }


}