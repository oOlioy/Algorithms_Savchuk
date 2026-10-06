#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;
using namespace std::chrono;

//гномове сортування
void gnomeSort(vector<int>& a, long long& comp, long long& swaps) {
    int n = a.size();
    int i = 0;
    while (i < n) {
        if (i == 0) {
            i++;
            continue;
        }
        comp++;
        if (a[i] >= a[i - 1]) {
            i++;
        } else {
            swap(a[i], a[i - 1]);
            swaps++;
            i--;
        }
    }
}

//сортування вибором
void selectionSort(vector<int>& a, long long& comp, long long& swaps) {
    int n = a.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            comp++;
            if (a[j] < a[minIdx]) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            swap(a[i], a[minIdx]);
            swaps++;
        }
    }
}

//швидке сортування 
int partition(vector<int>& a, int low, int high, long long& comp, long long& swaps) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        comp++;
        if (a[j] < pivot) {
            i++;
            swap(a[i], a[j]);
            swaps++;
        }
    }
    swap(a[i + 1], a[high]);
    swaps++;
    return i + 1;
}

void quickSort(vector<int>& a, int low, int high, long long& comp, long long& swaps) {
    while (low < high) {
        int p = partition(a, low, high, comp, swaps);
        if (p - low < high - p) {
            quickSort(a, low, p - 1, comp, swaps);
            low = p + 1;
        } else {
            quickSort(a, p + 1, high, comp, swaps);
            high = p - 1;
        }
    }
}
void quickSort(vector<int>& a, long long& comp, long long& swaps) {
    quickSort(a, 0, (int)a.size() - 1, comp, swaps);
}

vector<int> genRandom(int n) {
    vector<int> a(n);
    mt19937 gen(42);
    uniform_int_distribution<int> dist(0, 1000000);
    for (int i = 0; i < n; i++) a[i] = dist(gen);
    return a;
}
vector<int> genSorted(int n) {
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = i;
    return a;
}
vector<int> genReverse(int n) {
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = n - i;
    return a;
}

struct Result {
    double timeMs;
    long long comp;
    long long swaps;
};

Result runSort(void (*sortFunc)(vector<int>&, long long&, long long&), vector<int> arr) {
    long long comp = 0, swaps = 0;
    auto start = high_resolution_clock::now();
    sortFunc(arr, comp, swaps);
    auto end = high_resolution_clock::now();
    double ms = duration<double, milli>(end - start).count();
    return {ms, comp, swaps};
}

int main() {
    vector<int> sizes = {100, 1000, 10000};
    vector<string> orderNames = {"випадковий ", "впорядкований ", "зворотний "};
    vector<string> algNames = {"Гномове ", "Швидке ", "Вибором "};

    cout << left << setw(14) << "Алгоритм " << setw(8) << "N" << setw(16) << "Порядок"
         << setw(12) << "Час(мс) " << setw(14) << " Порівняння  " << setw(12) << " Перестановки " << "\n";
    cout << string(76, '-') << "\n";

    for (int n : sizes) {
        vector<vector<int>> arrays = { genRandom(n), genSorted(n), genReverse(n) };

        for (int o = 0; o < 3; o++) {
            Result rGnome = runSort(gnomeSort, arrays[o]);
            Result rQuick = runSort(quickSort, arrays[o]);
            Result rSelect = runSort(selectionSort, arrays[o]);

            vector<Result> results = {rGnome, rQuick, rSelect};

            for (int alg = 0; alg < 3; alg++) {
                cout << left << setw(14) << algNames[alg] << setw(8) << n
                     << setw(16) << orderNames[o]
                     << setw(12) << fixed << setprecision(2) << results[alg].timeMs
                     << setw(14) << results[alg].comp
                     << setw(12) << results[alg].swaps << "\n";
            }
        }
        cout << string(76, '-') << "\n";
    }
    return 0;
}
