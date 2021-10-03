#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <ctime>

typedef std::chrono::high_resolution_clock Clock;

void threads_init(std::vector<int> &arr, const int &threads_count);

void do_w_mtx(std::vector<int> &arr);

void print_if_all_equal(std::vector<int> &arr);

void print_vector(std::vector<int> &arr);

void mtx_starter();

std::mutex mtx;
int counter = 0;
const int array_size = 1024 * 1024;
bool sleep = false;

int main(int argc, char *argv[]) {
    srand(time(0));

    if (argc == 2 && *argv[1] == 's') {
        sleep = true;
    }
    if (sleep) {
        std::cout << "\nWith sleeping for 10 ns after increment: "<< std::endl;
    } else {
        std::cout << "\nWithout sleeping: "<< std::endl;
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
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
    std::cout << "One thread time: " << time << std::endl;

    mtx_starter();
    return 0;
}

void print_vector(std::vector<int> &arr) {
    for (int i = 0; i < array_size; ++i) {
        std::cout << arr[i];
    }
}

void mtx_starter() {
    std::vector<int> elements0(array_size, 0);
    std::cout << "\n4 Threads\n";
    threads_init(elements0, 4);
    int rand_idx = rand() % array_size;
    std::cout << "Element № " << rand_idx << ": " << elements0[rand_idx] << std::endl;
    print_if_all_equal(elements0);
    counter = 0;

    std::vector<int> elements1(array_size, 0);
    std::cout << "\n8 Threads\n";
    threads_init(elements1, 8);
    rand_idx = rand() % array_size;
    std::cout << "Element № " << rand_idx << ": " << elements0[rand_idx] << std::endl;
    print_if_all_equal(elements0);
    counter = 0;

    std::vector<int> elements2(array_size, 0);
    std::cout << "\n16 Threads\n";
    threads_init(elements2, 16);
    rand_idx = rand() % array_size;
    std::cout << "Element № " << rand_idx << ": " << elements0[rand_idx] << std::endl;
    print_if_all_equal(elements0);
    counter = 0;


    std::vector<int> elements3(array_size, 0);
    std::cout << "\n32 Threads\n";
    threads_init(elements3, 32);
    rand_idx = rand() % array_size;
    std::cout << "Element № " << rand_idx << ": " << elements0[rand_idx] << std::endl;
    print_if_all_equal(elements0);
    counter = 0;
}

void threads_init(std::vector<int> &arr, const int &threads_count) {
    std::vector<std::thread> threads;

    auto clock_start = Clock::now();
    for (auto i = 0; i < threads_count; ++i) {
        std::thread thread(do_w_mtx, std::ref(arr));
        threads.emplace_back(std::move(thread));
    }

    for (int i = 0; i < threads_count; ++i) {
        threads[i].join();
    }
    auto clock_end = Clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
    std::cout << "Function time: " << time << "\n";
}

void do_w_mtx(std::vector<int> &arr) {
    auto clock_start = Clock::now();
    while (counter < array_size) {
        mtx.lock();
        arr[counter] += 1;
        counter++;
        mtx.unlock();
        if (sleep) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(10));
        }
    }
    auto clock_end = Clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
    mtx.lock();
    std::cout << "\tThread ID: " << std::this_thread::get_id() << "; time: " << time << std::endl;
    mtx.unlock();
}

void print_if_all_equal(std::vector<int> &arr) {
    bool first_check = std::adjacent_find(arr.begin(), arr.end(),
                                          std::not_equal_to<>()) == arr.end();
    bool second_check = std::equal(arr.begin() + 1, arr.end(), arr.begin());

    if (first_check && second_check) {
        std::cout << "All elements are equal" << std::endl;
    } else {
        std::cout << "ERROR" << std::endl;
    }
}
