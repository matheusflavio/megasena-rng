#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <cstdint>
#include <string>

using namespace std;

static const int TOTAL_COMBINACOES = 50063860;
static const int ALVO_REPETICOES = 10;

// Tabela de Combinação pré-calculada para Mapeamento O(1)
int C[61][7];

void inicializarCombinatoria() {
    for (int i = 0; i <= 60; ++i) {
        C[i][0] = 1;
        for (int j = 1; j <= min(i, 6); ++j) {
            C[i][j] = (j == i) ? 1 : C[i - 1][j - 1] + C[i - 1][j];
        }
    }
}

inline int bilheteParaIndice(const array<int, 6>& num) {
    return C[num[0] - 1][1] + C[num[1] - 1][2] + C[num[2] - 1][3] +
           C[num[3] - 1][4] + C[num[4] - 1][5] + C[num[5] - 1][6];
}

array<int, 6> indiceParaBilhete(int idx) {
    array<int, 6> bilhete;
    for (int k = 6; k >= 1; --k) {
        int n = k - 1;
        while (C[n + 1][k] <= idx) n++;
        bilhete[k - 1] = n + 1;
        idx -= C[n][k];
    }
    return bilhete;
}

// PRNG Xoroshiro128+ de altíssima velocidade
struct Xoroshiro128Plus {
    uint64_t s[2];
    static inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }
    inline uint64_t next() {
        const uint64_t s0 = s[0];
        uint64_t s1 = s[1];
        const uint64_t result = s0 + s1;
        s1 ^= s0;
        s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
        s[1] = rotl(s1, 37);
        return result;
    }
    inline uint32_t next_range(uint32_t range) {
        return static_cast<uint32_t>((next() & 0xFFFFFFFF) % range);
    }
};

// Gerenciador de Impressão Assíncrona (Produtor-Consumidor)
class AsyncPrinter {
    queue<string> bufferQueue;
    mutex queueMutex;
    condition_variable cv;
    bool active = true;
    thread printThread;

    void processQueue() {
        while (true) {
            unique_lock<mutex> lock(queueMutex);
            cv.wait(lock, [this] { return !bufferQueue.empty() || !active; });

            while (!bufferQueue.empty()) {
                string chunk = move(bufferQueue.front());
                bufferQueue.pop();
                lock.unlock();
                cout.write(chunk.data(), chunk.size());
                lock.lock();
            }

            if (!active && bufferQueue.empty()) break;
        }
    }

public:
    AsyncPrinter() : printThread(&AsyncPrinter::processQueue, this) {}
    
    ~AsyncPrinter() {
        {
            lock_guard<mutex> lock(queueMutex);
            active = false;
        }
        cv.notify_one();
        if (printThread.joinable()) printThread.join();
    }

    void push(string chunk) {
        {
            lock_guard<mutex> lock(queueMutex);
            bufferQueue.push(move(chunk));
        }
        cv.notify_one();
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    inicializarCombinatoria();

    // Tabela única contínua de memória atômica compartilhada (47.7 MB)
    vector<atomic<uint8_t>> historico(TOTAL_COMBINACOES);

    atomic<int> globalSorteios(0);
    atomic<bool> encontrado(false);
    atomic<int> idxVencedor(-1);

    AsyncPrinter printer;
    unsigned int numThreads = thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;

    vector<thread> workers;

    for (unsigned int t = 0; t < numThreads; ++t) {
        workers.emplace_back([&, t]() {
            // Inicializador único por thread
            Xoroshiro128Plus rng;
            uint64_t seed = chrono::steady_clock::now().time_since_epoch().count() + t * 0x9E3779B97F4A7C15ULL;
            rng.s[0] = seed;
            rng.s[1] = seed ^ 0xBF58476D1CE4E5B9ULL;

            array<int, 6> bilhete;
            array<int, 60> pool;
            string localBuffer;
            localBuffer.reserve(4096);

            while (!encontrado.load(memory_order_relaxed)) {
                int idSorteio = globalSorteios.fetch_add(1, memory_order_relaxed) + 1;

                for (int i = 0; i < 60; ++i) pool[i] = i + 1;

                // Amostragem parcial tipo Fisher-Yates
                for (int i = 0; i < 6; ++i) {
                    uint32_t idx = rng.next_range(60 - i);
                    bilhete[i] = pool[idx];
                    pool[idx] = pool[59 - i];
                }

                sort(bilhete.begin(), bilhete.end());

                int idxCombinacao = bilheteParaIndice(bilhete);
                uint8_t count = historico[idxCombinacao].fetch_add(1, memory_order_relaxed) + 1;

                // Formatação local acumulada em lote (Batching de E/S)
                localBuffer += "Bilhete " + to_string(idSorteio) + ": [ ";
                for (int num : bilhete) {
                    if (num < 10) localBuffer += '0';
                    localBuffer += to_string(num) + ' ';
                }
                localBuffer += "]\n";

                // Descarrega o buffer local para a thread de I/O a cada 64 linhas
                if (localBuffer.size() >= 3072) {
                    printer.push(move(localBuffer));
                    localBuffer.clear();
                    localBuffer.reserve(4096);
                }

                if (count >= ALVO_REPETICOES) {
                    bool esperado = false;
                    if (encontrado.compare_exchange_strong(esperado, true)) {
                        idxVencedor.store(idxCombinacao);
                    }
                    if (!localBuffer.empty()) printer.push(move(localBuffer));
                    break;
                }
            }
        });
    }

    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }

    int idxFinal = idxVencedor.load();
    array<int, 6> bilheteRepetido = (idxFinal != -1) ? indiceParaBilhete(idxFinal) : array<int, 6>{};

    string resumo = "\n====================================================================\n";
    resumo += "CRITERIO ATINGIDO: Bilhete selecionado " + to_string(ALVO_REPETICOES) + " vezes!\nBilhete: [ ";
    for (int num : bilheteRepetido) resumo += to_string(num) + " ";
    resumo += "]\nTotal de bilhetes sorteados: " + to_string(globalSorteios.load()) + "\n";
    resumo += "====================================================================\n";

    printer.push(resumo);

    return 0;
}