#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>

using namespace std;

vector<int> sortInstance(int size) {
  vector<int> numbers(size);
  for (int i = 0; i < size; i++) {
    numbers[i] = rand() % 60 + 1;
    for (int j = 0; j < i; j++) {
      while (numbers[j] == numbers[i])
        numbers[i] = rand() % (60 + 1);
    }
  }
  sort(numbers.begin(), numbers.end());
  return numbers;
}

void sortAndPrint(int size, int numberOfGames) {
  vector<int> numbers(size);
  for (int j = 0; j < numberOfGames; j++) {
    numbers = sortInstance(size);

    for (auto i : numbers) {
      cout << i << " ";
    }
    cout << endl;
  }
}

int main() {
  srand(time(NULL));

  int numberOfGames;

  cout << "Digite quantos jogos da Mega-Sena quer fazer:" << endl;
  cin >> numberOfGames;
  sortAndPrint(6, numberOfGames);

  return 0;
}