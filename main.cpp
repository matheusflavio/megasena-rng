#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>

using namespace std;

int main() {
  srand(time(NULL));

  // Tamanho de um jogo simples da mega-sena e a quantidade de números possíveis de selecionar
  int size = 6;
  int megaSenaOptions = 60;

  // Criando vetor de pares para os 60 números possíveis e os 6 números selecionados para o jogo
  vector<pair<int,int>> touple(megaSenaOptions, make_pair(0,0)); // O primeiro valor são os números possíveis de marcar na cartela, e o segundo são as ocorrência do número
  vector<int> numbers;

  // Atualizando o primeiro valor dos pares para refletir os números do possíveis da cartela
  for (int i = 0; i < megaSenaOptions; i++) {
    touple[i].first = i + 1;
  }

  // Variáveis auxiliares para o contador de números que atingiram 60 ocorrências, número sorteado atualmente e quantidade de números sorteados no total
  int count = 0;
  int sortedNumber = 0;
  int sortedNumbersCounter = 0;

  while(count < size) {
    // Sorteando número de 1 a 60 e atualizando os contadores
    sortedNumber = rand() % 60 + 1;
    touple[sortedNumber - 1].second++;
    sortedNumbersCounter++;
    
    // Atualizando valor da contagem para números sorteados ao menos 60 vezes
    for(auto i : touple) {
      if (i.second >= 60) {
        count++;
      }
    }

    // Ternário para resetar o contador caso necessário
    count == 6 ? count = 6 : count = 0;
  }

  // Impressão dos pares de números e ocorrências apenas de curiosidade
  for (auto i : touple) {
    cout << i.first << " " << i.second << endl;
  }
  cout << endl;

  // Adicionando os valores em que há mais de 60 ocorrências para o vetor de números selecionados para o jogo
  for(auto i : touple) {
    if(i.second >= 60) {
      numbers.push_back(i.first);
    }
  }
  
  cout << "Jogo sugerido:" << endl;
  // Imprimindo os números sorteados para o jogo
  for (auto i : numbers) {
    cout << i << " ";
  }
  cout << endl << endl;

  // Imprimindo a quantidade total de números que foram sortedos no processo de criação do jogo
  cout << "Foram sorteados " << sortedNumbersCounter << " numeros no total para realizar esse jogo" << endl;

  return 0;
}