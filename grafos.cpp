#include "grafos.h"

Grafo::Grafo(int tipoGrafo, ifstream& arquivo) : tipo(tipoGrafo) {
    if (tipo == 0) {
        inicializaLista(arquivo);
    } else if (tipo == 1) {
        inicializaMatriz(arquivo);
    }
    else {
        throw std::invalid_argument("Tipo de grafo invalido. Use 0 para lista de adjacencia ou 1 para matriz de adjacencia.");
    }
}

ifstream Grafo::saidaGrafo(){
    if (tipo ==0){
        return saidaLista();
    } else if (tipo ==1){
        return saidaMatriz();
    }
} 

ifstream Grafo::bfs(int vertice){
    if (tipo ==0){
        return bfsLista(vertice);
    } else if (tipo ==1){
        return bfsMatriz(vertice);
    }
}

ArvoreBusca Grafo::implementacaobfsMatriz(int vertice) {
    // Implementação do BFS para matriz de adjacência
    ArvoreBusca arv;
    arv.raiz = vertice;
    arv.pai.assign(numVertices, -1);
    arv.nivel.assign(numVertices, -1);
    vector<bool>visitado(numVertices, false);
    vector<int>fila(numVertices, -1);
    int indiceloop=0;
    int indicefila=1;
    vector<int>&pai=arv.pai;
    vector<int>&nivel=arv.nivel;

    fila[0]=vertice;
    nivel[vertice-1]=0;
    pai[vertice-1]=-1;
    visitado[vertice-1]=true;
    while(indiceloop<indicefila){
        int atual=fila[indiceloop];
        for(int i=0;i<numVertices;i++){
            if(matrizAdjacencia[atual-1][i] && !visitado[i]){
                visitado[i]=true;
                fila[indicefila]=i+1;
                pai[i]=atual;
                nivel[i]=nivel[atual-1]+1;
                indicefila++;
            }
        }
        indiceloop++;
    }
    int ultimo=fila[indicefila-1];
    arv.nivelMaximo=nivel[ultimo-1];
    arv.maisDistante=ultimo;
    return arv;
/*    ofstream arquivoSaida("arvore_bfs.txt");
    arquivoSaida <<"# Arvore BFS a partir do vertice "<< vertice << "\n"; arquivoSaida << "# Vertice Pai Nivel\n";
    for (int i = 0; i < numVertices; i++) {
        arquivoSaida << i+1 << " " << pai[i] << " " << nivel[i] << "\n";
        }
    return arquivoSaida;
*/
}

ArvoreBusca Grafo::implementacaodfsMatriz(int vertice) {
    ArvoreBusca arv;
    arv.raiz = vertice;
    arv.pai.assign(numVertices, -1);
    arv.nivel.assign(numVertices, -1);
    vector<int>& pai = arv.pai;
    vector<int>& nivel = arv.nivel;
    vector<int> proximo(numVertices, 0);
    int indice = 0;
    vector<int> pilha(numVertices, -1);

    pilha[0]=vertice;
    pai[vertice-1] = -1;
    nivel[vertice-1] = 0;

    while (indice >= 0) {
        int atual = pilha[indice]-1;
        for (int i = proximo[atual]; i <= numVertices; i++) {
            if (i == numVertices) {
                indice--;
                break;
            }
            else if (matrizAdjacencia[atual][i] && nivel[i]==-1) {
                pai[i] = atual+1;
                nivel[i] = nivel[atual] + 1;
                indice++;
                pilha[indice] = i+1;
                proximo[atual] = i+1;
                break;
                }
        }
    }
    return arv;
/*
    ofstream arquivoSaida("arvore_dfs.txt");
    arquivoSaida << "# Arvore DFS a partir do vertice " << vertice << "\n";
    arquivoSaida << "# Vertice Pai Nivel\n";

    for (int i = 0; i < numVertices; i++) {
        arquivoSaida << i+1 << " " << pai[i] << " " << nivel[i] << "\n";
    }

    return arquivoSaida;
*/
}

ifstream Grafo::distanciaMatriz(int vertice1, int vertice2){
    ArvoreBusca arv = implementacaobfsMatriz(vertice1);
    //return arv.nivel[vertice2-1];
}

int Grafo::diametroMatriz(){
    int diametro = 0;
    int temp=0;
    for (int i = 1; i <= numVertices; i++) {
        temp=implementacaobfsMatriz(i).nivelMaximo;
        if (diametro < temp) {
            diametro = temp;
        }
    }
    return diametro;
}

vector<vector<int>> Grafo::componentesConexasMatriz() {
    vector<bool> visitado(numVertices, false);
    vector<vector<int>> componentes;

    for (int raiz = 1; raiz <= numVertices; raiz++) {
        if (visitado[raiz-1]) continue;        // ja pertence a uma componente anterior

        ArvoreBusca arv = implementacaobfsMatriz(raiz);     // alcanca exatamente a componente de raiz

        vector<int> vertices;
        for (int i = 0; i < numVertices; i++) {
            if (arv.nivel[i] != -1) {          // nivel != -1 == foi alcancado
                visitado[i] = true;
                vertices.push_back(i+1);
            }
        }
        componentes.push_back(move(vertices));
    }

    // ordem decrescente de tamanho, como pede o enunciado
    sort(componentes.begin(), componentes.end(),
         [](const vector<int>& a, const vector<int>& b) { return a.size() > b.size(); });

    return componentes;
}

ifstream Grafo::dfs(int vertice){
    if (tipo ==0){
        return dfsLista(vertice);
    } else if (tipo ==1){
        return dfsMatriz(vertice);
    }
} 

ifstream Grafo::distancia(int vertice1, int vertice2){
    if (tipo ==0){
        return distanciaLista(vertice1, vertice2);
    } else if (tipo ==1){
        return distanciaMatriz(vertice1, vertice2);
    }
}