#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <ctime>
#include <atomic>
#include <functional>

#include "part1.h"

std::mutex mtx;
int mutex_counter = 0;
const int array_size = 1024 * 1024;
bool sleep = false;
std::atomic<int> atomic_counter;
bool do_w_mutex = true;

int main(int argc, char *argv[]) {
    srand(time(nullptr));

    if (argc == 2 && *argv[1] == 's') {
        sleep = true;
    }
    if (sleep) {
        std::cout << "\nWith sleeping for 10 ns after increment: " << std::endl;
    } else {
        std::cout << "\nWithout sleeping: " << std::endl;
    }

    int *arr = new int[array_size]{0};
    auto clock_start = Clock::now();
    for (int i = 0; i < array_size; ++i) {
        arr[i]++;
        if (sleep) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(10));
        }
    }
    auto clock_end = Clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    std::cout << "One thread time: " << time << std::endl;
    delete[] arr;

    std::cout << "\nMUTEX:\n";
    starter(4);
    starter(8);
    starter(16);
    starter(32);

    std::cout << "\nATOMIC:\n";
    do_w_mutex = false;
    starter(4);
    starter(8);
    starter(16);
    starter(32);
    return 0;
}

void print_arr(int *arr) {
    for (int i = 0; i < array_size; ++i) {
        std::cout << arr[i];
    }
}

void starter(const int &threads_count) {
    if (do_w_mutex) {
        mutex_counter = 0;
    } else {
        atomic_counter = 0;
    }
    int *arr = new int[array_size]{0};
    std::cout << std::endl << threads_count << " Threads\n";
    init_threads(arr, threads_count, calculate);
    int rand_idx = rand() % array_size;
    std::cout << "Element № " << rand_idx << ": " << arr[rand_idx] << std::endl;
    print_if_all_equal(arr);
    delete[] arr;
}

template<typename Function>
void init_threads(int *arr, const int &threads_count, Function func) {
    std::vector<std::thread> threads;

    auto clock_start = Clock::now();
    for (auto i = 0; i < threads_count; ++i) {
        std::thread thread(func, std::ref(arr));
        threads.emplace_back(std::move(thread));
    }

    for (int i = 0; i < threads_count; ++i) {
        threads[i].join();
    }
    auto clock_end = Clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    std::cout << "Function time: " << time << "\n";
}

void calculate(int *arr) {
    auto clock_start = Clock::now();
    if (do_w_mutex) {
        while (true) {
            mtx.lock();
            if (mutex_counter >= array_size) {
                mtx.unlock();
                break;
            }
            arr[mutex_counter]++;
            mutex_counter++;
            mtx.unlock();
            if (sleep) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
            }
        }
    } else {
        int tmp_counter;
        while (true) {
            tmp_counter = atomic_counter.fetch_add(1, std::memory_order_seq_cst);
            if (tmp_counter >= array_size) {
                break;
            } else {
                arr[tmp_counter]++;
            }
            if (sleep) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
            }
        }
    }
    auto clock_end = Clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
    mtx.lock();
    std::cout << "\tThread ID: " << std::this_thread::get_id() << "; time: " << time << std::endl;
    mtx.unlock();
}

void print_if_all_equal(const int *arr) {
    bool check = true;
    for (int i = 0; i < array_size; ++i) {
        if (arr[i] != 1) {
            check = false;
            break;
        }
    }

    if (check) {
        std::cout << "All elements are equal to 1" << std::endl;
    } else {
        std::cout << "ERROR" << std::endl;
    }
}
