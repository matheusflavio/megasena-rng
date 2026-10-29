#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <random>
#include <map>
#include <chrono>

using namespace std;

// Tamanho fixo do jogo e quantidade inicial de números
const int TAM_BILHETE = 6;
const int TOTAL_NUMEROS = 60;
const int ALVO_REPETICOES = 5; // Pode ser alterado para 10

int main() {
    // Semente do gerador de números aleatórios rápido (Mersenne Twister)
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    // 1. Buffer Mestre de Possibilidades (1 a 60)
    vector<int> possibilidadesBase(TOTAL_NUMEROS);
    for (int i = 0; i < TOTAL_NUMEROS; ++i) {
        possibilidadesBase[i] = i + 1;
    }

    // Estrutura para contar as ocorrências de cada bilhete único
    map<array<int, TAM_BILHETE>, int> historicoBilhetes;
    
    array<int, TAM_BILHETE> bilheteSorteado;
    array<int, TAM_BILHETE> bilheteRepetido;
    
    int totalSorteios = 0;
    bool encontrado = false;

    // Otimização de I/O no C++
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    while (!encontrado) {
        totalSorteios++;

        // 2. Duplicação rápida do vetor de possibilidades para a rodada atual
        vector<int> poolAtual = possibilidadesBase;

        // 3. Seleção dos 6 números usando amostragem sem repetição (Swap & Pop)
        for (int i = 0; i < TAM_BILHETE; ++i) {
            // Sorteia um índice no intervalo válido atual
            uniform_int_distribution<int> dist(0, poolAtual.size() - 1);
            int idxSorteado = dist(rng);

            // Armazena o número selecionado
            bilheteSorteado[i] = poolAtual[idxSorteado];

            // Troca o número sorteado com o último elemento e faz o pop (remoção O(1))
            swap(poolAtual[idxSorteado], poolAtual.back());
            poolAtual.pop_back();
        }

        // Ordena o bilhete para garantir comparação uniforme de conjuntos
        sort(bilheteSorteado.begin(), bilheteSorteado.end());

        // Imprime o bilhete selecionado
        cout << "Bilhete " << totalSorteios << ": [ ";
        for (int num : bilheteSorteado) {
            cout << (num < 10 ? "0" : "") << num << " ";
        }
        cout << "]\n";

        // Incrementar o histórico de frequência
        int ocorrencias = ++historicoBilhetes[bilheteSorteado];

        // Critério de Parada: Sequência idêntica atingiu o número de repetições
        if (ocorrencias == ALVO_REPETICOES) {
            bilheteRepetido = bilheteSorteado;
            encontrado = true;
        }
    }

    // Exibição do resultado final
    cout << "\n====================================================================\n";
    cout << "CRITERIO ATINGIDO: Bilhete selecionado " << ALVO_REPETICOES << " vezes!\n";
    cout << "Bilhete: [ ";
    for (int num : bilheteRepetido) {
        cout << num << " ";
    }
    cout << "]\n";
    cout << "Total de bilhetes sorteados: " << totalSorteios << "\n";
    cout << "====================================================================\n";

    return 0;
}