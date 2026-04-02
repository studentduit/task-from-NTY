#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <stdexcept>

using namespace std;

// =============================
// Task 1 — Race condition
// =============================
int counter = 0;

void increment_race() {
    for (int i = 0; i < 100000; i++) {
        counter++;
    }
}

void task1() {
    counter = 0;

    thread t1(increment_race);
    thread t2(increment_race);

    t1.join();
    t2.join();

    cout << "Task1 (Race condition): " << counter << endl;
}

// =============================
// Task 2 — Mutex fix
// =============================
mutex mtx;

void increment_safe() {
    for (int i = 0; i < 100000; i++) {
        lock_guard<mutex> lock(mtx);
        counter++;
    }
}

void task2() {
    counter = 0;

    thread t1(increment_safe);
    thread t2(increment_safe);

    t1.join();
    t2.join();

    cout << "Task2 (Mutex): " << counter << endl;
}

// =============================
// Task 3 — No global state
// =============================
void task3() {
    vector<int> data(10, 0);
    vector<int> result(10);

    transform(data.begin(), data.end(), result.begin(),
              [](int x) { return x + 1; });

    cout << "Task3: ";
    for (int x : result) cout << x << " ";
    cout << endl;
}

// =============================
// Task 4 — Parallel square
// =============================
int square(int x) {
    return x * x;
}

void task4() {
    vector<int> data = {1,2,3,4,5};
    vector<future<int>> futures;

    for (int x : data) {
        futures.push_back(async(launch::async, square, x));
    }

    cout << "Task4: ";
    for (auto& f : futures) cout << f.get() << " ";
    cout << endl;
}

// =============================
// Task 5 — parallel_map
// =============================
template<typename Func>
vector<int> parallel_map(Func func, const vector<int>& data) {
    vector<future<int>> futures;

    for (int x : data) {
        futures.push_back(async(launch::async, func, x));
    }

    vector<int> result;
    for (auto& f : futures) {
        result.push_back(f.get());
    }

    return result;
}

void task5() {
    vector<int> data = {1,2,3,4};
    auto result = parallel_map(square, data);

    cout << "Task5: ";
    for (int x : result) cout << x << " ";
    cout << endl;
}

// =============================
// Task 6 — Time comparison
// =============================
void task6() {
    vector<int> data(1000000);
    iota(data.begin(), data.end(), 0);

    auto start = chrono::high_resolution_clock::now();

    vector<int> result(data.size());
    transform(data.begin(), data.end(), result.begin(), square);

    auto end = chrono::high_resolution_clock::now();
    cout << "Task6 (sequential): "
         << chrono::duration<double>(end - start).count() << endl;

    start = chrono::high_resolution_clock::now();

    parallel_map(square, data);

    end = chrono::high_resolution_clock::now();
    cout << "Task6 (parallel): "
         << chrono::duration<double>(end - start).count() << endl;
}

// =============================
// Task 7 — CPU-bound
// =============================
long long heavy_task(int x) {
    long long total = 0;
    for (int i = 0; i < 10000000; i++) {
        total += i * x;
    }
    return total;
}

void task7() {
    vector<int> data = {1,2,3,4};

    auto start = chrono::high_resolution_clock::now();
    for (int x : data) heavy_task(x);
    cout << "Task7 (sequential): "
         << chrono::duration<double>(
                chrono::high_resolution_clock::now() - start).count()
         << endl;

    start = chrono::high_resolution_clock::now();
    parallel_map(heavy_task, data);
    cout << "Task7 (parallel): "
         << chrono::duration<double>(
                chrono::high_resolution_clock::now() - start).count()
         << endl;
}

// =============================
// Task 8 — Pipeline
// =============================
void task8() {
    vector<int> data(100);
    iota(data.begin(), data.end(), 0);

    transform(data.begin(), data.end(), data.begin(), square);

    vector<int> filtered;
    copy_if(data.begin(), data.end(), back_inserter(filtered),
            [](int x) { return x > 100; });

    int result = accumulate(filtered.begin(), filtered.end(), 0);

    cout << "Task8: " << result << endl;
}

// =============================
// Task 10 — Safe execution
// =============================
double risky(int x) {
    if (x == 0) throw runtime_error("error");
    return 10.0 / x;
}

void task10() {
    vector<int> data = {1,2,0,5};

    cout << "Task10: ";
    for (int x : data) {
        try {
            cout << risky(x) << " ";
        } catch (...) {
            cout << "ERR ";
        }
    }
    cout << endl;
}

// =============================
// Task 11 — Transactions
// =============================
void task11() {
    vector<int> data(100000);
    iota(data.begin(), data.end(), 1);

    vector<int> filtered;
    copy_if(data.begin(), data.end(), back_inserter(filtered),
            [](int x) { return x % 2 == 0; });

    auto mapped = parallel_map([](int x) { return x * 2; }, filtered);

    int result = accumulate(mapped.begin(), mapped.end(), 0);

    cout << "Task11: " << result << endl;
}

// =============================
// Task 12 — API simulation
// =============================
int fetch_data(int x) {
    this_thread::sleep_for(chrono::seconds(1));
    return x;
}

void task12() {
    vector<int> data(10);
    iota(data.begin(), data.end(), 0);

    auto start = chrono::high_resolution_clock::now();
    for (int x : data) fetch_data(x);
    cout << "Task12 (sequential): "
         << chrono::duration<double>(
                chrono::high_resolution_clock::now() - start).count()
         << endl;

    start = chrono::high_resolution_clock::now();
    parallel_map(fetch_data, data);
    cout << "Task12 (parallel): "
         << chrono::duration<double>(
                chrono::high_resolution_clock::now() - start).count()
         << endl;
}

// =============================
// MAIN
// =============================
int main() {
    task1();
    task2();
    task3();
    task4();
    task5();
    task6();
    task7();
    task8();
    task10();
    task11();
    task12();
}
