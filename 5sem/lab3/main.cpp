#include <iostream>
#include <vector>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;

int vector_size;

signed long long calculate_parallel(std::vector<int> first_vector, std::vector<int> second_vector);

signed long long calculate_regular(std::vector<int> first_vector, std::vector<int> second_vector);


int main() {
    srand(time(nullptr));

    std::cout << "Input vector size: ";
    std::cin >> vector_size;

    std::vector<int> first_vector(vector_size);
    std::vector<int> second_vector(vector_size);

    for (int i = 0; i < vector_size; ++i) {
        first_vector[i] = rand() % 50 - 25;
        second_vector[i] = rand() % 50 - 25;
    }

    long long regular_sum = calculate_regular(first_vector, second_vector);
    long long parallel_sum = calculate_parallel(first_vector, second_vector);

    std::cout << "Regular: " << regular_sum << std::endl;
    std::cout << "Parallel: " << parallel_sum << std::endl;

    if (parallel_sum == regular_sum) {
        std::cout << "Results are equal" << std::endl;
    } else {
        std::cout << "ERROR" << std::endl;
    }

    return 0;
}

signed long long calculate_regular(std::vector<int> first_vector, std::vector<int> second_vector) {
    signed long long sum = 0;

    auto clock_start = Clock::now();
    for (int i = 0; i < vector_size; ++i) {
        sum += first_vector[i] * second_vector[i];
    }
    auto clock_end = Clock::now();

    auto func_time = std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
    std::cout << "Regular time: " << func_time << " microseconds" << std::endl;

    return sum;
}

signed long long calculate_parallel(std::vector<int> first_vector, std::vector<int> second_vector) {
    signed long long sum = 0;

    int i;
    auto clock_start = Clock::now();
#pragma omp parallel for reduction(+: sum)
    for (i = 0; i < vector_size; ++i) {
        sum += first_vector[i] * second_vector[i];
    }
    auto clock_end = Clock::now();
    auto func_time = std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
    std::cout << "Parallel time: " << func_time << " microseconds" << std::endl;

    return sum;
}
