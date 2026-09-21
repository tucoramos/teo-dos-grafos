#include "grafos.h"
#include <chrono>
#include <numeric>
#include <string>

using namespace std;
using namespace chrono;

int main(void){
    srand(time(nullptr));
    vector<string> nomeArquivo = {"grafo_1.txt", "grafo_2.txt"};
    
    for (const auto& arquivo : nomeArquivo) {
        int numeroDeVertices;

        ifstream pegar(arquivo);
        pegar >> numeroDeVertices;
        pegar.close();

        ifstream entrada(arquivo);
        if (!entrada.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << arquivo << endl;
            continue;
        }

        Grafo grafoLista(0, entrada); // Inicializa o grafo como lista de adjacência

        entrada.clear();             // Limpa o estado de fim de arquivo
        entrada.seekg(0, ios::beg);  // Volta ao início

        Grafo grafoMatriz(1, entrada); // Inicializa o grafo como matriz de adjacência

        

        vector<int> vertices(100,0);
        for (int i = 0; i < 100; ++i) {
            vertices[i] = rand() % numeroDeVertices + 1; // Gera vértices aleatórios entre 1 e numeroDeVertices
        }

        vector<int> temposListaBFS(100), temposListaDFS(100);
        vector<int> temposMatrizBFS(100), temposMatrizDFS(100);
        for (int i = 0; i < 100; ++i) {
            
            auto inicioListaBFS = high_resolution_clock::now();
            grafoLista.implementacaoBFSLista(vertices[i]);
            auto fimListaBFS = high_resolution_clock::now();
            temposListaBFS[i] = duration_cast<milliseconds>(fimListaBFS - inicioListaBFS).count();

            auto inicioMatrizBFS = high_resolution_clock::now();
            grafoMatriz.implementacaoBFSMatriz(vertices[i]);
            auto fimMatrizBFS = high_resolution_clock::now();
            temposMatrizBFS[i] = duration_cast<milliseconds>(fimMatrizBFS - inicioMatrizBFS).count();

            auto inicioListaDFS = high_resolution_clock::now();
            grafoLista.implementacaoDFSLista(vertices[i]);
            auto fimListaDFS = high_resolution_clock::now();
            temposListaDFS[i] = duration_cast<milliseconds>(fimListaDFS - inicioListaDFS).count();
            
            auto inicioMatrizDFS = high_resolution_clock::now();
            grafoMatriz.implementacaoDFSMatriz(vertices[i]);
            auto fimMatrizDFS = high_resolution_clock::now();
            temposMatrizDFS[i] = duration_cast<milliseconds>(fimMatrizDFS - inicioMatrizDFS).count();
        }

        string nomeArquivoMedidas = "medidas_" + arquivo;
        ofstream medidas(nomeArquivoMedidas);

        medidas << "Arquivo: " << arquivo << endl;
        
        ifstream saidaLista = grafoLista.saidaGrafo();
        string linha;
        getline(saidaLista, linha);
        medidas << linha << "\n"; // Escreve a primeira linha (cabeçalho) no arquivo de medidas
        getline(saidaLista, linha); // Pula a segunda linha 
        while (getline(saidaLista, linha)) {
            medidas << linha << "\n";
        }
        saidaLista.close();

        medidas << "\n\n";
        medidas << "Busca em largura (BFS) na Lista de Adjacência" << "\n";
        medidas << "Tempo médio: " << accumulate(temposListaBFS.begin(), temposListaBFS.end(), 0LL) / 100.0 << " ms" << "\n";
        medidas << "Medidas individuais: [";
        for (int i = 0; i < 100; ++i) {
            if (i > 0) medidas << ", ";
            medidas << temposListaBFS[i] << " ms";
        }
        medidas << "]" << "\n\n";
        
        medidas << "Busca em largura (BFS) na Matriz de Adjacência" << "\n";
        medidas << "Tempo médio: " << accumulate(temposMatrizBFS.begin(), temposMatrizBFS.end(), 0LL) / 100.0 << " ms" << "\n";
        medidas << "Medidas individuais: [";
        for (int i = 0; i < 100; ++i) {
            if (i > 0) medidas << ", ";
            medidas << temposMatrizBFS[i] << " ms";
        }
        medidas << "]" << "\n\n";

        medidas << "Busca em profundidade (DFS) na Lista de Adjacência" << "\n";
        medidas << "Tempo médio: " << accumulate(temposListaDFS.begin(), temposListaDFS.end(), 0LL) / 100.0 << " ms" << "\n";
        medidas << "Medidas individuais: [";
        for (int i = 0; i < 100; ++i) {
            if (i > 0) medidas << ", ";
            medidas << temposListaDFS[i] << " ms";
        }
        medidas << "]" << "\n\n";

        medidas << "Busca em profundidade (DFS) na Matriz de Adjacência" << "\n";
        medidas << "Tempo médio: " << accumulate(temposMatrizDFS.begin(), temposMatrizDFS.end(), 0LL) / 100.0 << " ms" << "\n";
        medidas << "Medidas individuais: [";
        for (int i = 0; i < 100; ++i) {
            if (i > 0) medidas << ", ";
            medidas << temposMatrizDFS[i] << " ms";
        }
        medidas << "]" << "\n\n";

        //4. Determine o pai dos vertices 10, 20, 30 na arvore geradora induzida pela BFS e pela DFS quando iniciamos a busca nos vertices 1, 2, 3.
        vector<int> origens = {1, 2, 3};
        vector<int> destinos = {10, 20, 30};
        vector<int> pais(18, 0);
        vector<int> duracao0(18, 0);
        
        medidas << "\n";
        for (int ori = 0; ori < 3; ++ori){
            for (int des = 0; des < 3; ++des){
                auto tempo = high_resolution_clock::now();
                auto arvore = grafoLista.implementacaoBFSLista(origens[ori]);
                pais[(ori)*3 + (des)] = arvore.pai[destinos[des]];
                auto tempoFinal = high_resolution_clock::now();
                duracao0[(ori)*3 + (des)] = duration_cast<milliseconds>(tempoFinal - tempo).count();
                medidas << "Pai do vertice " << destinos[des] << " na arvore geradora da BFS iniciada no vertice " << origens[ori] << ": " << pais[(ori)*3 + (des)] << "\n";
                medidas << "Tempo para calcular o pai do vertice " << destinos[des] << " na arvore geradora da BFS iniciada no vertice " << origens[ori] << ": " << duracao0[(ori)*3 + (des)] << " ms" << "\n\n";
            }
        }

        for (int ori = 0; ori < 3; ++ori){
            for (int des = 0; des < 3; ++des){
                auto tempo = high_resolution_clock::now();
                auto arvore = grafoLista.implementacaoDFSLista(origens[ori]);
                pais[(ori)*3 + (des) + 9] = arvore.pai[destinos[des]];
                auto tempoFinal = high_resolution_clock::now();
                duracao0[(ori)*3 + (des) + 9] = duration_cast<milliseconds>(tempoFinal - tempo).count();
                medidas << "Pai do vertice " << destinos[des] << " na arvore geradora da DFS iniciada no vertice " << origens[ori] << ": " << pais[(ori)*3 + (des) + 9] << "\n";
                medidas << "Tempo para calcular o pai do vertice " << destinos[des] << " na arvore geradora da DFS iniciada no vertice " << origens[ori] << ": " << duracao0[(ori)*3 + (des) + 9] << " ms" << "\n\n";
            }
        }

        //5. Determine a distancia entre os seguintes pares de vertices (10,20), (10,30), (20,30).
        vector<int> duracao(6, 0);
        vector<int> distancias(6, 0);

        auto tempo0 = high_resolution_clock::now();
        auto arvore0 = grafoLista.implementacaoBFSLista(10);
        distancias[0] = arvore0.nivel[20];
        auto tempoFinal0 = high_resolution_clock::now();
        duracao[0] = duration_cast<milliseconds>(tempoFinal0 - tempo0).count();

        auto tempo1 = high_resolution_clock::now();
        auto arvore1 = grafoLista.implementacaoBFSLista(10);
        distancias[1] = arvore1.nivel[30];
        auto tempoFinal1 = high_resolution_clock::now();
        duracao[1] = duration_cast<milliseconds>(tempoFinal1 - tempo1).count();

        auto tempo2 = high_resolution_clock::now();
        auto arvore2 = grafoLista.implementacaoBFSLista(20);
        distancias[2] = arvore2.nivel[30];
        auto tempoFinal2 = high_resolution_clock::now();
        duracao[2] = duration_cast<milliseconds>(tempoFinal2 - tempo2).count();
        

        // Calcular distâncias na Matriz de Adjacência
        auto tempo3 = high_resolution_clock::now();
        auto arvore3 = grafoMatriz.implementacaoBFSMatriz(10);
        distancias[3] = arvore3.nivel[20 - 1];
        auto tempoFinal3 = high_resolution_clock::now();
        duracao[3] = duration_cast<milliseconds>(tempoFinal3 - tempo3).count();

        auto tempo4 = high_resolution_clock::now();
        auto arvore4 = grafoMatriz.implementacaoBFSMatriz(10);
        distancias[4] = arvore4.nivel[30 - 1];
        auto tempoFinal4 = high_resolution_clock::now();
        duracao[4] = duration_cast<milliseconds>(tempoFinal4 - tempo4).count();

        auto tempo5 = high_resolution_clock::now();
        auto arvore5 = grafoMatriz.implementacaoBFSMatriz(20);
        distancias[5] = arvore5.nivel[30 - 1];
        auto tempoFinal5 = high_resolution_clock::now();
        duracao[5] = duration_cast<milliseconds>(tempoFinal5 - tempo5).count();

        medidas << "\n";
        medidas << "Distâncias: " << "\n";
        medidas << "Distância entre 10 e 20: " << distancias[0] << "\n";
        medidas << "Distância entre 10 e 30: " << distancias[1] << "\n";
        medidas << "Distância entre 20 e 30: " << distancias[2] << "\n";
        medidas << "Tempo medio para calcular distâncias na Lista de Adjacência: " << (duracao[0] + duracao[1] + duracao[2]) / 3 << " ms" << "\n";
        medidas << "Tempos na Lista de Adjacência: [" << duracao[0] << " ms, " << duracao[1] << " ms, " << duracao[2] << " ms]" << "\n";
        medidas << "Tempo medio para calcular distâncias na Matriz de Adjacência: " << (duracao[3] + duracao[4] + duracao[5]) / 3 << " ms" << "\n";
        medidas << "Tempos na Matriz de Adjacência: [" << duracao[3] << " ms, " << duracao[4] << " ms, " << duracao[5] << " ms]" << "\n\n";

        medidas.close();
        entrada.close();
        
    }
    
}