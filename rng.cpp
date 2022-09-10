#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>

using namespace std;

int main() {
  srand(time(NULL));

  int size = 6;
  vector<int> numbers(size);

  for (int i = 0; i < size; i++) {
    numbers[i] = rand() % 60 + 1;
    for (int j = 0; j < i; j++) {
      while (numbers[j] == numbers[i])
        numbers[i] = rand() % (60 + 1);
    }
  }

  sort(numbers.begin(), numbers.end());

  for (auto i : numbers) {
    cout << i << " ";
  }

  cout << endl;

  return 0;
}